/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package isabel.lib.reliableudp;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.SocketException;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ScheduledFuture;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * This class implements a reliable UDP socket. A socket is a communication
 * endpoint between two machines.
 * @author Fernando Escribano
 */
public class RUDPSocket {

    public static final int CLOSE_LINGER = 10000;

    /**
     * Time between retransmissions
     */
    public static final int RT_TIME = 500;

    /**
     * Maximun number of unacknowledged packets
     */
    private static final int MAX_QUEUE = 64;

    /**
     * Maximun number of packets in the rcvd queue
     */
    private static final int MAX_RCVD_QUEUE = 640;

    /**
     * Connection time out
     */
    private static final long CONNECT_TIMEOUT = 5000;

    /**
     * The UDP socket
     */
    private DatagramSocket udpsock;

    /**
     * Table that holds the received packets to allow reordering and buffering
     */
    private final Map<Integer, RUDPPacket> rcvdMap;

    /**
     * Table with the unacknowledge packets and the retransmission future to
     * allow cancellation when the ack arrives
     */
    private final Map<Integer, ScheduledFuture<?>> unackMap;

    /**
     * Object that provides InputStream/OutputStream for RUDP sockets
     */
    private RUDPStreams streams;

    /**
     * Last seq number delivered to the upper layer
     */
    private int lastSeqRcvd = 0;

    /**
     * Last seq number sended
     */
    private int lastSeqSent = 0;

    /**
     * SeqNumber of the reset package.
     */
    private int resetSeqNumber = 0;

    /**
     * Class logger
     */
    private Logger logger = Logger.getLogger(RUDPSocket.class.getName());

    /**
     * Boolean that indicates ownership of the UDP socket and the receiver thread
     */
    private boolean clientSock;

    private enum RUDPState {
        CONNECTED,
        DISCONNECTED,
        CLOSE_SENT,
        CLOSE_RCVD,
        CLOSING
    }

    /**
     * Boolean that indicates if the scket is connected
     */
    private RUDPState state;

    /**
     * Sender thread
     */
    private RUDPSender sender;

    /**
     * Receiver thread
     */
    private RUDPReceiver receiver;

    /**
     *  Stats object
     */
    private Stats stats;


    /**
     * Build a new client socket
     * @param address Adress to connect to
     * @param port Port to connect to
     * @throws java.net.SocketException
     * @throws java.io.IOException
     * @throws java.lang.InterruptedException
     */
    public RUDPSocket(InetAddress address, int port)
            throws SocketException,  IOException, InterruptedException {
        this(address, port, null, 0);
    }

    /**
     * Build a new client socket
     * @param remoteAddr Adress to connect to
     * @param rport Port to connect to
     * @param localAddr Local address
     * @param lport Local port
     * @throws java.net.SocketException
     * @throws java.io.IOException
     * @throws java.lang.InterruptedException
     */
    public RUDPSocket(InetAddress remoteAddr, int rport, InetAddress localAddr, int lport)
            throws SocketException, IOException, InterruptedException {
        clientSock = true;
        state = RUDPState.DISCONNECTED;
        udpsock = new DatagramSocket(lport, localAddr);
        udpsock.setSoTimeout(100);
        rcvdMap = new HashMap<Integer, RUDPPacket>();
        unackMap = new HashMap<Integer, ScheduledFuture<?>>();
        stats = new Stats();
        sender = new RUDPSender(udpsock, remoteAddr, rport, stats);
        receiver = new RUDPReceiver(udpsock);
        receiver.registerSocket(new InetSocketAddress(remoteAddr, rport) , this);
        streams = new RUDPStreams(this);
        receiver.start();
        connect();
    }

    /**
     * Build a new socket. constructor used by RUDPServerSocket
     * @param remoteAddr Remote address to where this socket is connected
     * @param rport Remote port to where this socket is connected
     * @param sock The UDP socket
     * @param recv The receiver thread
     */
    RUDPSocket(InetAddress remoteAddr, int rport, DatagramSocket sock, RUDPReceiver recv, int rcvdseq, int sentseq) {
        setLastSeqRcvd(rcvdseq);
        setLastSeqSent(sentseq);
        clientSock = false;
        state = RUDPState.CONNECTED;
        udpsock = sock;
        rcvdMap = new HashMap<Integer, RUDPPacket>();
        unackMap = new HashMap<Integer, ScheduledFuture<?>>();
        stats = new Stats();
        sender = new RUDPSender(udpsock, remoteAddr, rport, stats);
        receiver = recv;
        receiver.registerSocket(new InetSocketAddress(remoteAddr, rport) , this);
        streams = new RUDPStreams(this);
    }

    /**
     * Get an input stream for this socket
     * @return InputStream that allows to receive data through the socket
     */
    public InputStream getInputStream() {
        return streams.getInputStream();
    }

    /**
     * Get an output stream for this socket
     * @return OutputStream that allows to send data through the socket
     */
    public OutputStream getOutputStream() {
        return streams.getOutputStream();
    }

