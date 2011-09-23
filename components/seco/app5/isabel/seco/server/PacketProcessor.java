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
 * PacketProcessor.java
 */

package isabel.seco.server;

import isabel.lib.reliableudp.RUDPSocket;
import java.io.*;
import java.net.*;
import java.util.logging.*;
import java.util.*;

/**
 * PacketProcessor is a class used to send and receive SeCo packets.
 * 
 * Para que los clientes y el servidor SeCo se comuniquen, hay que crear un
 * socket TCP. Una vez que existe este socket, deben crearse unos objetos
 * BufferedInputStream y BufferedOutputStream asociados a este socket, y que se
 * usan para enviar o recibir los paquetes SeCo.
 * 
 * Esta clase se encarga de crear los objetos BufferedInputStream y
 * BufferedOutputStream asociados al socket que se pasa en el constructor, y
 * proporciona metodos para enviar y recibir los paquetes SeCo.
 * 
 * @author Santiago Pavon Gomez
 */
public class PacketProcessor {

    /**
     * The socket that connect the SeCo server and the client.
     */
    private final Socket sock;

    /**
     * The socket that connect the SeCo server and the client.
     */
    private final RUDPSocket rudpsock;

    /**
     * Reader built to read packets.
     */
    private BufferedInputStream in;

    /**
     * Writer built to send packets.
     */
    private BufferedOutputStream out;

    /**
     * Object to make mutex on the "receive" method. I don't use synchronize to
     * enable calls to "send" while I am blocked receiving.
     */
    private Object recvLock = new Object();

    /**
     * Logger where logs are written.
     */
    private Logger mylogger;

    /**
     * Sets as true when the associated client is disconnected. It sent a BYE
     * command.
     */
    private boolean invalid;

    /**
     * Class where sockets with a write operation in course are registered.
     * These sockets are closed if the write operation does not finish in a
     * given timeout period.
     */
    private static NoBlocking noblock = new NoBlocking();

    /**
     * Thread to send the SeCoPackets. It is used to avoid the deadlock of all
     * the client connections because one socket.write operation is blocked.
     * 
     * The "send" method inserts the SeCoPacket to send in a list, and this
     * thread get the packet from this list and send then.
     */
    private class PPSender extends Thread {

        /**
         * List with the packet to send.
         */
        private List<SeCoPacket> l = new LinkedList<SeCoPacket>();

        /**
         * Constructor.
         */
        PPSender() {
            setDaemon(true);
        }

        /**
         * Add a new packet into l in order to be send.
         */
        synchronized void add(SeCoPacket packet) {

            if (l == null)
                return;

            l.add(packet);
            notify();
        }

        /**
         * Finish the thread
         */
        synchronized void finish() {

            l = null;
            notify();
        }

        /**
         * Send the given packet.
         * 
         * @param packet
         *            the SeCoPackeet to send.
         * @throws IOException
         *             A exception describing a communication problem.
         */
        private void sendPkt(SeCoPacket packet) throws IOException {
            try {
                byte[] msg = packet.getBytes();
                mylogger.finest("PP.send to=" + packet.getToID() + " PL="
                        + packet.getDataSize() + " L=" + msg.length
                        + " COMMAND: " + packet.getCommand());
                if (sock != null)
                    noblock.add(sock);
                else if (rudpsock != null)
                    noblock.add(rudpsock);
                out.write(msg, 0, msg.length);
                out.flush(); // send it now.

            } catch (IOException ioe) {
                invalid();
                close();
                throw ioe;
            } finally {
                if (sock != null)
                    noblock.del(sock);
                else if (rudpsock != null)
                    noblock.del(rudpsock);
            }
        }

        /**
         * Loop to send packet stores in the list l. This loop ends (and the
         * thread) when an exception is thrown.
         */
        public void run() {

            try {
                SeCoPacket packet = null;
                main: while (true) {
                    synchronized (this) {
                        if (l == null) {
                            break main;
                        }
                        if (l.isEmpty()) {
                            wait();
                            continue main;
                        }
                        packet = l.remove(0);
                    }
                    if (!packet.isDeprecated()) {
                        sendPkt(packet);
                        yield();
                    } else {
                        mylogger.info("PPSender: Marvellous - Discarting a deprecated SeCoPacket.");
                    }
                }
            } catch (Exception e) {
                mylogger.warning("PPSender: Received Exception -> Finishing thread.");
            }
            synchronized (this) {
                l = null;
            }
            mylogger.info("PPSender thread has finished.");
        }
    }

