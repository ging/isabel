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
 * NeReDa.java
 *
 */

package isabel.nereda;

import isabel.lib.*;

import java.net.*;
import java.util.logging.*;


/**
 *  NEt of REflector DAemons
 */
public class NeReDa {

    /** 
     *  Logger where logs are written. 
     */
    private static Logger mylogger;


    /**
     *  Peers object. 
     *  Stores the PacketProcessor objects of the peer sites.
     */
    static Peers peers;


    /**
     *  RFB port used to access to reflector daemons.
     */
    private static int rfbPort;


    /**
     *  Host where I have to connect to.
     *  When I am not the root nereda node, I must connect to this host.
     */
    private static String tohost = "";


    /**
     *  Data port used at "tohost".
     *  Default value is 53020.
     */
    private static int toport = 53020;
	

    /**
     *  Local data port.
     *  Local node use this port to accept TCP socket connections from other nodes.
     *  Default value: 53020
     */
    private static int dport = 53020;

 
    /**
     *  Port where control connections are accepted.
     *  Value -1 indicates that no control connection will be accepted.
     */
    private static int cport  = -1;

    /**
     * Check java version.
     */
    private static void checkJavaVersion() {

	String ver = System.getProperty("java.specification.version");

	if (ver.compareTo("1.4") < 0) {
	    mylogger.severe("Invalid Java version. Please install version 1.4 or later.");
	    System.exit(1);
	}
    }


    //--------------------------------------------------------


    /** 
     * Usage
     */
    private static void usage() {

	System.out.println("NeReDa usages:");
	System.out.println("");
	System.out.println("  isabel_nereda <rfbPort>");
	System.out.println("                [-tohost <host> [-toport <port>]]");
	System.out.println("                [-dport <data_port>]");
	System.out.println("                [-cport <control_port>]");
	System.out.println("");
	System.out.println("  isabel_nereda -h.");
    }


    //--------------------------------------------------------

    
    /**
     * Create the thread to manage the control connections.
     */
    public static void controlLink() {

	if (cport == -1) {
	    mylogger.info("NeReDa has no control.");
	    return;
	}

	try {
	    mylogger.info("Creating the thread to manage the control connections.");
	    mylogger.info("NeReDa control port = "+cport+".");
	    ControlLink ccc = new ControlLink(cport);
	    ccc.start();
	}
	catch (Exception e) {
	    mylogger.severe("Fatal Error: "+e.getMessage());
	}
    }


    //--------------------------------------------------------


    /**
     * Create the thread to manage down data connections.
     */
    public static void downDataLink() {
	
	try {
	    mylogger.info("Creating the thread to manage the Down Data Connections.");
	    mylogger.info("NeReDa data port = "+dport+".");
	    DownDataLink ccc = new DownDataLink(dport);
	    ccc.start();
	    
	}
	catch (Exception e) {
	    mylogger.severe("Fatal Error: "+e.getMessage());
	}
    }


    //--------------------------------------------------------


    /**
     * Create the thread to manage the up data connection.
     */
    public static void upDataLink() {
	
	if ( tohost.equals("") ) {
	    mylogger.info("This NeReDa daemon is the root node.");
	    return;
	}


	try {
	    mylogger.info("Creating the thread to manage the Up Data Connection.");
	    mylogger.info("This NeReDa daemon connect to "+tohost+"-"+toport+".");
	    UpDataLink ccc = new UpDataLink(tohost, toport);
	    ccc.start();
	    
	}
	catch (Exception e) {
	    mylogger.severe("Fatal Error: "+e.getMessage());
	}
    }


    //--------------------------------------------------------


    /**
     *  Parse the commmand line arguments.
     *
     *  @param args The command line arguments.
     */
    private static void parseArgs(String[] args) {
	
	if (args.length < 1) {
	    usage();
	    System.exit(1);
	}

	if (args[0].equals("-h")) {
	    usage();
	    System.exit(1);
	}
	
	
	// Get local reflector rfb port.
	try {
	    rfbPort = Integer.parseInt(args[0]);	    
	}
	catch (Exception exc) {
	    mylogger.severe("Bad reflector RFB port \""+args[0]+"\".");
	    System.exit(1);
	}
	mylogger.info("Local reflector RFB port = "+rfbPort);
	
	for (int i=1 ; i<args.length ; ) {
	    
	    if (args[i].equals("-tohost")) {
		if (i+1 > args.length) {
		    mylogger.warning("Incomplete list of arguments.");
		    break;
		}
		tohost = args[i+1];
		try {
		    InetAddress ia = InetAddress.getByName(tohost);
		}
		catch (UnknownHostException uhe) {
		    mylogger.severe("Unknown host \""+tohost+"\".");
		    System.exit(1);
		}
		i += 2;

	    } else if (args[i].equals("-toport")) {
		if (i+2 > args.length) {
		    mylogger.warning("Incomplete list of arguments.");
		    break;
		}
		try {
		    toport = Integer.parseInt(args[i+1]);	    
		}
		catch (Exception exc) {
		    mylogger.severe("Bad port number \""+args[i+1]+"\".");
		    System.exit(1);
		}
		i += 2;

	    } else if (args[i].equals("-dport")) {
		if (i+2 > args.length) {
		    mylogger.warning("Incomplete list of arguments.");
		    break;
		}
		try {
		    dport = Integer.parseInt(args[i+1]);	    
		}
		catch (Exception exc) {
		    mylogger.severe("Bad port number \""+args[i+1]+"\".");
		    System.exit(1);
		}
		i += 2;

	    } else if (args[i].equals("-cport")) {
		if (i+2 > args.length) {
		    mylogger.warning("Incomplete list of arguments.");
		    break;
		}
		try {
		    cport = Integer.parseInt(args[i+1]);	    
		}
		catch (Exception exc) {
		    mylogger.severe("Bad port number \""+args[i+1]+"\".");
		    System.exit(1);
		}
		i += 2;

	    } else {
		mylogger.warning("Unknown option \""+args[i]+"\".");
		i++;
	    }
	}
	
    }
    
    //--------------------------------------------------------


    /**
     *  Crea las threads que atienden las conexiones de control, y los enlaces
     *  con los demas demonios.
     *  @param args Lista de argumentos. 
     *              They can be -h to see the online help.
     *              They can be also:
     *                   <rfbPort>
     *	                 [-tohost <host> [-toport <port>]]
     *                   [-dport <data_port>]
     *                   [-cport <control_port>]
     */
    public static void main(String[] args) {

	// Inicializacion de los logs.
	Logs.configure("nereda");
	mylogger = Logger.getLogger("isabel.nereda.NeReDa");
	mylogger.info("Launching NeReDa.");
    
	checkJavaVersion();

	parseArgs(args);


	mylogger.info("Starting Peers ("+rfbPort+").");
	peers = new Peers(rfbPort);

	controlLink();

	downDataLink();

	upDataLink();
    }
}
