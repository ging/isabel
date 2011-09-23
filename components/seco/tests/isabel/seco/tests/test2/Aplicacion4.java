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
package isabel.seco.tests.test2;

import isabel.seco.dispatcher.javaser.*;
import isabel.seco.network.*;
import isabel.seco.network.javaser.*;

import java.util.logging.*;
import java.io.*;

/**
 * Test Paquetes Seco
 * 
 * Contesta a Aplicacion1 el resultado de la suma pedida.
 * 
 * Contesta a los mensajes de petición de identificación de Aplicacion3, con su
 * id de cliente seco.
 * 
 * @author José Carlos del Valle
 */
public class Aplicacion4 extends Thread{

	private static final int SECO_PORT = 53024;

	public void run(){
		final Logger mylogger = Logger.getLogger("isabel.seco.tests.test2");
		JavaDispatcher jDispatcher = JavaDispatcher.getDispatcher("app4");

		Network net = null;
		try {
				net = Network.createNetwork("app4", "localhost", SECO_PORT,
						new JavaMarshaller());
			net.addNetworkListener(jDispatcher);
			System.out.println("he conseguido añadir el listener");
		} catch (IOException ioe) {
			mylogger.severe("I can't create Network object: "
					+ ioe.getMessage());
			System.exit(1);
		}

		try {
			net.joinLocalGroup("pregunta");
			mylogger.info("Me he unido al grupo local del SeCo 2: pregunta");
			
			net.joinGroup("pregunta");
			mylogger.info("Me he unido al grupo -pregunta- general");

			
		} catch (Exception e) {
			mylogger.severe("No he podido unirme a los grupos");
		}
		
		String id = net.getClientID();
		
		mylogger.info("Soy la Id: "+id);

		final Network fn = net;

		jDispatcher.addDestiny(SumaMsg.class, new JavaDispatcherListener() {
			public void processMessage(JavaMessage msg) {
				SumaMsg s = (SumaMsg) msg;
				String suma = s.toString();
				int sol = s.s1 + s.s2;
				try {
					fn.sendClient(s.senderId, new RespuestaMsg("Aplicación 4", suma, sol));
					mylogger.info("He enviado el mensaje RespuestaMsg a la suma: "+suma+" de " +s.senderName);
				} catch (Exception e) {
					mylogger.warning("No puedo enviar RespuestaMsg");
				}
			}
		});
		

		jDispatcher.addDestiny(HelloMsg.class,
				new JavaDispatcherListener() {
					public void processMessage(JavaMessage msg) {
						HelloMsg h = (HelloMsg) msg;
						try{
							mylogger.info("Recibido mensaje hello de: "+h.name+" e id: "+h.id);
						fn.sendClient(h.id, new HelloMsg("Aplicacion4",fn.getClientID()));
						} catch (Exception e) {
							mylogger.warning("No puedo enviar HelloMsg");
						}
					}

				});


	}
}
