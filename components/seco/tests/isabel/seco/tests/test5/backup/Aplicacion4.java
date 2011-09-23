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
package isabel.seco.tests.test5.backup;

import isabel.seco.dispatcher.javaser.*;
import isabel.seco.network.*;
import isabel.seco.network.javaser.*;
import isabel.seco.tests.test2.HelloMsg;

import java.util.Calendar;
import java.util.Date;
import java.util.Random;
import java.util.logging.*;
import java.io.*;

/**
 * Test Paquetes Seco Envía mensajes a Aplicacion2 preguntando por la suma de 2
 * números. 1+3 sin sincronismo y 1+5 con sincronismo.
 * 
 * Contesta a los mensajes de petición de identificación de Aplicacion3, con su
 * id de cliente seco.
 * 
 * @author ibau
 */
public class Aplicacion4 extends Thread {

	private static final int SECO_PORT = 53023;
	
	private final String num;
	
	private Aplicacion4(String numero){
		
		this.num=numero;
		
		final Logger mylogger = Logger.getLogger("isabel.seco.tests.test5");
		JavaDispatcher jDispatcher = JavaDispatcher.getDispatcher(""+num);
		
		Network net = null;
		try {
			
				net = Network.createNetwork(""+num, "localhost", SECO_PORT,
						new JavaMarshaller());

			net.addNetworkListener(jDispatcher);
			System.out.println("he conseguido añadir el listener");
		} catch (IOException ioe) {
			mylogger.severe("I can't create Network object: "
					+ ioe.getMessage());
			System.exit(1);
		}
		
		
		final String from = net.getClientID();
		
		final long empieza = System.currentTimeMillis();
		final Network fn = net;	
		jDispatcher.addDestiny(IdMsg.class, new JavaDispatcherListener() {
			public void processMessage(JavaMessage msg) {
				IdMsg s = (IdMsg) msg;
				try {
					if (s.getId()==-1){
						if (s.getFrom().equals(from)){
							long tiempo = System.currentTimeMillis()-empieza;
							mylogger.info("Aplicación "+num+": He recibido mi último mensaje, TIEMPO------------>>>>"+tiempo);
							System.gc();
						}
					}
				} catch (Exception e) {
					System.out.println("No puedo enviar IdMsg, Excepcion: "+e);
				}
			}
		});
		
		
		try {
			int aux = Integer.parseInt(num);
			net.joinGroup("pregunta"+(aux%5));
			net.joinGroup("pregunta");

		} catch (Exception e) {
			mylogger.severe("No he podido unirme a los grupos");
		}


		try {
		    mylogger.info("Probando con muchos mensajes cortos");
		    IdMsg msg = new IdMsg(0,from);
		    Random rnd = new Random();
		    byte[] data = new byte[100];
		    rnd.nextBytes(data);
		    msg.setData(data);
		    for (int k=0; k<=4000; k++){
			net.sendGroup("pregunta"+(k%5), msg);
		    }
		    net.sendGroup("pregunta", new IdMsg(-1,from));
		    
		    
		} catch (Exception e) {
			mylogger.severe("No he podido enviar el mensaje al grupo pregunta, Excepción: "+e);
			e.printStackTrace(System.err);
		}

	}

	public static void main (String []args) {
		
		new Aplicacion4(args[0]);
		
		
	}
}