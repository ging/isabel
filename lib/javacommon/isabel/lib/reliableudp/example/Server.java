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

package isabel.lib.reliableudp.example;

import isabel.lib.reliableudp.*;
import java.io.*;

public class Server {

    static class Worker extends Thread {
        RUDPSocket socket;
        Worker(RUDPSocket sock) {
            socket = sock;
        }

        @Override
        public void run() {
            try {
                System.out.println("Conexión aceptada. Port = " + socket.getPort());
                InputStream is = socket.getInputStream();
                OutputStream os = socket.getOutputStream();
                int readed = 0;
                byte[] buf = new byte[200];
                while ((readed = is.read(buf)) != -1) {                    
                    System.out.println("recibido:" + new String(buf, 0, readed));
                    os.write(buf, 0, readed);
                }
                System.out.println("Cerrando socket");
                os.close();
                System.out.println(socket.getStats());
                socket.close(5000);
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
    }

    public static void main(String[] args) throws Exception {
        RUDPServerSocket ss = new RUDPServerSocket(25000);
        System.out.println("Creado server en el puerto:" + ss.getLocalPort());

        while (true) {
            RUDPSocket sock = ss.accept();
            new Worker(sock).start();
        }
    }
}