    /**
     * Receive a new packet from this socket. This method is blocking.
     * Will return null if the other half of the connection is closed
     * @return An RUDPPacket, null if the connection is closed
     * @throws java.lang.InterruptedException
     */
    public RUDPPacket receive() throws InterruptedException {
        //System.out.println("WAITING FOR PACKET#" + getNextSeq(lastSeqRcvd));
        if((state == RUDPState.DISCONNECTED) ||
           ((state != RUDPState.CONNECTED) && (getLastSeqRcvd() == resetSeqNumber-1))) {
            return null;
        }

        synchronized(rcvdMap) {
            while (!rcvdMap.containsKey(getNextSeq(getLastSeqRcvd()))) {
                if((state == RUDPState.DISCONNECTED) ||
                   ((state != RUDPState.CONNECTED) && (getLastSeqRcvd() == resetSeqNumber-1))) {
                    return null;
                }
                rcvdMap.wait(100);
            }
            return rcvdMap.remove(incrementLastSeqRcvd());
        }
    }

    /**
     * Tells if a call to receive will block the calling thread.
     * @return True if the thread will not block. False otherwise.
     */
    public boolean available() {
        return rcvdMap.containsKey(getNextSeq(getLastSeqRcvd()));
    }

    /**
     * Send a data packet. This method will block the calling thread if the limit
     * of unacknowledge packages has been reached.
     * @param packet The data packet to be sent
     * @throws java.io.IOException If an IO error occurs
     * @throws java.lang.InterruptedException
     */
    public void sendData(RUDPPacket packet) throws IOException, InterruptedException {

        while(unackMap.size() >= MAX_QUEUE ) {
            if (state == RUDPState.DISCONNECTED ||
                state == RUDPState.CLOSE_SENT ||
                state == RUDPState.CLOSING) {
                throw new IOException("Socket is closed");
            }
            synchronized(unackMap) {
                unackMap.wait(100);
            }
        }

        // Complete the packet header
        packet.header.setSeqNumber(incrementLastSeqSent());
        packet.header.setChecksum();
        
        // Program retransmission
        ScheduledFuture<?> ret = sender.programRetransmission(packet);
        unackMap.put(packet.header.getSeqNumber(), ret);

        // Send the package
        sender.send(packet);
    }

    /**
     * Return the remote address to which this socket is connected.
     * @return InetAddress object
     */
    public InetAddress getInetAddress() {
        return sender.remoteAddress;
    }

    /**
     * Return the local address of this socket.
     * @return InetAddress object
     */
    public InetAddress getLocalAddress() {
        return udpsock.getLocalAddress();
    }

    /**
     * Return the remote port to which this socket is connected.
     * @return port number
     */
    public int getPort() {
        return sender.remotePort;
    }

    /**
     * Return the local port of this socket
     * @return port number
     */
    public int getLocalPort() {
        return udpsock.getLocalPort();
    }

    /**
     * Close this half of the connection
     * After this method is called calls to send will thowr an Exception
     * @throws IOException
     */
    public void closeOut() throws IOException {
        //Sends RST
        RUDPPacket rst = RUDPPacket.createRst(incrementLastSeqSent());
        ScheduledFuture<?> ret = sender.programRetransmission(rst);
        unackMap.put(rst.header.getSeqNumber(), ret);
        sender.send(rst);

        switch(state) {
            case CONNECTED:
                state = RUDPState.CLOSE_SENT;
                break;
            case CLOSE_RCVD:
                // TODO: Close everything after a few seconds
                state = RUDPState.CLOSING;
                break;
            default:
                logger.log(Level.SEVERE, "closeOut(): UNEXPECTED STATE:" + state);
        }
    }

    /**
     * Destroy the socket. Should be called a few seconds after closeOut().
     * @param linger The time to wait before closing
     */
    public void close(int linger) {
        try {
            Thread.sleep(linger);
        }catch (Exception ex) {
        }
        
        sender.stop();
        receiver.unregisterSocket(new InetSocketAddress(sender.remoteAddress, sender.remotePort));
        
        if (clientSock) {
            receiver.stopReceiver();
            udpsock.close();
        }
    }

    /**
     * Get stats as a String
     * @return String 
     */
    public String getStats() {
        return stats.toString();
    }

    //--------------------------------------------------------------------------
    //                              PRIVATE METHODS
    //--------------------------------------------------------------------------

    private synchronized void setLastSeqRcvd(int last) {
        lastSeqRcvd = last;
    }

    private synchronized void setLastSeqSent(int last) {
        lastSeqSent = last;
    }

    private synchronized int getLastSeqRcvd() {
        return lastSeqRcvd;
    }

    private synchronized int getLastSeqSent() {
        return lastSeqSent;
    }

