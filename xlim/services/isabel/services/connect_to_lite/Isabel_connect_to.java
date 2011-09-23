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

// $Id: Isabel_connect_to.java 18434 2009-09-16 13:53:13Z gabriel $

/////////////////////////////////////////////////////////////////////////

package services.isabel.services.connect_to_lite;

import isabel.tunnel.TunnelUtils;
import isabel.tunnel.Tunnelator;

import java.io.*;
import java.net.*; 

import org.w3c.dom.Document;

// Para el manejo de logs
//import isabel.lib.Logs;

// Para el manejo de la Agenda
import xedl.lib.jaxb.Edl;
import xedl.lib.jaxb.SESSION;
import xedl.lib.jaxb.SITE;
import xedl.lib.xedl.*;

// For write operation

// Para el manejo de menus
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

import javax.swing.*;

// Para el manejo de horas..
import java.text.SimpleDateFormat;
import java.util.*; 

import services.isabel.lib.*;

import services.isabel.services.guis.*;
import services.isabel.services.options.Options;

/**
 * Cliente del servicio Connect To. A partir de los parametros recogidos por
 * linea de comandos o con el panel grafico se realiza el proceso de suscripcion 
 * con el flowserver dado: se genera un xedl parcial, se envia al flow y se espera
 * recibir un XEDL completo con el que se lanza ISABEL.
 * @Author lailoken
 */
public class Isabel_connect_to { 

	static Document document;
	static Document optionsDocument;
	static Document localDocument;


	// Nombres de ficheros para guardar las trazas
	//private static final String ISABEL_GENERAL_CONNECT_TO_TRACES = Servutilities.XLIM_LOGS_WORK_DIR + Servutilities.FILE_SEPARATOR +"isacto_general.log";
	private static final String ISABEL_CONNECT_TO_TRACES = Constants.XLIM_LOGS_WORK_DIR + Constants.FILE_SEPARATOR +"connect_to.log";
	private static final String ISABEL_REFRESH_TO_TRACES = Constants.XLIM_LOGS_WORK_DIR + Constants.FILE_SEPARATOR +"isacto_refresh.log";
	private static final String ISABEL_RECONNECT_TO_TRACES = Constants.XLIM_LOGS_WORK_DIR + Constants.FILE_SEPARATOR +"isacto_reconnect.log";

	// Fichero de parametros de connect to
	private static final String CONNECT_PARAMS = Constants.XLIM_TMP_WORK_DIR + Constants.FILE_SEPARATOR +"connect_to.params";

	private static final String DELIVERY_PLATFORM = "isabel";

	private static final String USAGE = "Usage: \r\n   isabel_connect_to --graphical\r\n or\r\n   isabel_connect_to --url URL --id ID [--passwd PASSWD] --action ACTION [--graphical] [--slides] [--location LOCATION] [--oldpasswd OLD_PASSWD]";

	//private static final String[] XERL_IDENTIFIER = {"</XERL>","</error>"};

	/**
	 * Copia el fichero origen en el fichero destino.
	 * @param fileInName Path del fichero origen.
	 * @param fileOutName Path del fichero destino.
	 * @return True, si la copia tuvo exito; False, en caso contrario
	 * @Author lailoken
	 */
	static private boolean copyFile (String fileInName, String fileOutName){
		try {
			FileInputStream in = new FileInputStream(fileInName);
			FileOutputStream out = new FileOutputStream(fileOutName);
			byte buffer[] = new byte[16];
			int n;
			while ((n=in.read(buffer))>-1)
				out.write(buffer,0,n);
			out.close();
			in.close();
			return true;
		} catch (Exception e) {
			return false;
		}
	}


	/**
	 * llama al programa Options
	 *
	 */
	static private void editLocalConfiguration()
	{	   
		final Options the_options = new Options(true);
		the_options.pack();
		the_options.setVisible(true);	  
	}


	/**
	 * Copy the file options_default.xml from /usr/local/isabel to .isabel/config/profiles
	 * @throws Exception if default file does not exist
	 * 
	 */
	static private void copyOptionsDefault(File optionsFile) throws Exception
	{
		File opdefault = new File(Constants.ISABEL_DIR + Constants.FILE_SEPARATOR + "lib" + Constants.FILE_SEPARATOR + "xlimconfig" + Constants.FILE_SEPARATOR + "options_Default.xml" );
		if(!opdefault.exists())
		{
			throw new Exception ("The file options_default.xml does not exist in: " + Constants.ISABEL_DIR + Constants.FILE_SEPARATOR + "lib" + Constants.FILE_SEPARATOR + "xlimconfig" + Constants.FILE_SEPARATOR);
		}
		copyFile(opdefault.getAbsolutePath(), optionsFile.getAbsolutePath());
	}


