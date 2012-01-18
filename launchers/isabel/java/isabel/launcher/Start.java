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
 * Start.java
 */

package isabel.launcher;


import java.io.*;

import java.util.*;
import java.util.logging.*;

import java.net.*;

import isabel.lib.Logs;
import isabel.lib.tasks.*;
import isabel.lib.tasksock.*;
import isabel.tunnel.*;

/*import isabel.gwsip.service_manager.*;

/**
 *  Class used to launch isabel in the local site.
 *  Session and Site identifications are passed as command line arguments.
 *  @author Santiago Pavon
 */
public class Start {
    
    /**
     *  Name of the session to start.
     */
    private static String sessionName;

    /**
     *  Name of the site to start.
     */
    private static String siteName;


    /**
     *  Isabel instalation path.
     */ 
    private static String ISABEL_DIR;

    /**
     *  Home path.
     */ 
    private static String ISABEL_WORKDIR;

    /**
     *  Isabel user path.
     */ 
    private static String ISABEL_USER_DIR;

    /**
     *  Temporal directory path.
     */ 
    private static String ISABEL_TMP_DIR;

    /**
     *  Logs directory path.
     */ 
    private static String ISABEL_LOGS_DIR;

    /**
     *  Session logs directory path.
     */ 
    private static String ISABEL_SESSION_LOGS_DIR;

    /**
     *  Configuration directory path.
     */ 
    private static String ISABEL_CONFIG_DIR;

    /**
     *  Slides directory path.
     */ 
    private static String ISABEL_PRES_DIR;

    /**
     *  Sessions directory path.
     */ 
    private static String ISABEL_SESSIONS_DIR;

    /**
     *  Setup directory path.
     */ 
    private static String ISABEL_SETUP_DIR;

    /**
     *  True if I am the session master.
     */ 
    private static boolean ISABEL_IS_MASTER;

    /**
     *  Path del fichero de configuracion del sitio.
     */
    private static String confSiteFilename;

    /**
     *  Values defined in the site configuration file.
     */
    private static Properties siteConfig;

    /** 
     *  Logger where logs are written. 
     */
    private static Logger mylogger;


    /**
     *  Prints a USAGE message and exits.
     */
    private static void usage() {

	System.out.println("java -cp isabel_lib.jar:isabel_launcher.jar");
	System.out.println("     -Disabel.dir=$ISABEL_DIR");
	System.out.println("     -Disabel.user.dir=$ISABEL_USER_DIR");
	System.out.println("     -Disabel.tmp.dir=$ISABEL_TMP_DIR");
	System.out.println("     -Disabel.logs.dir=$ISABEL_LOGS_DIR");
	System.out.println("     -Disabel.config.dir=$ISABEL_CONFIG_DIR");
	System.out.println("     -Disabel.sessions.dir=$ISABEL_SESSIONS_DIR");
	System.out.println("     -Disabel.setup.dir=$ISABEL_SETUP_DIR");
	System.out.println("     -Djava.util.logging.config.file=<filename>");
	System.out.println("     [-DDISPLAY=<control_display>]");
	System.out.println("     [-DADISPLAY=<scenary_display>]");
	System.out.println("     isabel.launcher.Start <sessionName> <siteName>");
	System.exit(1);
    }    
    
    /**
     *  Prints some error messages and exits.
     *  @param msgs Messages to print
     */
    private static void error(String[] msgs) {
	System.err.print("ERROR: ");
	for (int i=0 ; i<msgs.length; i++)
	    System.err.println(msgs[i]);
	System.exit(1);
    }


    /**
     *  Prints an error message and exits.
     *  @param msg Message to print
     */
    private static void error(String msg) {
	System.err.println("ERROR: "+msg);
	System.exit(1);
    }


    /**
     *  Prints an error message, display a dialog window and exits.
     *  @param msgs Messages to print
     */
    private static void winError(String[] msgs) {
	String msg = "";
	for (int i=0 ; i<msgs.length; i++)
	    msg += msgs[i] + "\n";
	winError(msg);
    }


    /**
     *  Prints an error message, display a dialog window and exits.
     *  @param msg Message to print
     */
    private static void winError(String msg) {
	javax.swing.JOptionPane.showMessageDialog(null,
						  msg,
						  "ISABEL Error Message",
						  javax.swing.JOptionPane.WARNING_MESSAGE,
						  new javax.swing.ImageIcon(ISABEL_DIR+"/lib/images/gif/tiburon_icon.gif")
						  );
	error(msg);
    }


