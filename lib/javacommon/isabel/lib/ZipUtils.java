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
 * Zip.java
 * 
 * Clase que provee metodos para comprimir y descomprimir ficheros utilizando la
 * libreria java.util.zip de sun.
 *
 * Created on 5 de agosto de 2004, 16:59
 */

package isabel.lib;

import java.util.Enumeration;
import java.util.zip.*;
import java.io.*;

/**
 *
 * @author  lailoken
 */
public class ZipUtils {
  
  public static final int NO_PATH = 0;
  public static final int RELATIVE_PATH = 1;
  public static final int ABSOLUTE_PATH = 2;

  // File separator
  public static final String FILE_SEPARATOR = System.getProperty("file.separator");  

  public static final String XLIM_WORK_DIR = System.getProperty("user.home") + FILE_SEPARATOR + ".xlim" + FILE_SEPARATOR;
  public static final String XLIM_LOGS_WORK_DIR = System.getProperty("user.home") + FILE_SEPARATOR + ".xlim"+ FILE_SEPARATOR + "logs" + FILE_SEPARATOR;
  public static final String XLIM_TMP_WORK_DIR = System.getProperty("user.home") + FILE_SEPARATOR + ".xlim"+ FILE_SEPARATOR + "tmp" + FILE_SEPARATOR;
  public static final String XLIM_CFG_WORK_DIR = System.getProperty("user.home") + FILE_SEPARATOR + ".xlim" + FILE_SEPARATOR + "config" + FILE_SEPARATOR;
  
  
 
  /**
   * Dado un path de un fichero, devuelve el nombre de ese fichero, sin los directorios que lo contienen
   * @param name el path (absoluto o relativo) del fichero
   * @return el nombre del fichero.
   *
   */
  static public String getBaseName (String name) {
    // Primero vemos cual es el separador en este path
    String separator;
    if (name.indexOf("/")!=-1) {
      separator = "/"; // Path de linux, en windows no puede haber ficheros con \. 
      return name.substring(name.lastIndexOf(separator)+1);
    } else {
      if (name.indexOf("\\")!=-1) {
        separator = "\\"; // Path de windows, en linux no puede haber nombres de ficheros con \
        return name.substring(name.lastIndexOf(separator)+1);
      } else {
        return name;
      }
    }
  }
  
  
  /**
   * Dado un path de un fichero, devuelve la ruta de ese fichero, es decir, la lista
   * de directorios en los que esta el fichero
   * @param name el el path del fichero
   * @return el rootname, es decir, la ruta para llegar al fichero.
   *
   */
  static public String getRootName (String name) {
    // Primero vemos cual es el separador en este path
    String separator;
    int cont=name.indexOf("/");
    if (cont!=-1) {
      separator = "/"; // Path de linux, en windows no puede haber ficheros con \. 
      if (cont==0) name = name.substring(1);
      return name.substring(0,name.lastIndexOf(separator)+1);
    } else {
      if (name.indexOf("\\")!=-1) {
        separator = "\\"; // Path de windows, en linux no puede haber nombres de ficheros con \
        // En windows quitamos la letra del disco donde esta el fichero. Usamos los : para quitarla
        cont = name.indexOf(":");
        if (cont !=-1) name = name.substring(cont+2);
        return name.substring(0,name.lastIndexOf(separator)+1);
      } else return "";
    }
    // Eliminamos el primer / si lo hubiese (en linux) y la letra (en windows)
    
  }
  
  /**
   * Dado un path de un fichero, devuelve el root de un fichero, es decir, "/" en linux
   * o "X:\" en windows, si es que se encuentran en el nombre dado.
   * @param name el el path del fichero
   * @return el rootname, es decir, la ruta para llegar al fichero.
   *
   */
  static public String getRoot (String name) {
    // Primero vemos cual es el separador en este path
    String separator;
    int cont=name.indexOf("/");
    if (cont!=-1) {
      separator = "/"; // Path de linux, en windows no puede haber ficheros con \. 
      if (cont==0) return "/";
      else return "";
    } else {
      if (name.indexOf("\\")!=-1) {
        separator = "\\"; // Path de windows, en linux no puede haber nombres de ficheros con \
        // En windows quitamos la letra del disco donde esta el fichero. Usamos los : para quitarla
        cont = name.indexOf(":");
        if (cont !=-1) return name.substring(0,cont+2);
        else return "";
      } else return "";
    }
  }
  

