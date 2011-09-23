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
package isabel.seco.tests.test1;

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
public class Aplicacion1 {

	private static final int SECO_PORT = 53023;

	public static void main(String args[]) {
		final Logger mylogger = Logger.getLogger("isabel.seco.tests.test1");
		JavaDispatcher jDispatcher = JavaDispatcher.getDispatcher();

		Network net = null;
		try {
			if (args.length != 0) {
				int seco_port = Integer.parseInt(args[0]);
				net = Network.createNetwork("", "localhost", seco_port,
						new JavaMarshaller());

			} else {
				net = Network.createNetwork("", "localhost", SECO_PORT,
						new JavaMarshaller());

			}

			net.addNetworkListener(jDispatcher);
			System.out.println("he conseguido añadir el listener");
		} catch (IOException ioe) {
			mylogger.severe("I can't create Network object: "
					+ ioe.getMessage());
			System.exit(1);
		}

		try {
			net.joinGroup("respuesta");
			mylogger.info("Me he unido al grupo respuesta");

		} catch (Exception e) {
			mylogger.severe("No he podido unirme a los grupos");
		}

		jDispatcher.addDestiny(RespuestaMsg.class,
				new JavaDispatcherListener() {
					public void processMessage(JavaMessage msg) {
						RespuestaMsg r = (RespuestaMsg) msg;						
						System.out.println("Respuesta a la suma "+r.suma+" = " + r.resp);
					}

				});

		try {
			net.sendGroup("pregunta", new SumaMsg(1, 3));
			mylogger.info("He enviado un mensaje sin sicronismo al grupo pregunta con la suma: 1+3");

			net.sendGroup("pregunta", new SumaMsg(1, 5), true);
			mylogger.info("He enviado un mensaje con sicronismo al grupo pregunta con la suma 1+5");

		} catch (Exception e) {
			mylogger.severe("No he podido enviar el mensaje al grupo pregunta");
		}

	}
}