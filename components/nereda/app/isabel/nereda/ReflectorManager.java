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
 *  ReflectorManager.java
 *
 */

package isabel.nereda;

import isabel.lib.tasks.*;
import isabel.lib.Logs;

import java.util.logging.*;

import java.io.*;

import java.net.InetAddress;
import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.UnknownHostException;

import java.util.HashMap;


/**
 *  This class starts and stops the vnc reflector program.
 */
class ReflectorManager {
    
    /**
     *  Task used to run the isabel_x11vnc (vnc reflector) program.
     */
    private Task task_x11vnc;

    /**
     *  Task used to run the isabel_inet4to6 () program.
     */
    private Task task_inet4to6;


    /**
     *  Current display where the local vnc refector connect to.
     */
    private String display;


    /**
     *  Current password used by the local vnc refector to connect to the current display.
     */
    private String password;


    /**
     *  The port where the vnc reflector accept client connections.
     */
    private int rfbPort;    


    /**
     *  Temporal file to save the connection password value.
     */
    private File tmpCCFile;


    /** 
     *  Logger where logs are written. 
     */
    private Logger mylogger;


    /**
     * Reflector log filename.
     */
    private String reflectorLogsFilename;


    /**
     *  Create a ReflectorManager object.
     *  @param rfbPort Port where the vnc reflector program accept connections.
     */
    ReflectorManager(int rfbPort) {

	// Inicializacion de los mensajes de log.
	mylogger = Logger.getLogger("isabel.nereda.ReflectorManager");
	mylogger.info("Creating ReflectorManager object.");

	task_x11vnc = null;
	task_inet4to6 = null;

	display  = "";
	password = "";

	this.rfbPort = rfbPort;

	if (System.getProperty("user.name").equals("ubuntu")) {
	    reflectorLogsFilename = "/dev/null";
	} else {
	    String logsDir = System.getenv("ISABEL_SESSION_LOGS_DIR");
	    if (logsDir == null) {
		logsDir = "/tmp";
	    }
	    reflectorLogsFilename = logsDir + "/nereda_x11vnc.log";
	    
	    File logsFile = new File(reflectorLogsFilename);
	    try {
		logsFile.delete();
	    } catch (Exception e) {
		mylogger.warning("I can't delete reflector logs file: \""+ reflectorLogsFilename +"\". "+
				 "I get: "+e.getMessage());
		
	    }		
	}
    }
    
    
    /**
     *  Stops the vnc reflector program launched by me.
     */
    synchronized void unsetDisplay() {
	
	mylogger.info("Stopping vnc reflector.");

	this.display  = "";
	this.password = "";
	
	killVncReflectorTasks();

	if (tmpCCFile != null) {
	    tmpCCFile.delete();
	    tmpCCFile = null;
	}
    }


    /**
     *  Starts a vnc reflector that connect to the given display.
     *  @param display Display where the vnc reflector program must connect to.
     *  @param password Password needed by my reflector program to connect to the remote display
     *  @param rfbPasswd Password used by my clients to connect to my reflector program.
     */
    synchronized void setDisplay(String display, String password, String rfbPasswd) {

	mylogger.info("Setting connection to "+display);
	
	if (password == null) {
	    password = "";
	}
	
	// Starts a new vnc reflector (isabel_x11vnc) program 
	
	this.display  = display;
	this.password = password;
	
	killVncReflectorTasks();
	
	try {

	    // Temporal file to save the connection password value.
	    String tmpCCFilename;    
	    try {
		String tmpDir = System.getenv("ISABEL_TMP_DIR");
		if (tmpDir == null) {
		    tmpDir = "/tmp";
		}

		if (tmpCCFile != null) {
		    tmpCCFile.delete();
		}
		
		tmpCCFile = File.createTempFile("ref_","_pw",new File(tmpDir));
		tmpCCFile.deleteOnExit();
		tmpCCFilename = tmpCCFile.getAbsolutePath();
	    }
	    catch (IOException ioe) {
		mylogger.severe("I can't create a temporal file to start isabel_x11vnc. "+
				"I get: "+ioe.getMessage());
		tmpCCFilename = "/tmp/ref_0_pw";
	    }
	    PrintWriter pw = new PrintWriter(new FileWriter(tmpCCFilename));
	    pw.println(rfbPasswd);
	    pw.close();
	   
	    // If display contains an IPv6 address then isabel_inet4to6 is launched.
	    // isabel_x11vnc can only reflect to IPv4 addresses.

	    String[] ihd = analizeDisplay(display);

	    String reflectDisplay;

	    if (ihd != null && ihd[0].equals("IPv6")) {

		reflectDisplay = "127.0.0.1:51777";

		// If the display port is lesser than 200, add 5900.
		int dport = Integer.parseInt(ihd[2]);
		if (dport<200) dport += 5900;

		task_inet4to6 = new Task("inet4to6 reflector",
					 new String[] {
					     "isabel_inet4to6",
					     "51777",
					     ihd[1]+":"+dport
					 },
					 true);
		task_inet4to6.start();
	    } else {
		reflectDisplay = display;
	    }

 
	    mylogger.info("Starting a new vnc reflector (isabel_x11vnc) instance.");
	    
	    HashMap<String,String> env = new HashMap<String,String>();
	    env.put("X11VNC_REFLECT_PASSWORD",password);
	    
	    task_x11vnc = new Task("x11vnc reflector",
			    new String[] {
				"isabel_x11vnc",
				"-reflect", reflectDisplay,
				"-rfbport", ""+rfbPort,
				"-passwdfile", tmpCCFilename,
				"-forever",
				"-nopw",
				"-norc",
				"-6",
				"-reopen",
				"-solid",
				"-quiet",
				"-nodragging",
				"-wmdt", "gnome",
				"-nodpms",
				"-threads",
				"-sleepin","0",
				"-defer","5",
				"-wait","20",
				"-o", reflectorLogsFilename
			    },
			    true,
			    env);
	    
	    task_x11vnc.start();
	} catch (Exception e) {
	    mylogger.severe("I can't start vnc reflector (isabel_x11vnc) program to connect to "+display+". "+
			    "I get: "+e.getMessage());
	    this.display  = "";
	    this.password = "";
	    killVncReflectorTasks();
	}
    }

