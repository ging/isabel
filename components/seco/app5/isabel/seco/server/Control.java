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
 * Control.java
 */

package isabel.seco.server;

import java.net.*;
import java.util.logging.*;

/**
 * Este thread es el encargado de gestionar el acceso al socket (tcp) de control textual.
 * Para cada cliente que se conecta a este socket de control, se crea un thread ControlAttendant
 * para gestionar los comandos que ese cliente envia.
 * @author Santiago Pavon
 */
class Control extends Thread {
    
    /** 
     *  Socket servidor donde se esperan las conexiones de los clientes. 
     */
    private ServerSocket ss;
    
    /** 
     *  Logger where logs are written. 
     */
    private Logger mylogger;
    
    /**
     *  Puerto del socket (tcp) de control textual.
     */
    private int controlPort = 51011;
    
    /**
     *  Construye el objeto que atiende el socket de control.
     */
    Control (int controlPort) {
    this.controlPort = controlPort;
	
	// Inicializacion de los mensajes de log.
	mylogger = Logger.getLogger("isabel.seco.server.Control");
	mylogger.finer("Creating Control object.");

	setDaemon(true);
	
	try {
	    ss = new ServerSocket(controlPort);
	    mylogger.finest("Control port = "+ss.getLocalPort());
	}
	catch (Exception e) {
	    mylogger.severe("I can not create control thread: "+e.getMessage());
	}
    }
    
    /**
     *  Construye el objeto que atiende el socket de  con un puerto dado.
     */
    Control () {
	
    // Inicializacion de los mensajes de log.
    mylogger = Logger.getLogger("isabel.seco.server.Control");
    mylogger.finer("Creating Control object.");

    setDaemon(true);
    	
    try {
        ss = new ServerSocket(controlPort);
        mylogger.finest("Control port = "+ss.getLocalPort());
    }
    catch (Exception e) {
        mylogger.severe("I can not create control thread: "+e.getMessage());
    }
	}
    
    
    /**
     * Bucle principal del thread.
     */
    public void run() {
	
	try {
	    while (true) {
		Socket s = ss.accept();
		
		try {
		    new ControlLink(s).start();
		} catch (Exception e) {
		    mylogger.severe("I can not create ControlAttendant object: "+e.getMessage());
		}
	    }
	} catch (Exception e) {
	    mylogger.severe("Control thread stopped: "+e.getMessage());
	}
    }
}

//---------------------------------------