    /**
     *  Set the value of attributes with directory paths.
     */
    private static void getDirectories() {

	// Directorio de instalacion:
	ISABEL_DIR = System.getProperty("isabel.dir");
	if (ISABEL_DIR == null) usage();

	// Comprueba instalacion del paquete isabel.
	if (!new File(ISABEL_DIR+File.separator+"lib","version.Isabel").isFile()) {
	    winError("Isabel package not installed at "+ISABEL_DIR);
	}

	// Directorio de trabajo:
	ISABEL_WORKDIR = System.getProperty("user.home");

	// Directorio .isabel del usuario:
	ISABEL_USER_DIR = System.getProperty("isabel.user.dir");
	if (ISABEL_USER_DIR == null) usage();
	if (!new File(ISABEL_USER_DIR).isDirectory()) {
	    if (!new File(ISABEL_USER_DIR).mkdirs()) {
		winError("I can not create the isabel user directory: "+ISABEL_USER_DIR);
	    }
	}

	// directorio temporal:
	ISABEL_TMP_DIR = System.getProperty("isabel.tmp.dir");
	if (ISABEL_TMP_DIR == null) usage();
	if (!new File(ISABEL_TMP_DIR).isDirectory()) {
	    if (!new File(ISABEL_TMP_DIR).mkdirs()) {
		winError("I can not create the temporal directory: "+ISABEL_TMP_DIR);
	    }
	}
	
	// Directorio de Logs:
	ISABEL_LOGS_DIR = System.getProperty("isabel.logs.dir");
	if (ISABEL_LOGS_DIR == null) usage();
	if (!new File(ISABEL_LOGS_DIR).isDirectory()) {
	    if (!new File(ISABEL_LOGS_DIR).mkdirs()) {
		winError("I can not create the logs directory: "+ISABEL_LOGS_DIR);
	    }
	}
	// Directorio de Logs de la session:
	ISABEL_SESSION_LOGS_DIR = System.getenv("ISABEL_SESSION_LOGS_DIR");
	
	// Directorio de configuraciones:
	ISABEL_CONFIG_DIR = System.getProperty("isabel.config.dir");
	if (ISABEL_CONFIG_DIR == null) usage();
	if (!new File(ISABEL_CONFIG_DIR).isDirectory()) {
	    if (!new File(ISABEL_CONFIG_DIR).mkdirs()) {
		winError("I can not create the config directory: "+ISABEL_CONFIG_DIR);
	    }
	}
	
	// Directorio de presentaciones:
	ISABEL_PRES_DIR = ISABEL_WORKDIR + File.separator + "pres";
	if (!new File(ISABEL_PRES_DIR).isDirectory()) {
	    if (!new File(ISABEL_PRES_DIR).mkdirs()) {
		winError("I can not create the presentations directory: "+ISABEL_PRES_DIR);
	    }
	}
	
	// Directorio de sesiones:
	ISABEL_SESSIONS_DIR = System.getProperty("isabel.sessions.dir");
	if (ISABEL_SESSIONS_DIR == null) usage();
	if (!new File(ISABEL_SESSIONS_DIR).isDirectory()) {
	    if (!new File(ISABEL_SESSIONS_DIR).mkdirs()) {
		winError("I can not create the sessions directory "+ISABEL_SESSIONS_DIR);
	    }
	}

	// Directorio setup:
	ISABEL_SETUP_DIR = System.getProperty("isabel.setup.dir");
	if (ISABEL_SETUP_DIR == null) usage();
	if (!new File(ISABEL_SETUP_DIR).isDirectory()) {
	    if (!new File(ISABEL_SETUP_DIR).mkdirs()) {
		winError("I can not create the setup directory "+ISABEL_SETUP_DIR);
	    }
	}
    }

 
    /**
     *  Loads the values defined in the config file.,
     */
    private static void loadConfigFile() {

	if (!new File(confSiteFilename).isFile()) {
	    winError(new String[] {
		"I can not find the site configuration file: ",confSiteFilename});
	}
	
	// Creamos la tabla con la configuracion del sitio:
        try {
	    siteConfig = new Properties();
	    siteConfig.load(new FileInputStream(confSiteFilename));
        }
	catch (IOException e) {
	    winError(new String[] {
		"I can not load the site configuration file.",
		"I get: "+e.getMessage()});
        }
    }


    /**
     *  Saves the values stored in the siteConfig property object into
     *  the config file.
     */
    private static void saveConfigFile() {

	try {
	    File setupdir = new File(ISABEL_SETUP_DIR,
				     siteConfig.getProperty("ISABEL_SESSION_ID"));
	    
	    siteConfig.store(new FileOutputStream(confSiteFilename),
			     "Setup configuration updated by isabel.launcher.Start");
	}
	catch (IOException ioe) {
	    winError(new String[] {
		"I can not save configuration file: "+confSiteFilename,
		"I get: "+ioe.getMessage()});
	}
    }


