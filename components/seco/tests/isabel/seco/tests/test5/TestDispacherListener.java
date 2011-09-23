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

import java.io.FileWriter;
import java.io.IOException;
import java.util.Date;
import java.util.logging.Logger;

import isabel.lib.Logs;
import isabel.seco.dispatcher.javaser.JavaDispatcherListener;
import isabel.seco.network.javaser.JavaMessage;

public class TestDispacherListener implements JavaDispatcherListener {
	

	
	final Logger mylogger = Logger.getLogger("isabel.seco.tests.test5");

	private long empieza;
	private boolean uno;
	
	protected void startTime(){
		empieza = System.currentTimeMillis();
		mylogger.info("Comienza el cronometro1 en el milisegundo: "+empieza);
	}
	
	public TestDispacherListener (){
		uno = true;
	}
	
	public void processMessage(JavaMessage msg) {
		IdMsg s = (IdMsg) msg;
		try {
			long fin = System.currentTimeMillis();
			mylogger.info("Recibida respuesta del último mensaje de una tanda en el instante: "+fin);
			long resta = fin-empieza;
			mylogger.info("El tiempo tardado en mensajes ha sido---------->>>>>>>>>"+resta);
			uno = false;
			
			
		} catch (Exception e) {
			System.out.println("No puedo enviar IdMsg");
		}
		
	
	}

}
