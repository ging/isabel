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
package services.isabel.services.startsessionserver;

import java.io.*;
import java.net.*; 
import java.util.*; 

// Para el manejo de la Agenda
import isabel.lib.*;
import services.isabel.lib.*;
import services.isabel.services.ac.ServiceNames;
import services.isabel.services.guis.*;
import services.isabel.services.options.Options;

// Para el manejo de menus
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.xml.parsers.*;
import javax.xml.transform.*;

import xedl.lib.xerl.*;
import xedl.lib.xedl.*;

public class StartSessionServer { 

  // DEFINE EL NOMBRE DEL FICHERO DE TRAZAS
  private static final String TRACES = Constants.XLIM_LOGS_WORK_DIR + Constants.FILE_SEPARATOR +"start_server.log";

  // DEFINE EL NOMBRE DEL FICHERO DONDE SE GUARDA LA SALIDA DE LA LLAMADA A XLIM
  private static final String XLIM_CALL_FILE = Constants.XLIM_LOGS_WORK_DIR + Constants.FILE_SEPARATOR +"sss_xlim_call.log";

  // DEFINE EL NOMBRE DE ESTE SERVICIO PARA MOSTRAR EN LOS MENSAJES DE ERROR
  private static final String SERVICE_NAME = "Start Session Server";

  // DEFINE EL NOMBRE DE ESTE SERVICIO EN LA LLAMADA A XLIM
  private static final String XLIM_SERVICE_NAME = "StartSessionServer";
  
  // DEFINE EL NOMBRE DEL FICHERO DONDE SE GUARDAN LOS PARAMETROS PARA LA LLAMADA XLIM
  private static final String PARAMS_FILE = Constants.XLIM_TMP_WORK_DIR + Constants.FILE_SEPARATOR + XLIM_SERVICE_NAME + ".params";

  // DEFINE EL NOMBRE DEL FICHERO XERL PARA ESTE SERVICIO
  private static final String XERL_NAME = Constants.SSS_XERL;
  
  // USAGE
  private static final String USAGE = "Usage:\r\n   CLIENT: --session SESSION --id ID [--location LCOATION] [--passwd PASSWD] [--graphical]";

   
  
  // POSIBLES ACCIONES DEFINIDAS PARA ESTE SERVICIO (diferentes comportamientos del server)
  
  
  // DIRECTORIOS DE TRABAJO
  File xlimLogsWorkDir = new File(Constants.XLIM_LOGS_WORK_DIR); 
  File xlimTmpWorkDir = new File(Constants.XLIM_TMP_WORK_DIR);
  File xlimCfgWorkDir = new File(Constants.XLIM_CFG_WORK_DIR);
  File xlimXerlWorkDir = new File(Constants.XLIM_LOGS_WORK_DIR+Constants.FILE_SEPARATOR+"xerl");
  
  // RECOLECTOR DE FICHEROS TEMPORALES
  FileGarbageCollector fileGarbageCollector;
  
  // APLICACION GRAFICA O NO
  boolean isGraphical;
  
  // MENSAJES DE TRAZAS
  PrintWriter outTraceFile;
  
  // XLIM NOTIFICATIONS
  XlimNotification xlimNotif;
  
  // Agenda de Sessiones
  Agenda agenda = null;  
    
  // POSIBLES COMPORTAMIENTOS:
  private static final int SERVER_PARAMS = 2;
  private static final int GET_PARTICIPANTS_PARAMS = 0;
  