    /**
     *  Return true is master is the master site.
     *  @param master hostname.
     *  @return true is master is the master site.
     */
    private static boolean isLocalMaster(String master) {

	try {
	    InetAddress ia = InetAddress.getByName(master);
	    
	    if (ia.isLoopbackAddress()) {
		return true;
	    }

	    NetworkInterface ni = NetworkInterface.getByInetAddress(ia);
	    
	    return ni != null;
	}
	catch (SocketException se) {
	    winError(new String[] {
		"I do not know if the master site ("+master+") is myself.",
		"I get: "+se.getMessage()});
	}
	catch (UnknownHostException uhe) {
	    winError(new String[] {
		"Master site ("+master+") is unknown.",
		"I get: "+uhe.getMessage()});
	}
	return false;
    }
    

    /**
     *  Return true is host is a local address, else returns false.
     *  An error message is printed is there is any problem checking the given address.
     *  @param host hostname to check
     *  @param errorMsg Message to display if there is any problem checking the
     *                  the given address.
     *  @return trus host is a local address, else false.
     */
    private static boolean isLocalAddress(String host,
					  String errorMsg) {

	try {
	    InetAddress ia = InetAddress.getByName(host);
	    
	    if (ia.isLoopbackAddress()) {
		return true;
	    }
	    
	    NetworkInterface ni = NetworkInterface.getByInetAddress(ia);

	    return ni != null;
	}
	catch (SocketException se) {
	    winError(new String[] {
		errorMsg,
		"I can not know if \""+host+"\" is a local address.",
		"I get: "+se.getMessage()});
	}
	catch (UnknownHostException uhe) {
	    winError(new String[] {
		errorMsg,
		"Unknown address \""+host+"\".",
		"I get: "+uhe.getMessage()});
	}
	return false;
    }
    


    /** 
     *  Gets the local FQDN or null if it can not be resolved
     */
    private static String getLocalFQDN() {      
	
	try {
	    NetworkInterface ni = NetworkInterface.getByName("eth0");
	    
	    if (ni != null) { 
		if (ni.isUp()) {
		    System.out.println("Interface = " + ni.getName());
		    Enumeration<InetAddress> ias = ni.getInetAddresses();
		    while (ias.hasMoreElements()) {
			
			InetAddress ia = ias.nextElement();
			
			if (!(ia instanceof Inet4Address)) continue;
			
			String hn = ia.getHostName();
			String ha = ia.getHostAddress();
			
			if (hn.equals(ha)) continue; 
			return hn;
		    }
		}
	    }
	} catch (SocketException e) {
	}

	try {
	    
	    Enumeration<NetworkInterface> ifs = NetworkInterface.getNetworkInterfaces();
	    
	    while (ifs.hasMoreElements()) {
		
		NetworkInterface ni = ifs.nextElement();

		if ( ! ni.isUp() ) continue;
		
		System.out.println("Interface = " + ni.getName());
		
		Enumeration<InetAddress> ias = ni.getInetAddresses();
		
		while (ias.hasMoreElements()) {
		    
		    InetAddress ia = ias.nextElement();
		    
		    if ( ia.isLoopbackAddress()) continue;
		    
		    if (!(ia instanceof Inet4Address)) continue;
		    
		    String hn = ia.getHostName();
		    String ha = ia.getHostAddress();
		    
		    if (hn.equals(ha)) continue; 
		    
		    return hn;
		    
		}
	    }
	} catch (SocketException e) {
	}
	
	winError(new String[] {
	    "I can not get the local FQDN.",
	    "Check the network configuration"
	});
	
	return null;
    }


 
    /**
     *  Check accessibility to the given host.
     *  An error message is printed is there is any problem checking the given address,
     *  and then I exit.
     *  @param host hostname to check accessibility.
     *  @param errorMsg Message to display if there is any problem checking the
     *                  the given address.
     */
    private static void checkHostAccess(String host,
					String errorMsg) {


	try {
	    InetAddress ia = InetAddress.getByName(host);
	}
	catch (UnknownHostException uhe) {
	    winError(new String[] {
		errorMsg,
		"Invalid address: \""+host+"\".",
		"I get: "+uhe.getMessage()});
	}
    }


