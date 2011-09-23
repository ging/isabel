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
/////////////////////////////////////////////////////////////////////////
//
// $Id: Aboutserver.java 22731 2011-06-30 13:26:45Z santiago $
//
/////////////////////////////////////////////////////////////////////////

package services.isabel.services.about;

import services.isabel.services.*;

// Para el manejo de menus
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

import java.io.*;

import services.isabel.lib.*;

/**
 * Servidor del servicio About. Recibe un fichero de par�metros
 * donde se indica el programa del que se quiere saber la version. El servidor
 * ejecuta rpm -qa | grep programa y la salida la envia al cliente.
 * @Author lailoken
 */
public class Aboutserver { 

  // Fichero donde estan almacenados los parametros
  private static final String ABOUT_PARAM_FILE = "about.params";

  public static void main (String args[]) throws Exception { 

    try {
      // Parametros de entrada
      String FileInName = args[0];
      String FileOutName = args[1];

      // Leemos los parametros del fichero de entrada
      LineNumberReader paramLine = null;
      try {
        paramLine = new LineNumberReader(new FileReader(FileInName));
      } catch (FileNotFoundException e) {
        // Limpiamos
        Servutilities.writeMsgError ("File Not Found. " + FileInName,FileOutName,"ABOUT", false);
      }
      String aux = paramLine.readLine();
      String program = aux.substring(aux.indexOf(":")+1,aux.length());

      String version = Servutilities.getVersion(program);

      //lo metemos en el fichero de salida
      PrintWriter fileParam = new PrintWriter (new FileOutputStream (FileOutName),true);
      fileParam.println (version);
      fileParam.close();
      return;
    } catch (Exception ultimateE) {
      ultimateE.printStackTrace();
      Servutilities.writeMsgError ("Error running About Server: " + ultimateE.getMessage(),args[1],"( SERVER ) About",false);
    }
  }
}

