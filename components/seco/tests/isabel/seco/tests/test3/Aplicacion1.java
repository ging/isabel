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
 * 
 * @author José Carlos del Valle
 */
public class Aplicacion1 extends Thread {

	private static final int SECO_PORT = 53024;
	
	private FileWriter escribe;
	
	protected Aplicacion1(FileWriter escribe){
		this.escribe=escribe;
	}
	
	public void run() {
		final Logger mylogger = Logger.getLogger("isabel.seco.tests.test3");
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

		/*
		try {
			net.joinGroup("test");
			mylogger.info("Me he unido al grupo -test- general");

		} catch (Exception e) {
			mylogger.severe("No he podido unirme a los grupos");
		}
		*/
		
		
		jDispatcher.addDestiny(TestMsg.class,
				new TestDispacherListener(1, escribe));
		
		String id = net.getClientID();
		

		try {
			TestMsg mensajito = new TestMsg("Aplicacion1",id,0);

			
			for (int i=0;i<10;i++){
				mensajito.number = i;
				net.sendGroup("test", mensajito, false);
				net.sendLocalGroup("test", mensajito, false);
				mylogger.info("He envíado el mensaje con historia número: "+i+", al grupo test");
			}
			
			mylogger.info("--> Arranco la aplicación 3");
			new Aplicacion3(escribe).start();
			
			try {
				Thread.sleep(300);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			for (int i=10;i<20;i++){
				mensajito.number = i;
				net.sendGroup("test", mensajito, false);
				net.sendLocalGroup("test", mensajito, false);
				mylogger.info("He envíado el mensaje con historia número: "+i+", al grupo test");
			}
			
			mensajito.number = 20;
			net.sendGroup("test", mensajito, true);
			net.sendLocalGroup("test", mensajito, false);
			
			try {
				Thread.sleep(300);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			mylogger.info("--> Arranco la aplicación 4");
			new Aplicacion4(escribe).start();

			try {
				Thread.sleep(300);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			for (int i=21;i<30;i++){
				mensajito.number = i;
				net.sendGroup("test", mensajito, false);
				net.sendLocalGroup("test", mensajito, false);
				mylogger.info("He envíado el mensaje con historia número: "+i+", al grupo test");
			}
			
			mensajito.number = 30;
			net.sendGroup("test", mensajito, false);
			net.sendLocalGroup("test", mensajito, true);
			
			try {
				Thread.sleep(300);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			mylogger.info("--> Arranco la aplicación 5");
			new Aplicacion5(escribe).start();

			try {
				Thread.sleep(300);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			for (int i=31;i<40;i++){
				mensajito.number = i;
				net.sendGroup("test", mensajito, false);
				net.sendLocalGroup("test", mensajito, false);
				mylogger.info("He envíado el mensaje con historia número: "+i+", al grupo test");
			}
				
				
			
			
			
			
		} catch (Exception e) {
			mylogger.severe("No he podido enviar el mensaje al grupo pregunta");
		}

	}
}