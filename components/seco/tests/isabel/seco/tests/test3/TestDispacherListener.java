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

import java.io.FileWriter;
import java.io.IOException;
import java.util.logging.Logger;

import isabel.lib.Logs;
import isabel.seco.dispatcher.javaser.JavaDispatcherListener;
import isabel.seco.network.javaser.JavaMessage;

public class TestDispacherListener implements JavaDispatcherListener {
	
	FileWriter escribe;
	
	private static int a = 0;
	
	private static int b = 0;
	
	private static int c = 20;
	
	private static int d = 30;
	
	private static boolean error = false;
	
	
	private int aplicacion;
	
	final Logger mylogger = Logger.getLogger("isabel.seco.tests.test3");

	
	public TestDispacherListener (int aplicacion, FileWriter escribe){
		
		this.aplicacion = aplicacion;
		this.escribe = escribe;
	}
	
	public void processMessage(JavaMessage msg) {

		try {
			if (msg instanceof TestMsg){
				if (aplicacion==1){
					write("==> Respuesta recibida que no se debería recibir en aplicación 1 ERROR ==>" + msg.toString()+"\n");
					error = true;
				} else if (aplicacion==2){
					TestMsg r = (TestMsg) msg;
					mylogger.info("Aplicación "+aplicacion+": Recibido TestMsg número: "+r.number+" de "+r.name);
					write("Aplicación "+aplicacion+": Recibido TestMsg número: "+r.number+" de "+r.name+" \n");
					if (!(a==r.number)){
						write(" ==> Respuesta recibida que no se debería recibir ERROR ==> Aplicacion "+aplicacion+
								" ha recibido un mensaje de número :"+r.number+" cuando debía recibir: "+a+"\n");
						error = true;
					} else {
					a++;
					}					
				} else if (aplicacion==3){
					TestMsg r = (TestMsg) msg;
					mylogger.info("Aplicación "+aplicacion+": Recibido TestMsg número: "+r.number+" de "+r.name);
					write("Aplicación "+aplicacion+": Recibido TestMsg número: "+r.number+" de "+r.name+" \n");
					if (!(b==r.number)){
						write(" ==> Respuesta recibida que no se debería recibir ERROR ==> Aplicacion "+aplicacion+
								" ha recibido un mensaje de número :"+r.number+" cuando debía recibir: "+b+"\n");
						error = true;
					} else {
					b++;
					}
				} else if (aplicacion==4){
					TestMsg r = (TestMsg) msg;
					mylogger.info("Aplicación "+aplicacion+": Recibido TestMsg número: "+r.number+" de "+r.name);
					write("Aplicación "+aplicacion+": Recibido TestMsg número: "+r.number+" de "+r.name+" \n");
					if (!(c==r.number)){
						write(" ==> Respuesta recibida que no se debería recibir ERROR ==> Aplicacion "+aplicacion+
								" ha recibido un mensaje de número :"+r.number+" cuando debía recibir: "+c+"\n");
						error = true;
					} else {
					c++;
					}					
				} else if (aplicacion==5){
					TestMsg r = (TestMsg) msg;
					mylogger.info("Aplicación "+aplicacion+": Recibido TestMsg número: "+r.number+" de "+r.name);
					write("Aplicación "+aplicacion+": Recibido TestMsg número: "+r.number+" de "+r.name+" \n");
					if (!(d==r.number)){
						write(" ==> Respuesta recibida que no se debería recibir ERROR ==> Aplicacion "+aplicacion+
								" ha recibido un mensaje de número :"+r.number+" cuando debía recibir: "+c+"\n");
						error = true;
					} else {
					d++;
					}					
				}
				if ((a==40)&&(b==40)&&(c==40)&&(d==40)&&error==false){
					write(" =/\\=> Prueba superada con éxito <=/\\= ");
					mylogger.info(" =/\\=> Prueba superada con éxito <=/\\= ");					
				}
			} else {
				write("Respuesta recibida que no se debería recibir ERROR ==>" + msg.toString());
			} 
		} catch (IOException e1) {
			mylogger.warning("Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
		}

	}
	
	private synchronized void write(String s) throws IOException{
		escribe.write(s);
		escribe.flush();
	}

}
