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

import java.util.logging.Logger;
import isabel.seco.dispatcher.javaser.JavaDispatcherListener;
import isabel.seco.network.javaser.JavaMessage;

/**
 * 
 * Listener que además mide tiempo entre la recepción de paquetes.
 * 
  * @author José Carlos del Valle
 * 
 */

public class ListenerTemporizador implements JavaDispatcherListener {
	

	
	final Logger mylogger = Logger.getLogger("isabel.seco.tests.test5");

	private long[] empieza, fin;
	
	private int contador, descanso;
	
	protected void startTime(int i){
		empieza[i] = System.currentTimeMillis();
		mylogger.info("Comienza el cronometro de la aplicacion "+i+" en el milisegundo: "+empieza[i]);
	}
	
	public ListenerTemporizador (){
		empieza = new long [50];
		fin = new long [50];
		contador = 0;
		descanso = 0;
	}
	
	public ListenerTemporizador (int lento){
		empieza = new long [50];
		fin = new long [50];
		contador = 0;
		descanso = lento;
		mylogger.info("----------Creo un listener lentooooo-----------");
	}
	
	public void processMessage(JavaMessage msg) {
		try {
			Thread.sleep(descanso);
			if (msg instanceof TmpMsg){
				TmpMsg s = (TmpMsg) msg;
				int de = s.getFrom();
				mylogger.info("Recibido mensaje de: "+de);
				if (s.getId()==-1){
					startTime(de);				
				} else if (s.getId()==-2){
					fin[de] = System.currentTimeMillis();
					mylogger.info("Recibido último de  "+de+" en: "+fin[de]);
					long tiempo = fin[de]-empieza[de];
					mylogger.info("Tiempo Tardado en recibir de "+de+"------------------------------> "+tiempo+"\n" +
						"Recibidos un total de "+contador+" mensajes con historia");
				} 
			} else if (msg instanceof IdMsg){
				contador++;			
			}
			
		} catch (Exception e) {
			System.out.println("No puedo enviar IdMsg: "+e);
		}
		
	
	}

}
