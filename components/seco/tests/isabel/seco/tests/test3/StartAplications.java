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
import java.util.Date;
import java.util.logging.Logger;

public class StartAplications {
	
	
	public static void main(String args[]) {
		
		final Logger mylogger = Logger.getLogger("isabel.seco.tests.test3");

		String fichero = "ResultadosTest3.txt";
		
		if (args.length!=0){
			fichero = args[0];
		}
		
		
		FileWriter escribe = null;
		try {
			escribe = new FileWriter(fichero, true);
			Date dia = new Date();
			escribe.write("\n\n=====>>> Test done on: "+dia.toString()+" <<<=====\n");
		} catch (IOException e1) {
			mylogger.warning("Error al crear el archivo para los resultados de la prueba: "+e1);
		}
		
		mylogger.info("--> Arranco la aplicación 2");
		new Aplicacion2(escribe).start();
		
			
		try {
			Thread.sleep(300);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		mylogger.info("--> Arranco la aplicación 1");
		new Aplicacion1(escribe).start();

	}

}