    /**
     * Split the given display value into hostname and display number.
     * @param display A value with this format hostname:number.
     *                hostname is and IPv4 or IPv6 hostname, and it is optional.
     *                :display is also optional (default to 5900).
     * @return An array with the hostname and the display number.
     */
    private String[] splitHostnameAndDisplayNumber(String display) {
	
	int pos = display.lastIndexOf(":");
	
	if (pos == -1) {
	    return new String[] { display, "5900" };
	}
	
	try {
	    return new String[] { display.substring(0, pos), display.substring(pos+1) };
	} catch (Exception e) {
	}
	return null;
    }
    
    
    /**
     * Analyse the given display value to obtain the hostname and the display number. It also
     * determines if the hostname is an IPv4 or IPv6 address.
     * @param display A value with this format hostname:number.
     *                hostname is and IPv4 or IPv6 hostname, and it is optional.
     *                :display is also optional (default to 5900).
     * @return An array with three values: "IPv4" or "IPv6", a hostname and a display number.
     *         It returns null if the given display value can not be analysed.
     */
    private String[] analizeDisplay(String display) {
	
	InetAddress ia = null;
	
	String hostname = null;
	String displayNumber = null;
	
	try {
	    ia = InetAddress.getByName(display);
	    hostname = ia.getHostAddress();	
	    displayNumber = "5900";
	} catch (UnknownHostException e) {
	    // Removing display number
	    String[] hn = splitHostnameAndDisplayNumber(display);
	    if (hn == null) {
		return null;
	    }
	    hostname = hn[0];
	    displayNumber = hn[1];

	    
	    try {
		ia = InetAddress.getByName(hostname);				
		hostname = ia.getHostAddress();
	    } catch (UnknownHostException e2) {
		return null;
	    }
	}
	
	String [] res = new String[3];
	
	if (ia.getClass() == Inet4Address.class) {
	    res[0] = "IPv4";
	} else if (ia.getClass() == Inet6Address.class) {
	    res[0] = "IPv6";
	}
	
	res[1] = hostname;
	res[2] = displayNumber;
	
	return res;		
    }
    
    
    /**
     * Kills the isabel_x11vnc (vnc reflector) and isabel_inet4to6  tasks.
     */
    private void killVncReflectorTasks() {

	if (task_x11vnc != null) {
	    task_x11vnc.kill();
	    task_x11vnc = null;
	    try {
		Runtime.getRuntime().exec("killall -q isabel_x11vnc");
	    } catch (Exception rte) {
	    }
	}

	if (task_inet4to6 != null) {
	    task_inet4to6.kill();
	    task_inet4to6 = null;
	    try {
		Runtime.getRuntime().exec("killall -q isabel_inet4to6");
	    } catch (Exception rte) {
	    }
	}
    }
}
