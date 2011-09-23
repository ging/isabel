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
 * Constants.java
 *
 * Created on 17 de enero de 2006, 10:54
 */

package services.isabel.lib;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

import java.util.Calendar;
import java.util.GregorianCalendar;

public class Constants {
	
	
	//File separator
    public static final String FILE_SEPARATOR = System.getProperty("file.separator");
	  
	  
	/**
     * Directorio de isabel.
     */
    public static final String ISABEL_DIR = System.getProperty("isabel.dir");
    
    
    /**
     * Directorio que guarda la configuracion del usuario
     * de la aplicacion isabel. (el .isabel)
     */
    public static final String ISABEL_USER_DIR = System.getProperty("isabel.user.dir");
    
           
    /**
     * Directorio que guarda la configuracion de la aplicacion isabel.
     */
    public static final String ISABEL_CONFIG_DIR = System.getProperty("isabel.config.dir");
    
	
    /**
     * Directorio que guarda la configuracion del usuario
     * de la aplicacion isabel.
     */
    public static final String ISABEL_SESSIONS_DIR = System.getProperty("isabel.sessions.dir");
    
    
    /**
     * Directorio que guarda los perfiles
     */
    public static final String ISABEL_PROFILES_DIR = System.getProperty("isabel.profiles.dir");
    
    
    /**
     * Directorio de logs
     */
    public static final String ISABEL_LOGS_WORK_DIR = System.getenv("ISABEL_LOGS_DIR");
    
    
    /**
     * Directorio de logs de la session
     */
    public static final String ISABEL_SESSION_LOGS_DIR = System.getenv("ISABEL_SESSION_LOGS_DIR");
    
    
    /**
     * fichero de configuración para ver si validamos o no el xedl
     */
    public static final String VALIDATE_XEDL = "validate_xedl.cfg";    
    
    
    /**
     * clave para ver si validamos o no el xedl
     */
    public static final String VALIDATE_KEY = "validate_xedl";    
      
    
    /**
     * fichero de configuración para el email
     */    
    public static final String EMAIL_CONFIG = "email.cfg";    

    
    /**
     * fichero de configuración para el email
     */    
    public static final String BBDD_CONFIG = "bbdd.cfg";    

    
    /**
     * key for the smtp server in the config file
     */
    public static final String SMTP_SERVER = "SMTP_SERVER";
    
    /**
     * key for the domain in the config file
     */
    public static final String DOMAIN_NAME = "DOMAIN_NAME";
    
    /**
     * key for the user name in the config file
     */
    public static final String USER_NAME = "USER_NAME";
    
    /**
     * key for the user passwd in the config file
     */
    public static final String USER_PASSWD = "USER_PASSWD";
    
    /**
     * key for the user passwd in the config file
     */
    public static final String FROM_SENDER = "FROM_SENDER";
   
    
    public static final String BBDD_NAME = "BBDD_NAME";   
   
    public static final String BBDD_LOGIN = "LOGIN";   
   
    public static final String BBDD_PASSWD = "PASSWD";
   
    public static final String XEDLS_PATH = "XEDLS_PATH";    //indica el path donde esta la carpeta de los xedls del sir, normalmente rails-root/public    
    
    /**
     * path donde se encuentra el directorio setup
     */
    public static final String SETUP_PATH = ISABEL_SESSIONS_DIR + FILE_SEPARATOR + "setup";
	
	/**
	 * extensión para los archivos xedl, atención que no lleva punto
	 */
	public static final String XEDL_EXTENSION = "xedl";
	
	/**
	 * nombre de la sesion de test
	 */
	public static final String NOMBRE_SESION_TEST = "LOCALTEST";
	
	/**
	 * ruta de la sesion de test
	 */
	public static final String RUTA_SESION_TEST = ISABEL_SESSIONS_DIR + FILE_SEPARATOR + "master" + FILE_SEPARATOR 
										+ NOMBRE_SESION_TEST + FILE_SEPARATOR + NOMBRE_SESION_TEST + "." + XEDL_EXTENSION; 
   
	/**
     * nombre del fichero del historial del connect to, esta situado en ~/.isabel/sessions/
     */
	public static final String FICHERO_HISTORIAL_CONNECT_TO = "history.connect_to";
	
	/**
     * nombre del fichero del historial del connect to, esta situado en ~/.isabel/sessions/
     */
	public static final String FICHERO_HISTORIAL_SSS = "history.start_session_server";
    
	/**
     * Nombre por defecto que se le da a la sesion en el servicio star session server
     */
    public static final String NOMBRE_SESION_POR_DEFECTO = "MySession";
    public static final String SERVICIO_SESION_POR_DEFECTO = "Tele-Meeting";
    public static final String CALIDAD_SESION_POR_DEFECTO = "1M";
    
//  clave para cifrar el código, se cifra con esta clave mas el siteId
    public static final String KEY_CODE = "error";

