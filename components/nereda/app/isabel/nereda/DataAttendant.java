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
 * DataAttendant.java
 *
 */

package isabel.nereda;

import isabel.lib.Logs;

import java.net.*;
import java.io.*;
import java.util.*;
import java.util.logging.*;


/**
 *  This class manages data connections with other peer sites.
 */
class DataAttendant extends Thread implements Attendant {

    
    /** 
     *  Socket that connect with a peer site.
     */
    private Socket sock;


    /** 
     *  Used to send and receive messages by sock.
     */
    final PacketProcessor myPP;


    /** 
     *  Logger where logs are written. 
     */
    private Logger mylogger;

    //---------------------------------------

    /**
     * Construye el objeto que gestionara la comunicacion de datos.
     * @param s Socket de conexion.
     * @throws Exception
     */
    public DataAttendant(Socket s) throws Exception {
	mylogger = Logger.getLogger("isabel.nereda.DataAttendant");
	mylogger.fine("Creating DataAttendant object.");

	sock = s;
	myPP = new PacketProcessor(sock);

	NeReDa.peers.Add(this);
    }
    
    //---------------------------------------

    /**
     * Finalizador del objeto.
     */
    public void finalize() {
	mylogger.info("Finalicing DataAttendant.");
    }

    //---------------------------------------

    /**
     * Bucle principal del thread.
     */
    public void run() {

	try {
	    while (true) {
		mylogger.finer("Waiting for a message.");
		String pkt = myPP.receive();
		
		mylogger.finer("Received message: "+pkt);
		
		if (pkt == null) 
		    break;
		
		StringTokenizer st = new StringTokenizer(pkt);
		
		int nsec = Integer.parseInt(st.nextToken());
		
		if (st.hasMoreTokens()) {
		    String disp = st.nextToken();
		    mylogger.fine("Requesting Start to Peers ("+nsec+","+disp+")");
		    NeReDa.peers.start(this,nsec,disp,null);
		} else {
		    mylogger.fine("Requesting Stop to Peers ("+nsec+")");
		    NeReDa.peers.stop(this,nsec);
		}
	    }
	} catch (Exception e) {
	    mylogger.warning("Terminated DataAttendant: "+e.getMessage());
	}


	try {
	    synchronized(sock) {
		sock.close();
		sock.notify();
	    }
	    
	} catch (Exception e) {
	    mylogger.warning("Closing DataAttendant socket: "+e.getMessage());
	}

	mylogger.fine("Deregistering");
	NeReDa.peers.Del(this);
    }
}

//---------------------------------------