    private void connect() throws IOException, InterruptedException {
        // Send syn
        setLastSeqRcvd(15);
        setLastSeqSent(15);
        RUDPPacket syn = RUDPPacket.createSyn(getLastSeqSent());
        ScheduledFuture<?> ret = sender.programRetransmission(syn);
        unackMap.put(syn.header.getSeqNumber(), ret);
        sender.send(syn);

        // Wait for ack
        long init = System.currentTimeMillis();
        long wait = CONNECT_TIMEOUT;
        while (wait > 0) {
            if (!ret.isCancelled()) {
                synchronized(unackMap) {
                    unackMap.wait(wait);
                }
                wait = CONNECT_TIMEOUT - (System.currentTimeMillis() - init);
            }
            else {
                break;
            }
        }


        if (ret.isCancelled()) {
            state = RUDPState.CONNECTED;
        }
        else {
            ret.cancel(true);
            throw new IOException("Unable to connect");
        }
    }

    private int getNextSeq(int seq) {
        return (seq+1)%0xFFFF;
    }

    private synchronized int incrementLastSeqRcvd() {
        lastSeqRcvd = getNextSeq(lastSeqRcvd);
        return lastSeqRcvd;
    }

    private synchronized int incrementLastSeqSent() {
        lastSeqSent = getNextSeq(lastSeqSent);
        return lastSeqSent;
    }

    void packetReceived(RUDPPacket packet) {
        stats.packetRcvd(packet);
        if (packet.header.isACK()) {
            synchronized(unackMap) {
                // Cacel the retranmission task for the acknowledged packet
                ScheduledFuture<?> ret = unackMap.remove(packet.header.getSeqNumber());
                if (ret != null) {
                    ret.cancel(true);
                }
                else {
                    logger.log(Level.SEVERE, "Unexpected ack:" + packet);
                }
                unackMap.notifyAll();
            }
        }

        if (packet.header.isDAT()) {
            //Process only if the queue is not full
            //if(true) {  // Cola infinita
            //if (rcvdMap.size() < MAX_RCVD_QUEUE) {
            if (inRange(packet.header.getSeqNumber())) {
                // Save if not duplicated
                if (!isDuplicated(packet.header.getSeqNumber(), getLastSeqRcvd())) {
                    synchronized(rcvdMap) {
                        // Save the packet and notify receivers
                        rcvdMap.put(packet.header.getSeqNumber(), packet);
                        rcvdMap.notifyAll();
                    }
                }
                // Send ACK anyway
                try {
                    sender.send(RUDPPacket.createAck(packet.header.getSeqNumber()));
                }catch (IOException ioe) {
                    logger.log(Level.SEVERE,"Error sending ACK", ioe);
                }
            }
        }

        if (packet.header.isRST()) {
            // Save SeqNumber and change state
            resetSeqNumber = packet.header.getSeqNumber();

            switch(state) {
                case CONNECTED:
                    state = RUDPState.CLOSE_RCVD;
                    break;
                case CLOSE_SENT:
                case CLOSING:
                    // TODO: Close everything after a few seconds
                    state = RUDPState.CLOSING;
                    break;
                default:
                    logger.log(Level.SEVERE,"RST Packet received: UNEXPECTED STATE:" + state);
            }

            // Send ACK
            try {
                sender.send(RUDPPacket.createAck(packet.header.getSeqNumber()));
            }catch (IOException ioe) {
                logger.log(Level.SEVERE,"Error sending ACK", ioe);
            }
        }
        if (packet.header.isSYN()) {
            // Send ACK
            try {
                sender.send(RUDPPacket.createAck(packet.header.getSeqNumber()));
            }catch (IOException ioe) {
                logger.log(Level.SEVERE,"Error sending ACK", ioe);
            }
        }
        /*
        // si he recibido un RST paro de recibir datos en este socket
        // si ya ha llegado el último
        if ((state == RUDPState.CLOSE_RCVD || state == RUDPState.CLOSING) &&
             lastSeqRcvd == resetSeqNumber - 1) {
            if (clientSock) {
                receiver.stopReceiver();
            }
            else {
                receiver.unregisterSocket(new InetSocketAddress(sender.remoteAddress, sender.remotePort));
            }
        }
        */
    }

    private boolean  isDuplicated(int seqnew, int seqlast) {
        if ((seqlast > seqnew) && ( (seqlast-seqnew) < 2*MAX_QUEUE)) {
            //System.out.println("DUPLICATED RECEIVED seqnew:" + seqnew + " seqlast:" + seqlast);
            return true;
        }
        return false;
    }

    private synchronized boolean inRange(int seqNumber) {
        boolean result = false;
        if (seqNumber >= getLastSeqRcvd()) {
            if ( (seqNumber - getLastSeqRcvd()) < MAX_RCVD_QUEUE) {
                result = true;
            }
        }
        else {
            if ( (0xFFFF - getLastSeqRcvd() + seqNumber) < MAX_RCVD_QUEUE) {
                result = true;
            }
        }
        return result;
    }
}
