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
import java.net.*; 
import java.util.*; 

public class FileGarbageCollector { 
  // Lista de objetos File
  LinkedList fileCollector;
  // Varible activa/desactiva el borrado
  boolean deleteFiles;
  
  // Directorio de trabajo del XLIM
  private static final String XLIM_LOGS_WORK_DIR = System.getProperty("user.home") + Constants.FILE_SEPARATOR +".xlim"+Constants.FILE_SEPARATOR +"logs"+Constants.FILE_SEPARATOR;
  private static final String TRACE_FILE_NAME = XLIM_LOGS_WORK_DIR + Constants.FILE_SEPARATOR + "filegarbagecollector.log";

/**
 * Esta clase permite llevar un control de los ficheros temporales que se utilizan
 * para guardar informacion durante la ejecucion de un programa. Permite borrar 
 * todos los ficheros que se han ido a�adiendo al recolector.
 * @Author lailoken
 */
  public FileGarbageCollector () {
    try {
      deleteFiles = true;
      fileCollector = new LinkedList();
      PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (TRACE_FILE_NAME)),true);
    } catch (Exception e) {
    }
  }
  
  
  /**
   * Permite activar/desactivar el borrado de los ficheros que se encuentren en el collector
   * delete:  - true: los ficheros del recolector se borran al hacer cleanGarbage (por defecto)
   *          - false: aunque se haga cleanGarbage no se borran los ficheros.
   */
  public void setDeleteGarbage (boolean delete) {
    deleteFiles = delete;
  }
  
  /**
   * A�ade un objeto File al recolector de ficheros basura
   * File file: objeto File que queremos a�adir al recolector
   */
  public void addFileToCollector (File file) {
    try {
      // Especificamos por donde sacamos las trazas.
      PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (TRACE_FILE_NAME,true)),true);
      outTraceFile.println ("A�adiendo Objeto a la cola..."+file.getAbsolutePath());
      outTraceFile.println ("Hay " + fileCollector.size() + " objetos en el recolector de basura.");
      fileCollector.add (file);
      outTraceFile.println ("Hay " + fileCollector.size() + " objetos en el recolector de basura.");
    } catch (Exception e) {
    }
  }

  /**
   * A�ade un objeto File al recolector de ficheros basura
   * String fileName: Nombre del fichero que queremos a�adir al recolector
   */
  public void addFileToCollector (String fileName) {
    try {
      // Especificamos por donde sacamos las trazas.
      PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (TRACE_FILE_NAME,true)),true);
      outTraceFile.println ("A�adiendo Objeto a la cola..."+fileName);
      outTraceFile.println ("Hay " + fileCollector.size() + " objetos en el recolector de basura.");
      fileCollector.add (new File(fileName));
      outTraceFile.println ("Hay " + fileCollector.size() + " objetos en el recolector de basura.");
    } catch (Exception e) {
    }
  }

  /**
   * Elimina un objeto File del recolector de ficheros basura
   * File file: objeto a eliminar de la lista.
   */
   public void removeFileFromCollector (File file) {
    try {
      // Especificamos por donde sacamos las trazas.
      PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (TRACE_FILE_NAME,true)),true);
      outTraceFile.println ("Eliminando Objeto de la cola..."+file.getAbsolutePath());
      outTraceFile.println ("Hay " + fileCollector.size() + " objetos en el recolector de basura.");
      fileCollector.remove (file);
      outTraceFile.println ("Hay " + fileCollector.size() + " objetos en el recolector de basura.");
    } catch (Exception e) {
    }
   }

  /**
   * Elimina un objeto File dado por el nombre fileName del recolector de ficheros basura
   * File file: objeto a eliminar de la lista.
   */
  public void removeFileFromCollector (String fileName) {
    try {
      PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (TRACE_FILE_NAME,true)),true);
      outTraceFile.println ("Eliminando Objeto de la cola..."+fileName);
      outTraceFile.println ("Hay " + fileCollector.size() + " objetos en el recolector de basura.");
      for (int i=0; i < fileCollector.size(); i++) {
        if ((((File)fileCollector.get(i)).getAbsolutePath()==fileName) || (((File)fileCollector.get(i)).getName()==fileName)) {
          fileCollector.remove (fileCollector.get(i));
        }
      }
      outTraceFile.println ("Hay " + fileCollector.size() + " objetos en el recolector de basura.");
    } catch (Exception e) {
      
    }
  }

  /**
   * Elimina todos los ficheros que se hayan a�adido al recolector de basura
   * File file: objeto File que queremos a�adir al recolector
   */
  //Modificado por david: recorria mal la lista de ficheros
  public void cleanGarbage () {
    PrintWriter outTraceFile = null;
    if (deleteFiles) {
      try {
        // Especificamos por donde sacamos las trazas.
        outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (TRACE_FILE_NAME,true)),true);
        for (int i=0; i < fileCollector.size(); i++) {
          outTraceFile.println ("Trabajando con objeto File del fichero: " + ((File)fileCollector.get(i)).getAbsolutePath());
          if (((File)fileCollector.get(i)).isDirectory()) { // directorio
            // Primero borramos todos ficheros de dentro
            File[] fileList = ((File)fileCollector.get(i)).listFiles();
            for (int j=0;j<fileList.length;j++) {
              outTraceFile.println ("Borrando fichero " + fileList[j].getAbsolutePath() +" dentro de directorio..." + ((File)fileCollector.get(i)).getAbsolutePath());
              fileList[j].delete();
            }
            // Y luego el directorio
            outTraceFile.println ("Borrando directorio..." + ((File)fileCollector.get(i)).getAbsolutePath());
            ((File)fileCollector.get(i)).delete();
          } else { // fichero
            ((File)fileCollector.get(i)).delete();
            outTraceFile.println ("Borrando fichero..." + ((File)fileCollector.get(i)).getAbsolutePath());
          }
          // y lo quitamos de la linkedList => Bravo lailo!!!
          //fileCollector.remove(fileCollector.get(i));
        }
        fileCollector = new LinkedList();
      } catch (IOException ioe) {
        System.out.println ("You shouldn't see this....Congratulations!!");
      } catch (Exception e){
        outTraceFile.println ("Problems cleaning garbage.. tamos de basura hasta el cuello.");
      }
    }
  }
}