    /**
     *  Launch the SeCo service.
     */
    private static void launchSeCo() {
	
	System.out.println("Launching SeCo.");

	int port = 53023;
	int cport = 51011;
	String lpuniqid = "uniqidtokillisabelseco";
	
	String jarseco = ISABEL_DIR+"/libexec/isabel_seco.jar";
	String jarlib  = ISABEL_DIR+"/libexec/isabel_lib.jar";
	String jars = 
	    jarlib+File.pathSeparatorChar+
	    jarseco;

	String seco_host_target = (String)siteConfig.get("ISABEL_CONNECT_TO_IP");

	String[] cmd = null;
	if (!seco_host_target.equals("")) {
	    System.out.println("Local SeCo configured to connect to SeCo at \""+
			       seco_host_target + "\".");
	    cmd = new String[] {
		"java",
		"-cp", jars,
		"-Disabel.dir="+ISABEL_DIR,
		"-Disabel.logs.dir="+ISABEL_LOGS_DIR,
		"-Djava.util.logging.config.file="+ISABEL_CONFIG_DIR+"/logging.properties",
		"-Duniqid="+lpuniqid,
		"isabel.seco.server.SeCo",
		"-dataport",		
		""+port, 
		seco_host_target, 
		""+port};
	} else {
	    System.out.println("Local SeCo configured as root SeCo.");
	    cmd = new String[] {
		"java",
		"-cp", jars,
		"-Disabel.dir="+ISABEL_DIR,
		"-Disabel.logs.dir="+ISABEL_LOGS_DIR,
		"-Djava.util.logging.config.file="+ISABEL_CONFIG_DIR+"/logging.properties",
		"-Duniqid="+lpuniqid,
		"isabel.seco.server.SeCo",
		"-dataport",
		""+port};
	}
	
	try {
	    final TaskSock secotask = new TaskSock("SeCo",cmd,cport,"seco_nop()","seco_quit()");

	    secotask.addDeadTaskSockListener(new DeadTaskSockListener() {
		    
		    public void dead() {
			System.out.println("ISABEL has finished (SeCo).");
			System.exit(0);
		    }
		});
	}
	catch (Exception ioe) {
	    if (!seco_host_target.equals("")) {
		winError(new String[] {
			"I can not connect to Isabel at " + (String)siteConfig.get("ISABEL_CONNECT_TO"),
			"I get: "+ioe.getMessage()});
	    } else {
		winError(new String[] {
			"I can not start the Isabel Control module.",
			"I get: "+ioe.getMessage()});
	    }
	}
    }


    /**
     *  Creates ther service file
    */    
    private static void makeService() throws Exception {

	System.out.println("Creating service ...");

	String servFilename = (String)siteConfig.get("ISABEL_SERVICE");

	File servFile = new File(servFilename);
	if (!servFile.exists()) {
	    servFile = new File(ISABEL_USER_DIR,servFilename);
	    if (!servFile.exists()) {
		servFile = new File(ISABEL_WORKDIR,servFilename);
		if (!servFile.exists()) {
		    servFile = new File(ISABEL_WORKDIR,"act/"+servFilename);
		    if (!servFile.exists()) {
			servFile = new File(ISABEL_DIR,"lib/"+servFilename);
			if (!servFile.exists()) {
			    winError("Service \""+servFilename+"\" not found.");
			}
		    }
		}
	    }
	}
	System.out.println("Service file:: "+servFile);

	// Creando fichero de definiciones M4 con la configuracion del sitio.

	File defines = File.createTempFile("serv_","_m4",new File(ISABEL_TMP_DIR));
	    
	PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(defines)));
	
	Enumeration ekey = siteConfig.keys();
	while (ekey.hasMoreElements()) {
	    String key = (String)ekey.nextElement();
	    String val = (String)siteConfig.get(key);
	    out.println("define(`"+key+"',`"+val+"')");
	}
	out.close();
	
	// Mezclando con M4:
	String [] cmd = new String[] { 
	    "m4",
	    "-I", ISABEL_WORKDIR,
	    "-I", ISABEL_WORKDIR+"/act",
	    "-I", ISABEL_DIR+"/lib",
	    defines.getAbsolutePath(),
	    servFile.getAbsolutePath()};
	
	// Redirige la salida estandar del programa al fichero final con el servicio, 
	// y la salida de error a la pantalla.
	final PrintStream service = new PrintStream(
	    new BufferedOutputStream(
		new FileOutputStream(ISABEL_SETUP_DIR+"/"+sessionName+"/service")));
	
	
	final Task t = new Task("m4",cmd,false,service,System.err);
	t.start();
	
	// Esperar a que termine.
	t.join();
	service.close();
    }
    
    /**
     *  Launch a master site.
     */
    private static void startMasterLinux() {

	System.out.println("Launching master.");

	String[] cmd = new String[] {
	    System.getenv("ISABEL_TCL"),
	    ISABEL_DIR+"/lib/isatcl/isabel_master",
	    confSiteFilename,
	    "-geometry", "+1+1",
            "-display", System.getProperty("DISPLAY")};
	
	// Redirige las salidas estandar y de error del programa a un fichero de log.
	PrintStream flog = System.out;
	try {
	    flog = new PrintStream(new BufferedOutputStream(new FileOutputStream(ISABEL_SESSION_LOGS_DIR+"/isabel_master.log")),true);
	} 
	catch (Exception ioe) {
	    System.out.println("I can not open master log file: I get: "+ioe.getMessage());
	}

	try {
	    Task mastertask = new Task("Master",cmd,false,flog,flog);
	    mastertask.addTaskListener(new ExitListener("Master"));
	    mastertask.start();
	}
	catch (Exception ioe) {
	    winError(new String[] {
		"I can not launch master.",
		"I get: "+ioe.getMessage()});
	}

    }



    /**
     *  Launch a MCU site.
     */
    private static void startMCU() {

	System.out.println("Launching mcu.");

	String[] cmd = new String[] {
	    System.getenv("ISABEL_TCL"),
	    ISABEL_DIR+"/lib/isatcl/isabel_mcu",
	    "-geometry", "+1+1",
            "-display", System.getProperty("DISPLAY")};

	try {
	    Task mcutask = new Task("MCU",cmd,false);
	    mcutask.addTaskListener(new ExitListener("MCU"));
	    mcutask.start();
	}
	catch (Exception ioe) {
	    winError(new String[] {
		"I can not launch mcu.",
		"I get: "+ioe.getMessage()});
	}


    }