    /**
     * The instance of PPSender used to send the packet of this Packetprocessor.
     */
    private PPSender sender;

    /**
     * Initializes the PacketProcessor objects.
     * 
     * @param sock
     *            Socket for sending and receiving the packets.
     * 
     * @exception IOException
     *                An exception is thrown when problems appear.
     */
    public PacketProcessor(final Socket sock) throws IOException {

        mylogger = Logger.getLogger("isabel.seco.server.PacketProcessor");
        mylogger.finer("Creating TCP PacketProcessor object.");

        this.sock = sock;
        rudpsock = null;

        in = new BufferedInputStream(sock.getInputStream());
        out = new BufferedOutputStream(sock.getOutputStream());

        sender = new PPSender();
        sender.start();
    }

    /**
     * Initializes the PacketProcessor objects.
     *
     * @param sock
     *            Socket for sending and receiving the packets.
     *
     * @exception IOException
     *                An exception is thrown when problems appear.
     */
    public PacketProcessor(final RUDPSocket sock) throws IOException {

        mylogger = Logger.getLogger("isabel.seco.server.PacketProcessor");
        mylogger.finer("Creating UDP PacketProcessor object.");

        this.rudpsock = sock;
        this.sock = null;

        in = new BufferedInputStream(rudpsock.getInputStream());
        out = new BufferedOutputStream(rudpsock.getOutputStream());

        sender = new PPSender();
        sender.start();
    }

    /**
     * Envia el paquete dado.
     * 
     * @param packet
     *            El paquete que se quiere enviar.
     * @throws <tt>IOException</tt> Si surge algun problema de comunicacion.
     */
    public synchronized void send(SeCoPacket packet) throws IOException {

        if (invalid)
            return;

        sender.add(packet);
    }

    /**
     * Read a packet.
     * 
     * @return Returns the read packet.
     * 
     * @throws <tt>IOException</tt> thrown when problems appear.
     */
    public SeCoPacket receive() throws IOException {
        synchronized (recvLock) {

            // Leo los bytes de la cabecera y obtengo el tama#o de los datos.
            int size = SeCoPacket.HEAD_SIZE;
            int pos = 0;
            int leidos;
            byte[] head = new byte[size];
            mylogger.finest("PP.received HEAD antes. Head size = " + size);
            while (true) {
                leidos = in.read(head, pos, size);
                mylogger.finest("PP.received HEAD leidos=" + leidos);
                if (leidos == -1)
                    throw new IOException("Socket closed 1");
                if (leidos == size) {
                    break;
                }
                pos += leidos;
                size -= leidos;
            }

            mylogger.finest("PP.received HEAD size=" + size);
            SeCoPacket packet = new SeCoPacket(head);
            size = packet.getDataSize();

            // Leo los bytes restantes y devuelvo el paquete completo
            if (size > 0) {
                byte[] data = new byte[size];
                pos = 0;
                while (true) {
                    leidos = in.read(data, pos, size);
                    if (leidos == -1)
                        throw new IOException("Socket closed 2");
                    if (leidos == size) {
                        packet.setData(data);
                        break;
                    }
                    pos += leidos;
                    size -= leidos;
                }
            }
            mylogger.finest("Recibido paquete");
            return packet;
        }
    }

    /**
     * Close the connection.
     * 
     * @throws <tt>IOException</tt> thrown when problems appear.
     */
    public void close() throws IOException {
        mylogger.finest("PP Closed.");
        invalid = true;
        if (sock != null)
            sock.close();
        if (rudpsock != null) {
            mylogger.info(rudpsock.getStats());
            rudpsock.closeOut();
            rudpsock.close(2000);
        }
        sender.finish();
    }

    public synchronized void invalid() {
        invalid = true;
    }
}
