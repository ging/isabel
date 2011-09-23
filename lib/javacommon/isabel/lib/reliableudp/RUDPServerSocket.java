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
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketAddress;
import java.net.InetSocketAddress;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * This class implements a reliable UDP server socket. A server sockets waits
 * for connection requests.
 * @author Fernando Escribano
 */
public class RUDPServerSocket {

    /**
     * The UDP socket
     */
    private DatagramSocket udpsock;

    /**
     * The receiver thread for this
     */
    private RUDPReceiver receiver;

    /**
     * SocketAddress of the last incomming connection
     */
    private InetSocketAddress lastConnection;

    /**
     * Flag that indicates if socket is accepting connections
     */
    private boolean accepting = false;

    /**
     * seqNumber of the last incoming connection
     */
    private int connectionSeqNumber = 0;

    /**
     * Class logger
     */
    private Logger logger = Logger.getLogger(RUDPServerSocket.class.getName());

    /**
     * Create a new server socket on any free port
     * @throws java.io.IOException
     */
    public RUDPServerSocket() throws IOException {
        udpsock = new DatagramSocket();
        udpsock.setSoTimeout(100);
        receiver = new RUDPReceiver(udpsock, this);
        receiver.start();
    }

    /**
     * Create a new server socket on the specified port
     * @param port the port to bind the socket
     * @throws java.io.IOException
     */
    public RUDPServerSocket(int port) throws IOException {
        udpsock = new DatagramSocket(port);
        receiver = new RUDPReceiver(udpsock, this);
        receiver.start();
    }

    /**
     * Create a new socket boubnd to the specified address and port
     * @param port the port to bind the socket
     * @param backlog not used
     * @param bindAddr the address to bind the socket
     * @throws java.io.IOException
     */
    public RUDPServerSocket(int port, int backlog, InetAddress bindAddr) throws IOException {
        udpsock = new DatagramSocket(port, bindAddr);
        receiver = new RUDPReceiver(udpsock, this);
        receiver.start();
    }

    /**
     * Return the local address of this socket.
     * @return InetAddress object
     */
    public InetAddress getLocalAddress() {
        return udpsock.getLocalAddress();
    }

    /**
     * Return the local port of this socket
     * @return port number
     */
    public int getLocalPort() {
        return udpsock.getLocalPort();
    }

    /**
     * Listens for a connection to be made to this socket and accepts it.
     * @return the new Socket
     * @throws java.lang.InterruptedException
     */
    public synchronized RUDPSocket accept() throws InterruptedException {
        accepting = true;
        while (lastConnection == null) {
            wait();
        }
        RUDPSocket sock = new RUDPSocket(lastConnection.getAddress(),
                             lastConnection.getPort(), udpsock, receiver,
                             connectionSeqNumber, connectionSeqNumber);

        lastConnection = null;
        accepting = false;
        notifyAll();
        return sock;
    }

    /**
     * Closes the server socket.
     */
    public void close() {
        receiver.stopReceiver();
        udpsock.close();
    }

    //-------------------------------------------------------------------------

    void processPacket(RUDPPacket packet, SocketAddress addr) {
        try {
            // Si no estoy aceptando conexiones descarto el paquete
            if (!accepting)
                return;
            
            // Send ACK
            RUDPPacket resp = RUDPPacket.createAck(packet.header.getSeqNumber());

            //System.out.println(addr + ":ENVIANDO: " + resp);

            byte[] buf = resp.marshall();
            SocketSync.send(udpsock, new DatagramPacket(buf, buf.length, addr));

            // Save connection data
            synchronized(this) {
                lastConnection = (InetSocketAddress)addr;
                connectionSeqNumber = packet.header.getSeqNumber();
                notifyAll();
                wait();
            }
        }catch (Exception ioe) {
            logger.log(Level.SEVERE, "Error sending ACK", ioe);
        }
    }
}
