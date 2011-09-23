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

import isabel.lib.reliableudp.RUDPSocket;
import java.io.*;
import java.net.InetAddress;

/**
 *
 * @author Fernando Escribano
 */
public class ThreadedClient {

    private static final String FILENAME = "/file";

    public static void main(String[] args) throws Exception {
        RUDPSocket socket = new RUDPSocket(InetAddress.getLocalHost(), 25000);
        InputStream is = new BufferedInputStream(socket.getInputStream());
        OutputStream os = new BufferedOutputStream(socket.getOutputStream());

        BufferedInputStream br = new BufferedInputStream(new FileInputStream(FILENAME));
        BufferedOutputStream br2 = new BufferedOutputStream(new FileOutputStream(FILENAME+".copy"));

        new writer("escritor", br, os).start();
        new writer("lector", is, br2).start();
    }
}

class writer extends Thread {

    String name;
    InputStream in;
    OutputStream os;

    writer(String name, InputStream in, OutputStream os) {
        this.name = name;
        this.in = in;
        this.os = os;
    }

    @Override
    public void run() {
        try {
            byte[] buf = new byte[1000];
            int total = 0;
            int readed = 0;
            while((readed = in.read(buf)) > 0) {
                os.write(buf, 0, readed);
                os.flush();
                total += readed;
                //System.out.println(name+" " + readed + " bytes. Total = " + total);

                //Thread.sleep(50);
            }
            System.out.println("FIN: readed = " + readed + ". Total = " + total);
            //in.close();
            //os.close();
        }catch (Exception e) {
            e.printStackTrace();
        }
    }
}
