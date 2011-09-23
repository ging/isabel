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

public class NodoLocal {
   public static void main (String args[]) {
     try {
              InetAddress local = InetAddress.getLocalHost();
              System.out.println("La direccion local es:" + local.toString());
              System.out.println("\nY el nombre de host es : "+local.getHostName() );
     }catch(UnknownHostException ex) {
          System.err.println("Host desconocido");
           return;
       }
   }
}