  /**
   * Introduce un fichero en el stream de zip.
   * @param file Fichero que queremos introducir en el stream
   * @param out stream de zip donde metemos el fichero que queremos comprimir
   * @Author lailoken
   */
  static private void zipOneFile (File file, ZipOutputStream out, String relativePath) throws IOException, FileNotFoundException{
    // Donde mandamos los mensajes de trazas: 
    PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (XLIM_LOGS_WORK_DIR+FILE_SEPARATOR+"zip.log",true)),true);
    if (file.isDirectory()) {
      outTraceFile.println ("Zipping..." + file.getPath());
       //Create an array with all of the files and subdirectories
       //of the current directory.
       String[] fileNamesInDir = file.list();
       if (fileNamesInDir != null) {
          //Recursively add each array entry to make sure that we get 
          //subdirectories as well as normal files in the directory.
          for (int i=0; i<fileNamesInDir.length; i++)  {
             zipOneFile(new File(file, fileNamesInDir[i]),out,relativePath);
          }
       }
    }
    //Otherwise, a file so add it as an entry to the Zip file.
    else {
      //----- For each file --
      outTraceFile.println ("Zipping..." + file.getPath());
      byte[] buf = new byte[1024];
      FileInputStream in = new FileInputStream(file);
      // Add ZIP entry to output stream.
      // Sacamos el nombre de la entry de quitarle al nombre del fichero su relativePath:
      outTraceFile.println("filePath: " + file.getPath() + "; relativePath: " + relativePath);
      String entryName;
      // Para generar el entryName hay que asegurarnos  de que le hemos dejado el path relativo pedido
      entryName = file.getPath().substring(relativePath.length(),file.getPath().length());
      // ...y de que los separadores de directorio son /
      outTraceFile.println("EntryName: " + entryName);
      entryName = entryName.replaceAll("\\\\", "/");
      outTraceFile.println("EntryName: " + entryName);
      out.putNextEntry(new ZipEntry(entryName));
      // Transfer bytes from the file to the ZIP file
      int len;
      while ((len = in.read(buf)) > 0) {
        out.write(buf, 0, len);
      }
      // Complete the entry
      out.closeEntry();
      in.close();
      //----------------------
    }
  }

  /**
   * Comprime ficheros.
   * @param zipName fichero zip donde se comprimiran los ficheros
   * @param fileNames array que contiene los paths de los ficheros que queremos comprimir
   * @param path indica si se guardan los paths de los ficheros (absolutos) o no. true para mantener el path absoluto; false para no guardar paths 
   * @param relativePaths array para indicar cual es el path relativo de cada fichero que se quiere comprimir
   * @return
   * @throws
   *
   */
  static public void zip (String zipName, String[] fileNames, boolean path)  throws IOException,FileNotFoundException {
    int pathMode;
    if (path) pathMode = ABSOLUTE_PATH;
    else pathMode = NO_PATH;
    String [] relativePaths = null;
    zip (zipName,fileNames, pathMode,relativePaths);
  }
  
  
  /**
   * Comprime ficheros.
   * @param zipName fichero zip donde se comprimiran los ficheros
   * @param fileNames array que contiene los paths de los ficheros que queremos comprimir
   * @param path indica si se guardan los paths de los ficheros (relativos o absolutos) 
   * o no. Valores posibles: NO_PATH, RELATIVE_PATH, ABSOLUTE_PATH.
   * @param relativePaths array para indicar cual es el path relativo de cada 
   * fichero que se quiere comprimir. Solo se usa cuando path=RELATIVE_PATH e 
   * indica que parte del path del fichero se eliminara al comprimirlo.
   * @throws
   *
   */
  static public void zip (String zipName, String[] fileNames, int path, String[] relativePaths) throws IOException,FileNotFoundException {
    // Create a buffer for reading the files
    // Donde mandamos los mensajes de trazas: 
    PrintWriter outTraceFile = null;
    outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (XLIM_LOGS_WORK_DIR+FILE_SEPARATOR+"zip.log", true)),true);
    outTraceFile.println("zipName: "+ zipName);
    // Create the ZIP file
    ZipOutputStream out = new ZipOutputStream(new FileOutputStream(zipName));
    // Compress the files
    for (int i=0; i<fileNames.length; i++) {
      // File asociated to this file...
      outTraceFile.println("fileNames[" +i+"]: "+ fileNames[i]);
      File file = new File(fileNames[i]);
      String relativePath = "";
      // Si no queremos guardar la ruta, el relativePath correspondiente es todo 
      // el path, menos el nombre del fichero.
      //if (path == NO_PATH) relativePath = fileNames[i].substring(0,fileNames[i].length()-getBaseName(fileNames[i]).length());
      if (path == NO_PATH) relativePath = getRoot(fileNames[i]) + getRootName(fileNames[i]);
      if (path == RELATIVE_PATH) relativePath = relativePaths[i];
      if (path == ABSOLUTE_PATH) {
        // Con esto, conseguimos que el getPath que se usa en zipOneFile de como resultado el path absoluto
        file = new File(file.getAbsolutePath());
        relativePath = getRoot(file.getAbsolutePath());
      }
      outTraceFile.println("relativePath value: "+ relativePath);
      zipOneFile (file,out,relativePath);
    }
    // Complete the ZIP file
    out.close();
  }
  
  /**
   * Descomprime ficheros en el path especificado
   * @param zipName nombre del fichero zip
   * @param root path donde se quieren guardar los ficheros descomprimidos
   * @path true si se quieren descomprimir las rutas; false si no se quieren conservar las rutas.
   * @return los paths absolutos de todos los ficheros descomprimidos
   * @throws IOException, FileNotFoundException
   */
  
  //Modificada por david ya que daba problemas de concurrencia muy extraños
  //Crea un nuevo inputstream por cada entrada en vez de un inputstream global para todas las entradas.
  public static String[] unzip (String zipName, String root, boolean path) throws IOException, FileNotFoundException{
    PrintWriter outTraceFile = null;
    outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (XLIM_LOGS_WORK_DIR+FILE_SEPARATOR+"zip.log", true)),true);

    // Si no existe el directorio donde hay que descomprimir, lo creamos...
    File rootFile = new File (root);
    rootFile.mkdirs();

    ZipFile zipFile = new ZipFile(zipName);
    String[] fileNames = new String[zipFile.size()];

    int i = 0;
    for (Enumeration e = zipFile.entries(); e.hasMoreElements();){
      // Get the next entry
    	ZipEntry entry = (ZipEntry) e.nextElement();
      //System.out.println("Nombre de la entry: " + entry.getName());
      String entryName;
      //System.out.println("entry " + entry);
 
      if (!path) {
        entryName = getBaseName(entry.getName());
      } else {
        // Nos aseguramos de que existen los directorios absolutos
        try {
          entryName = entry.getName();
          outTraceFile.println("EntryName: " + entryName);
          // Ponemos el entryName con los separadores del sistema operativo
          if (!FILE_SEPARATOR.equals("/")) {
            // Estamos comprimiendo en un windows, asi que hay que invertir las barras
            entryName = entryName.replaceAll("/", "\\\\"); // Hay que poner \\\\
                      // .. porque \ es caracter de escape tanto en expresiones regulares como en strings
                      // .. por tanto al convertir \\\\ en expresion regular queda \\ y al usarlo en string queda \
            outTraceFile.println("EntryName: " + entryName);
          } else {
            // Estamos descomprimiendo en un Linux, asi que no hay que cambiar nada del entryName
          }
          String dirName = root+FILE_SEPARATOR+getRootName(entryName);
          outTraceFile.println("Directorio a comprobar: " + dirName);
          File dirNameFile = new File(dirName);
          dirNameFile.mkdirs();
        } catch (Exception ex) {
        }
        entryName = entry.getName();
      }
      // Metemos el nombre del fichero descomprimido en el array que vamosa devolver
      outTraceFile.println("Fichero a descomprimir. " + entryName);
      fileNames[i] = entryName;
      //System.out.println("Donde copiamos el fichero descomprimido: "+entryName);
      // Open the output file
      String outFilename = root+FILE_SEPARATOR+entryName;
      OutputStream out = new FileOutputStream(outFilename);
      // Transfer bytes from the ZIP file to the output file
      byte[] buf = new byte[1024];
      int len;
      //Creamos un input stream de la entrada que vamos a descomprimir y la descomprimimos
      InputStream in = zipFile.getInputStream(entry);
      while ((len = in.read(buf)) > 0) {
        out.write(buf, 0, len);
      }
      // Close the streams
      out.close();
      outTraceFile.println("Fichero descomprimido.");
    i++;  
    }
    //in.close();
    return fileNames;
  }

  /**
   * @param args the command line arguments
   */
  public static void main(String[] args) {
    if (args[0].equals("--help")) {
      System.out.println("Usage: zipFile file2zip1[,file2zip2,file2zip3...] relativePath1[,relativePath2,relativePath3..] 0/1/2(NO_PATH/RELATIVE_PATH/ABSOLUTE_PATH)");
    }
    if (args.length==4)
      try {
//        System.out.println("ROOT " + getRoot(args[1])+  "; ROOTNAME: " + getRootName(args[1]) +"; BASENAME: " + getBaseName(args[1]));
        String[] filesToZip = args[1].split(",");
        String[] relativePaths = args[2].split(",");
        int path = new Integer(args[3]).intValue();
        ZipUtils.zip(args[0],filesToZip,path,relativePaths);
      } catch (Exception e) {
        System.out.println ("Excepcion capturada: " + e);
      }
    if (args.length==3) {
      try {
        boolean path;
        if (args[2].equals("true")) {
          path = true;
        } else {
          path = false;
        }
        ZipUtils.unzip(args[0],args[1],path);
      } catch (Exception e) {
        System.out.println ("Excepcion capturada: " + e);
      }
    }
    if (args.length==1) {
      try {
        System.out.println("ROOT " + getRoot(args[0])+  "; ROOTNAME: " + getRootName(args[0]) +"; BASENAME: " + getBaseName(args[0]));
        File file = new File(args[0]);
        System.out.println("Absolute Path: " + file.getAbsolutePath());
        System.out.println("Canonical Path: " + file.getCanonicalPath());
        System.out.println("Name: " + file.getName());
        System.out.println("Path: " + file.getPath());
      } catch (Exception e) {
        System.out.println ("Excepcion capturada: " + e);
      }
    }
  }
  
}
