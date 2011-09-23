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
 * DownDataLink.java
 *
 */

package isabel.nereda;

import isabel.lib.Logs;

import java.net.*;
import java.io.*;
import java.util.*;
import java.util.logging.*;

/**
 *  This thread accepts data connection from other NeReDa nodes.
 *  Each connections is managed by a DataAttendant object.
 */
class DownDataLink extends Thread {


    /** 
     *  Server socket accepting node connections.
     */
    private ServerSocket ss;


    /** 
     *  Logger where logs are written. 
     */
    private Logger mylogger;

    
    /**
     *  Constructs a new object.
     *  @param dport server port.
     */
    DownDataLink (int dport) {
	
	// Inicializacion de los mensajes de log.
	mylogger = Logger.getLogger("isabel.nereda.DownDataLink");
	mylogger.fine("Creating DownDataLink object.");
	
	try {
	    ss = new ServerSocket(dport);
	    // Prints the control port in stdout.
	    mylogger.fine("Data port = "+ss.getLocalPort());
	}
	catch (Exception e) {
	    mylogger.severe("I can not create control thread: "+e.getMessage());
	}
    }

    /**
     * Thread main loop.
     */
    public void run() {
	
	Socket s = null;
	while (ss != null) {
	    try {
		s = ss.accept();
		
		mylogger.info("Acepted data conecction from "+
			      s.getInetAddress().toString());
		
		mylogger.info("Creating a new DataAttendant thread to manage the new connection.");
		new DataAttendant(s).start();
	    }
	    catch (Exception e) {
		mylogger.severe("DownDataLink thread exception: "+e.getMessage());
		try {s.close();} 
		catch (Exception e2) {}
		finally {s = null;}
	    }
	}
    }
}

//---------------------------------------









