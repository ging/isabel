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
 * Peers.java
 *
 */

package isabel.nereda;

import isabel.lib.Logs;

import java.util.*;
import java.io.*;
import java.util.logging.*;

//---------------------------------------

/**
 *  This class stores all the created DataAttendant object, which connects with all
 *  the peers sites.
 */
class Peers {


    /**
     *  Object to manage the local reflector.
     */
    private ReflectorManager refMgr;


    /**
     *  My RFB port where other peers reflector or vncclients should connect to.
     *  The "start" messages sent to registered PacketProcessors are equal to
     *  my hostname, concatenated with ":" plus the rfb port value.
     *  Note: the hostname is obtained from the PacketProcessor socket.
     */
    private int rfbPort;

 
    /** 
     *  Vector to store the DataAttendant objects.
     */
    private Vector<DataAttendant> all;


    /**
     *  Current source.
     *  It is the last source (DataAttendant or ControlAttendant) object which has 
     *  sent a start message.
     *  This value is used to synchronize a new site when it connect with me, or
     *  I connect to it.
     */
    private Attendant currentSource;


    /**
     *  Last nsec value received.
     */
    private int currentNsec;


    /** 
     *  Logger where logs are written. 
     */
    private Logger mylogger;


    /**
     *  Constructor.
     */
    Peers(int rfbPort) {
	mylogger = Logger.getLogger("isabel.nereda.Peers");
	mylogger.fine("Initializing Peers class.");
	
	this.rfbPort = rfbPort;
	all = new Vector<DataAttendant>();    

	currentSource = null;
	currentNsec   = 0;
	
	refMgr = new ReflectorManager(rfbPort);
    }


    /**
     *  Returns the currentNsec value.
     *  @return Returns the currentNsec value.
     */
    synchronized int getLastNsec() {
	return currentNsec;
    }

    /**
     * Register the DataAttendant object of a new connection.
     * @param da DataAttendant object to store.
     */
    synchronized void Add(DataAttendant da) {

	all.add(da);

	if (currentSource != null) {
	    mylogger.info("Synchronize new site with the current state.");
	    try {
		da.myPP.sendStart(rfbPort,currentNsec);
	    }
	    catch (IOException ioe) {
		mylogger.warning("I can't send start message.");
	    }
	}
    }


    /**
     * Deregister a DataAttendant object when a connection to a peer site finish.
     * @param da DataAttendant object to delete.
     */
    synchronized void Del(DataAttendant da) {

	all.remove(da);

	if (da == currentSource) {
	    currentSource = null;
	}
    }


    /**
     *  Send to all the registered DataAttendant, but source, a message with my 
     *  local reflector display, and the given nsec value.
     *  The local reflector display is created using my IP address and the
     *  refDisplayNum value.
     *  Note: my IP address to send is obtained from the DataAttendant sockets.
     *  It also starts the vnc reflector (isabel_x11vnc) program.
     *  Note: If the given nsec value is less or equal than currentNsec, then nothing is done.
     *  @param source The DataAttendant or ControlAttendant object which call
     *                to this method. The message must be send to all the 
     *                DataAttendant objects which are different of source.
     *  @param nsec Secuence number.
     *  @param display Display where the vncreflector program must connect to.
     *  @param password Password needed to connect to the given display, or null if
     *                  password is not needed.
     */
    synchronized void start(Attendant source,int nsec, String display,String password) {

	if (source != currentSource) {
	    if (nsec < currentNsec) return;
	}

	currentSource = source;
	currentNsec = nsec;
	sendStart(source);
	refMgr.setDisplay(display,password,""+nsec);
    }


    /**
     *  Send to all the registered DataAttendant, but source, a message only containing
     *  the given nsec value.
     *  Note: If the given nsec value is less or equal than currentNsec, then nothing is done.
     *  @param source The DataAttendant or ControlAttendant object which call
     *                to this method. The message must be send to all the 
     *                DataAttendant objects which are different of source.
     */
    synchronized void stop(Attendant source, int nsec) {

	if (source != currentSource) {
	    if (nsec < currentNsec) return;
	}

	currentSource = null;
	currentNsec = nsec;
	sendStop(source);
	refMgr.unsetDisplay();
    }


    /**
     *  Send the start message to all the registered DataAttendant, but source.
     *  @param source The DataAttendant or ControlAttendant object which call
     *                to this method. The message must be send to all the 
     *                DataAttendant objects which are different of source.
     */
    private void sendStart(Attendant source) {

	mylogger.fine("Sending a start message to some peer sites.");

	for (DataAttendant da : all) {

	    if (da != source) {
		try {
		    da.myPP.sendStart(rfbPort,currentNsec);
		}
		catch (IOException ioe) {
		    mylogger.warning("I can't send start message.");
		}
	    }
	}
    }


    /**
     *  Send the stop message to all the registered DataAttendant, but source.
     *  @param source The DataAttendant or ControlAttendant object which call
     *                to this method. The message must be send to all the 
     *                DataAttendant objects which are different of source.
     */
    private void sendStop(Attendant source) {

	mylogger.fine("Sending a stop message to some peer sites.");

	for (DataAttendant da : all) {

	    if (da != source) {
		try {
		    da.myPP.sendStop(currentNsec);
		}
		catch (IOException ioe) {
		    mylogger.warning("I can't send stop message.");
		}
	    }
	}
    }
}

//---------------------------------------



