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
 * ConnectionsManager.java
 *
 * Created on 18 de agosto de 2004, 12:50
 */

package services.isabel.lib;

import xedl.lib.jaxb.Edl;
import xedl.lib.jaxb.ISDN;
import xedl.lib.jaxb.ISDNCalled;
import xedl.lib.jaxb.ISDNParameters;
import xedl.lib.jaxb.NETWORKACCESS;
import xedl.lib.jaxb.SITE;
import xedl.lib.xedl.*;
import java.net.*;
import java.util.*;
import java.io.*;

import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;


/**
 *
 * @author  alvaro
 */
public class ConnectionsManager {
    
    private static final String ISABEL_GENERAL_CONNECT_TO_TRACES = Constants.XLIM_LOGS_WORK_DIR;
    public static final String ISDN_LOG_FILE = ISABEL_GENERAL_CONNECT_TO_TRACES+"cm_isdn.log";
    public static final String IPSEC_LOG_FILE = ISABEL_GENERAL_CONNECT_TO_TRACES+"cm_ipsec.log";
    
   /**
   * Dado un path a un fichero XEDL y el nombre de un sitio, toma los par�metros de ISDN del XEDL y lanza la ISDN del sitio 
   * @param siteName Nombre del sitio
   * @param xedlName Path al fichero XEDL
   * @return void
   * @Author adiaz
   */
    public static void startISDNIfNeeded(String xedlName, String siteName) throws IOException, XEDLException {
      // miramos si en la definicion del sitio dentro del xedl esta activado el ISDN
      // Xlim Notifications
      XlimNotification xlimNotif = new XlimNotification("Connections Manager");
      boolean isdnOn = false;
      
      
      XEDL xedl = new XEDL(xedlName);
      
      Edl miedl = xedl.getEdl();
      SITE misitio = miedl.getSiteDescription().getSITE(siteName);
      
      NETWORKACCESS acceso = misitio.getNETWORKACCESS();
      ISDN sdn = acceso.getISDN();      
      if (sdn!=null) {
    	misitio.setNetworkAccessISDN();
        // ISDN habilitado
        xlimNotif.println("ISDN enabled");
        // Miramos si ISDN esta arrancado
        if (!isISDNUp()) {
          xlimNotif.println("Starting ISDN");
          startISDN(xedlName, siteName, ""); // No importa el nombre de sesi�n... el XEDL solo admite una sesion por fichero
        } else {
          xlimNotif.println("ISDN already UP");
        }
      } else {
        // Network access es distinto de ISDN
        xlimNotif.println("ISDN disabled");            
      }
    }

       
    /**
   * Combrueba si los interfaces del ISDN estan configurados en la m�quina en la que se ejecuta (utiliza "isdnctrl").
   * @return boolean Ser� true o false si los interfaces est�n configurados o no respect�vamente.
   * @Author adiaz
   */
    public static boolean isISDNUp(){
        boolean result = false;
        //Check if ISDN commands are installed
        try{
            String[] cmd = {"sh","-l","isdnctrl"};
            Process  isdnctrl = Runtime.getRuntime().exec(cmd);
        }catch (Exception e){
            e.printStackTrace();
        }
        //Check if ISDN interfaces are up: check if there is any interface ipppX
        Enumeration interfaces = null;
        String tempString = null;
        int tempInt = -1;
        try{
            interfaces = NetworkInterface.getNetworkInterfaces();
        }catch(SocketException e){}
        while (interfaces.hasMoreElements()){
            NetworkInterface ni = (NetworkInterface)interfaces.nextElement();
            tempString = String.valueOf(ni);
            //System.out.println(tempString);
            tempInt=tempString.indexOf("ippp");
            //System.out.println(tempInt);
            if (tempInt>0){
                result=true;
            }
        }
        return result;
    }
    
    /**
   * Comprueba si el IPsec est� configurado en la m�quina en la que se ejecuta.
   * @return boolean Ser� true o false si el IPsec est� configurado o no respect�vaente.
   * @Author adiaz
   */
    public static boolean isIPsecUp(){
        //We should check if rcracoon is running
        try{
            Process racoonCheck = Runtime.getRuntime().exec("rcracoon status");
            BufferedReader reader = new BufferedReader(new InputStreamReader(racoonCheck.getInputStream()));
            String statusString = reader.readLine();
            if (statusString.indexOf("running")<0){
                //Some problem has happened and rcracoon is not running
                return false;
            }else{
                //Status is running
                return true;
            }
        }catch (Exception e){
            e.printStackTrace();
        }
        return false;
    }
    