  /**
   * Constructor
   */
  public StartSessionServer() throws IOException{
    // Comprobamos los directorios de trabajo
    checkWorkingDirs();
    // Creamos el recolector de basuras de ficheros
    fileGarbageCollector = new FileGarbageCollector(); 
    fileGarbageCollector.addFileToCollector(XLIM_CALL_FILE);

    // De momento, modo texto
    isGraphical = false;

    // Declaramos los mensajes de trazas: 
    outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (TRACES)),true);

    // Declaramos las XLIM Notifications
    xlimNotif = new XlimNotification(SERVICE_NAME);
   

    // Creamos la agenda.
    try {
      if (Constants.AGENDA_ROOT_DIR != null) agenda = new Agenda(Constants.AGENDA_ROOT_DIR);
      else agenda = new Agenda(Constants.ISABEL_USER_DIR);
    } catch (IOException e){
      e.printStackTrace(outTraceFile);
      xlimNotif.println("Session Agenda can not be opened.");
      throw new IOException ("Contact Agenda can not be opened: " + e.getMessage());
    }
  
    
    
  }

  /**
   * Chequea que existen los directorios de trabajo. Si no existen, los crea.
   */
  private void checkWorkingDirs() {
    // Comprobamos la existencia de los directorios de trabajo...
    if (!xlimLogsWorkDir.exists()) xlimLogsWorkDir.mkdirs();
    if (!xlimTmpWorkDir.exists()) xlimTmpWorkDir.mkdirs();
    if (!xlimCfgWorkDir.exists()) xlimCfgWorkDir.mkdirs();
    if (!xlimXerlWorkDir.exists()) xlimXerlWorkDir.mkdirs();
    // Borramos cualquier xerl de este servicio que pueda haber...
    File xerlFile = new File (XERL_NAME);
    if (xerlFile.exists()) xerlFile.delete();
  }

  /**
   * llama al programa Options
   *
   */
  public void editLocalConfiguration()
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
	 Servutilities.copyFile(opdefault.getAbsolutePath(), optionsFile.getAbsolutePath());
  }
  

  public static void main (String args[]) { 

    try {
      StartSessionServer sss = new StartSessionServer();
      sss.outTraceFile.println("Processing...");

      if (args.length == 1) {
        try {
          String[] params = {"SSS-session", args[0]};
          Servutilities.saveParamsToFile(Constants.SAVED_VALUES_FILE,params);
        } catch (FileNotFoundException fnfe) {
          fnfe.printStackTrace(sss.outTraceFile);
        } catch (IOException ioe) {
          ioe.printStackTrace(sss.outTraceFile);
        }
      }

      // Gestion de parametros de entrada
      if ((args.length != 0) && (args.length != 1) && (args.length != 4) && (args.length != 6) && (args.length != 8)){ 
        System.out.println(USAGE);
        System.out.println("Vamos a hacer System.exit(0) peligro");
        System.exit(0);
        return;
      } else { 
        boolean isGraphical = false;
        boolean ISPARAMsession = false;
        boolean ISPARAMid = false;
        boolean ISPARAMlocation = false;
        boolean ISPARAMpasswd = false;
        boolean ISPARAMservice = false;
        boolean ISPARAMquality = false;
        String session = "";
        String id = "";
        String passwd = "";
        String location = "";
        String profile_name = null;
        String service = "Tele-Meeting";
        String quality = "1M";
        if ((args.length == 0) || (args.length == 1)) {
          boolean passwdOk = false;
          while (!ISPARAMid || !passwdOk) {
            // Modo grafico
            isGraphical = true;
            
            File optionsFile = sss.agenda.getOptionsFile();
            
            if (!optionsFile.exists()) {            
                //editLocalConfiguration();
            	try{
            		copyOptionsDefault(optionsFile);
            	}
            	catch(Exception e)
            	{
            		System.out.println("Default Options file does not exist, we give the user the possibility to create it");
            		sss.editLocalConfiguration();
            	}
            	
            }

            // Comprobamos que hay al menos una sesion local, si no, mensaje informativo:
            /*
            if (sss.agenda.getLocalSessions().length == 0) {
              JFrame parentFrame = new JFrame();
              JOptionPane.showMessageDialog(parentFrame, "No local sessions defined. Please use \"Edit Session\" to create local sessions.", "Start Session Server...", JOptionPane.ERROR_MESSAGE);
              parentFrame.dispose();
              return;
	    }
	    */
            // Creo el panel que quiero mostar
            XlimServicePanel xlimServicePanel = new XlimServicePanel(ServiceDefinitions.SSS_SERVICE);
            SessionDataPanel_start_server sessionDataPanel = (SessionDataPanel_start_server) xlimServicePanel.getPrimaryPanel();
            SiteDataPanel siteDataPanel = (SiteDataPanel) xlimServicePanel.getSecondaryPanel();
            Object[] message = {xlimServicePanel};

            // Llamamos a JOptionPane
            JFrame parentFrame = new JFrame();
            //Icon icon = new ImageIcon(Loadxedl.class.getClass().getResource(Servutilities.SHARK_IMAGE));	
            //parentFrame.setDefaultCloseOperation(parentFrame.EXIT_ON_CLOSE);
            Object[] options = {"Start Server", "Cancel"};
            int resultPanel = JOptionPane.showOptionDialog(parentFrame, message, "Start Session Server...",JOptionPane.OK_CANCEL_OPTION,JOptionPane.PLAIN_MESSAGE,null,options,options[0]);
            if (resultPanel == 1) {
              //System.exit(0);
              parentFrame.dispose();
              System.out.println("Vamos a hacer System.exit(0) peligro");
              System.exit(0);
              return;
            }
            if(resultPanel == JOptionPane.CLOSED_OPTION )
            {
                parentFrame.dispose();
                System.out.println("Vamos a hacer System.exit(0) peligro");
                System.exit(0);
            	return;
            }
            parentFrame.dispose();
            //borramos las sesiones anteriores
            Servutilities.deleteDirectory(sss.agenda.localDir);
            session = sessionDataPanel.getSessionName();
            quality = sessionDataPanel.getSessionQuality();
            service = sessionDataPanel.getSessionService();
            id = siteDataPanel.getSiteID();
            passwd = sessionDataPanel.getSitePasswd();
            location = siteDataPanel.getSiteLocation();
            profile_name = siteDataPanel.getProfileName();
            // Comprobamos que el passwd es correcto:
            String oldPasswd = siteDataPanel.getOldPasswd();
            //System.out.println("Old Passswd: " + oldPasswd);
            if (!oldPasswd.equals("") && (!Crypt.checkPasswd(passwd,oldPasswd))) {
              // Par�metro incorrecto
              JOptionPane.showMessageDialog(parentFrame, "ERROR: Permission denied. Check Password.", "Start Session Server...", JOptionPane.ERROR_MESSAGE);
            } else passwdOk = true;

            if (passwd.equals("")) ISPARAMpasswd = false;
            else ISPARAMpasswd = true;
            
            if (id.equals("")) {
              // Estos parametros no pueden ser vacio
              ISPARAMid = false;
              JOptionPane.showMessageDialog(parentFrame, "ERROR: NickName missing.", SERVICE_NAME, JOptionPane.ERROR_MESSAGE);
            } else ISPARAMid = true;
            
            if (session.equals("")) {
                // Estos parametros no pueden ser vacio                
                JOptionPane.showMessageDialog(parentFrame, "ERROR: Session can't be empty.", SERVICE_NAME, JOptionPane.ERROR_MESSAGE);
                System.out.println("Vamos a hacer System.exit(0) peligro");
                System.exit(0);
                return;
            }
              
            //una vez visto que el nick y la session no están vacíos guardo el xedl
            sessionDataPanel.saveXEDL();
            //una vez que ha llegado aquí están todos los datos bien, los guardamos en el historial
            String history_file = Constants.ISABEL_SESSIONS_DIR + Constants.FILE_SEPARATOR + Constants.FICHERO_HISTORIAL_SSS;
            Servutilities.add_history_entry_sss(sessionDataPanel.getSessionName(),sessionDataPanel.getSessionService(),
            									sessionDataPanel.getSessionQuality(),history_file);
            siteDataPanel.saveProfile();
            // Intentamos guardar los valores en el fichero de ultimos valores seleccionados.
            try { sessionDataPanel.saveValues(); }
            catch (Exception e) {
            	e.printStackTrace();
            }
          }
        } else {
          // Modo comandos
          // Comprobamos que se han introducido todos los parametros
          for (int i = 0; i <args.length; i+=2) { // incrementamos i en 2 porque son pares variable-valor
            if (args[i].equals("--session")) {
              ISPARAMsession = true;
              session = args[i+1];
            }
            if (args[i].equals("--id")) {
              ISPARAMid = true;
              id = args[i+1];
            }
            if (args[i].equals("--passwd")) {
              ISPARAMpasswd = true;
              passwd = args[i+1];
            }
            if (args[i].equals("--location")) {
              ISPARAMlocation = true;
              location = args[i+1];
            }
            if (args[i].equals("--graphical")) {
              isGraphical = true;
            }
            if (args[i].equals("--service")) {
            	ISPARAMservice = true;
                service = args[i+1];
            }
            if (args[i].equals("--quality")) {
            	ISPARAMquality = true;
                quality = args[i+1];
            }
          }
          //if (!ISPARAMpasswd) {
          //  ISPARAMpasswd = true;
          //  passwd = "";
          //}
          if (!(ISPARAMsession && ISPARAMid )) { // Si alguna de las variables es false, faltan parametros
            System.out.println("Error: missing parameters");
            System.out.println(USAGE);
            //Servutilities.writeMsgError ("ERROR: Missing parameters",CONNECT_TO_XERL,"Connect To",IsGraphical);
            Servutilities.writeErrorMessage ("localhost","Start Session Server...","Missing parameters.","Found " + args.length + " parameters. Number of parameters allowed: 1,6,7,8 or 9.",isGraphical,Constants.CONNECT_TO_XERL);
            System.out.println("Vamos a hacer System.exit(0) peligro");
            System.exit(0);

            return;
          }          
        }
        //Comprobamos que el options.xml existe, Si no existe sacamos un mensaje de error.
        File optionsFile = sss.agenda.getOptionsFile();
        if (!optionsFile.exists()) {
          //JFrame parentFrame = new JFrame();
          // sacamos un mensaje y terminamos
          //JOptionPane.showMessageDialog(parentFrame, "Please, configure your personal information first in \"Edit Local Configuration\" service.", "Start Session Server...", JOptionPane.ERROR_MESSAGE);
          //parentFrame.dispose();
          sss.editLocalConfiguration();
          //return;
        }
        try {
          // Generamos el composeLocal
          if (!ISPARAMpasswd) passwd = null;
          
          //ya no usamos ::1 como ip sino que cogemos la ip buena
          
          
          //si el passwd no es null lo encriptamos
          String encrypted_passwd = null;
          if(passwd != null)
          {
        	  String cipher_key = Constants.KEY_CODE + id;
        	  //ofuscated_passwd = passwd + cipher_key;
        	  DesEncrypter encrypter = new DesEncrypter(cipher_key);
      	      // Encrypt
      	      encrypted_passwd = encrypter.encrypt(passwd);      	    
              XEDLMixerTools.composeLocal(session, id, location, encrypted_passwd, profile_name, "::1", null, true);
          }
          else
          {
        	  XEDLMixerTools.composeLocal(session, id, location, passwd, profile_name, "::1", null, true);              
          }
          // Generamos el total
          XEDLMixerTools.xedlMixer(session,  true);          
          
          // Lanzamos ISABEL
          Servutilities.launchLocalFromXedl(session, id, sss.agenda.getFullXedl(session).getAbsolutePath());
        } catch (Exception e) {
          e.printStackTrace();
          try { 
            Servutilities.writeErrorMessage(SERVICE_NAME,"Error starting session server",e.getMessage(), isGraphical, XERL_NAME);
          }
          catch (Exception weme) {weme.printStackTrace();}
          System.out.println("Vamos a hacer System.exit(0) peligro");
          System.exit(0);
          return;
        }
        
      }
      
    } catch (Exception e) {
      e.printStackTrace();
      try { Servutilities.writeErrorMessage(SERVICE_NAME,"Error starting session server",e.getMessage(), false, XERL_NAME);}
      catch (Exception weme) {weme.printStackTrace();}
      System.out.println("Vamos a hacer System.exit(0) peligro");
      System.exit(0);
      return;
    } 
      
  }
}