    public static final String MCU_ADDRESS_MULTICAST = "OPTIONS";
    
//  CONSTANTES CONSTANTES CONSTANTES CONSTANTES CONSTANTES CONSTANTES CONSTANTES CONSTANTES CONSTANTES CONSTANTES CONSTANTES


    // Fichero con los valores de quality de Start Session Server
    public static final String QUALITY_FILE= ISABEL_DIR + FILE_SEPARATOR + "lib/act/act.qualities";	
  	

    public static final String SHARK_IMAGE = "/general/shark.gif";

    
    // CLASSPATH

    public static final String CLASSPATH = System.getProperty("java.class.path");


    // OPERATIVE SYSTEM

    public static final String OS = System.getProperty("os.name");

    

    // TEMPORAL DIR

    public static final String TEMP_DIR = System.getProperty("java.io.tmpdir");



    // ---------------- DIRECTORIOS DE TRABAJO DEL XLIM --------------------

    public static final String XLIM_WORK_DIR = System.getProperty("user.home") + FILE_SEPARATOR + ".xlim" + FILE_SEPARATOR;

    public static final String XLIM_LOGS_WORK_DIR = System.getProperty("user.home") + FILE_SEPARATOR + ".xlim"+ FILE_SEPARATOR + "logs" + FILE_SEPARATOR;

    public static final String XLIM_TMP_WORK_DIR = System.getProperty("user.home") + FILE_SEPARATOR + ".xlim"+ FILE_SEPARATOR + "tmp" + FILE_SEPARATOR;

    public static final String XLIM_CFG_WORK_DIR = System.getProperty("user.home") + FILE_SEPARATOR + ".xlim" + FILE_SEPARATOR + "config" + FILE_SEPARATOR;



    // ---------------- DIRECTORIO DONDE SE GUARDAN LOS XERLS --------------

    public static final String ISABEL_XERLDIR = XLIM_LOGS_WORK_DIR + FILE_SEPARATOR + "xerl";



    // ---------------- DIRECTORIO DE LA AGENDA ----------------------------

    public static final String AGENDA_ROOT_DIR = System.getProperty("agenda.root.dir");

    

    // ---------------- DIRECTORIO DE LOS CONTACTOS ------------------------

    public static final String CONTACTS_ROOT_DIR = System.getProperty("contacts.root.dir");

    

    // ---------------- DIRECTORIO DE TRABAJO DE ISABEL --------------------

    //public static final String ISABEL_WORK_DIR = Constants.ISABEL_USER_DIR + FILE_SEPARATOR;


    

    // ---------------- FICHERO DONDE SE ALMACENAN LOS ULTIMOS VALORES DE LOS DISTINTOS PANELES DE LAS INTERFACES

    public static final String SAVED_VALUES_FILE = XLIM_CFG_WORK_DIR + FILE_SEPARATOR +"svalues.cfg";



    // Nombre del fichero XERL

    public static final String ISABEL_XERLFILE = "error.xerl";



    // --------------- PROGRAMAS EJECUTABLES --------------

    //public static final String ENV_VAR_JAVA_ENDORSED_DIRS = "-Djava.endorsed.dirs=" + System.getProperty("java.endorsed.dirs");

    public static final String ENV_VAR_JAVA_ENDORSED_DIRS = "";

    public static final String ENV_VAR_PRIOR_CONFIG_FILE = "-Dprior.config.file=" + System.getProperty("prior.config.file");

    public static final String ENV_VAR_ISABEL_DIR = "-Disabel.dir=" + System.getProperty("isabel.dir");

    public static final String ENVIROMENT_VARS = " " + ENV_VAR_PRIOR_CONFIG_FILE + " " + ENV_VAR_ISABEL_DIR + " ";

    public static final String JAVA = System.getProperty("java.home") + FILE_SEPARATOR + "bin"+ FILE_SEPARATOR + "java -cp "+ CLASSPATH + " " + ENVIROMENT_VARS +" ";

    public static final String ISABEL_LIM_CLIENT = JAVA +" app.isabel.Client";

    public static final String HIERLAUNCH = JAVA + " services/isabel/services/hierlaunch/Hierlaunch ";

    public static final String ISABEL_CONNECT_TO = JAVA + " services/isabel/services/connect_to/Isabel_connect_to ";

    public static final String XDOWNREQUEST = JAVA + " services/isabel/services/xdownrequest/Xdownrequest ";

    //numero de entradas en el fichero de historial
    public static final int NUMBER_OF_ENTRIES = 10;

//    private static final String ISABEL_FP = "/usr/local/bin/fp";

    public static final String ISABEL_FP = "/usr/local/xlim/bin/checkIrouter";   //se usa en servutilities pero mal



    // GetRunningSession program