    /**
   * Dado un objeto XEDL y el nombre de un sitio, toma los par�metros de ISDN del XEDL y lanza el sitio 
   * @param XEDL Objeto XEDL.
   * @param siteID Identificador del sitio en que se quiere activar el ISDN.
   * @param sessionID Identificador de la sesion (no se utiliza).
   * @return void
   * @Author adiaz
   */
    public static void startISDN(XEDL xedl, String siteID, String sessionID){
        Edl miedl = xedl.getEdl();
        SITE misitio = miedl.getSiteDescription().getSITE(siteID);
    	
        ISDNParameters isdnp = misitio.getNETWORKACCESS().getISDN().getISDNParameters();
        XlimNotification xlimNotif =null;
        try{
         xlimNotif = new XlimNotification("Connections Manager");        
        }catch (Exception e){}
        if (isdnp.isCaller()){
            xlimNotif.println("Caller role detected");
            //Caller
            try{
            	List<String> phoneNumbersList = isdnp.getISDNCaller().getISDNPHONENUMBER();
            	List<String> phoneMSNList = isdnp.getISDNCaller().getISDNPHONEMSN();
                StringBuffer phoneBuffer = new StringBuffer();
                int i=0;
                while (i<phoneNumbersList.size()){
                    phoneBuffer.append(phoneMSNList.get(i));
                    phoneBuffer.append(" ");
                    phoneBuffer.append(phoneNumbersList.get(i));
                    phoneBuffer.append(" ");
                    i++;
                }
                xlimNotif.println("Executing : "+"sudo /usr/local/ISDN/bin/isdn.dial -t \"" + phoneBuffer.toString() + "\"");
                String[] cmd = {"sh","-c","sudo /usr/local/ISDN/bin/isdn.dial -t \"" + phoneBuffer.toString() + "\""};
                Process dialer = Runtime.getRuntime().exec(cmd);
                BufferedReader extApplIn = new BufferedReader(new InputStreamReader(dialer.getInputStream()));
                BufferedReader extApplErr = new BufferedReader(new InputStreamReader(dialer.getErrorStream()));
                File logFile = new File(ISDN_LOG_FILE);
                logFile.createNewFile();
                PrintWriter logFileWriter = new PrintWriter (new BufferedWriter (new FileWriter (logFile)),true);
                String extApplStr="";
                String extApplStrErr="";
                while (((extApplStr = extApplIn.readLine()) != null) || ((extApplStrErr = extApplErr.readLine()) != null)) {
                    logFileWriter.println(extApplStr);
                    logFileWriter.println(extApplStrErr);
                    if (extApplStr.equals("ISDN interfaces stablished!")){
                        break;
                    }
                }
                System.out.println("cerrando fichero de log...");
                logFileWriter.close();
                if (dialer.waitFor() != 0){
                    throw new IOException("Error: executing isdn.dial.Check installation of ISDN package and 'sudo' program. See "+ISDN_LOG_FILE+" for details.");
                }else{
                    xlimNotif.println("ISDN configured!");
                }
            }catch(Exception e){
                e.printStackTrace();
            }
        }
        if (isdnp.isCalled()){
            xlimNotif.println("Called role detected");
            //Called
            try{
                int channels = isdnp.getISDNCalled().getISDNCHANNELS();
                int bw = misitio.getConnectionParameters().getBANDWIDTH();
                if (bw<0) {
                    bw = misitio.getConnectionParameters().getUpanddownbandwidth().getDOWNBANDWIDTH();                    
                }
                int boundling = bw / channels;
                xlimNotif.println("Executing : "+"sudo /usr/local/ISDN/bin/isdn.up -n " + channels + " -b "+boundling);
                String[] cmd = {"sh","-c","sudo /usr/local/ISDN/bin/isdn.up -n " + channels + " -b "+boundling};
                Process up = Runtime.getRuntime().exec(cmd);
                //BufferedReader extApplIn = new BufferedReader(new InputStreamReader(up.getInputStream()));
                //BufferedReader extApplErr = new BufferedReader(new InputStreamReader(up.getErrorStream()));
                LineNumberReader extApplIn = new LineNumberReader(new InputStreamReader(up.getInputStream()));
                LineNumberReader extApplErr = new LineNumberReader(new InputStreamReader(up.getErrorStream()));
                File logFile = new File(ISDN_LOG_FILE);
                logFile.createNewFile();
                PrintWriter logFileWriter = new PrintWriter (new BufferedWriter (new FileWriter (logFile)),true);
                String extApplStr="";
                String extApplStrErr="";
                //Waiting for "ISDN interfaces stablished!" string as isdn.up script last line output
                while (((extApplStr = extApplIn.readLine()) != null) || ((extApplStrErr = extApplErr.readLine()) != null)) {
                    logFileWriter.println(extApplStr);
                    logFileWriter.println(extApplStrErr);
                    if (extApplStr.equals("ISDN interfaces stablished!")){
                        break;
                    }
                }
                logFileWriter.close();
                if (up.waitFor() != 0){
                    throw new IOException("Error: executing isdn.up.Check installation of ISDN package and 'sudo' program. See "+ISDN_LOG_FILE+" for details.");
                }else{
                    xlimNotif.println("ISDN configured!");
                }
            }catch(Exception e){
                e.printStackTrace();
            }
        }                
    }
    
