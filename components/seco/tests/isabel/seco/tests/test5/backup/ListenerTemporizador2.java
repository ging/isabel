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
package isabel.seco.tests.test5.backup;

import java.io.FileWriter;
import java.io.IOException;
import java.util.Date;
import java.util.logging.Logger;

import isabel.lib.Logs;
import isabel.seco.dispatcher.javaser.JavaDispatcherListener;
import isabel.seco.network.javaser.JavaMessage;

public class ListenerTemporizador2 implements JavaDispatcherListener {
	

	
	final Logger mylogger = Logger.getLogger("isabel.seco.tests.test5");

	private long[] empieza, fin;
	
	protected void startTime(int i){
		empieza[i] = System.currentTimeMillis();
		mylogger.info("Comienza el cronometro en el milisegundo: "+empieza[i]);
	}
	
	public ListenerTemporizador2 (int i){
		mylogger.info("Creo empieza y fin de tamaño: "+i);
		empieza = new long[i];
		fin = new long [i];
	}
	
	public void processMessage(JavaMessage msg) {
		TmpMsg s = (TmpMsg) msg;
		try {
			int de = Integer.parseInt(s.getFrom());
			mylogger.info("Recibido mensaje de: "+de);
			if (s.getId()==-1){
				startTime(de);				
			} else if (s.getId()==-2){
				fin[de] = System.currentTimeMillis();
				mylogger.info("Acabo en: "+fin[de]);
				long tiempo = fin[de]-empieza[de];
				mylogger.info("Tiempo Tardado en recibir de "+de+"------------------------------> "+tiempo);
			}
			
		} catch (Exception e) {
			System.out.println("No puedo enviar IdMsg: "+e);
		}
		
	
	}

}
