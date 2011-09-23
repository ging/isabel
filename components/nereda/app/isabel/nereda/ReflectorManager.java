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

/**
 *  This class starts and stops the vncreflector program.
 */
class ReflectorManager {
    
    /**
     *  Task used to run the vncreflector program.
     */
    private Task task;


    /**
     *  Current display where the local vncrefector connect to.
     */
    private String display;


    /**
     *  Current password used by the local vncrefector to connect to the current display.
     */
    private String password;


    /**
     *  The port where the vncreflector accept client connections.
     */
    private int rfbPort;    


    /**
     *  Temporal file to save the connection display and password values.
     *  This file is an argument of the vncreflector command line.
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
     *  @param rfbPort Port where the vncreflector program accept connections.
     */
    ReflectorManager(int rfbPort) {

	// Inicializacion de los mensajes de log.
	mylogger = Logger.getLogger("isabel.nereda.ReflectorManager");
	mylogger.info("Creating ReflectorManager object.");

	task = null;

	display  = "";
	password = "";

	this.rfbPort = rfbPort;

	try {
	    String tmpDir = System.getenv("ISABEL_TMP_DIR");
	    if (tmpDir == null) {
		tmpDir = "/tmp";
	    }
	    
	    tmpCCFile = File.createTempFile("ref_","_cc",new File(tmpDir));
	    tmpCCFile.deleteOnExit();
	}
	catch (IOException ioe) {
	    mylogger.severe("I can't create a temporal file to start isabel_vncreflector. "+
			    "I get: "+ioe.getMessage());
	}
	
	if (System.getProperty("user.name").equals("ubuntu")) {
	    reflectorLogsFilename = "/dev/null";
	} else {
	    String logsDir = System.getenv("ISABEL_LOGS_DIR");
	    if (logsDir == null) {
		logsDir = "/tmp";
	    }
	    reflectorLogsFilename = logsDir + "/reflector.log";
	    
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
     *  Invoke setDisplay(display,password) with no password.
     *  @param display Display where the vncreflector program must connect to.
     *  @param password Password needed to connect to display
     */
    synchronized void setDisplay(String display) {
	setDisplay(display,"");
    }


    /**
     *  If display is not equals to "", then it starts a vncreflector that
     *  connect with display.
     *  Id display is "", it stops any vncreflector program.
     *  @param display Display where the vncreflector program must connect to.
     *  @param password Password needed to connect to display
     */
    synchronized void setDisplay(String display,String password) {
	
	if (display.equals("")) {
	    
	    // Stops the vncreflector.
	    
	    mylogger.info("Stopping vncreflector.");

	    this.display  = "";
	    this.password = "";

	    killVncReflectorTask();

	} else {

	    if (password == null) {
		password = "";
	    }

	    mylogger.info("Setting connection to "+display+"-"+password);
	    
	    // Starts a new vncreflector program 
	    
	    this.display  = display;
	    this.password = password;
	    
	    killVncReflectorTask();
	    
	    try {
		
		PrintWriter pw = new PrintWriter(new FileWriter(tmpCCFile));
		pw.println(display+" "+password);
		pw.close();
		
		mylogger.info("Starting a new vncreflector instance.");

		task = new Task("vncreflector",
				new String[] {
				    "isabel_vncreflector",
				    "-t",
				    "-f", "1",
				    "-l", ""+rfbPort,
				    "-g", reflectorLogsFilename,
				    tmpCCFile.getAbsolutePath()},
				true);
		task.start();
	    } catch (Exception e) {
		mylogger.severe("I can't start isabel_vncreflector program to connect to "+
				display+" with password "+password+". "+
				"I get: "+e.getMessage());
		this.display  = "";
		this.password = "";
		killVncReflectorTask();
	    }
	}
    }

    /**
     * Kills the isabel_vncreflector task.
     */
    private void killVncReflectorTask() {

	if (task != null) {
	    task.kill();
	    task = null;
	    try {
		Runtime.getRuntime().exec("killall -q isabel_vncreflector");
	    } catch (Exception rte) {
	    }
	}
    }
}
