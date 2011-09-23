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
import java.net.SocketAddress;
import java.net.SocketTimeoutException;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Class that reads packets as the arrive from the socket
 * @author Fernando Escribano
 */
class RUDPReceiver extends Thread {

    /**
     * RUDP ServerSocket to deliver the received data
     */
    private RUDPServerSocket serverSock;

    /**
     * UDP socket to receive data from
     */
    private DatagramSocket socket;

    /**
     * Flag to stop the thread.
     */
    private boolean run = true;

    /**
     * Table to store sockets to deliver data received from the socket
     */
    private Map<SocketAddress, RUDPSocket> socketMap;

    /**
     * Class logger
     */
    private Logger logger = Logger.getLogger(RUDPReceiver.class.getName());

    /**
     * Build a receiver that serves a single RUDPsocket (client).
     * @param sock the UDP socket
     */
    public RUDPReceiver(DatagramSocket sock) {
        this(sock, null);
    }

    /**
     * Build a receiver that serves a RUDPServerSocket and all associated RUDPSockets
     * @param sock the UDP socket
     * @param ss The RUDPServerSocket to listen for incomming connections
     */
    public RUDPReceiver(DatagramSocket sock, RUDPServerSocket ss) {
        socket = sock;
        serverSock = ss;
        socketMap = new HashMap<SocketAddress, RUDPSocket>();
    }

    /**
     * Registers a new socket to deliver data received. The receiver delivers data
     * to a particular socket by looking at the remote socket address.
     * @param sa Remote address+port for this socket
     * @param sock The RUDPSocket to deliver data
     */
    public synchronized void registerSocket(SocketAddress sa, RUDPSocket sock) {
        socketMap.put(sa, sock);
    }

    /**
     * Unregister a socket.
     * @param sa The socket address to unregister.
     */
    public synchronized void unregisterSocket(SocketAddress sa) {
        socketMap.remove(sa);
    }

    /**
     * Stop the Thread
     */
    public void stopReceiver() {
        run = false;
    }

    @Override
    public void run() {
        while(run) {
            try {
                // Receive packets
                DatagramPacket p = new DatagramPacket(new byte[RUDPPacket.RUDP_MTU], RUDPPacket.RUDP_MTU);
                socket.receive(p);
                SocketAddress addr = p.getSocketAddress();
                RUDPPacket rp = new RUDPPacket(p.getData());

                //System.out.println(p.getPort() + " RECIBIDO PAQUETE: " + rp);

                // Deliver valid packets to the appropiate socket
                if (rp.header.isValid()) {
                    if (socketMap.containsKey(addr)) {
                        socketMap.get(addr).packetReceived(rp);
                    }
                    else if (serverSock != null && rp.header.isSYN()) {
                        serverSock.processPacket(rp, addr);
                    }
                    else {
                        logger.log(Level.SEVERE, "Paquete no despachado: " + rp);
                    }
                }
            }catch (SocketTimeoutException tex) {
                
            }catch (IOException ex) {
                if (run) {
                    logger.log(Level.SEVERE, null, ex);
                }
            }
        }
    }
}
