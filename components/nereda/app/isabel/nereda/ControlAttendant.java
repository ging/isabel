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
 * ControlAttendant.java
 *
 */

package isabel.nereda;

import isabel.lib.Logs;

import java.net.*;
import java.io.*;
import java.util.*;
import java.util.logging.*;

import java.awt.event.*;

import javax.swing.Timer;

//---------------------------------------

/**
 *  ControlAttendant objects are threads used to manage the control (tcp) sockets.
 */
class ControlAttendant extends Thread implements Attendant {
    

    /** 
     *  Socket to the control client.
     */
    private Socket s;


    /**
     * Reader built to read commands from the control client.
     */
    private BufferedReader in;


    /**
     * Writer built to send responses to the control client.
     */
    private BufferedWriter out;


    /** 
     *  Logger where logs are written. 
     */
    private Logger mylogger;


    //---------------------------------------

    /**
     *  Construct the object to attend one control client.
     * @param s socket to the control client.
     * @throws Exception 
     */ 
    public ControlAttendant(Socket s) throws Exception {

	// Initialize logs messages.
	mylogger = Logger.getLogger("isabel.nereda.ControlAttendant");
	mylogger.info("Creating ControlAttendant object.");

	this.s  = s;

        in  = new BufferedReader(new InputStreamReader(s.getInputStream()));
        out = new BufferedWriter(new OutputStreamWriter(s.getOutputStream()));

    }
    
    //---------------------------------------

    /**
     *  Manages these commands:
     *     nereda_start(nsec,display)
     *     nereda_start(nsec,display,password)
     *
     *  Note:
     *     - nsec is a secuence number to decide what is the last sent command.
     *            It is used to avoid collision in a node when several commands are
     *            invoked at the same time in different nodes.
     *            The Peers object stores the last received nsec, so new commands
     *            with a nsec values less or equal to the stored one, are ignored.
     *  @param st StringTokenized with the parameters.
     */
    private void setStart(StringTokenizer st) {
	
	try {
	    int    nsec    = Integer.parseInt(st.nextToken());
	    String display = st.nextToken();
	    String passwd  = "";
	    if (st.hasMoreTokens()) {
		passwd = st.nextToken().trim();
	    }
	    
	    mylogger.info("Received command: nereda_start("+nsec+","+display+","+passwd+")");

	    NeReDa.peers.start(this,nsec,display,passwd);
	    
	    response("OK");
	}
	catch (Exception e) {
	    response("ERROR "+e.getMessage());
	}
    }
    
    //---------------------------------------

    /**
     *  Manages the nereda_stop(nsec) command.
     *  @param st StringTokenized with the parameters.
     */
    private void setStop(StringTokenizer st) {

	try {
	    int nsec = Integer.parseInt(st.nextToken());
	    
	    mylogger.info("Received command: nereda_stop("+nsec+")");
	    
	    NeReDa.peers.stop(this,nsec);
	    
	    response("OK");
	}
	catch (Exception e) {
	    response("ERROR "+e.getMessage());
	}
    }
    
    //---------------------------------------

    /**
     *  Returns the last nsec value used.
     */
    private void getNsec() {

	int nsec =  NeReDa.peers.getLastNsec();

	response(""+nsec);
    }

    //---------------------------------------
    
    /**
     *  Manages the nereda_nop() command.
     */
    private void nop() {

	response("OK");
    }

    //---------------------------------------
    
    /**
     *  Send a response.
     *  @param res The command response to send to the control client.
     */
    private void response(String res) {

	try {
	    out.write(res);
	    out.newLine();
	    out.flush(); // send now.
	}
	catch (Exception e) {
	    mylogger.severe(e.getMessage());
	}
    }

    //---------------------------------------

    /**
     *  Main thread loop.
     *  Read client comand, manages then, and send a response.
     */
    public void run() {
	
	try {
	    while (true) {
		mylogger.finer("Waiting for client command.");
		String msg = in.readLine();

		mylogger.finer("Received command: "+msg);

		if (msg == null) 
		    break;

		StringTokenizer st = new StringTokenizer(msg,"(),");
		
		String cmd = st.nextToken();

		if (cmd.equals("nereda_nop")) {
		    nop();
		} else if (cmd.equals("nereda_start")) {
		    setStart(st);
		} else if (cmd.equals("nereda_stop")) {
		    setStop(st);
		} else if (cmd.equals("nereda_nsec")) {
		    getNsec();
		} else if (cmd.equals("nereda_quit")) {
		    response("OK");
		    System.exit(0);
		} else if (cmd.equals("nereda_bye")) {
		    response("OK");
		    break;
		} else {
		    response("ERROR");
		    mylogger.severe("Received invalid command: \""+msg+"\"");
		}
	    }
	}
	catch (Exception e) {
	    mylogger.severe(e.getMessage());
	}

	try {
	    s.close();
	}
	catch (Exception e) {
	    mylogger.severe(e.getMessage());
	}
    }

}

//---------------------------------------











