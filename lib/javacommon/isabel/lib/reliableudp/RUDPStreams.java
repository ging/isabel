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
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * This class provides InputStream/OutputStream for RUDP sockets
 * @author Fernando Escribano
 */
class RUDPStreams {

    /**
     * Maximum packet size
     */
    public static final int PACKET_SIZE = 1024;

    /**
     * The RUDP socket
     */
    private RUDPSocket socket;

    private Logger logger = Logger.getLogger(RUDPStreams.class.getName());

    /**
     * Class constructor
     * @param sock
     */
    public RUDPStreams(RUDPSocket sock) {
        socket = sock;
    }

    /**
     * Get an inputstream for the socket wrapped by this object
     * @return InputStream that allows to receive data
     */
    public InputStream getInputStream() {
        return new RUDPInputStream();
    }

    /**
     * Get an OutputStream for the socket wrapped by this object
     * @return OutputStrea taht allows to send data
     */
    public OutputStream getOutputStream(){
        return new RUDPOutputStream();
    }

    private class RUDPOutputStream extends OutputStream {

        private int pos = 0;
        private byte[] buffer = new byte[PACKET_SIZE];

        @Override
        public void write(int b) throws IOException {
            buffer[pos++] = (byte)b;
            if (pos == PACKET_SIZE) {
                flush();
            }
        }

        @Override
        public void write(byte[] b) throws IOException {
            send(b, 0, b.length);
        }

        @Override
        public void write(byte[] b, int off, int len) throws IOException {
            send(b, off, len);
        }

        @Override
        public void close() throws IOException {
            socket.closeOut();
        }

        @Override
        public void flush() throws IOException {
            send(buffer, 0, pos);
            pos = 0;
        }

        private void send(byte[] b, int off, int len) throws IOException {
            try {
                int idx = off;
                int rest = len;
                while (rest > 0) {
                    int plen = (rest > PACKET_SIZE?PACKET_SIZE:rest);
                    socket.sendData(new RUDPPacket(b, idx, plen));
                    idx += plen;
                    rest -= plen;
                }
            } catch (InterruptedException ex) {
                logger.log(Level.SEVERE, null, ex);
            }
        }
    }

    private class RUDPInputStream extends InputStream {

        private int idx = 0;
        private int buffered = 0;
        private byte[] buffer;

        @Override
        public int read() throws IOException {

            int result = -1;
            // Leo el siguiente paquete si no hay nada en el buffer
            getNewData();

            if (buffer != null) {
                result = 0x00FF & buffer[idx++];
                // Si se gasto el buffer...
                if (--buffered == 0) {
                    buffer = null;
                }
            }
            return result;
        }

        @Override
        public int read(byte[] b) throws IOException {
            return read(b, 0, b.length);
        }

        @Override
        public int read(byte[] b, int off, int len) throws IOException {
            int result = -1;
            getNewData();

            if (buffer != null) {
                result = (buffered < len)?buffered:len;
                buffered -= result;
                System.arraycopy(buffer, idx, b, off, result);
                idx+=result;
                if (buffered == 0) {
                    buffer = null;
                }
            }

            return result;
        }

        @Override
        public int available() throws IOException {
        	return buffered;
        }

/*
        @Override
        public void close() throws IOException {
        }
*/
        
        private void getNewData() {
            // Leo el siguiente paquete si no hay nada en el buffer
            if (buffer == null) {
                try {
                    RUDPPacket p = socket.receive();
                    if (p != null) {
                        buffer = p.getData();
                        idx = p.getOffset();
                        buffered = p.getLength();
                    }
                } catch (InterruptedException ex) {
                    logger.log(Level.SEVERE, null, ex);
                }
            }
        }
    }
}
