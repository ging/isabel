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
 * ControlLink.java
 */

package isabel.nereda;

import isabel.lib.Logs;

import java.net.*;
import java.io.*;
import java.util.*;
import java.util.logging.*;

/**
 * Este thread es el encargado de gestionar el acceso al socket (tcp) de control textual.
 * Para cada cliente que se conecta a este socket de control, se crea un thread ControlAttendant
 * para gestionar los comandos que ese cliente envia.
 */
class ControlLink extends Thread {


    /** Socket servidor donde se esperan las conexiones de los clientes. */
    private ServerSocket ss;


    /** Logger where logs are written. */
    private Logger mylogger;


    /**
     * Construye el objeto.
     */
    ControlLink (int cport) {

	// Inicializacion de los mensajes de log.
	mylogger = Logger.getLogger("isabel.nereda.ControlLink");
	mylogger.fine("Creating ControlLink object.");

	try {
	    ss = new ServerSocket(cport);

	    mylogger.fine("Control port = "+ss.getLocalPort());
	}
	catch (Exception e) {
	    mylogger.severe("I can not create control thread: "+e.getMessage());
	}
    }

    /**
     * Bucle principal del thread.
     */
    public void run() {
	
	Socket s = null;
	while (ss != null) {
	    try {
		s = ss.accept();
		
		mylogger.info("Accepted control connection from "+
			      s.getInetAddress().toString());
		
		new ControlAttendant(s).start();
	    }
	    catch (Exception e) {
		mylogger.severe("ControlLink thread exception: "+e.getMessage());
		try {s.close();} 
		catch (Exception e2) {}
		finally {s = null;}
	    }
	}
    }
}

//---------------------------------------









