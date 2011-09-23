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
/*
 * XlimNotifications.java
 *
 *
 *
 * Created on 17 de junio de 2004, 17:37
 */

package services.isabel.lib;


import java.io.*;

// Para el manejo de horas..
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.*; 


/**
 *
 * @author  lailoken
 */
public class XlimNotification {

  private PrintWriter tracePrinter;
  private String service;
  private SimpleDateFormat timeFormatter;
  
  private static String DEF_DATE_FORMAT = "EEE, MMM d, yyyy 'at' HH:mm:ss.SSS";
  
  private static String XLIM_NOTIF_FILE = System.getProperty("xlim.notif.file");
  public static String LINUX_DEF_FILE_NAME = Constants.XLIM_LOGS_WORK_DIR + Constants.FILE_SEPARATOR + "notif.xlim.log";
  private static String DEF_FILE_NAME = LINUX_DEF_FILE_NAME;
  public static String WINDOWS_DEF_FILE_NAME = "C:"+Constants.FILE_SEPARATOR+"windows"+Constants.FILE_SEPARATOR+"temp"+Constants.FILE_SEPARATOR+"notif.xlim.log";
  
  public void initVars (String fileName, String serviceName, String dateFormat, boolean append) throws IOException {
    tracePrinter = new PrintWriter (new BufferedWriter (new FileWriter (fileName, append)),true);
    //if (new File(fileName).exists()) tracePrinter = new PrintWriter (new BufferedWriter (new FileWriter (fileName, append)),true);
    //else tracePrinter = new PrintWriter (new BufferedWriter (new FileWriter (fileName, false)),true);
    service = serviceName;
    // Para trazas horarias....
    timeFormatter = new SimpleDateFormat(DEF_DATE_FORMAT);
  }
  
  /** Creates a new instance of XlimLogs
   * @param fileName Nombre del fichero donde queremos escribir los logs.
   * @param service Nombre del servicio que escribe el mensaje de log.
   * @param append False si queremos que no sobreescriba el fichero.
   */
  public XlimNotification(String fileName, String serviceName, boolean append) throws IOException {
    initVars(fileName, serviceName, DEF_DATE_FORMAT, append);
  }

  /** Creates a new instance of XlimLogs
   * @param fileName Nombre del fichero donde queremos escribir los logs.
   * @param append False si queremos que no sobreescriba el fichero.
   */
  public XlimNotification(String fileName, boolean append) throws IOException {
    initVars(fileName, "", DEF_DATE_FORMAT, append);
  }
  
  /** Creates a new instance of XlimLogs
   * @param fileName Nombre del fichero donde queremos escribir los logs.
   * @param service Nombre del servicio que escribe el mensaje de log.
   */
  public XlimNotification(String fileName, String serviceName) throws IOException {
    initVars(fileName, serviceName, DEF_DATE_FORMAT, true);
  }
  
  /** Creates a new instance of XlimLogs
   * @param serviceName Nombre del servicio que escribe la notificacion en fichero
   */
  public XlimNotification(String serviceName) throws IOException {
    String fileName;
    if (XLIM_NOTIF_FILE != null) fileName = XLIM_NOTIF_FILE;
    else if (System.getProperty("os.name").indexOf("Linux")!= -1) fileName = LINUX_DEF_FILE_NAME;
    else fileName = WINDOWS_DEF_FILE_NAME;
    initVars(fileName, serviceName, DEF_DATE_FORMAT, true);
  }

  /* Escribe el mensaje en el fichero de logs segun el formato especificado.
   * @param msg Mensaje a escribir en el fichero de logs.
   */  
  public void writeNotif (String msg) {
    String currentTime = "[" + timeFormatter.format(new Date(System.currentTimeMillis())) + "]";
    tracePrinter.println (currentTime + " [" + service + "]: " + msg);
  }
  
  /* Escribe el mensaje en el fichero de logs segun el formato especificado.
   * @param msg Mensaje a escribir en el fichero de logs.
   */  
  public void println (String msg) {
    writeNotif(msg); 
  }

  /* Establece el nombre del servicio que escribe el mensaje
   * @param service Nombre del servicio.
   */
  public void setService (String serviceName) {
    service = serviceName;
  }

  /* Establece el formato en que se presenta la fecha y hora
   * @param dateFormat formato en el que se presenta la fecha y hora.
   */
  public void initVars (String dateFormat) {
    timeFormatter = new SimpleDateFormat(dateFormat);
  }

}