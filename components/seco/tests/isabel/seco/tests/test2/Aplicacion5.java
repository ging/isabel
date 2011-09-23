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
public class Aplicacion5 extends Thread{

	private static final int SECO_PORT = 53025;
	
	private FileWriter escribe;
	
	protected Aplicacion5(FileWriter escribe){
		this.escribe=escribe;
	}

	public void run() {
		final Logger mylogger = Logger.getLogger("isabel.seco.tests.test2");
		JavaDispatcher jDispatcher = JavaDispatcher.getDispatcher("app5");

		Network net = null;
		try {
			net = Network.createNetwork("app5", "localhost", SECO_PORT,
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
			mylogger.info("Me he unido al grupo LOCAL pregunta");
			
			//net.joinGroup("pregunta");
			//mylogger.info("Me he unido al grupo -pregunta- general");

		} catch (Exception e) {
			mylogger.severe("No he podido unirme a los grupos");
		}
		
		String id = net.getClientID();
		

		jDispatcher.addDestiny(RespuestaMsg.class,
				new TestDispacherListener(5, escribe));
		
		
		jDispatcher.addDestiny(HelloMsg.class,
				new TestDispacherListener(5, escribe));


		try {
			net.sendLocalGroup("pregunta", new SumaMsg("GRUPO LOCAL del SeCo 3: Pregunta", 5, 1, id, "Aplicación 5"));
			mylogger.info("He enviado un mensaje sin sicronismo al grupo local pregunta del SeCo 3 con la suma: 5+1");

			net.sendLocalGroup("pregunta", new SumaMsg("GRUPO LOCAL del SeCo3: Pregunta", 5, 10, id, "Aplicación 5"), true);
			mylogger.info("He enviado un mensaje con sicronismo al grupo local pregunta del SeCo 3 con la suma 5+10");
			
			
			net.sendGroup("pregunta", new HelloMsg("Aplicacion5", net.getClientID()));
			mylogger.info("He enviado un mensaje de hola al grupo -pregunta- general");

		} catch (Exception e) {
			mylogger.severe("No he podido enviar el mensaje al grupo pregunta");
		}

	}
}