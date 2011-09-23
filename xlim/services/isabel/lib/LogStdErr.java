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
package services.isabel.lib;

import java.io.*;


public class LogStdErr {
  /**
   * Guarda la salida estandar
   */
  private PrintStream defaultErr;

  /**
   * Stream asociado al fichero donde desviamos la salida estandar
   */
  private PrintStream fileStream;
  
  /**
   * Indica si queremos borrar el fichero antes de escribir en el
   */
  private boolean delFile = true;
  
  
  /**
   * Constructor de la clase
   * @param logFile fichero donde queremos echar la salida estandar
   * @throws FileNotFoundException si hubo problemas al abrir el fichero
   */
  public LogStdErr (String logFile) throws FileNotFoundException {
    defaultErr = System.err;
    fileStream = new PrintStream(new BufferedOutputStream(new FileOutputStream(logFile,!delFile)),true);
  }


  /**
   * Constructor de la clase
   * @param logFile fichero donde queremos echar la salida estandar
   * @param deleteFile indica si queremos borrar el fichero donde vamos a guardar la salida estandar
   * @throws FileNotFoundException si hubo problemas al abrir el fichero
   */
  public LogStdErr (String logFile, boolean deleteFile) throws FileNotFoundException {
    defaultErr = System.err;
    delFile = deleteFile;
    fileStream = new PrintStream(new BufferedOutputStream(new FileOutputStream(logFile,!delFile)),true);
  }


  /**
   * Desvia la salida estandar al fichero
   */
  public void start () {
    System.setErr(fileStream);
  }


  /**
   * Desvia la salida estandar al fichero
   * @param logFile fichero donde queremos echar la salida estandar
   * @throws FileNotFoundException si hubo problemas al abrir el fichero
   */
  public void start (String logFile) throws FileNotFoundException {
    fileStream = new PrintStream(new BufferedOutputStream(new FileOutputStream(logFile,!delFile)),true);
    System.setErr(fileStream);
  }

  /**
   * Reestablece la salida estandar por defecto
   * @throws FileNotFoundException si hubo problemas al abrir el fichero
   */
  public void stop () {
    System.setErr(defaultErr);
  }
}  