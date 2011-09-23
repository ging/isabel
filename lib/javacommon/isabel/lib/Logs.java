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

package isabel.lib;

import java.util.logging.*;
import java.util.*;
import java.io.*;
import java.text.SimpleDateFormat;

/**
 * Esta clase crea un objeto Logger para manejar los mensajes de log,
 * y lo configura para adaptar los mensajes a nuestros gustos personales.
 * @author Santiago Pavon
 */
public class Logs {
    
    /** 
     *  Initialization of the "logger" member.
     *  @param filename Name used to create the log file. 
     *                  The log file is placed in the $ISABEL_LOGS_DIRS directory.
     *                  If this environment variable does no exist, them /tmp is used.
     */
    public static void configure(String filename) {

	String logsDir = System.getProperty("isabel.logs.dir");
	if (logsDir == null) {
	    logsDir = "/tmp";
	}
	configure(logsDir,filename);
    }
    
    
    /** 
     *  Initialization of the "logger" member.
     *  @param filename Name used to create the log file.
     *  @param dir      Name of the directory where the log file is created.
     */
    public static void configure(String dirname, String filename) {
	
	Logger logger = Logger.getLogger("");
	
	try {
	    Handler fh = new FileHandler(dirname+"/"+filename+".log");

	    //fh.setFormatter(new SimpleFormatter());
	    fh.setFormatter(new java.util.logging.Formatter() {

		    private Date date = new Date();
		    private SimpleDateFormat format = new SimpleDateFormat("yyyy.MM.dd:HH:mm:ss:SSS");

		    public String format(LogRecord record) {

			date.setTime(record.getMillis());

			return 
			    format.format(date) + " | " +
			    record.getThreadID()+" | "+
			    record.getSourceClassName()+" | "+
			    record.getSourceMethodName()+" | "+
			    record.getLevel() + " | " +
			    record.getMessage() +
                            "\n";
			
			// record.getLoggerName()+" | "+
			
		    }
		});

	    logger.addHandler(fh);
	}
	catch (Exception e) {
	    System.err.println("I cannot create the log manager: \""+filename+"\". "+e.getMessage());
	}
    }

    /** 
     *  Read a properties file to configure the LogManager.
     *  @param conf Properties file with the configuration for the LogManager object.
     *  @return true if success.
     */
    public static boolean readProperties(File conf) {
	
	if (!conf.exists()) return false;

	try {
	    FileInputStream fis = new FileInputStream(conf);
	    LogManager.getLogManager().readConfiguration(fis);
	    return true;
	}
	catch (Exception e) {
	    System.err.println("I cannot configure log manager. I get: "+e.getMessage());
	    return false; 
	}
    }
}
  
