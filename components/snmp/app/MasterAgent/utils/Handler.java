
// ------------------ Handler.java


// This program is free software; you can redistribute it and/or
// modify it under the terms of the Affero GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details (cf. file COPYING).


import java.net.*;
import java.io.*;

import sck.*;

/** A small app which could help you to debug snmp apps or handle traps.
 * Listens for snmp messages arriving at a specified port (first argument :
 * java SnmpMessageHandler 161). By default, port is 162 (standard trap port). 
 */

 public class Handler{
  private static int port = 161; // Listens to the well-known SNMP port.
  
  /*public static void main(String[] args) {
    if (args.length!=0)
      port = java.lang.Integer.valueOf(args[0]).intValue();
      listen(port);
  }*/
  
  
  public static void listen(DatagramSocket sock){
    try {      
      //DatagramSocket sock = new DatagramSocket(port);
      //while (true){
        byte[] b = new byte[1024];
        DatagramPacket p = new DatagramPacket(b,b.length);

        sock.receive(p); // block 

        ByteArrayInputStream ber = new ByteArrayInputStream(b,1,p.getLength()-1); // without tag, like all smi object !
        Message m = new Message(ber);

        System.out.println("Message received at : " + new java.util.Date());
        StringBuffer buf = new StringBuffer();
        m.println("",buf);
        System.out.println(buf);        
      //}
    } 
    catch (Exception e){
        System.err.println(e);
        System.exit(1);
    }
  }  
}