    public static final String GET_RUNNING_SESSION = "ps auxw";

    

    // Limite del contador para comprobaciones de procesos en getResultProcess.

    //public static final int MAX_COUNT = 10000;



    // PROGRAMA que cierra una session de isabel....

    public static final String ISABEL_CLEAN_UP = System.getProperty("isabel.dir") + FILE_SEPARATOR + "bin" + FILE_SEPARATOR + "isabel_cleanup";

    

    // Para la funcion getVersion

    public static final String RPM_COMMAND = "rpm -qa | grep ";

    //  private static final String RPM_COMMAND = "ls ";

    public static final String RPM_OUT_FILE = XLIM_TMP_WORK_DIR + "rpm_output.txt";





    // DEFINE EL NOMBRE DEL FICHERO DE TRAZAS

    private static final String TRACES;

    static {
	Calendar nowc = new GregorianCalendar();
	String nows = String.format("%1$tY%1$tm%1$td%1$tH%1$tM%1$tS", nowc);
	
	TRACES = XLIM_LOGS_WORK_DIR + FILE_SEPARATOR +"servutilities."+nows+".log";
    }



    // Fichero de parametros de about (metodo About)

    public static final String REMOTE_VERSION_FILE = XLIM_TMP_WORK_DIR + "about_params.txt";

    public static final String OUTPUT_VERSION_FILE = XLIM_TMP_WORK_DIR + "about_output.txt";



    // Current work directory

    public static final String CURRENT_DIR = System.getProperty("user.dir") + FILE_SEPARATOR;



    // Patrones de error

    public static final String[] XERL_IDENTIFIER = {"</XERL>","</error>"};



    // Programa que indica si una ip es local o no

    public static final String LOCALADD_PROG = System.getProperty("isabel.dir") + FILE_SEPARATOR +"bin" + FILE_SEPARATOR + "isabel_localaddress";

    

    // Nombre del fichero que indica si ISABEL esta corriendo en la version 4.8

    public static final String ISABEL_IS_RUNNING_FILE = ISABEL_USER_DIR + FILE_SEPARATOR + "tmp" + FILE_SEPARATOR + "isabel_is_running";



    // Pragrama utilizado para ver si un proceso esta corriendo

    public static final String CHECK_PROCESS_PROGRAM = "ps";

    

    //  Fichero donde se guarda la salida del programa

    public static final String PROCESS_OUT_FILE = XLIM_TMP_WORK_DIR + "process_output.txt";



    public static final String PROC_UPTIME_FILE = "/proc/uptime";

    // Para la funcion isIsabelRunning

    //public static final String GET_BOOT_TIME = ""  

    

    // Trazas de salida del isabel_launch

    public static final String ISA_LAUNCH_LOG = Constants.ISABEL_SESSION_LOGS_DIR + FILE_SEPARATOR + "isabel_launch.log";


    // Lanzador de la ANTENNA y RECORDER (launchLocalFromXedl)

    public static final String ANTENNA_LAUNCHER = System.getProperty("isabel.dir") + FILE_SEPARATOR + "bin" + FILE_SEPARATOR + "antenna.sh";

    public static final String RECORDER_LAUNCHER = System.getProperty("isabel.dir") + FILE_SEPARATOR + "bin" + FILE_SEPARATOR + "recorder.sh";



    

    // Fichero de parametros para el metodo get remote running session (getRemoteRunningSession)

    public static final String GET_REMOTE_SESSION_PARAMS = XLIM_TMP_WORK_DIR + FILE_SEPARATOR +"remotesession.params";

    public static final String GET_REMOTE_SESSION_LOG = XLIM_TMP_WORK_DIR + FILE_SEPARATOR +"remotesession.output";


    // Fichero de parametros para el metodo launch iperf server (launchIperfServer)
    
    public static final String LAUNCH_IPERF_SERVER = "launchIperfServer";

    public static final String LAUNCH_IPERF_SERVER_PARAMS = XLIM_TMP_WORK_DIR +"launchIperfServer.params";
    
    public static final String LAUNCH_IPERF_SERVER_PARAMS_FILE_NAME = "launchIperfServer.params";

    public static final String LAUNCH_IPERF_SERVER_REPLAY = XLIM_TMP_WORK_DIR +"launchIperfServer.output";
    

    //  Ficheros del servicio LinksStatusService
    public static final String LINKS_STATUS_SERVICE = "linksStatusService";

    /*public static final String LINKS_STATUS_SERVICE_PARAMS = XLIM_TMP_WORK_DIR +"linksStatusService.params";
    
    public static final String LINKS_STATUS_SERVICE_PARAMS_FILE_NAME = "linksStatusService.params";
*/
    public static final String LINKS_STATUS_SERVICE_OUTPUT = XLIM_TMP_WORK_DIR +"linksStatusService.output";

