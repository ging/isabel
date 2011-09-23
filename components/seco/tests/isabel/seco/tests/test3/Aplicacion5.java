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
package isabel.seco.tests.test3;

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
public class Aplicacion5 extends Thread{

	private static final int SECO_PORT = 53024;
	
	private FileWriter escribe;
	
	protected Aplicacion5(FileWriter escribe){
		this.escribe=escribe;
	}
	
	
	
	public void run() {
		final Logger mylogger = Logger.getLogger("isabel.seco.tests.test3");
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
			net.joinLocalGroup("test");
			mylogger.info("Me he unido al grupo -test- local");
			
		} catch (Exception e) {
			mylogger.severe("No he podido unirme a los grupos");
		}

		String id = net.getClientID();
		
		mylogger.info("Soy la Id: "+id);
		
		final Network fn = net;

		jDispatcher.addDestiny(TestMsg.class,
				new TestDispacherListener(5, escribe));


	}
}
