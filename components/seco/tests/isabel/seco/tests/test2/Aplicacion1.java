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
 * Test Paquetes Seco Envía mensajes a Aplicacion2 preguntando por la suma de 2
 * números. 1+3 sin sincronismo y 1+5 con sincronismo.
 * 
 * Contesta a los mensajes de petición de identificación de Aplicacion3, con su
 * id de cliente seco.
 * 
 * @author José Carlos del Valle
 */
public class Aplicacion1 extends Thread {

	private static final int SECO_PORT = 53023;
	
	private FileWriter escribe;
	
	protected Aplicacion1(FileWriter escribe){
		this.escribe=escribe;
	}
	
	public void run() {
		final Logger mylogger = Logger.getLogger("isabel.seco.tests.test2");
		JavaDispatcher jDispatcher = JavaDispatcher.getDispatcher("app1");
		
		
		Network net = null;
		try {
			net = Network.createNetwork("app1", "localhost", SECO_PORT,
						new JavaMarshaller());
			net.addNetworkListener(jDispatcher);
			System.out.println("he conseguido añadir el listener");
		} catch (IOException ioe) {
			mylogger.severe("I can't create Network object: "
					+ ioe.getMessage());
			System.exit(1);
		}

		try {
			net.joinGroup("pregunta");
			mylogger.info("Me he unido al grupo -pregunta- general");

		} catch (Exception e) {
			mylogger.severe("No he podido unirme a los grupos");
		}
		
		String id = net.getClientID();

		
		SumaMsg suma1 = new SumaMsg("GRUPO LOCAL del Root: Pregunta", 1, 1, id, "Aplicación 1");
		SumaMsg suma2 = new SumaMsg("GRUPO LOCAL del Root: Pregunta", 1, 10, id, "Aplicación 1");
		
		jDispatcher.addDestiny(RespuestaMsg.class,
				new TestDispacherListener(1, escribe));
				
		final Network fn = net;
		
		jDispatcher.addDestiny(HelloMsg.class,
				new JavaDispatcherListener() {
					public void processMessage(JavaMessage msg) {
						HelloMsg h = (HelloMsg) msg;
						try{
							mylogger.info("Recibido mensaje hello de: "+h.name+" e id: "+h.id);
						fn.sendClient(h.id, new HelloMsg("Aplicacion1",fn.getClientID()));
						} catch (Exception e) {
							mylogger.info("No puedo enviar HelloMsg");
						}
					}

				});

		try {
			net.sendLocalGroup("pregunta", suma1);
			mylogger.info("He enviado un mensaje sin sicronismo al grupo local pregunta del root con la suma: 1+1");
			net.sendLocalGroup("pregunta", suma2, true);
			mylogger.info("He enviado un mensaje con sicronismo al grupo local pregunta del root con la suma 1+10");
		} catch (Exception e) {
			mylogger.severe("No he podido enviar el mensaje al grupo pregunta");
		}

	}
}