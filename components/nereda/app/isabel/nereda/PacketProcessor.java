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
 * PacketProcessor.java
 *
 */

package isabel.nereda;

import isabel.lib.Logs;

import java.io.*;
import java.net.*;
import java.util.logging.*;



/**
 *  PacketProcessor is a class used to send and receive messages
 *  between peers sites.
 */
public class PacketProcessor {


    /**
     * The socket that connect to a peer site, i.e. a remote NeReDa daemon.
     */
    private Socket sock;


    /**
     * Reader built to read messages from other peer sites.
     */
    private BufferedReader in;


    /**
     * Writer built to send messages to other peer sites.
     */
    private BufferedWriter out;


    /** 
     *  Logger where logs are written. 
     */
    private Logger mylogger;


    /**
     * Initializes the PacketProcessor objects.
     *
     * @param sock Socket that connect with one peer site, i.e. anoter NeReDa node.
     *
     * @exception IOException An exception is thrown when problems appear.
     */
    public PacketProcessor(Socket sock) throws IOException {

	mylogger = Logger.getLogger("isabel.nereda.PacketProcessor");
	mylogger.fine("Creating PacketProcessor object.");

	this.sock = sock;

        in  = new BufferedReader(new InputStreamReader(sock.getInputStream()));
        out = new BufferedWriter(new OutputStreamWriter(sock.getOutputStream()));
    }
    

	
    /** 
     *  Send a message with a display name. The display name created with my hostname
     *  and the given rfb port.
     *  This display is where the local vncreflector program is accepting connections.
     *  @param rfbPort RFB port where the local vncreflector program listens.
     *  @param nsec Secuence number.
     *  @throws <tt>IOException</tt> Describes communication errors.
     */
    public synchronized void sendStart(int rfbPort,int nsec) throws IOException {

	String ha = sock.getLocalAddress().getHostAddress();

	String display;

	if (ha.indexOf(':') == -1) {
	    display = ha+":"+rfbPort;
	} else {
	    display = "["+ha+"]:"+rfbPort;
	}
	
	mylogger.fine("Sending Start: <"+nsec+" "+display+">");

	out.write(nsec+" "+display);
	out.newLine();
	out.flush(); // send now.
    }
    

    /** 
     *  Send a empty message to stop remore vncreflector programs.
     *  If the last sent mesage was also empty, nothing is sent.
     *  @param nsec Secuence number.
     *  @throws <tt>IOException</tt> Describes communication errors.
     */
    public synchronized void sendStop(int nsec) throws IOException {

	mylogger.fine("Sending Stop: <"+nsec+">");

        out.write(""+nsec);
        out.newLine();
        out.flush(); // send now.
    }
    

    /**
     * Read a message from the peer site.
     *
     * @return The received message.
     *
     * @throws <tt>IOException</tt> thrown when communications problems appear.
     */
    public String receive() throws IOException {

	String msg = in.readLine();

	mylogger.fine("Received: <"+msg+">");

	return msg;
    }
}