    /**
   * Dado un path a un fichero XEDL y el nombre de un sitio, toma los par�metros de ISDN del XEDL y lanza el sitio 
   * @param path Path al fichero XEDL
   * @param siteID Identificador del sitio en el que se quiere lanzar el ISDN.
   * @param sessionID Identificador de la sesion (no se utiliza).
   * @return void
   * @Author adiaz
   */
    public static void startISDN(String path, String siteID, String sessionID) throws IOException, XEDLException{
    	XEDL xedl = new XEDL(path);
        ConnectionsManager.startISDN(xedl, siteID, sessionID);
    }
    
   
    
    /**
   * Dado un objeto XEDL y el nombre de un sitio, toma los par�metros de ISDN del XEDL y para el ISDN.
   * @param XEDL Objeto XEDL
   * @param siteID Identificador del sitio en el que se quiere parar el ISDN.
   * @param sessionID Identificador de la sesion (no se utiliza).
   * @return void
   * @Author adiaz
   */
    public static void stopISDN(XEDL xedl, String siteID, String sessionID){
    	Edl miedl = xedl.getEdl();
        SITE misitio = miedl.getSiteDescription().getSITE(siteID);
    	
        ISDNParameters isdnp = misitio.getNETWORKACCESS().getISDN().getISDNParameters();
         
        if (isdnp.isCaller()){
            //Caller
            try{                
                Process hangup = Runtime.getRuntime().exec("sudo /usr/local/ISDN/bin/isdn.hangup");
                BufferedReader extApplIn = new BufferedReader(new InputStreamReader(hangup.getInputStream()));
                BufferedReader extApplErr = new BufferedReader(new InputStreamReader(hangup.getErrorStream()));
                File logFile = new File(ISDN_LOG_FILE);
                logFile.createNewFile();
                PrintWriter logFileWriter = new PrintWriter (new BufferedWriter (new FileWriter (logFile)),true);
                String extApplStr="";
                String extApplStrErr="";
                while (((extApplStr = extApplIn.readLine()) != null) || ((extApplStrErr = extApplErr.readLine()) != null)) {
                    //System.out.println(extApplStr);
                    //System.out.println(extApplStrErr);
                    logFileWriter.println(extApplStr);
                    logFileWriter.println(extApplStrErr);
                }
                logFileWriter.close();
                if (hangup.waitFor() != 0){
                    throw new IOException("Error: executing isdn.hangup.Check installation of ISDN package and 'sudo' program. See "+ISDN_LOG_FILE+" for details.");
                }
            }catch(Exception e){
                e.printStackTrace();
            }
        }
        if (isdnp.isCalled()){
            //Called
            try{
                Process down = Runtime.getRuntime().exec("sudo /usr/local/ISDN/bin/isdn.down");
		BufferedReader extApplIn = new BufferedReader(new InputStreamReader(down.getInputStream()));
                BufferedReader extApplErr = new BufferedReader(new InputStreamReader(down.getErrorStream()));
                File logFile = new File(ISDN_LOG_FILE);
                logFile.createNewFile();
                PrintWriter logFileWriter = new PrintWriter (new BufferedWriter (new FileWriter (logFile)),true);
                String extApplStr="";
                String extApplStrErr="";
                while (((extApplStr = extApplIn.readLine()) != null) || ((extApplStrErr = extApplErr.readLine()) != null)) {
                    //System.out.println(extApplStr);
                    //System.out.println(extApplStrErr);
                    logFileWriter.println(extApplStr);
                    logFileWriter.println(extApplStrErr);
                }
                logFileWriter.close();
                if (down.waitFor() != 0){
                    throw new IOException("Error: executing isdn.down.Check installation of ISDN package and 'sudo' program. See "+ISDN_LOG_FILE+" for details.");
                }
            }catch(Exception e){
                e.printStackTrace();
            }
        }
    }
    
    /**
   * Dado un path a un fichero XEDL y el nombre de un sitio, toma los par�metros de ISDN del XEDL y para el ISDN.
   * @param XEDL Objeto XEDL
   * @param siteID Identificador del sitio en el que se quiere parar el ISDN.
   * @param sessionID Identificador de la sesion (no se utiliza).
   * @return void
   * @Author adiaz
   */
    public static void stopISDN(String path, String siteID, String sessionID) throws IOException, XEDLException {
    	XEDL xedl = new XEDL(path);
        ConnectionsManager.stopISDN(xedl, siteID, sessionID);
    }
   
    
    public static void main(String args[]) throws Exception{
        System.out.println("Is ISDN up?: " + ConnectionsManager.isISDNUp());
        //System.out.println("Is IPsec up?: " +ConnectionsManager.isIPsecUp());
        if (args.length >0){
                if (args[1].equals("start")){
                    ConnectionsManager.startISDN(args[2],args[3],args[4]);
                }else{
                    ConnectionsManager.stopISDN(args[2],args[3],args[4]);
                }
            
            System.out.println("Is ISDN up?: " + ConnectionsManager.isISDNUp());
            //System.out.println("Is IPsec up?: " +ConnectionsManager.isIPsecUp());        
        }
    }
}
