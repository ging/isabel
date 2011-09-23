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
 * Igual que PruebaCliente pero con mensajes de sincronismo
 * 
 * 
 * @author José Carlos del Valle
 */
public class PruebaClienteSincronismo extends Thread {

	private static final int SECO_PORT = 53023;
	
	private final int num;
	
	private PruebaClienteSincronismo(String numr){
		
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
			IdMsg msg = new IdMsg(0,num);
			Random rnd = new Random();
			byte[] data = new byte[10000];
			rnd.nextBytes(data);
			msg.setData(data);
			net.sendGroup("Prueba", new TmpMsg(-1,num), false);
			for (int k=1; k<=mensajes/2; k++){
			msg.setId(k);
			net.sendGroup("Prueba", msg, false);
			}
			msg.setId((mensajes/2)+1);
			net.sendGroup("Prueba", msg, true);
			for (int k=(mensajes/2)+2; k<=mensajes; k++){
				msg.setId(k);
				net.sendGroup("Prueba", msg, false);
				}
			net.sendGroup("Prueba", new TmpMsg(-2,num), false);
			mylogger.info(num+": Envío el último paquete en: "+ System.currentTimeMillis());
			

		} catch (Exception e) {
			mylogger.severe("No he podido enviar el mensaje al grupo pregunta, Excepción: "+e);
			e.printStackTrace(System.err);
		}

	}

	public static void main (String []args) {
		
		new PruebaClienteSincronismo(args[0]);
		
		
	}
}