    public static final String LINKS_STATUS_SERVICE_OUTPUT_FILE_NAME = "linksStatusService.output";
    
    public static final String LINKS_STATUS_SERVICE_SITES = "linksStatusService.sites";
    
    public static final String LINKS_STATUS_SERVICE_SITES_FILE = XLIM_TMP_WORK_DIR + "linksStatusService.sites";
    

    // Ficheros donde se guardan los XERLs que se genera en caso de error para cada servicio

    public static final String XERL_DIR = XLIM_LOGS_WORK_DIR + FILE_SEPARATOR +"xerl"+ FILE_SEPARATOR;

    public static final String LOAD_XEDL_XERL = XERL_DIR +"lxedl.xerl";

    public static final String CONNECT_TO_XERL = XERL_DIR +"connectto_c.xerl";

    public static final String SERVER_CONNECT_TO_XERL = XERL_DIR +"connectto_s.xerl";

    public static final String IRIS_XERL = XERL_DIR +"iris_c.xerl";

    public static final String SERVER_IRIS_XERL = XERL_DIR +"iris_s.xerl";

    public static final String XDOWNREQUEST_XERL = XERL_DIR +"xdownrequest_c.xerl";

    public static final String SERVER_XDOWNREQUEST_XERL = XERL_DIR +"xdownrequest_s.xerl";

    public static final String HIERLAUNCH_XERL = XERL_DIR +"hierlaunch_c.xerl";

    public static final String SERVER_HIERLAUNCH_XERL = XERL_DIR +"hierlaunch_s.xerl";

    public static final String WEBRELAUNCH_XERL = XERL_DIR +"webrelaunch_c.xerl";

    public static final String SERVER_WEBRELAUNCH_XERL = XERL_DIR +"webrelaunch_s.xerl";

    public static final String SESSION_LOCKER_XERL = XERL_DIR +"sessionlocker.xerl";

    public static final String SERVER_SESSION_LOCKER_XERL = XERL_DIR +"sessionlocker_s.xerl";

    public static final String SYNCRO_SLIDES_XERL = XERL_DIR +"syncroslides.xerl";

    public static final String SERVER_SYNCRO_SLIDES_XERL = XERL_DIR +"syncroslides_s.xerl";

    public static final String COPY_PROGRESS_XERL = XERL_DIR +"cprogress.xerl";

    public static final String C2c_XERL = XERL_DIR +"c2c.xerl";

    public static final String CONTACTS_MGR_XERL = XERL_DIR +"contactsmgr.xerl";

    public static final String SESSION_INFO_XERL = XERL_DIR +"sinfo.xerl";

    public static final String SSS_XERL = XERL_DIR +"sss.xerl";
    
    //ENRIQUE, añadido para el servicio SimpleMaster
    public static final String SM_XERL = XERL_DIR + "sm.xerl";

    public static final String PROGRAM_UPDATER_XERL = XERL_DIR +"pupdater.xerl";

    public static final String ILAUNCHER_THREAD  = XERL_DIR +"ilauncher_thread.xerl";

    public static final String SIMPLE_LAUNCHER_XERL = XERL_DIR +"simple_launcher.xerl";
    
    public static final String INVITE_TO_XERL = XERL_DIR +"invite.xerl";

    

    
    
    
    
    
    
    
    
    
    /**
	 * configuración de trazas de jaxb, parseo del xedl
	 */
	private static final String JAXB_TRACES = XLIM_LOGS_WORK_DIR + FILE_SEPARATOR +"jaxb.log";
	private static PrintWriter outTraceFile= null;
	private static boolean ya_inicializadas = false;
    /**
     * Metodo para inicializar las trazas de jaxb y que todo lo que tiene que ver con el parseo de xedl vaya al mismo fichero
     * el fichero es JAXB_TRACES
     * intención de sólo inicializarlas una vez por ejecución de isabel
     *
     */
    public static PrintWriter inicializa_trazas_jaxb()
    {
    	if(ya_inicializadas==true)
    		return outTraceFile;
		else
		{
			try {
				outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (JAXB_TRACES)),true);
				ya_inicializadas = true;
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
    	return outTraceFile;
    }
    
    
    //metodo de prueba, simplemente imprime algunas variables de las fijadas arriba
    public static void main(String args[]){
    	
    	System.out.println("ISABEL_DIR es:" + ISABEL_DIR);
    	System.out.println("ISABEL_USER_DIR es:" + ISABEL_USER_DIR);
    	System.out.println("ISABEL_PROFILES_DIR es:" + ISABEL_PROFILES_DIR);
    	System.out.println("ISABEL_CONFIG_DIR es:" + ISABEL_CONFIG_DIR);
    	System.out.println("ISABEL_SESSIONS_DIR es:" + ISABEL_SESSIONS_DIR);    	
    	
    }
    
}
