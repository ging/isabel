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
/**
 *  ClassLauncher.java
 */

package isabel.lib;

import java.lang.reflect.*;
import java.net.*;
import java.io.*;
import java.util.*;

/**
 *  This class is used to start java applications using a ClassLoader
 *  which allows to add new URL to its classpath at runtime.
 *  @author Santiago Pavon
 */
public class ClassLauncher extends URLClassLoader {


    /**
     *  Set with the already added URL where classes and resources must be
     *  searched.
     */
    private HashSet added;


    /**
     *  Internal ClassLauncher object.
     *  Use to load classes.
     */
    private static ClassLauncher loader;


    /**
     *  Constructor: don't add any URL to the ClassLoader.
     */
    private ClassLauncher() {
	super(new URL[0]);	

	added = new HashSet();
    }
    

    /**
     * Appends the specified URL to the list of URLs to search for classes and resources.
     * @param url URL to add to the ClassLoader.
     */
    public void addURL(URL url) {
	super.addURL(url);
    } 


    /**
     *  Static method, called from main, used to add new path to the ClassLoader.
     *  Path should be directory or jar names.
     *  @param path Path of the directory or jar file to add to the ClassLoader.
     */
    public void addURL(String path) {

	File f = new File(path);
	if (!f.exists()) {
	    System.err.println("ClassLauncher can not find this URL: \""+path+"\".");
	    new Exception().printStackTrace();
	    System.exit(1);
	}

	if (f.isDirectory()) {
	    path = f.getAbsolutePath()+"/";
	}

	if (added.contains(path)) return;
	added.add(path);

	try {
	    URL url = new URL("file:"+path); 
	    addURL(url);
	}
	catch (MalformedURLException mue) {
	    System.err.println("ClassLauncher can not create a malformed URL: \""+mue+"\".");
	    System.exit(1);
	}
    } 


    /**
     *  Return the specified Class.
     *  @param classname Class to look for in the ClassLoader.
     *  @throws ClassNotFoundException When the Class is not found.
     */
    public static Class getClass(String classname) throws ClassNotFoundException {

	return Class.forName(classname,true,loader);
    }


    /**
     *  Prints a usage message, and exits.
     */
    public static void usage() {
	System.err.println("Usage: Loader [-cp <path>:...] <class> [<args>]");
	System.exit(1);
    }


    /** 
     *  Main method.
     *  @param args Arguments.
     */
    public static void main(String args[]) {

	// Comprueba que se haya pasado el nombre de una clase y argumentos opcionales
	// por lo menos.
	if (args.length == 0) {
	    usage();
	}

	// usingCP is true is the option "-cp path:.." is given in the command line.
	boolean usingCP = args[0].equals("-cp");

	// Comprueba que exista valor para -cp, y que se haya pasado el nombre 
	// de una clase y argumentos opcionales por lo menos.
	if (usingCP && (args.length < 3)) {
	    usage();
	}

	loader = new ClassLauncher();

	// Cargar URLS specified with the -cp option:
	if (usingCP) {
	    StringTokenizer st = new StringTokenizer(args[1],File.pathSeparator);
	    while (st.hasMoreTokens()) {
		String path = st.nextToken();
		loader.addURL(path);
	    }
	}

	int offset = 0;
	if (usingCP) offset = 2;

	// Load the args[offset] class:
	Class topc = null;
	try {
	    topc = loader.loadClass(args[offset]);
	}
	catch (ClassNotFoundException cnfe) {
	    System.err.println("Class \""+args[offset]+"\" not found.");
	    System.exit(1);
	}	

	// Gets the main method:
	Method main = null;
	try {
	    main =  topc.getMethod("main",new Class[] {String[].class});
	}
	catch (NoSuchMethodException nsme) {
	    System.err.println("No method main defined in "+args[offset]+".");
	    System.exit(1);
	}
	catch (SecurityException se) {
	    System.err.println("Security violation: "+se);
	    System.exit(1);
	}

	// Ejecuto el metodo args[offset].main pasandole el resto de argumentos.    
	try {
	    String[] newargs = new String[args.length - 1 - offset];
	    for (int i=1 ; i<args.length - offset; i++)
		newargs[i-1] = args[i+offset];
	    main.invoke(null, (Object)new String[][] {newargs} );
	}
	catch (IllegalAccessException iae) {
	    System.err.println("IllegalAccessException:: method \""+args[offset]+"main\" is inaccessible: "+ iae);
	    System.exit(1);
	}
	catch (IllegalArgumentException iae) {
	    System.err.println("IllegalArgumentException:: "+ iae);
	    System.exit(1);
	}
	catch (InvocationTargetException ite) {
	    System.err.println("InvocationTargetException:: "+ ite +
			    " :: "+ite.getCause());
	    ite.printStackTrace();
	    System.exit(1);
	}
	catch (NullPointerException npe) {
	    System.err.println("Method \""+args[offset]+".main\" is an instance method: "+ npe);
	    System.exit(1);
	}
	catch (ExceptionInInitializerError eiie) {
	    System.err.println("ExceptionInInitializerError:: initialization of method \""+
			       args[offset]+".main\" fails: "+eiie);
	    System.exit(1);
	}
    }
}

