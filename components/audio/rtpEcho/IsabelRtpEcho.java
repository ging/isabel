/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2011 Agora System S.A.
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

import java.net.*;
import java.io.*;

public class IsabelRtpEcho {  
    
    public static void main (String args[]) throws Exception {
	
	DatagramSocket sock = new DatagramSocket();
	
	// Print used port.
	System.out.println(sock.getLocalPort());
	System.out.flush();
	System.out.close();
	
	// Datagrama recibido:
	byte[] d = new byte[1024];
	DatagramPacket dp = new DatagramPacket(d, d.length);
	
	while (true) {

	    // Esperar un nuevo paquete de audio
	    sock.receive(dp);
	    
	    // Procesar el paquete recibido: poner SSRC igual a 4.
	    ByteArrayOutputStream baos = new ByteArrayOutputStream(4);
	    DataOutputStream dos = new DataOutputStream(baos);
	    try {
		int ssrc = 4;
		dos.writeInt(ssrc);
		dos.flush();
		
		System.arraycopy(baos.toByteArray(),0,d,8,4);
		dos.close();
	    } catch (IOException ioe) {
	    }
	    
	    // Devolver con el mismo paquete al emisor.
	    sock.send(dp);
	}
    }
}