//-------------------------------------------------------------------------------


    /**
     *  Launch an interactive site.
     */
    private static void startInteractive(String tool) {
	startInteractive(tool,System.getProperty("ADISPLAY"));
    }


    /**
     *  Launch an interactive site.
     * @param adisplay Scenary display.
     */
    private static void startInteractive(String tool, String adisplay) {

	System.out.println("Launching "+tool+".");

	String irole = (String)siteConfig.get("ISABEL_ROLE");

	// Redirige las salidas estandar y de error del programa a un fichero de log.
	PrintStream flog = System.out;
	try {
	    flog = new PrintStream(new BufferedOutputStream(new FileOutputStream(ISABEL_SESSION_LOGS_DIR+"/isabel_interactive.log")),true);
	} 
	catch (Exception ioe) {
	    System.out.println("I can not open interactive log file: I get: "+ioe.getMessage());
	}

	String[] cmd = new String[] {
	    "/bin/bash",
	    "-c",
	    "ADISPLAY="+adisplay+" "+
	    System.getenv("ISABEL_TCL")+" "+
            ISABEL_DIR+"/lib/isatcl/isabel_interactive " +
	    irole +" "+
	    confSiteFilename};

	try {
	    Task interactivetask = new Task(tool,cmd,false,flog,flog);
	    interactivetask.addTaskListener(new ExitListener(tool));
	    interactivetask.setStreamsClosingTimeout(1000);
	    interactivetask.start();
	}
	catch (Exception ioe) {
	    winError(new String[] {
		"I can not launch "+tool+" site.",
		"I get: "+ioe.getMessage()});
	}
    }


    private static void launchVncDesktopCapturer(int rfbport, String passwdfile) {

	try {
	    new Task("vncShmCapture",
		     ISABEL_DIR + "/bin/vncShmCapture " +
		     "-shmCapture " +
		     "-idShMemory 1350 " +
		     "-sizeShMemory 12582912 " +
		     "-encodings tight " +
		     "-compresslevel 9 " +
                     "-passwd " + passwdfile +
		     " :" + rfbport,
		     true).start();
	} catch (Exception ioe) {
	}
    }


    /**
     *  Start a Xvnc server.
     *  @param display Display num to use
     *  @param rfbport RFB port
     *  @param passwdfile Filename of the passwords file.
     */
    private static void startVncServer(String display, int rfbport, String passwdfile) {

	String serversize = "1024x768";
	
	setCookie(display);
	
	
	if (which("Xtightvnc") != null) {
	    System.out.println("Launching Xtightvnc server.");
	    launchXtightvnc(display, serversize, rfbport, passwdfile);
	} else if (which("Xvnc") != null) {
	    System.out.println("Launching Xvnc server.");
	    launchXvnc(display, serversize, rfbport, passwdfile);
	}  else {
	    winError("I can not find neither Xvnc nor Xtightvnc");
	}
	
	try {
		Thread.sleep(2000);
	} catch (InterruptedException e) {
		e.printStackTrace();
	}
	/*
	try {
	    new Task("xhost","xhost + -display "+display,false).start();
	} catch (Exception ioe) {
	}
	*/

	try {
	    if (new File(ISABEL_WORKDIR+"/.Xresources").exists()) {
		new Task("xrdb","xrdb -display "+display+" "+ISABEL_WORKDIR+"/.Xresources",false).start();
	    }
	} catch (Exception ioe) {
	}
	
	try {
	    new Task("xsetroot","xsetroot -display "+display+" -grey",false).start();
	} catch (Exception ioe) {
	}
/*
	try {
	    new Task("twm","twm -f "+ISABEL_DIR+"/lib/localvnc.twmrc -display "+display,false).start();
	} catch (Exception ioe) {
	}
*/


    }
    
    private static void startXServer() {
    	try {
    		System.out.println("Starting xfakeserver");
    		String[] cmd = {"sh", "-c", ISABEL_DIR + "/bin/xservervnc.sh start"};
    	    new Task("xfakeserver", cmd, false).start();
    	    
    	    Thread.sleep(6000);
    	} catch (Exception e) {
    		e.printStackTrace();
    	}    	
    }



    /**
     *  Launch the Xtightvnc program
     */
    private static void launchXtightvnc(String serverdisplay,
					String serversize,
					int rfbport, 
					String passwdfile) {

	String xauthority = System.getProperty("user.home") + "/.Xauthority";

	String[] cmd = {"Xtightvnc",
			serverdisplay,
			"-desktop", "Isabel",
			"-lf",  "1024",
			"-auth",  xauthority,
			"-geometry", serversize,
			"-depth", "24",
			"-co", getColorPath(),
			"-fp", getFontPath(),
			"-deferupdate", "100",
			"-rfbwait", "120000",
			"-ac", 
			"-localhost", 
			"-rfbauth", passwdfile,
			"-rfbport", rfbport+"",
			"-alwaysshared"};

	try {
	    Task vnctask = new Task("xtightvnc",cmd,false);
	    vnctask.addTaskListener(new ExitListener("Xtightvnc"));
	    vnctask.start();
	} catch (Exception ioe) {
	    winError(new String[] {
		"I can not launch Xtightvnc server.",
		"I get: "+ioe.getMessage()});
	}
    }

    /**
     *  Launch the Xvnc program
     */
    private static void launchXvnc(String serverdisplay,
				   String serversize,
				   int rfbport, 
				   String passwdfile) {

	String xauthority = System.getProperty("user.home") + "/.Xauthority";

	String[] cmd = {"Xvnc",
			serverdisplay,
			"-desktop", "Isabel",
			"-lf",  "1024",
			"-auth",  xauthority,
			"-geometry", serversize,
			"-depth", "24",
			"-deferupdate", "100",
			"-rfbwait", "120000",
			"-ac", 
			"-localhost",
			"-rfbauth", passwdfile,
			"-rfbport", rfbport+"",
			"-alwaysshared"};

	try {
	    Task vnctask = new Task("xvnc",cmd,false);
	    vnctask.addTaskListener(new ExitListener("Xvnc"));
	    vnctask.start();
	} catch (Exception ioe) {
	    winError(new String[] {
		"I can not launch Xvnc server.",
		"I get: "+ioe.getMessage()});
	}
    }



    /**
     *  Returns the hostname
     */
    private static String getHostname() {

	PrintStream ps = new PrintStream(new ByteArrayOutputStream());
	
	try {
	    Task t = new Task("uname","uname -r",false,ps,System.err);
	    t.start();
	    t.join(); // Esperar a que termine.
	} catch (Exception e) {
	    winError("I can not get my hostname: "+e.getMessage());
	}
	ps.close();
	return ps.toString();
    }

    
    /**
     * Set a cookie
     */ 
    private static void setCookie(String serverdisplay) {

	ByteArrayOutputStream baos = new ByteArrayOutputStream();
	PrintStream ps = new PrintStream(baos);

	try {
	    Task t = new Task("cookie","mcookie",false,ps,System.err);
	    t.start();
	    t.join(); // Esperar a que termine.
	} catch (Exception e) {
	}
	ps.close();

	String cookie = baos.toString();

	System.out.println("Cookie = "+cookie);

	String hostname = getHostname();
	String userhome = System.getProperty("user.home");

	String[] cmd = {"xauth",
			"-f",
			userhome+File.separator+".Xauthority",
			"add",
			hostname+File.separator+"unix"+serverdisplay,
			".",
			cookie};

	try {
	    Task t = new Task("xauth",cmd,false);
	    t.start();
	    t.join(); // Esperar a que termine.
	} catch (Exception ioe) {
	}
    }


    /**
     *  Gets the color path.
     */
    private static String getColorPath() {
	
	String[] candidates = {"/etc/X11",
			       "/usr/share/X11",
			       "/usr/X11R6/lib/X11",
			       "/usr/lib/X11"};
	
	for (String c : candidates) {
	    if (new File(c,"rgb.txt").exists()) {
		return c + "/rgb";
	    }
	}
	return "/usr/lib/X11/rgb";
    }
    
    /**
     *  Get the font path
     */
    private static String getFontPath() {

	String[] dirs = {"/usr/X11R6/lib/X11/fonts",
			  "/usr/share/fonts/X11",
			  "/usr/share/X11/fonts"};
	
	String[] styles = {"Type1",
			   "Speedo",
			   "misc",
			   "75dpi",
			   "100dpi"};

	String fontpath = "";
	for (String d : dirs)
	    for (String s : styles)
		if (new File(d,s).exists())
		    if (fontpath.equals(""))
			fontpath = d + "/" + s;
		    else
			fontpath += "," + d + "/" + s;
	return fontpath;
    }


    /**
     *  Returns: which <program_name>
     */
    private static String which(String programName) {
	
	PrintStream ps = new PrintStream(new ByteArrayOutputStream());
	
	try {
	    Task t = new Task("which","which "+programName,false,ps,System.err);
	    t.start();
	    t.join(); // Esperar a que termine.
	} catch (Exception e) {
	    return null;
	}
	ps.close();
	return ps.toString();
    }
    
    

    /**
     *  Launch a MediaServer site.
     */
    private static void startMediaServer() {

	System.out.println("Launching Media Server.");

	startVncServer(":33", 53018, ISABEL_DIR+"/lib/mediaserver.passwd");

	startInteractive("MediaServer",":33");
    }

    /**
     *  Launch a FLASH Gateway site.
     */
    private static void startFlashGateway() {

	System.out.println("Launching FLASH Gateway.");
	
	if (System.getenv("IGW_ENABLE")!=null)
	    startXServer();
	else
	    startVncServer(":33", 53018, ISABEL_DIR+"/lib/sipgateway.passwd");

	//launchVncDesktopCapturer(53018, ISABEL_DIR+"/lib/sipgateway.passwd");

	startInteractive("FlashGateway",":33");

	// Lanzar el FlashGateway
	try {
	    new isabel.gwflash.FlashGateway();
	}
	catch (Exception sme) {
	    winError(new String[] {
		    "I can not launch GWFLASH FlashGateway: ",
		    "I get: "+sme.getMessage()});
	    sme.printStackTrace();
	}
    }


    /**
     *  Launch a SIP Gateway site.
     */
    private static void startSipGateway() {

	System.out.println("Launching SIP Gateway.");

	startVncServer(":33", 53018, ISABEL_DIR+"/lib/sipgateway.passwd");

	launchVncDesktopCapturer(53018, ISABEL_DIR+"/lib/sipgateway.passwd");

	startInteractive("SipGateway",":33");

	// Lanzar el SipGateway
	try {
	    // new isabel.gw.isabel_client.simulator2.Simulator();
	    new isabel.gwsip.SipGateway();
	}
	catch (Exception sme) {
	    winError(new String[] {
		    "I can not launch GWSIP SipGateway: ",
		    "I get: "+sme.getMessage()});
	}
	
	
	/*
	  Properties gsp = new Properties();
	  
	  // Hostname where the SIP register is running.
	  String sipRegister = (String)siteConfig.get("ISABEL_SIP_REGISTER_HOST");
	  if (sipRegister.equals("")) {
	  winError(new String[] {"Undefined \"ISABEL_SIP_REGISTER_HOST\" parameter.",
	  "Please, define it at Applications -> Isabel -> Edit Local Configuration -> Parameters.",
	  "Its value must be the hostname of your SIP register server."});
	  }
	  checkHostAccess(sipRegister,
	  "I can not access to the SIP register server (" + sipRegister + ").\n" +
	  "Please, update its value at Applications -> Isabel -> Edit Local Configuration -> Parameters.\n" +
	  "Its value must be the hostname of your SIP register server.");
	  gsp.put("sip register",sipRegister);
	  
	  
	  // MY SIP URI: name@domain
	  String myUriSip = (String)siteConfig.get("ISABEL_GWSIP_URI");
	  if (myUriSip.equals("")) {
	  myUriSip = (String)siteConfig.get("SITE_ID") + "@" + sipRegister;
	  }
	  gsp.put("gwsip uri",myUriSip);
	  
	  
	  // Puertos de escucha del servidor
	  gsp.put("proxy sip port", "5060");
	  gsp.put("sip local port", "5099");
	  gsp.put("video local port","53200:53249");
	  gsp.put("audio local port","53250:53299");
	  
	  
	  // Conferencia			
	  gsp.put("default bw","0");
	  gsp.put("isabel video Payload Type","96");
	  
	  String iqlty = (String)siteConfig.get("ISABEL_QUALITY");
	  String apt = null;
	  if (iqlty.equals("10M")) {
	  apt = "0"; // pcm
	  } else if (iqlty.equals("2M")) {
	  apt = "0"; // pcm
	  } else if (iqlty.equals("1M")) {
	  apt = "107"; // gsm 16
	  } else {
	  apt = "3";   // gsm 8
	  }
	  gsp.put("isabel audio Payload Type",apt);
	  
	  gsp.put("isabel video port","53025");
	  
	  //gsp.put("isabel audio port","53021");
	  gsp.put("isabel audio port","51002");
	  
	  gsp.put("isabel rfb port","53018");
	  gsp.put("isabel vnc display",":33");
	  gsp.put("fake display",System.getenv("DISPLAY"));
	  
	  // Lanzar el Service Manager: gestiona los clientes SIP
	  try {
	  // new ServiceManager(gsp);
	  }
	  catch (Exception sme) {
	  error(new String[] {
	  "I can not launch GWSIP ServiceManager: ",
	  "I get: "+sme.getMessage()});
	  }
	  
	*/
    }
    

	
    /**
     *  TaskListener used with different tasks to exit isabel.
     */
    private static class ExitListener implements TaskListener {
		
	private String exitMsg;

	ExitListener(String exitMsg) {
	    this.exitMsg = exitMsg;
	}
	
	public void deadTask() {
	    System.out.println("ISABEL has finished ("+exitMsg+").");
	    System.exit(0);
	}
    }



    /**
     *  Main method where the application starts.
     */
    public static void main(String[] args) {

	// Inicializacion de los logs del master.
	Logs.configure("start_isabel");
	mylogger = Logger.getLogger("isabel.launcher.Start");
	mylogger.info("Starting ISABEL launcher.");

	// Comprobar numero de argumentos
	if (args.length != 2) {
	    usage();
	}	

	sessionName = args[0];
	siteName = args[1];
	
	getDirectories();

	// Fichero configuracion del sitio: 
	confSiteFilename = ISABEL_SETUP_DIR + File.separator +
	    sessionName + File.separator + 
	    "config." + siteName;
	
	loadConfigFile();

	// Ver si mi hostname es valido
	String hn = (String)siteConfig.get("ISABEL_HOSTNAME");
	boolean ilh = isLocalAddress(hn,"My hostname \""+hn+"\" is not valid.");
	if (!ilh) {
	    winError(hn+" It is not a local network interface.");
	}

	// Comprobar acceso a isabone y al flowserver:
	String ict = (String)siteConfig.get("ISABEL_CONNECT_TO");
	if (!ict.equals("")) {
	    checkHostAccess(ict,"Unavailable isabone access point.");
	    if (isLocalAddress(ict,"Invalid isabone access.")) {
		winError("Isabone access point ("+ict+") is a local address.");
	    }
	} else {
	    String iuf = (String)siteConfig.get("ISABEL_USE_FLOWSERVER");
	    if (iuf.equals("1")) {
		winError("Requested use of flowserver, but not given isabone access point.");
	    }
	}

	// Determinar si soy el master:
	ISABEL_IS_MASTER = ict.equals("");
	if (ISABEL_IS_MASTER) {
	    System.out.println("I am the master site.");
	}

        // Start openvpn server tunnel
	int res = ServerTunnel.launch();
	if (res != 1) {
	    error("I can not start openvpn server side.");
	}

	if (! ISABEL_IS_MASTER) {

	    String icti = (String)siteConfig.get("ISABEL_CONNECT_TO_IP");
	    if (icti == null) {
		System.out.println("No estaba la variable ISABEL_CONNECT_TO_IP, ponemos el mismo valor que el de ISABEL_CONNECT_TO");
		String ict2 = (String)siteConfig.get("ISABEL_CONNECT_TO");
		siteConfig.put("ISABEL_CONNECT_TO_IP",ict2);
		saveConfigFile();
	    }
	}


	// Launching SeCo daemon:
	launchSeCo();


	// Lanzar el master
	if (ISABEL_IS_MASTER) {
	    try {
		makeService();
	    }
	    catch (Exception ioe) {
		winError(new String[] {
		    "I can not create the service file.",
		    "I get: "+ioe.getMessage()});
	    }
	    startMasterLinux();
	}


	// Arrancar la tool especificada en ISABEL_ROLE:
	String irole = (String)siteConfig.get("ISABEL_ROLE");
	if (irole.equals("MCU")) {
	    startInteractive("MCU");
	} else if (irole.equals("Interactive")) {
	    startInteractive("Interactive");
	} else 	if (irole.equals("MediaServer")) {
	    startMediaServer();
	} else 	if (irole.equals("SipGateway")) {
	    startSipGateway();
	} else 	if (irole.equals("FlashGateway")) {
	    startFlashGateway();
	} else {
	    winError("Invalid role: "+irole);
	} 

	System.out.println("Ready");
	
    }
    
}
