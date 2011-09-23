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
package isabel.seco.tests.test5;


import isabel.seco.network.*;
import isabel.seco.network.javaser.*;
import java.util.Random;
import java.util.logging.*;
import java.io.*;

/**
 * 
 * Aplicacion que envia una serie de mensajes marcando el primero con un -1
 * y el último con un -2
 * 
 * @author José Carlos del Valle
 * 
 */
public class PruebaCliente extends Thread {

	private static final int SECO_PORT = 53023;
	
	private final int num;
	
	private PruebaCliente(String numr){
		
		this.num=Integer.parseInt(numr);
		
		final Logger mylogger = Logger.getLogger("isabel.seco.tests.test5");
		
		Network net = null;
		try {
			net = Network.createNetwork("Prueba"+num, "localhost", SECO_PORT,
						new JavaMarshaller());
			System.out.println("he conseguido añadir el listener");
		} catch (IOException ioe) {
			mylogger.severe("I can't create Network object: "
					+ ioe.getMessage());
			System.exit(1);
		}

		
		try {
			int mensajes = 20000;
			mylogger.info("Probando con "+mensajes+" mensajes");
			IdMsg msg = new IdMsg(-1,num);
			Random rnd = new Random();
			byte[] data = new byte[100];
			rnd.nextBytes(data);
			msg.setData(data);
			net.sendGroup("Prueba", new TmpMsg(-1,num));
                        for (int k=0; k<=mensajes; k++){
                            msg.setId(k);
                            net.sendGroup("Prueba", msg);
                        }
			/* Para probar con historia:
                        for (int k=0; k<=mensajes; k++){
				if (k%1000!=0){
					msg.setId(k);
					net.sendLocalGroup("Prueba", msg, false);
				} else {
					net.sendLocalGroup("Prueba", msg, true);
				}
			}
                        */
			net.sendGroup("Prueba", new TmpMsg(-2,num));
			

		} catch (Exception e) {
			mylogger.severe("No he podido enviar el mensaje al grupo pregunta, Excepción: "+e);
			e.printStackTrace(System.err);
		}

	}

	public static void main (String []args) {
		
		new PruebaCliente(args[0]);
		
		
	}
}
