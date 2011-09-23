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
package isabel.seco.tests.test2;

import java.io.FileWriter;
import java.io.IOException;
import java.util.logging.Logger;

import isabel.lib.Logs;
import isabel.seco.dispatcher.javaser.JavaDispatcherListener;
import isabel.seco.network.javaser.JavaMessage;

public class TestDispacherListener implements JavaDispatcherListener {
	
	FileWriter escribe;
	
	private int aplicacion;
	
	final Logger mylogger = Logger.getLogger("isabel.seco.tests.test2");

	
	public TestDispacherListener (int aplicacion, FileWriter escribe){
		
		this.aplicacion = aplicacion;
		this.escribe = escribe;
	}
	
	public void processMessage(JavaMessage msg) {
				
		
		if (msg instanceof RespuestaMsg){
		RespuestaMsg r = (RespuestaMsg) msg;
		mylogger.info("Recibida respuesta a la suma "+r.suma+" = " + r.resp+ " de: "+r.senderName);
		
		String nombre;
		boolean resp1, resp2;
		
		switch (aplicacion){
		case 1:
			nombre = "Aplicación 1";
			resp1 = true;
			resp2 = true;
			if (r.resp==2&&resp1){
				try {
					write(nombre+": Respuesta a Mensaje Local sin sincronismo ==> OK \n");
					resp1 = false;
				} catch (IOException e1) {
					mylogger.warning(nombre+": Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
				}
			} else if (r.resp==11&&resp2){
				try {
					write(nombre+": Respuesta a Mensaje Local con sincronismo ==> OK \n");
					resp2 = false;
				} catch (IOException e1) {
					mylogger.warning(nombre+": Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
				}
			} else {
				try {
					write(nombre+": Respuesta recibida que no se debería recibir: ERROR ==> Recibida respuesta a la suma "+r.suma+" = " + r.resp+ " de: "+r.senderName+"\n");
				} catch (IOException e1) {
					mylogger.warning(nombre+": Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
				}
			}
			break;
			
		case 3:
			nombre = "Aplicación 3";
			resp1 = true;
			resp2 = true;
			if (r.resp==4&&resp1){
				try {
					write(nombre+": Respuesta a Mensaje Local sin sincronismo ==> OK \n");
					resp1 = false;
				} catch (IOException e1) {
					mylogger.warning(nombre+": Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
				}
			} else if (r.resp==13&&resp2){
				try {
					write(nombre+": Respuesta a Mensaje Local con sincronismo ==> OK \n");
					resp2 = false;
				} catch (IOException e1) {
					mylogger.warning(nombre+": Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
				}
			} else {
				try {
					write(nombre+": Respuesta recibida que no se debería recibir: ERROR ==> Recibida respuesta a la suma "+r.suma+" = " + r.resp+ " de: "+r.senderName+"\n");
				} catch (IOException e1) {
					mylogger.warning(nombre+": Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
				}
			}
			break;
			
		case 5:
			nombre = "Aplicación 5";
			resp1 = true;
			resp2 = true;
			if (r.resp==6&&resp1){
				try {
					write(nombre+": Respuesta a Mensaje Local sin sincronismo ==> OK \n");
					resp1 = false;
				} catch (IOException e1) {
					mylogger.warning(nombre+": Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
				}
			} else if (r.resp==15&&resp2){
				try {
					write(nombre+": Respuesta a Mensaje Local con sincronismo ==> OK \n");
					resp2 = false;
				} catch (IOException e1) {
					mylogger.warning(nombre+": Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
				}
			} else {
				try {
					write(nombre+": Respuesta recibida que no se debería recibir: ERROR ==> Recibida respuesta a la suma "+r.suma+" = " + r.resp+ " de: "+r.senderName+"\n");
				} catch (IOException e1) {
					mylogger.warning(nombre+": Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
				}
			}
			break;
			
		default:
			try {
				write("Respuesta recibida que no se debería recibir: ERROR ==> Recibida respuesta a la suma "+r.suma+" = " + r.resp+ " de: "+r.senderName+"\n");
			} catch (IOException e1) {
				mylogger.warning("Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
			}
			
			
		}
		}
		else if (msg instanceof HelloMsg){
			
			
		HelloMsg h = (HelloMsg) msg;
		mylogger.info("Recibida respuesta al Hello de parte de:"+ h.name);
		boolean h1 = false;
		boolean h2 = false;
		boolean h3 = false;
		boolean h4 = false;
		boolean h6 = false;
		
		if (!h1&&(h.name.equals("Aplicacion1"))){
			try {
				write("Contestación al Hello al grupo GENERAL pregunta de parte de Aplicación 1 ==> OK  \n");
				h1 = true;
			} catch (IOException e1) {
				mylogger.warning("Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
			}
		} else if (!h2&&(h.name.equals("Aplicacion2"))){
			try {
				write("Contestación al Hello al grupo GENERAL pregunta de parte de Aplicación 2 ==> OK  \n");
				h2 = true;
			} catch (IOException e1) {
				mylogger.warning("Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
			}
		} else if (!h3&&(h.name.equals("Aplicacion3"))){
			try {
				write("Contestación al Hello al grupo GENERAL pregunta de parte de Aplicación 3 ==> OK  \n");
				h3=true;
			} catch (IOException e1) {
				mylogger.warning("Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
			}
		} else if (!h4&&(h.name.equals("Aplicacion4"))){
			try {
				write("Contestación al Hello al grupo GENERAL pregunta de parte de Aplicación 4 ==> OK  \n");
				h4=true;
			} catch (IOException e1) {
				mylogger.warning("Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
			}
		} else if (!h6&&(h.name.equals("Aplicacion6"))){
			try {
				write("Contestación al Hello al grupo GENERAL pregunta de parte de Aplicación 6 ==> OK  \n");
				h6 =true;
			} catch (IOException e1) {
				mylogger.warning("Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
			}
		} else {
			try {
				write("Respuesta HelloMsg recibida que no se debería recibir de "+h.name+": ERROR ==> "+h.toString()+"\n");
			} catch (IOException e1) {
				mylogger.warning("Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
			}	
		}
		} else {
			try {
				write("Respuesta recibida que no se debería recibir ERROR ==>" + msg.toString());
			} catch (IOException e1) {
				mylogger.warning("Error al escribiendo en el archivo para los resultados de la prueba: "+e1+"\n");
			}
		}

		
		
		
		
		

	}
	
	private void write(String s) throws IOException{
		synchronized(escribe){
		escribe.write(s);
		escribe.flush();
		}
	}

}
