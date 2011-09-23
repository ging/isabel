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
import java.net.InetAddress;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.ScheduledExecutorService;
import java.util.logging.Level;
import java.util.logging.Logger;


/**
 * Class that sends packets and retransmissions.
 * @author Fernando Escribano
 */
class RUDPSender {

    /**
     * Class that implements the retransmission task
     */
    private class RetransmissionTask implements Runnable {

        /**
         * The packet to be retransmitted
         */
        RUDPPacket packet;
        
        RetransmissionTask(RUDPPacket p) {
            packet = p;
        }

        public void run() {
            try {
                stats.retransmissionSent();
                send(packet);
            }catch(IOException ioe) {
                logger.log(Level.SEVERE, "Error sending package", ioe);
            }
        }
    }

    /**
     * Remote address to send packets
     */
    InetAddress remoteAddress;

    /**
     * Remote port to send packets
     */
    int remotePort;

    /**
     * Excutor that performs the retransmissions
     */
    private ScheduledExecutorService executor;

    /**
     * UPDSockets to send the datagrams
     */
    private DatagramSocket socket;

    /**
     * Class logger
     */
    private Logger logger = Logger.getLogger(RUDPSender.class.getName());

    /**
     *  Stats object
     */
    private Stats stats;

    /**
     * Builds a new RUDPSender
     * @param sock UDP socket to send the data
     * @param address Remote address to send data
     * @param port Remote port to send data
     * @param stats Object to collect stats
     */
    public RUDPSender(DatagramSocket sock, InetAddress address, int port, Stats stats) {
        socket = sock;
        remoteAddress = address;
        remotePort = port;
        this.stats = stats;
        executor = Executors.newSingleThreadScheduledExecutor();
    }

    /**
     * Program retransmissions of a packet
     * @param packet The packets to be retransmitted
     * @return The Future that allows the retransmition cancellation when the ACK
     * is received.
     */
    public ScheduledFuture<?> programRetransmission(RUDPPacket packet) {
        // TODO: Numero maximo de retransmisiones?
        return executor.scheduleAtFixedRate(new RetransmissionTask(packet),
                RUDPSocket.RT_TIME, 2*RUDPSocket.RT_TIME, TimeUnit.MILLISECONDS);
    }

    /**
     * Send a new RUDPPacket
     * @param packet The packet to send
     * @throws java.io.IOException If an error occurs.
     */
    public void send(RUDPPacket packet) throws IOException {
        //if (new java.util.Random().nextInt(100) < 50) return;

        byte[] buf = packet.marshall();
        SocketSync.send(socket, new DatagramPacket(buf, buf.length, remoteAddress, remotePort));

        stats.packetSent(packet);

        //System.out.println(remotePort +" ENVIANDO PAQUETE: " + packet);
    }

    /**
     * Stop the sender thread.
     */
    public void stop() {
        executor.shutdownNow();
    }
}
