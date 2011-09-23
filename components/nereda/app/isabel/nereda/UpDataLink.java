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
 * UpDataLink.java
 */

package isabel.nereda;

import isabel.lib.Logs;

import java.net.*;
import java.io.*;
import java.util.*;
import java.util.logging.*;

/**
 *  Este thread es el encargado de conectarse con otro demonio.
 *  Cada conexion se gestiona con un objeto DataAttendant.
 */
class UpDataLink extends Thread {

    /**
     *  First port to use as local port in client TCP sockets.
     */
    private static final int FIRST_CLIENT_PORT = 51050;

    /**
     *  Last port to use as local port in client TCP sockets.
     */
    private static final int  LAST_CLIENT_PORT = 51499;


    /** Socket servidor donde se esperan las conexiones de los demonios.
     */
    private ServerSocket ss;
    
    /**
     *  Host to connect to.
     */
    private String host;

    /**
     *  Port to connect to.
     */
    private int port;

    /**
     *  Next port to try the local port in a client TCP socket.
     */
    private static int next_client_port = FIRST_CLIENT_PORT;


    /** Logger where logs are written. */
    private Logger mylogger;
    
    /**
     * Construye el objeto.
     */
    UpDataLink (String host, int port) {
	
	// Inicializacion de los mensajes de log.
	mylogger = Logger.getLogger("isabel.nereda.UpDataLink");
	mylogger.info("Creating UpDataLink object ("+host+","+port+").");

	this.host = host;
	this.port = port;
    }


    /**
     * Bucle principal del thread.
     */
    public void run() {
	
	Socket s = null;
	while (true) { // infinite loop

	    while (true) { // loop for the socket connection
		
		mylogger.finer("Trying connection to \""+host+"-"+port+"\".");
		int first_port_to_try = next_client_port;
		try {
		    s = new Socket(host,port,null,next_client_port);
		    mylogger.finer("Creata data connection to \""+host+"-"+port+"\". ");
		    
		    next_client_port++;
		    if (next_client_port > LAST_CLIENT_PORT) {
			next_client_port = FIRST_CLIENT_PORT;
		    }
		    break;
		} 
		catch (BindException be) {
		    
		    next_client_port++;
		    if (next_client_port > LAST_CLIENT_PORT) {
			next_client_port = FIRST_CLIENT_PORT;
		    }
		    
		    if (first_port_to_try == next_client_port) {
			mylogger.warning("Failure connection to \""+host+"-"+port+"\". "+
					 "Reason:: No free local ports.");
			
			try {
			    Thread.sleep(3000);
			} catch (Exception ie) {
			}
			
		    } else {
			mylogger.finest("Failure connection to \""+host+"-"+port+"\". "+
					"Reason:: Local port busy, trying another one.");
		    }
		    continue;
		}
		catch (Exception e) {
		    mylogger.warning("Failure connection to \""+host+"-"+port+"\". "+
				     "Reason:: "+e.getMessage());
		    try {
			Thread.sleep(3000);
		    } catch (Exception ie) {
		    }
		    continue;
		}
	    }
	    
	    try {
		mylogger.info("Creating thread UPDataLink::DataAttendant thread to manage the new UpData connection.");
		new DataAttendant(s).start();
	    } catch (Exception e) {
		mylogger.warning("I can not create the UpDataLink::DataAttendant thread: "+e.getMessage());
		try {
		    s.close();
		} catch (Exception ioe) {
		    mylogger.warning("Closing UpDataLink::DataAttendant socket: "+ioe.getMessage());
		}
	    }
	    
	    try {
		synchronized(s) {
		    if ( ! s.isClosed()) 
			s.wait();
		}
	    } catch (Exception ioe) {
		mylogger.warning("Closing UpDataLink::DataAttendant socket: "+ioe.getMessage());
	    }
	    
	    s = null;
	}   
    }
}

//---------------------------------------









