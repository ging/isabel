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
 * AudioConfigFile.java
 *
 * Created on 10 de julio de 2003, 11:02
 */

package isabel.sam;

import java.io.*;

import java.util.LinkedHashMap;
//import java.util.Vector;
import java.util.Iterator;

/**
 * Esta clase contiene los datos del fichero de configuración de audio. Este
 * fichero está formado por lineas y cada linea tiene dos palabras, la primera 
 * corresponde al nombre de un control de audio virtual y la segunda su
 * correspondiente nombre del control del driver ALSA.
 * La clase funciona commo una tabla de hash (extiende hashmap) de forma que la
 * clave es el nombre del control virtual y asoociado a este nombre esta el
 * nombre del control real correspondiente.
 * Si las lineas empiezan por el caracter # se consideran comentarios.
 * Los espacios al principio o al final del de cada linea son ignorados.
 * @author  Fernando Escribano
 */
public class AudioConfigFile extends LinkedHashMap {

    /**
     * String that separates two fields in a line in the config field.
     */
    static final String SEPARATOR = "<NEXT>";


    /**
     *  Vector used to save the order of the defined channels.
     */ 
    //  private Vector order;


    /**
     * Construye un objeto AudioConfigFile.
     * @param file String con el path completo del fichero de configuracion.
     * @throws IOException si hay algún problema al acceder al fichero.
     */
    public AudioConfigFile(String file) throws IOException {
	//order = new Vector(5);
        loadFile(file);
    }
    
    /**
     * Lee el fichero de configuaración y carga los datos en este objeto.
     * @param file String con el path completo del fichero de configuracion.
     * @throws IOException si hay algún problema al acceder al fichero.
     */
    private void loadFile(String file) throws IOException {
        BufferedReader in  = new BufferedReader(new FileReader(file));
        String line = "";
        int lineCounter = 0;
        while ((line = in.readLine()) != null) {
            line = line.trim();
            lineCounter++;
            // Si es un comentario paso a la siguiente o linea en blanco
            // paso a la siguiente
            if (line.startsWith("#") || line.equals(""))
                continue;

            String [] st = line.split(SEPARATOR);
            // Si la linea tiene formato incorrecto saco un error y paso a la siguiente
            if (st.length != 2) {
                System.err.println("Linea " + lineCounter + " incorrecta: " + line);
                continue;
            }
            //order.add(st[0]);
            this.put(st[0], st[1]);
        }
        in.close();
    }
    
    public void save(String fileName) throws IOException {

        PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(fileName)));

        Iterator it = keySet().iterator();
        while (it.hasNext()) {
            String virtual = (String)it.next();
            out.print(virtual);
            out.print(SEPARATOR);
            out.println((String)get(virtual));
        }
/*
	for (int i=0 ; i<order.size() ; i++) {
            String virtual = (String)order.elementAt(i);
            out.print(virtual);
            out.print(SEPARATOR);
            out.println((String)get(virtual));
        }
*/  
      out.flush();
        out.close();        
    }
    
    /** 
     *  Returns a String with the virtual control names.
     *  The names are ordered like in the read configuration file.
     *  @return A String with the virtual control names, and sorted like
     *          in the loades configuration file.
    public String getKeys() {

	String res = "";
	for (int i=0 ; i<order.size() ; i++) {
	    res += "{" + (String)order.elementAt(i) + "} ";
	}
	return res;
    }
     */


    /**
     * Método de prueba de la clase.
     * @param args Argumentos de la linea de comandos. El primer argumento es el nombre
     * del fichero.
     */
    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Incorrect arguments");
            System.exit(1);
        }
        try {
            System.out.println(new AudioConfigFile(args[0]));
        }catch (IOException e) {
            System.out.println(e);
        }
    }
}
