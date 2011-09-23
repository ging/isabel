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
import java.net.InetAddress;
import java.io.*;

public class Client {
    public static void main(String[] args) throws Exception {
        RUDPSocket socket = new RUDPSocket(InetAddress.getLocalHost(), 25000);
        InputStream is = socket.getInputStream();
        OutputStream os = socket.getOutputStream();

        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        String line;
        int len = 0;
        byte[] buf = new byte[1000];
        System.out.print(">>");
        while((line = br.readLine()) != null) {
            if (line.equals("quit")) break;

            os.write(line.getBytes());

            int expected = line.getBytes().length;
            byte[] output = new byte[expected];
            while (expected > 0) {
                len = is.read(output, output.length - expected, expected);
                expected -=len;
            }
            System.out.println(new String(output, 0, output.length));
            System.out.print(">>");

/*
            len = is.read(buf);
            if (len == -1) break;
            System.out.println(new String(buf, 0, len));
*/
        }
        os.close();
        socket.close(5000);
    }
}
