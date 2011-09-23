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
import isabel.seco.tests.test2.HelloMsg;

import java.util.Calendar;
import java.util.Date;
import java.util.Random;
import java.util.logging.*;
import java.io.*;

/**
 * 
 * Recibe los mensajes enviados al grupo prueba y devuelve el tiempo
 * en milisegundos que tarda en recibir todos estos mensajes.
 * 
  * @author José Carlos del Valle
 * 
 */
public class PruebaReceptor extends Thread {

	private static final int SECO_PORT = 53023;
	
	private PruebaReceptor(String arg){
		
		int cliente = Integer.parseInt(arg);
		
		final Logger mylogger = Logger.getLogger("isabel.seco.tests.test5");
		JavaDispatcher jDispatcher = JavaDispatcher.getDispatcher("PruebaReceptor"+cliente);
		
		Network net = null;
		try {
			
				net = Network.createNetwork("PruebaReceptor"+cliente, "localhost", SECO_PORT,
						new JavaMarshaller());

			net.addNetworkListener(jDispatcher);
			System.out.println("he conseguido añadir el listener");
		} catch (IOException ioe) {
			mylogger.severe("I can't create Network object: "
					+ ioe.getMessage());
			System.exit(1);
		}
		
		final Network fn = net;	
		ListenerTemporizador tempo = new ListenerTemporizador();
		jDispatcher.addDestiny(TmpMsg.class,tempo);
		jDispatcher.addDestiny(IdMsg.class, new JavaDispatcherListener() {
				public void processMessage(JavaMessage msg) {}});

		
		try {
			net.joinGroup("Prueba");
			mylogger.info("Me he unido al grupo Prueba");

		} catch (Exception e) {
			mylogger.severe("No he podido unirme a los grupos");
		}


	}

	public static void main (String []args) {
		
		new PruebaReceptor(args[0]);
		
		
	}
}