	public static void main (String args[]) throws MalformedURLException,Exception { 
		// Nos aseguramos de que existen los directorios de trabajo
		File xlimLogsWorkDir = new File(Constants.XLIM_LOGS_WORK_DIR);
		if (!xlimLogsWorkDir.exists()) xlimLogsWorkDir.mkdirs();
		File xlimTmpWorkDir = new File(Constants.XLIM_TMP_WORK_DIR);
		if (!xlimTmpWorkDir.exists()) xlimTmpWorkDir.mkdirs();
		File xlimCfgWorkDir = new File(Constants.XLIM_CFG_WORK_DIR);
		if (!xlimCfgWorkDir.exists()) xlimCfgWorkDir.mkdirs();
		File xlimXerlWorkDir = new File(Constants.XLIM_LOGS_WORK_DIR+Constants.FILE_SEPARATOR+"xerl");
		if (!xlimXerlWorkDir.exists()) xlimXerlWorkDir.mkdirs();

		//Borramos el fichero antiguo de error
		(new File(Constants.CONNECT_TO_XERL)).delete();

		// Creamos el recolector de basuras de ficheros
		FileGarbageCollector fileGarbageCollector = new FileGarbageCollector(); 

		// Variables para generar la agenda y solicitar la suscripcion
		String connect_id = "";
		String connect_location = "";
		String connect_passwd = "";
		//String encrypted_passwd = "";
		String connect_action = "";
		String connect_url = "";
		String url_FS = "";
		String url_session = "";
		String urlDelPlatform = DELIVERY_PLATFORM;
		// Almacena el ultimo nickname que se utilizo para una determinada sesion
		String oldID = "";
		String oldPasswd = "";

		// IsGraphical idica si se invoco con la opcion --graphical
		boolean IsGraphical = false;

		// Indica si en el url hay delivery platform definido
		boolean isDefinedDelPlatform = false;

		// indican que accion se ha solicitado al cliente
		boolean CONNECT = false;
		boolean RECONNECT = false;

		// Donde mandamos los mensajes de trazas: 
		PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (ISABEL_CONNECT_TO_TRACES,true)),true);

		// XLIM Notifications
		XlimNotification xlimNotif = new XlimNotification("Connect To Session");


		PrintWriter outMsgFile = new PrintWriter (System.out,true);

		// Para trazas horarias....
		SimpleDateFormat timeFormatter = new SimpleDateFormat("HH:mm:ss.SSS");

		outTraceFile.println ("Before creating Agenda: " + timeFormatter.format(new Date(System.currentTimeMillis())));

		//Creamos el objeto para manejar la Agenda.
		Agenda IsabelAgenda = null;
		try {
			IsabelAgenda = new Agenda (Constants.ISABEL_USER_DIR + Constants.FILE_SEPARATOR); //PARA Linux    
			//IsabelAgenda = new Agenda ("isabel/"); //PARA WIN    
		} catch (Exception e){
			e.printStackTrace(outTraceFile);
			Servutilities.writeErrorMessage ("localhost","Connect To","Error while opening Agenda.","Error while opening Agenda.\n" + e.getMessage(),IsGraphical,Constants.CONNECT_TO_XERL);
			outTraceFile.println("Vamos a hacer System.exit(0) peligro");
			System.exit(0);
			return;
		}

		outTraceFile.println ("Before processing parameters: " + timeFormatter.format(new Date(System.currentTimeMillis())));
		// --------------------------------------------
		// ----- GESTION DE PARAMETROS DE ENTRADA -----
		// --------------------------------------------
		if ((args.length != 1) && (args.length != 3)&& (args.length != 6) && (args.length != 7) && (args.length != 8) && (args.length != 9) && (args.length != 10) && (args.length != 12) && (args.length != 14)){
			System.out.println(USAGE);
			//Servutilities.writeMsgError ("ERROR: missing parameters.",CONNECT_TO_XERL,"Connect To",IsGraphical);
			Servutilities.writeErrorMessage ("localhost","Connect To","Missing parameters.","Found " + args.length + " parameters. Number of parameters allowed: 1,6,7,8 or 9.",IsGraphical,Constants.CONNECT_TO_XERL);
			outTraceFile.println("Vamos a hacer System.exit(0) peligro");
			System.exit(0);
			return;
		} else { // Separamos las dos opciones posibles (graphical o no graphical)
			//Creo el panel que quiero mostar, los creo fuera del if de modo gráfico
			//Comprobamos que el options.xml existe, Si no existe sacamos un mensaje de error.
			File optionsFile = IsabelAgenda.getOptionsFile();

			if (!optionsFile.exists()) {            
				//editLocalConfiguration();
				try{
					copyOptionsDefault(optionsFile);
				}
				catch(Exception e)
				{
					outTraceFile.println("Default Options file does not exist, we give the user the possibility to create it");
					editLocalConfiguration();
				}

			}

			boolean url_present = false;
			String the_url = "";
			if (args.length==3 && args[1].equals("--url_file")) {
				System.out.println("LLAMADA A ICTO_LAUNCHER");
				String url_file_path = args[2];
				System.out.println("argumento pasado: " + url_file_path); 


				//hay dos posibilidades para el icto_launcher o lanzado automatico desde explorador
				//que sea un fichero .icto o que sea la propia url
				if(url_file_path.endsWith(".icto"))
				{
					System.out.println("Es un fichero .icto, lo leemos");
					LineNumberReader urlLine = null;
					try {
						urlLine = new LineNumberReader(new FileReader(url_file_path));    			
						the_url = urlLine.readLine();
					} catch (FileNotFoundException e) {
						e.printStackTrace();
					}
					catch (IOException e) {
						e.printStackTrace();
					}
					url_present = true;
				}
				else
				{
					System.out.println("Es la url, la cogemos");
					url_present = true;
					the_url = url_file_path;
				}
			} 
			XlimServicePanel xlimServicePanel = null;
			if (url_present){
				xlimServicePanel = new XlimServicePanel(ServiceDefinitions.CONNECT_TO_SERVICE, the_url);
			}
			else {
				xlimServicePanel = new XlimServicePanel(ServiceDefinitions.CONNECT_TO_SERVICE);
			}
			SessionDataPanel_connect_to sessionDataPanel = (SessionDataPanel_connect_to) xlimServicePanel.getPrimaryPanel();
			SiteDataPanel siteDataPanel = (SiteDataPanel) xlimServicePanel.getSecondaryPanel();        
			if (args.length==1 || args.length==3) { // Modo grafico (o grafico + url)
				//Comprobamos que es la opcion correcta:
				if (args[0].equals("--graphical")) { // Todo bien:        	       		
					boolean passwdOk = false;
					while (connect_url.equals("") || connect_id.equals("") || !passwdOk) {
						// LOS DATOS SE RECOGEN DE UN MENU y LOS MENSAJES SE PRESENTAN EN FORMA GRAFICA
						IsGraphical = true;
						JFrame parentFrame = new JFrame();      
						// Lanzamos el menu y esperamos la recogida de datos....
						Object[] message = {xlimServicePanel};

						// Llamamos a JOptionPane	
						Object[] options = {"Connect", "Cancel"};

						final JOptionPane opti = new JOptionPane( message, JOptionPane.DEFAULT_OPTION,JOptionPane.PLAIN_MESSAGE,null,options,options[0]);
						final JDialog dialog = new JDialog(parentFrame, "Connect To Session...", true);
						dialog.setContentPane(opti);
						dialog.setLocation(75, 100);
						sessionDataPanel.setOptionPane(opti);
						opti.addPropertyChangeListener(new PropertyChangeListener() {
							public void propertyChange(PropertyChangeEvent e) {
								String prop = e.getPropertyName();

								if (dialog.isVisible() && (e.getSource() == opti) && (prop.equals(JOptionPane.VALUE_PROPERTY))) {
									//If you were going to check something
									//before closing the window, you'd do
									//it here.
									dialog.setVisible(false);
								}
							}
						});

						dialog.pack();
						dialog.setVisible(true);
						String resultPanel = (String)opti.getValue();

						if (!resultPanel.equals("Connect")) {
							parentFrame.dispose();
							outTraceFile.println("Vamos a hacer System.exit(0) peligro");
							System.exit(0);
							return;
						}

						// borramos las sesiones anteriores
						Servutilities.deleteDirectory(IsabelAgenda.remoteDir);

						connect_url = sessionDataPanel.getSessionURL();
						connect_id = siteDataPanel.getSiteID();
						connect_location = siteDataPanel.getSiteLocation();
						connect_passwd = sessionDataPanel.getSitePasswd();
						connect_action = "connect";

						if (connect_url.equals("")) {
							// Estos parametros no pueden ser vacio
							JOptionPane.showMessageDialog(parentFrame, "ERROR: Url missing.", "Connect To", JOptionPane.ERROR_MESSAGE);
							//vuelvo a llamar a connect to y salgo
							String[] argums = {"--graphical"};
							main(argums);
							outTraceFile.println("Vamos a hacer System.exit(0) peligro");
							System.exit(0);
							return;      	
						}
						if (connect_id.equals("")) {
							// Estos parametros no pueden ser vacio
							JOptionPane.showMessageDialog(parentFrame, "ERROR: Institution NickName missing.", "Connect To", JOptionPane.ERROR_MESSAGE);
							//vuelvo a llamar a connect to y salgo
							String[] argums = {"--graphical"};
							main(argums);
							outTraceFile.println("Vamos a hacer System.exit(0) peligro");
							System.exit(0);
							return;
						}
						// Comprobamos que el passwd es correcto:
						oldPasswd = siteDataPanel.getOldPasswd();
						if (!oldPasswd.equals("") && (!Crypt.checkPasswd(connect_passwd,oldPasswd))) {
							// Parametro incorrecto
							JOptionPane.showMessageDialog(parentFrame,"ERROR: Permission denied. Check Password.",
									"Connect To", JOptionPane.ERROR_MESSAGE);
						} else
							passwdOk = true;

						// Guardamos los valores seleccionados.
						try {
							sessionDataPanel.saveValues();
						} catch (Exception e) {
							outTraceFile.println("Problems saving values");
						}
						// ENRIQUE
						// salvamos el nick y location en el profile
						File theoptionsFile;
						try {
							theoptionsFile = new Agenda().getOptionsFile(
									(String) siteDataPanel.profile_combo.getSelectedItem());

							if (!theoptionsFile.exists())
							{
								outTraceFile.println("Vamos a hacer System.exit(0) peligro");
								System.exit(0);	
								return;
							}

							XEDL optionsXEDL = new XEDL(theoptionsFile.getAbsolutePath());
							String sitename = (String) new SiteList(optionsXEDL).getSiteIds().getFirst();
							SITE optionsSite = new SiteList(optionsXEDL).getSite(sitename);
							optionsSite.setPUBLICNAME(connect_id + "."+ connect_location);
							optionsSite.setID(connect_id);
							optionsXEDL.save( theoptionsFile.getAbsolutePath());
						} catch (IOException e) {
							e.printStackTrace();
						} catch (XEDLException e) {
							e.printStackTrace();
						}					
						parentFrame.dispose();
					}
				} else { // Error al invocar el cliente
					System.out.println(USAGE);
					Servutilities.writeErrorMessage("localhost","Connect To","Missing parameters.","Found "
							+ args.length+ " parameters. Number of parameters allowed: 1,6,7,8 or 9.",
							IsGraphical, Constants.CONNECT_TO_XERL);
					outTraceFile.println("Vamos a hacer System.exit(0) peligro");
					System.exit(0);
					return;
				}
			} else { // Parametros por linea de comandos
				// LOS DATOS SE RECOGEN POR LINEA DE COMANDOS Y si hay opcion --graphical LOS MENSAJES SE PRESENTAN EN FORMA GRAFICA
				// Comprobamos que se han introducido todos los parametros
				boolean ISPARAMurl = false;
				boolean ISPARAMid = false;
				boolean ISPARAMpasswd = false;
				boolean ISPARAMaction = false;
				for (int i = 0; i <args.length; i+=2) { // incrementamos i en 2 porque son pares variable-valor
					if (args[i].equals("--url")) {
						ISPARAMurl = true;
						connect_url = args[i+1];
					}
					if (args[i].equals("--id")) {
						ISPARAMid = true;
						connect_id = args[i+1];
					}
					if (args[i].equals("--passwd")) {
						ISPARAMpasswd = true;
						connect_passwd = args[i+1];
					}
					if (args[i].equals("--location")) {
						connect_location = args[i+1];
					}
					if (args[i].equals("--oldid")) {
						oldID = args[i+1];
					}
					if (args[i].equals("--oldpasswd")) {
						oldPasswd = args[i+1];
					}
					if (args[i].equals("--graphical")) {
						IsGraphical = true;
					}
					if (args[i].equals("--action")) {
						ISPARAMaction = true;
						connect_action = args[i+1].toLowerCase();
					}
				}
				if (!ISPARAMpasswd) {
					ISPARAMpasswd = true;
					connect_passwd = "";
				}
				if (!(ISPARAMurl && ISPARAMid && ISPARAMpasswd && ISPARAMaction)) { // Si alguna de las variables es false, faltan parametros
					System.out.println("Error: missing parameters");
					System.out.println(USAGE);
					//Servutilities.writeMsgError ("ERROR: Missing parameters",CONNECT_TO_XERL,"Connect To",IsGraphical);
					Servutilities.writeErrorMessage ("localhost","Connect To","Missing parameters.","Found " + args.length + " parameters. Number of parameters allowed: 1,6,7,8 or 9.",IsGraphical,Constants.CONNECT_TO_XERL);
					outTraceFile.println("Vamos a hacer System.exit(0) peligro");
					System.exit(0);
					return;
				}
			}
			// En este punto ya tenemos almacenados en las variables connect_* los parametros necesarios para iniciar el proceso de suscripcion.
			//    connect_url, connect_id, connect_passwd,  connect_action
			// ---------------------------------
			// ----- PROCESANDO PARAMETROS -----
			// ---------------------------------
			try {
				// ----------
				outTraceFile.println ("After processing parameters: " + timeFormatter.format(new Date(System.currentTimeMillis())));
				// ----------

				xlimNotif.println("Obtaining subscription information.");

				// Si el location es "" lo ponemos a null
				if (connect_location.equals("")) connect_location = null;

				if (connect_action.equals("connect")) {
					CONNECT = true;
					outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (ISABEL_CONNECT_TO_TRACES, true)),true);
				}
				if (connect_action.equals("reconnect")) {
					RECONNECT = true;  // se usa cuando le han cambiado el flow y tiene que reconectar
					connect_action = "connect";
					outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (ISABEL_RECONNECT_TO_TRACES, true)),true);
				}
				if (connect_action.equals("refresh")) {
					outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (ISABEL_REFRESH_TO_TRACES, true)),true);
				}

				//-------------------------------------------------------------------------//
				//-------------- chequeamos las conexiones ISDN e IPSEC -------------------//
				//-------------------------------------------------------------------------//
				//uso como fichero de options el Default, si se quiere usar otro habría que cambiar el main
				//y pasárlo como parámetro
				optionsFile = IsabelAgenda.getOptionsFile((String) siteDataPanel.profile_combo.getSelectedItem());
				if (optionsFile.exists()) {
					// Si existe el fichero de options, deberiamos porder leer las opciones relativas a ISDN e IPSEC, asi que adelante
					String siteName ;
					try {
						XEDL optionsXEDL = new XEDL ( optionsFile.getAbsolutePath());
						siteName = new SiteList(optionsXEDL).getSiteIds().get(0).toString(); // En las options solo hay un sitio definido
					} catch (XEDLException xedle) {
						xlimNotif.println(" Connections Check Failed");
						xedle.printStackTrace(outTraceFile);
						Servutilities.writeErrorMessage ("localhost","Connect To","Problems reading information from Options." ,"Problems reading information from Options.\n" + xedle.getMessage(),IsGraphical,Constants.CONNECT_TO_XERL);
						outTraceFile.println("Vamos a hacer System.exit(0) peligro");
						System.exit(0);
						return;
					}

					try {
						ConnectionsManager.startISDNIfNeeded(optionsFile.getAbsolutePath(),siteName); 
					} catch (XEDLException xedle) {
						xlimNotif.println(" ISDN checking process Failed");
						xedle.printStackTrace(outTraceFile);
						Servutilities.writeErrorMessage ("localhost","Connect To","Problems checking ISDN." ,"Problems checking ISDN.\n" + xedle.getMessage(),IsGraphical,Constants.CONNECT_TO_XERL);
						outTraceFile.println("Vamos a hacer System.exit(0) peligro");
						System.exit(0);
						return;
					} catch (IOException ioe) {
						xlimNotif.println(" ISDN checking process Failed");
						ioe.printStackTrace(outTraceFile);
						Servutilities.writeErrorMessage ("localhost","Connect To","Problems checking ISDN." ,"Problems checking ISDN.\n" + ioe.getMessage(),IsGraphical,Constants.CONNECT_TO_XERL);
						outTraceFile.println("Vamos a hacer System.exit(0) peligro");
						System.exit(0);
						return;
					}
				}

				// ----- Sacamos del URL la direccion del FlowServer y el nombre de la sesion: -----
				// ---------------------------------------------------------------------------------
				String[] url_total_parts;
				String[] url_parts;
				
				url_total_parts = connect_url.split("//"); // con esto separamos el protocolo (isabel://) del resto de la url
				if (url_total_parts.length == 2) {
					// La url contiene el protocolo: isabel://
					outTraceFile.println (" First part: " + url_total_parts[0] + ". Last part: " + url_total_parts[1]);
					urlDelPlatform = url_total_parts[0];
					url_parts = url_total_parts[1].split("/");
					isDefinedDelPlatform = true;
					System.out.println("ignorar esta linea");
				} else {
					// La url no contiene el protocolo: isabel://
					outTraceFile.println (" There is no isabel://. we have only this " + url_total_parts[0]);
					url_parts = url_total_parts[0].split("/");
				}   
				
				
				
				String url_pure = "";
				String port = null;
				//first we detect if it is IPv6 or IPv4
				int veces = new StringTokenizer(url_parts[0], ":").countTokens()-1;
				
				if(veces>1){
					//IPv6
					String[] url_and_port = url_parts[0].split("]");
					if(url_and_port.length==2){
						url_pure = url_and_port[0].substring(1);
						port = url_and_port[1].substring(1);
					}
					else
					{
						url_pure = url_and_port[0];
						if(url_pure.startsWith("["))
						{
							url_pure = url_pure.substring(1);
						}
					}
				}
				else
				{
					//IPv4
					//NEW CODE to allow user to add the port to the URL
					//like: isabel://triton.dit.upm.es:8080/sesion_name or isabel://triton.dit.upm.es:8080
					//or triton.dit.upm.es:8080
					String[] url_and_port = url_parts[0].split(":");
					url_pure = url_and_port[0];
					if(url_and_port.length==2){
						//there is explicit port
						port = url_and_port[1];
					}		
					
				}
				
				//Registers a shutdown hook to:
				//  - close all the tunnels that this instance has opened
				Runtime.getRuntime().addShutdownHook(new CloseTunnel(url_pure));

				//Parsear la url
				url_FS = url_pure.trim();
				url_session = "CT_LITE_SESSION";
					
				outTraceFile.println ("url: " + connect_url + " id: " + connect_id + " passwd: " + "no_lo_pongo" + " action: " + connect_action + " FS: " + url_FS + " session: " + url_session);

				//ENRIQUE salvo la url en el fichero de historial
				String history_file = Constants.ISABEL_SESSIONS_DIR + Constants.FILE_SEPARATOR + Constants.FICHERO_HISTORIAL_CONNECT_TO;
				Servutilities.add_history_entry(connect_url,history_file);


				// Comprobamos que el flowserver introducido NO es localhost. Si lo es, error y salimos.
				if (Servutilities.isLocalHost(url_FS)) {
					outTraceFile.println("Flow is local address.");
					Servutilities.writeErrorMessage ("localhost","Connect To","Error in URL: " + connect_url,"The address given in the URL is local. Please, get in contact with the session administrator to obtain a valid URL.",IsGraphical,Constants.CONNECT_TO_XERL);
					outTraceFile.println("Vamos a hacer System.exit(0) peligro");
					Tunnelator.getTunnelator().deleteTunnel(url_FS);
					System.exit(0);
					return;
				} else outTraceFile.println("Flow is not local address.");

				// Actualizamos el fichero de ultimos valores...
				try {
					String[] params = {"CONNECT_TO-url", connect_url};
					Servutilities.saveParamsToFile(Constants.SAVED_VALUES_FILE,params);
				} catch (FileNotFoundException fnfe) {
					fnfe.printStackTrace(outTraceFile);
				} catch (IOException ioe) {
					ioe.printStackTrace(outTraceFile);
				}

				// ----- Calculamos la IP que vamos a utilizar para esta conexion -----
				// --------------------------------------------------------------------
				String localAddress = Servutilities.getSourceAddress(url_FS);
				outTraceFile.println("We are using this IP: " + localAddress);
				if (localAddress == null) {
					// Limpiamos
					fileGarbageCollector.cleanGarbage();
					//Servutilities.writeMsgError ("Unable to connect to: " + url_FS,CONNECT_TO_XERL,"Connect To",IsGraphical);
					Servutilities.writeErrorMessage ("localhost","Connect To","Unable to connect to: " + url_FS,"The Address \"" + url_FS + "\" could not be resolved.",IsGraphical,Constants.CONNECT_TO_XERL);
					outTraceFile.println("Vamos a hacer System.exit(0) peligro");
					Tunnelator.getTunnelator().deleteTunnel(url_FS);
					System.exit(0);
					return;
				}

				// ----------
				outTraceFile.println ("Before checking remote or local session: " + timeFormatter.format(new Date(System.currentTimeMillis())));
				// ----------

				// Comprobamos si esta definida esta sesion como local (directorio master); si existe, la borramos
				if (IsabelAgenda.isLocalSession(url_session)) {
					IsabelAgenda.deleteSession(url_session);
				}

				if (args.length == 1) { // Solo para el modo comandos (no Modo grafico)
					// Guardamos el antiguo nickname utilizado para esta sesion, por si es necesario eliminar esa suscripcion
					try {
						XEDL xedl = new XEDL( IsabelAgenda.getLocalXedl(url_session).getAbsolutePath());
						SiteList lista = new SiteList(xedl);
						oldID = lista.getSiteIds().get(0).toString();
						oldPasswd = lista.getSite(oldID).getAccessControl();

						// Si es distinto de null, el sitio tenia passwd...
						if (oldPasswd != null) {
							//Hay que sacar lo que es el passwd en si. El string que nos devuelve el getAccessControl tiene dos partes de las cuales nos interesa la segunda.
							oldPasswd = oldPasswd.split(" ")[1];
							// Comprobamos si los passwds son correctos
							if (!Crypt.checkPasswd(connect_passwd,oldPasswd)) {
								// Error, Authentication failed. bad password.
								// Limpiamos
								fileGarbageCollector.cleanGarbage();
								//Servutilities.writeMsgError ("Unable to connect to: " + url_FS,CONNECT_TO_XERL,"Connect To",IsGraphical);
								Servutilities.writeErrorMessage (connect_id,"Connect To","Authentication failed.","Bad Password.",IsGraphical,Constants.CONNECT_TO_XERL);
								outTraceFile.println("Vamos a hacer System.exit(0) peligro");
								Tunnelator.getTunnelator().deleteTunnel(url_FS);
								System.exit(0);
								return;
							}
						} else {
							oldPasswd = "";
						}
					} catch (Exception nickException) {
						//Suponemos que no hubo una conexion a esta sesion anterior, por lo que no hay nickname antiguo.
						//Ponemos el nickname actual
						oldID = connect_id;
						oldPasswd = "";
					}
				}
				outTraceFile.println("Old Nickname: " + oldID);

				// ----------
				outTraceFile.println ("Before composing local.xedl: " + timeFormatter.format(new Date(System.currentTimeMillis())));
				// ----------

				// GENERAMOS EL LOCAL.XEDL //
				outTraceFile.println ("url FS before changing it: " + url_FS);
				url_FS = Servutilities.getIpByName(url_FS);
				outTraceFile.println ("url FS after changing it: " + url_FS);
				outTraceFile.println ("Composing local.xedl: " + timeFormatter.format(new Date(System.currentTimeMillis())));
				outTraceFile.println ("Running composeLocal with session " + url_session + " id " + connect_id + " passwd " + "no lo pongo" + " localAddress " + localAddress + " and flow " + url_FS);
				String profile_name = (String) siteDataPanel.profile_combo.getSelectedItem();

				// si el passwd no es null lo encriptamos
				String encrypted_passwd = null;
				XEDL xedl = null;
				if(connect_passwd != null)
				{
					String cipher_key = Constants.KEY_CODE + connect_id;
					//ofuscated_passwd = connect_passwd + cipher_key;
					DesEncrypter encrypter = new DesEncrypter(cipher_key);
					// Encrypt
					encrypted_passwd = encrypter.encrypt(connect_passwd);       
					xedl = XEDLMixerTools.composeLocal(url_session, connect_id, connect_location, encrypted_passwd, profile_name, localAddress, url_FS, true);
				}
				else
				{
					xedl = XEDLMixerTools.composeLocal(url_session, connect_id, connect_location, connect_passwd, profile_name, localAddress, url_FS, true);
				}
				
				
				Edl edl = xedl.getEdl();
				SESSION session = new SESSION();
				session.setID(url_session);
				session.setDELIVERYPLATFORM("");				
				session.setSERVICENAME("meeting.act");
				session.setSERVICEQUALITY("1M");
				session.setOPENSESSION(true);
				session.setACCESSCONTROL("");
				//vemos que version de isabel estamos usando y la metemos en DeliveryPlatform
				String deliv = "Isabel " + Servutilities.getIsabelVersionClear() + " " + Servutilities.getIsabelRelease();
				session.setDELIVERYPLATFORM(deliv);								
				edl.setSESSION(session);		
				xedl.setEdl(edl);
				xedl.save();
				// ----------
				outTraceFile.println ("Composing local.xedl2: " + timeFormatter.format(new Date(System.currentTimeMillis())));
				// ----------

				String localXedlPath = IsabelAgenda.getLocalXedl(url_session).getPath();
				// ----------
				outTraceFile.println ("Composing local.xedl3: " + timeFormatter.format(new Date(System.currentTimeMillis())));
				// ----------

				File localXedlFile = new File (localXedlPath);

				// ----------
				outTraceFile.println ("Before composing url file: " + timeFormatter.format(new Date(System.currentTimeMillis())));
				// ----------

				// Actualizamos el fichero url Solo si no la accion no es reconnect
				if (!RECONNECT) {
					FileOutputStream url_fileOut = new FileOutputStream(IsabelAgenda.getURLFile(url_session));
					String addedPlatform = urlDelPlatform + "://";
					if (!isDefinedDelPlatform) url_fileOut.write(addedPlatform.getBytes());
					url_fileOut.write(connect_url.getBytes());
				}

				// Metemos los parametros en un fichero:
				String fileParamName = CONNECT_PARAMS;
				String[] params = {"file",localXedlFile.getName(),"action",connect_action,"session",url_session,"sourceip",localAddress,"oldid",oldID};
				Servutilities.saveParamsToFile(fileParamName, params, "Connect To Params");
				if (!oldPasswd.equals("")) Servutilities.saveParamToFile(fileParamName, "oldpasswd", oldPasswd);
				// Metemos el fichero en el recolector de basuras
				fileGarbageCollector.addFileToCollector(fileParamName);

				// Es un XEDL.
				// 1.- COMPROBAMOS QUE LO QUE HEMOS RECIBIDO COINCIDE CON LO QUE ENVIAMOS (MISMO PUNTO DE ACCESO)

				xlimNotif.println("Receiving answer from: " + url_FS + ". XEDL received.");

				// 2.- Miramos si los datos del XEDL recibido son distintos de los de local.xedl (PUBLIC NAME, BANDWIDTH, IP?,DISPLAYS,ADITIONAL_PARAMS..
				// 2.1.- Si son distintos se informa al usuario de los cambios
				// 3.- Copiamos el fichero recibido a ~/.isabel/remote o master/sessionname/tmp.xedl
				String destFile= IsabelAgenda.getFullXedl(url_session).getAbsolutePath();
				if (!copyFile(localXedlFile.getAbsolutePath(),destFile)) {
					// Limpiamos
					fileGarbageCollector.cleanGarbage();
					Servutilities.writeErrorMessage ("localhost","Connect To","Error trying to copy received XEDL to final ubication: " + destFile,"Error trying to copy received XEDL to final ubication: " + destFile,IsGraphical,Constants.CONNECT_TO_XERL);
					outTraceFile.println("Vamos a hacer System.exit(0) peligro");
					Tunnelator.getTunnelator().deleteTunnel(url_FS);
					System.exit(0);
					return;
				}
				// 4.- Lanzamos la sesion si action es connect y no es refresh
				if ((CONNECT) || (RECONNECT)) {
					// ----------
					outTraceFile.println ("Before launching ISABEL: " + timeFormatter.format(new Date(System.currentTimeMillis())));
					// ----------
					if (connect_passwd.equals("")) {
						outTraceFile.println("There is no passwd.");
						connect_passwd = "\"\"";
					}
					outTraceFile.println("Launching from XEDL: " + IsabelAgenda.getFullXedl(url_session).getAbsolutePath() + " with session " + url_session + " and id " + connect_id);
					
					// Lanzamos ISABEL o lo que haga falta
					try {
						if (Servutilities.isIsabelRunning()) {
							String sessionRunning = Servutilities.getRunningSession();
							fileGarbageCollector.cleanGarbage();
							//Servutilities.writeMsgError ("Error trying to launch site " + connect_id + " from xedl: " + IsabelAgenda.getFullXedl(url_session).getAbsolutePath(),CONNECT_TO_XERL,"Connect To",IsGraphical);
							Servutilities.writeErrorMessage ("localhost","Connect To","Error trying to launch ISABEL.","ISABEL is already running session: " + sessionRunning,IsGraphical,Constants.CONNECT_TO_XERL);
							outTraceFile.println("Vamos a hacer System.exit(0) peligro");
							Tunnelator.getTunnelator().deleteTunnel(url_FS);
							System.exit(0);
							return;
						}
						outTraceFile.println("Let's call launchLocalFromXedl");
						Process app = null;
						if(port!=null){
							int port_int = Integer.parseInt(port);
							app = Servutilities.launchLocalFromXedl ( url_session, port_int, connect_id, IsabelAgenda.getFullXedl(url_session).getAbsolutePath(), true);	
						}
						else
						{
						    app = Servutilities.launchLocalFromXedl ( url_session, connect_id, IsabelAgenda.getFullXedl(url_session).getAbsolutePath(), true);
						}
						if(app!=null)
						{
							try {
								outTraceFile.println("esperamos a que se cierre isabel para cerrar los tuneles");
								app.waitFor();
								outTraceFile.println("se ha cerrado isabel, continuamos con el connect to");
							} catch (InterruptedException e) {
								e.printStackTrace(outTraceFile);
							}
						}
					} catch (Exception e) {
						fileGarbageCollector.cleanGarbage();
						e.printStackTrace(outTraceFile);
						//Servutilities.writeMsgError ("Error trying to launch site " + connect_id + " from xedl: " + IsabelAgenda.getFullXedl(url_session).getAbsolutePath(),CONNECT_TO_XERL,"Connect To",IsGraphical);
						Servutilities.writeErrorMessage ("localhost","Connect To","Error trying to launch ISABEL.","Session: " + url_session + " \r\n Site: " + connect_id + "\r\n from xedl: " + IsabelAgenda.getFullXedl(url_session).getAbsolutePath() + "\r\n Exception: " + e.getMessage(),IsGraphical,Constants.CONNECT_TO_XERL);
						outTraceFile.println("Vamos a hacer System.exit(0) peligro");
						Tunnelator.getTunnelator().deleteTunnel(url_FS);
						System.exit(0);
						return;
					}
					// Limpiamos
					fileGarbageCollector.cleanGarbage();
					outTraceFile.println("Vamos a hacer System.exit(0) peligro");
					Tunnelator.getTunnelator().deleteTunnel(url_FS);
					System.exit(0);
					return;
				} else {
					outTraceFile.println("Neither connect nor reconnect...exiting.");
					// Limpiamos
					fileGarbageCollector.cleanGarbage();
					return;
				}
			} catch (IllegalThreadStateException e ) {
			} catch (Exception e ) {
				e.printStackTrace(outTraceFile);
				Servutilities.writeErrorMessage ("localhost","Connect To","Unexpected Error!","" + e.getMessage(),IsGraphical,Constants.CONNECT_TO_XERL);
				outTraceFile.println("Vamos a hacer System.exit(0) peligro");
				Tunnelator.getTunnelator().deleteTunnel(url_FS);
				System.exit(0);
			}
		}
		// Terminamos...
		outTraceFile.println("Vamos a hacer System.exit(0) peligro");
		Tunnelator.getTunnelator().deleteTunnel(url_FS);
		System.exit(0);
	} 
}

class CloseTunnel extends Thread{

	private String server;

	public CloseTunnel(String server){
		this.server=server;
	}
	public void run() {
		System.out.println("shutdownhook de isabel_connect_to");
		Tunnelator.getTunnelator().deleteTunnel(server);
		System.out.println("hecho");
	}
}