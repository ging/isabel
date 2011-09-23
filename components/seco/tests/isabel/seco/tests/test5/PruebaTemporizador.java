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
import java.util.logging.*;
import java.io.*;

/**
 */
public class PruebaTemporizador extends Thread {

	private static final int SECO_PORT = 53023;
	
	private PruebaTemporizador(String arg){
		
		int clientes = Integer.parseInt(arg);
		
		final Logger mylogger = Logger.getLogger("isabel.seco.tests.test5");
		JavaDispatcher jDispatcher = JavaDispatcher.getDispatcher("Temporizador");
		
		Network net = null;
		try {
			
				net = Network.createNetwork("Temporizador", "localhost", SECO_PORT,
						new JavaMarshaller());

			net.addNetworkListener(jDispatcher);
			System.out.println("he conseguido añadir el listener");
		} catch (IOException ioe) {
			mylogger.severe("I can't create Network object: "
					+ ioe.getMessage());
			System.exit(1);
		}
		
		final Network fn = net;	
		ListenerTemporizador tempo = new ListenerTemporizador(clientes);
		jDispatcher.addDestiny(TmpMsg.class,tempo);
		
		try {
			net.joinGroup("Tiempo");
			mylogger.info("Me he unido al grupo Tiempo");

		} catch (Exception e) {
			mylogger.severe("No he podido unirme a los grupos");
		}


	}

	public static void main (String []args) {
		
		new PruebaTemporizador(args[0]);
		
		
	}
}