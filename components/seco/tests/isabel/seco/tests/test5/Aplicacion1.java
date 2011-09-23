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

import isabel.seco.dispatcher.javaser.*;
import isabel.seco.network.*;
import isabel.seco.network.javaser.*;

import java.util.Calendar;
import java.util.Date;
import java.util.Random;
import java.util.logging.*;
import java.io.*;

/**
 * Aplicación que envía muchos mensajes cortos.
 * Se une al grupo respuesta.
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
			
				net = Network.createNetwork("", "localhost", SECO_PORT,
						new JavaMarshaller());

			net.addNetworkListener(jDispatcher);
			System.out.println("he conseguido añadir el listener");
		} catch (IOException ioe) {
			mylogger.severe("I can't create Network object: "
					+ ioe.getMessage());
			System.exit(1);
		}

		
		TestDispacherListener escucha = new TestDispacherListener();
		
		
		jDispatcher.addDestiny(IdMsg.class, escucha);
		
		
		try {
			net.joinGroup("respuesta");
			mylogger.info("Me he unido al grupo respuesta");

		} catch (Exception e) {
			mylogger.severe("No he podido unirme a los grupos");
		}


		try {
			
			escucha.startTime();
			mylogger.info("Probando con muchos mensajes cortos");
			for (int i=0; i<=5000; i++){
			net.sendGroup("pregunta", new IdMsg(i));
			}
			net.sendGroup("pregunta", new IdMsg(-1));

		} catch (Exception e) {
			mylogger.severe("No he podido enviar el mensaje al grupo pregunta, Excepción: "+e);
			e.printStackTrace(System.err);
		}

	}
}