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
package xedl.lib.xedl;



import java.io.File;
import java.io.IOException;



import java.io.*;
import java.net.*;
import java.util.*; 

import services.isabel.lib.*;
import services.isabel.services.options.MulticastPanel;

//import isabel.xlim.services.Agenda;
import java.text.SimpleDateFormat;


// Para el manejo de XEDL
import xedl.lib.jaxb.IPSECCONF;
import xedl.lib.jaxb.ISDN;
import xedl.lib.jaxb.ISDNParameters;
import xedl.lib.jaxb.MulticastParameters;
import xedl.lib.jaxb.NETWORKACCESS;
import xedl.lib.jaxb.PersonalInformation;
import xedl.lib.jaxb.SESSION;
import xedl.lib.jaxb.SITE;
import xedl.lib.jaxb.SITEADDITIONALPARAMS;
import xedl.lib.xedl.*;

// Crypt
//import isabel.xlim.services.crypt.Crypt;


/**
 * Implementa la clase que se encarga de realizar la mezcla de XEDL para generar 
 * un XEDL completo de sesion, a partir de la informaci�n de los XEDL parciales.
 * @Author lailoken
 */
public class XEDLMixerTools {
  //Constantes
  // CONSTANTES DE PRIORIDAD
  private static final byte TOTAL = 0;
  private static final byte NULL = 10;
  private static final byte ORIGINAL = 1;
  private static final byte PARTIAL = 2;
  private static final byte NONE = 9;

  
  // Variable que contiene el directorio donde esta la agenda
  private static final String AGENDA_ROOT_DIR = System.getProperty("agenda.root.dir");
  
  // Donde estan los ficheros de configuracion de ISABEL
  private static final String ISABEL_PRIOR_CONFIG_FILE = "priorxedl.cfg";
  
  private static final String PRIOR_CONFIG_FILE = System.getProperty("prior.config.file");
  

  // Fichero de trazas de xedlmixertools.
  private static final String TRACE_FILE_NAME = Constants.XLIM_LOGS_WORK_DIR + Constants.FILE_SEPARATOR + "xedlmixer.log";
  private static final String LOCAL_TRACE_FILE_NAME = Constants.XLIM_LOGS_WORK_DIR + Constants.FILE_SEPARATOR + "xedlmixer-local.log";
  //private static final String XEDL_TRACE_FILE_NAME = XLIM_LOGS_WORK_DIR + Servutilities.FILE_SEPARATOR + "xedlmixer-xedl.log";
  
  //private static final String SCHEMAURL = "http://isabel.dit.upm.es/xml/xedl2/edl1-8.xsd";
  //public static final String SCHEMAURL = "file:/usr/local/isabel/lib/xedlsplitter/edl1-8.xsd";
  
  // Valor por defecto para el SiteRole
  private static final String DEFAULT_SITEROLE = "interactive";

  // Valor por defecto para Delivery Platform
  private static final String DEF_DELIVERY_PLATFR = "isabel";

  // Variables de prioridad:
  private static byte priorOnID, priorOnPublicName, priorOnAccessControl, priorOnDescription, priorOnURL, priorOnAdminContacts, priorOnTechContacts, priorOnControlCapabilities,
  priorOnDefaultCtrl, priorOnDataCtrl, priorOnAppSharingCtrl, priorOnAudioCtrl, priorOnVideoCtrl, priorOnNetworkAccess,
  priorOnConnectionMode, priorOnSiteAddress, priorOnPeerAddress, priorOnMCUAddress, priorOnBandwidth, priorOnUpBandwidth,
  priorOnDownBandwidth, priorOnEnableMCU, priorOnDN, priorOnFQDN, priorOnInstitution, priorOnIpsecAction, priorOnIPsecDirection,
  priorOnIpsecIKEADhGroup, priorOnIpsecIKEAExchangeMode, priorOnIpsecIKEAMinLifetimeKB, priorOnIpsecIKEAMinLifetimeSec,
  priorOnIpsecMinLifetimeKB, priorOnIpsecMinLifetimeSec, priorOnIpsecProtocol, priorOnIpsecUseIPComp, priorOnIpsecUsePfs, 
  priorOnIsdnCalledRole, priorOnIsdnCallerRole, priorOnMcastAppShGroup, priorOnMcastAudioGroup, priorOnMcastCTRLWPGroup, priorOnMcastFTPGroup,
  priorOnMcastPointerGroup, priorOnMcastTTL, priorOnMcastVideoGroup, priorOnModemParameters, priorOnSiteRole, priorOnUserID,
  priorOnAdditionalParams, priorOnIpsec, priorOnMulticast, priorOnHammingProtectionMode, priorOnParityProtectionMode;


/**
 * Inicializa las variables de prioridad.
 * @Author lailoken
 */
  public static boolean initPriorVars ()  {
    // Inicializamos variables de prioridad
    priorOnID = NULL; priorOnPublicName = NULL; priorOnAccessControl = NULL; priorOnDescription = NULL; priorOnURL = NULL; priorOnAdminContacts = NULL; priorOnTechContacts = NULL; priorOnControlCapabilities = NULL;
    priorOnDefaultCtrl = NULL; priorOnDataCtrl = NULL; priorOnAppSharingCtrl = NULL; priorOnAudioCtrl = NULL; priorOnVideoCtrl = NULL; priorOnNetworkAccess = NULL;
    priorOnConnectionMode = NULL; priorOnSiteAddress = NULL; priorOnPeerAddress = NULL; priorOnMCUAddress = NULL; priorOnBandwidth = NULL; priorOnUpBandwidth = NULL;
    priorOnDownBandwidth = NULL; priorOnEnableMCU = NULL; priorOnDN = NULL; priorOnFQDN = NULL; priorOnInstitution = NULL; priorOnIpsecAction = NULL; priorOnIPsecDirection = NULL;
    priorOnIpsecIKEADhGroup = NULL; priorOnIpsecIKEAExchangeMode = NULL; priorOnIpsecIKEAMinLifetimeKB = NULL; priorOnIpsecIKEAMinLifetimeSec = NULL;
    priorOnIpsecMinLifetimeKB = NULL; priorOnIpsecMinLifetimeSec = NULL; priorOnIpsecProtocol = NULL; priorOnIpsecUseIPComp = NULL; priorOnIpsecUsePfs = NULL; 
    priorOnIsdnCalledRole = NULL; priorOnIsdnCallerRole = NULL; priorOnMcastAppShGroup = NULL; priorOnMcastAudioGroup = NULL; priorOnMcastCTRLWPGroup = NULL; priorOnMcastFTPGroup = NULL;
    priorOnMcastPointerGroup = NULL; priorOnMcastTTL = NULL; priorOnMcastVideoGroup = NULL; priorOnModemParameters = NULL; priorOnSiteRole = NULL; priorOnUserID = NULL;
    priorOnAdditionalParams = NULL; priorOnIpsec = NULL; priorOnMulticast = NULL; priorOnHammingProtectionMode = NULL; priorOnParityProtectionMode = NULL;
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------

    // Especificamos por donde sacamos las trazas.
    PrintWriter outTraceFile;
    try {
      outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (TRACE_FILE_NAME)),true);
    } catch (IOException e) {
      return false;
    }
    //  PrintWriter outTraceFile = new PrintWriter (System.out,true);

    // Sacamos las prioridades del fichero .isabel/connectto.cfg
    LineNumberReader priorLine = null;
    try {
      if (PRIOR_CONFIG_FILE != null) {
        outTraceFile.println ("Prior config file: "+ PRIOR_CONFIG_FILE);
        priorLine = new LineNumberReader(new FileReader(PRIOR_CONFIG_FILE));
      } else {
        outTraceFile.println ("Prior config file: "+ Constants.XLIM_CFG_WORK_DIR + Constants.FILE_SEPARATOR + ISABEL_PRIOR_CONFIG_FILE);
        priorLine = new LineNumberReader(new FileReader(Constants.XLIM_CFG_WORK_DIR + Constants.FILE_SEPARATOR + ISABEL_PRIOR_CONFIG_FILE));
      }
    } catch (FileNotFoundException e) {
      outTraceFile.println("ERROR: File Not Found. " + e);
      return false;
    }
    String aux = null;
    try {
      while ( (aux = priorLine.readLine())!=null) {
        String param = aux.substring(0,aux.indexOf(":"));
        String value = aux.substring(aux.indexOf(":")+1,aux.length());
        outTraceFile.println (" Linea leida: "+ aux + ". Parametro: " + param + " y valor: " + value);
        if (param.equals("priorOnID")) {
          if (value.toLowerCase().equals("original")) priorOnID = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnID = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnID = NONE;
        }
        if (param.equals("priorOnPublicName")) {
          if (value.toLowerCase().equals("original")) priorOnPublicName = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnPublicName = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnPublicName = NONE;
        }
        if (param.equals("priorOnAccessControl")) {
          if (value.toLowerCase().equals("original")) priorOnAccessControl = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnAccessControl = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnAccessControl = NONE;
        }
        if (param.equals("priorOnDescription")) {
          if (value.toLowerCase().equals("original")) priorOnDescription = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnDescription = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnDescription = NONE;
        }
        if (param.equals("priorOnURL")) {
          if (value.toLowerCase().equals("original")) priorOnURL = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnURL = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnURL = NONE;
        }
        if (param.equals("priorOnAdminContacts")) {
          if (value.toLowerCase().equals("original")) priorOnAdminContacts = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnAdminContacts = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnAdminContacts = NONE;
        }
        if (param.equals("priorOnTechContacts")) {
          if (value.toLowerCase().equals("original")) priorOnTechContacts = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnTechContacts = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnTechContacts = NONE;
        }
        if (param.equals("priorOnControlCapabilities")) {
          if (value.toLowerCase().equals("original")) priorOnControlCapabilities = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnControlCapabilities = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnControlCapabilities = NONE;
        }
        if (param.equals("priorOnDefaultCtrl")) {
          if (value.toLowerCase().equals("original")) priorOnDefaultCtrl = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnDefaultCtrl = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnDefaultCtrl = NONE;
        }
        if (param.equals("priorOnDataCtrl")) {
          if (value.toLowerCase().equals("original")) priorOnDataCtrl = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnDataCtrl = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnDataCtrl = NONE;
        }
        if (param.equals("priorOnDataCtrl")) {
          if (value.toLowerCase().equals("original")) priorOnDataCtrl = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnDataCtrl = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnDataCtrl = NONE;
        }
        if (param.equals("priorOnAppSharingCtrl")) {
          if (value.toLowerCase().equals("original")) priorOnAppSharingCtrl = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnAppSharingCtrl = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnAppSharingCtrl = NONE;
        }
        if (param.equals("priorOnAudioCtrl")) {
          if (value.toLowerCase().equals("original")) priorOnAudioCtrl = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnAudioCtrl = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnAudioCtrl = NONE;
        }
        if (param.equals("priorOnVideoCtrl")) {
          if (value.toLowerCase().equals("original")) priorOnVideoCtrl = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnVideoCtrl = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnVideoCtrl = NONE;
        }
        if (param.equals("priorOnNetworkAccess")) {
          if (value.toLowerCase().equals("original")) priorOnNetworkAccess = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnNetworkAccess = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnNetworkAccess = NONE;
        }
        if (param.equals("priorOnConnectionMode")) {
          if (value.toLowerCase().equals("original")) priorOnConnectionMode = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnConnectionMode = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnConnectionMode = NONE;
        }
        if (param.equals("priorOnSiteAddress")) {
          if (value.toLowerCase().equals("original")) priorOnSiteAddress = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnSiteAddress = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnSiteAddress = NONE;
        }
        if (param.equals("priorOnPeerAddress")) {
          if (value.toLowerCase().equals("original")) priorOnPeerAddress = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnPeerAddress = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnPeerAddress = NONE;
        }
        if (param.equals("priorOnMCUAddress")) {
          if (value.toLowerCase().equals("original")) priorOnMCUAddress = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnMCUAddress = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnMCUAddress = NONE;
        }
        if (param.equals("priorOnBandwidth")) {
          if (value.toLowerCase().equals("original")) priorOnBandwidth = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnBandwidth = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnBandwidth = NONE;
        }
        if (param.equals("priorOnUpBandwidth")) {
          if (value.toLowerCase().equals("original")) priorOnUpBandwidth = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnUpBandwidth = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnUpBandwidth = NONE;
        }
        if (param.equals("priorOnDownBandwidth")) {
          if (value.toLowerCase().equals("original")) priorOnDownBandwidth = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnDownBandwidth = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnDownBandwidth = NONE;
        }
        if (param.equals("priorOnEnableMCU")) {
          if (value.toLowerCase().equals("original")) priorOnEnableMCU = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnEnableMCU = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnEnableMCU = NONE;
        }
        if (param.equals("priorOnDN")) {
          if (value.toLowerCase().equals("original")) priorOnDN = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnDN = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnDN = NONE;
        }
        if (param.equals("priorOnFQDN")) {
          if (value.toLowerCase().equals("original")) priorOnFQDN = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnFQDN = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnFQDN = NONE;
        }
        if (param.equals("priorOnInstitution")) {
          if (value.toLowerCase().equals("original")) priorOnInstitution = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnInstitution = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnInstitution = NONE;
        }
        if (param.equals("priorOnIpsecAction")) {
          if (value.toLowerCase().equals("original")) priorOnIpsecAction = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnInstitution = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnInstitution = NONE;
        }
        if (param.equals("priorOnIPsecDirection")) {
          if (value.toLowerCase().equals("original")) priorOnIPsecDirection = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnIPsecDirection = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnIPsecDirection = NONE;
        }
        if (param.equals("priorOnIpsecIKEADhGroup")) {
          if (value.toLowerCase().equals("original")) priorOnIpsecIKEADhGroup = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnIpsecIKEADhGroup = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnIpsecIKEADhGroup = NONE;
        }
        if (param.equals("priorOnIpsecIKEAExchangeMode")) {
          if (value.toLowerCase().equals("original")) priorOnIpsecIKEAExchangeMode = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnIpsecIKEAExchangeMode = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnIpsecIKEADhGroup = NONE;
        }
        if (param.equals("priorOnIpsecIKEAMinLifetimeKB")) {
          if (value.toLowerCase().equals("original")) priorOnIpsecIKEAMinLifetimeKB = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnIpsecIKEAMinLifetimeKB = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnIpsecIKEAMinLifetimeKB = NONE;
        }
        if (param.equals("priorOnIpsecIKEAMinLifetimeSec")) {
          if (value.toLowerCase().equals("original")) priorOnIpsecIKEAMinLifetimeSec = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnIpsecIKEAMinLifetimeSec = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnIpsecIKEAMinLifetimeSec = NONE;
        }
        if (param.equals("priorOnIpsecMinLifetimeKB")) {
          if (value.toLowerCase().equals("original")) priorOnIpsecMinLifetimeKB = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnIpsecMinLifetimeKB = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnIpsecMinLifetimeKB = NONE;
        }
        if (param.equals("priorOnIpsecMinLifetimeSec")) {
          if (value.toLowerCase().equals("original")) priorOnIpsecMinLifetimeSec = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnIpsecMinLifetimeSec = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnIpsecMinLifetimeSec = NONE;
        }
        if (param.equals("priorOnIpsecProtocol")) {
          if (value.toLowerCase().equals("original")) priorOnIpsecProtocol = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnIpsecProtocol = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnIpsecProtocol = NONE;
        }
        if (param.equals("priorOnIpsecUseIPComp")) {
          if (value.toLowerCase().equals("original")) priorOnIpsecUseIPComp = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnIpsecUseIPComp = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnIpsecUseIPComp = NONE;
        }
        if (param.equals("priorOnIpsecUsePfs")) {
          if (value.toLowerCase().equals("original")) priorOnIpsecUsePfs = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnIpsecUsePfs = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnIpsecUsePfs = NONE;
        }
        if (param.equals("priorOnIsdnCalledRole")) {
          if (value.toLowerCase().equals("original")) priorOnIsdnCalledRole = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnIsdnCalledRole = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnIsdnCalledRole = NONE;
        }
        if (param.equals("priorOnIsdnCallerRole")) {
          if (value.toLowerCase().equals("original")) priorOnIsdnCallerRole = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnIsdnCallerRole = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnIsdnCallerRole = NONE;
        }
        if (param.equals("priorOnMcastAppShGroup")) {
          if (value.toLowerCase().equals("original")) priorOnMcastAppShGroup = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnMcastAppShGroup = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnMcastAppShGroup = NONE;
        }
        if (param.equals("priorOnMcastAudioGroup")) {
          if (value.toLowerCase().equals("original")) priorOnMcastAudioGroup = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnMcastAudioGroup = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnMcastAudioGroup = NONE;
        }
        if (param.equals("priorOnMcastCTRLWPGroup")) {
          if (value.toLowerCase().equals("original")) priorOnMcastCTRLWPGroup = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnMcastCTRLWPGroup = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnMcastCTRLWPGroup = NONE;
        }
        if (param.equals("priorOnMcastFTPGroup")) {
          if (value.toLowerCase().equals("original")) priorOnMcastFTPGroup = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnMcastFTPGroup = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnMcastFTPGroup = NONE;
        }
        if (param.equals("priorOnMcastPointerGroup")) {
          if (value.toLowerCase().equals("original")) priorOnMcastPointerGroup = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnMcastPointerGroup = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnMcastPointerGroup = NONE;
        }
        if (param.equals("priorOnMcastTTL")) {
          if (value.toLowerCase().equals("original")) priorOnMcastTTL = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnMcastTTL = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnMcastTTL = NONE;
        }
        if (param.equals("priorOnMcastVideoGroup")) {
          if (value.toLowerCase().equals("original")) priorOnMcastVideoGroup = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnMcastVideoGroup = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnMcastVideoGroup = NONE;
        }
        if (param.equals("priorOnModemParameters")) {
          if (value.toLowerCase().equals("original")) priorOnModemParameters = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnModemParameters = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnModemParameters = NONE;
        }
        if (param.equals("priorOnSiteRole")) {
          if (value.toLowerCase().equals("original")) priorOnSiteRole = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnSiteRole = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnSiteRole = NONE;
        }
        if (param.equals("priorOnUserID")) {
          if (value.toLowerCase().equals("original")) priorOnUserID = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnUserID = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnUserID = NONE;
        }
        if (param.equals("priorOnAdditionalParams")) {
          if (value.toLowerCase().equals("original")) priorOnAdditionalParams = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnAdditionalParams = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnAdditionalParams = NONE;
        }
        if (param.equals("priorOnIpsec")) {
          if (value.toLowerCase().equals("original")) priorOnIpsec = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnIpsec = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnIpsec = NONE;
        }
        if (param.equals("priorOnMulticast")) {
          if (value.toLowerCase().equals("original")) priorOnMulticast = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnMulticast = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnMulticast = NONE;
        }
        if (param.equals("priorOnHammingProtectionMode")) {
          if (value.toLowerCase().equals("original")) priorOnHammingProtectionMode = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnHammingProtectionMode = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnHammingProtectionMode = NONE;
        }
        if (param.equals("priorOnParityProtectionMode")) {
          if (value.toLowerCase().equals("original")) priorOnParityProtectionMode = ORIGINAL;
          if (value.toLowerCase().equals("partial")) priorOnParityProtectionMode = PARTIAL;
          if (value.toLowerCase().equals("none")) priorOnParityProtectionMode = NONE;
        }
      }
    } catch (Exception e) {
      outTraceFile.println("Error while parsing prior file: " + Constants.ISABEL_CONFIG_DIR + Constants.FILE_SEPARATOR + ISABEL_PRIOR_CONFIG_FILE);
      return false;
    }
    return true;
  }


  /**
   * Calcula si se tienen o no prioridad para copiar el campo 
   * @param fromPrior Prioridad del sitio origen
   * @param toPrior Prioridad del sitio destino
   * @param fieldPrior Prioridad del campo q se quiere copiar. Viene dado por un fichero de configuracion
   * @param globalPrior Indica si todos los campos tienen la misma prioridad o no.
   * @return true si se tiene prioridad para copiar el campo
   * @Author lailoken
   */
  private static boolean calculatePrior (byte fromPrior, byte toPrior, byte fieldPrior, boolean globalPrior) {
    boolean writeAll = false;
    if (globalPrior==true) {
      // Todos los campos tienen igual prioridad; no hay que tener en cuenta el fichero de configuracion de prioridades; solo comparamos las prioridades de origen y destino 
      writeAll = (fromPrior<=toPrior);
    } else {
      // Si la prioridad es por campo:
      writeAll = (fromPrior==fieldPrior) || ((toPrior!=fieldPrior) && (fromPrior<=toPrior));
    }
    return writeAll;
  }
    
 
  /**
   * Copia un sitio definido en un XEDL en otro sitio definido en otro XEDL, en funcion de una serie de prioridades.
   * @param fromSite Site del que queremos copiar datos.
   * @param toSite Site al que queremos copiar los datos.
   * @param priorType Tipo de prioridad para copiar campos. Se consideran los siguientes tipos : 
   *     - 0 (TOTAL): fromSite tiene prioridad total sobre toSite, es decir, todos los campos de fromSite tienen prioridad sobre toSite (independientemente de lo que diga el fichero de prioridades)
   *     - 1 (ORIGINAL): Solo los campos de fromSite que tengan prioridad ORIGINAL en el fichero de prioridades tienen prioridad sobre los campos de toSite
   *     - 2 (PARTIAL): Solo los campos de fromSite que tengan prioridad PARTIAL en el fichero de prioridades tienen prioridad sobre los campos de toSite
   *     - 3 (NOT USED)
   *     - 4 (NOT USED)
   *     - 5 (NOT USED)
   *     - 6 (NOT USED)
   *     - 7 (NOT USED)
   *     - 8 (NOT USED)
   *     - 9 (NONE): no hay prioridad definida para este sitio, es decir, cualquiera puede copiar..
   *     - 10 (NULL): toSite tiene prioridad total sobre fromSite, es decir, todos los campos de toSite tienen prioridad sobre fromSite (independientemente de lo que diga el fichero de prioridades)
   * @return true si todo fue bien
   * @throws Exception si hubo algun problema inesperado al copiar el site.
   * @Author lailoken
   */
  public static boolean copySite (SITE fromSite, SITE toSite, byte fromPrior, byte toPrior,boolean globalPrior) throws Exception {

    // Especificamos por donde sacamos las trazas.
    //PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (TRACE_FILE_NAME,true)),true);
    PrintWriter outTraceFile = Constants.inicializa_trazas_jaxb();
    
    outTraceFile.println("SITE ID: " + fromSite.getID());
    //Para cada campo: miramos si existe en origen y destino y comprobamos prioridades

    outTraceFile.println("ID...");
    //ID
    if ((fromSite.getID()!=null) && ( (toSite.getID()==null)||calculatePrior(fromPrior,toPrior,priorOnID,globalPrior))) toSite.setID(fromSite.getID());

    outTraceFile.println("passwd...");
    //PASSWD - ACCESS CONTROL
    if ((fromSite.getAccessControl()!=null) && ( (toSite.getAccessControl()==null)||calculatePrior(fromPrior,toPrior,priorOnAccessControl,globalPrior))) 
    	toSite.setAccessControl(fromSite.getAccessControl());

    outTraceFile.println("Public Name...");
    //PublicName
    if ((fromSite.getPUBLICNAME()!=null) && ( (toSite.getPUBLICNAME()==null) || calculatePrior(fromPrior,toPrior,priorOnPublicName,globalPrior))) 
    	toSite.setPUBLICNAME(fromSite.getPUBLICNAME());

    outTraceFile.println("Description...");
    //Description
    if ((fromSite.getDescription()!=null) && ( (toSite.getDescription()==null) || calculatePrior(fromPrior,toPrior,priorOnDescription,globalPrior))) 
    	toSite.setDescription(fromSite.getDescription());

    outTraceFile.println("URL...");
    //URL
    if ((fromSite.getURL()!=null) && ( (toSite.getURL()==null) || calculatePrior(fromPrior,toPrior,priorOnURL,globalPrior)))
    	toSite.setURL(fromSite.getURL());

    outTraceFile.println("INSTITUTION...");
    // INSTITUTION
    if ((fromSite.getInstitution()!=null) && ( (toSite.getInstitution()==null) || calculatePrior(fromPrior,toPrior,priorOnURL,globalPrior)))
    	toSite.setInstitution(fromSite.getInstitution());
    
    outTraceFile.println("ADMIN CONTACTS...");
    // ADMIN CONTACTS
    LinkedList fromAdminContactsNames = fromSite.getAdminInfoEmails();
    for (int i=0; i<fromAdminContactsNames.size();i++) {
      toSite.insertAdminContact(fromSite.getAdminContact(String.valueOf(fromAdminContactsNames.get(i))));
    }

    outTraceFile.println("TECHNICAL CONTACTS...");
    // TECHNICAL CONTACTS
    LinkedList fromTechContactsNames = fromSite.getTechInfoEmails();
    for (int i=0; i<fromTechContactsNames.size();i++) {
      toSite.insertTechContact(fromSite.getTechContact(String.valueOf(fromTechContactsNames.get(i))));
    }

    outTraceFile.println("CONTROL CAPABILITIES...");
    // CONTROL CAPABILITIES
    if ((fromSite.getControlCapabilities()!=null) && ( (toSite.getControlCapabilities()==null) || calculatePrior(fromPrior,toPrior,priorOnControlCapabilities,globalPrior))) 
    	toSite.setControlCapabilities(fromSite.getControlCapabilities());

    outTraceFile.println("DISPLAYS...");
    // DISPLAYS
    //DefaultCtrl
    if ((fromSite.getDefaultCtrl()!=null) && ( (toSite.getDefaultCtrl()==null) || calculatePrior(fromPrior,toPrior,priorOnDefaultCtrl,globalPrior))) 
    	toSite.setDefaultCtrl(fromSite.getDefaultCtrl());
    //DataCtrl
    if ((fromSite.getDataCtrl()!=null) && ( (toSite.getDataCtrl()==null) || calculatePrior(fromPrior,toPrior,priorOnDataCtrl,globalPrior) ) ) 
    	toSite.setDataCtrl(fromSite.getDataCtrl());
    //AppSharingCtrl
    if ((fromSite.getAppSharingCtrl()!=null) && ( (toSite.getAppSharingCtrl()==null) || calculatePrior(fromPrior,toPrior,priorOnAppSharingCtrl,globalPrior) ) ) 
    	toSite.setAppSharingCtrl(fromSite.getAppSharingCtrl());
    //AudioCtrl
    if ((fromSite.getAudioCtrl()!=null) && ( (toSite.getAudioCtrl()==null) || calculatePrior(fromPrior,toPrior,priorOnAudioCtrl,globalPrior) ) ) 
    	toSite.setAudioCtrl(fromSite.getAudioCtrl());
    //VideoCtrl
    if ((fromSite.getVideoCtrl()!=null) && ( (toSite.getVideoCtrl()==null) || calculatePrior(fromPrior,toPrior,priorOnVideoCtrl,globalPrior) ) ) 
    	toSite.setVideoCtrl(fromSite.getVideoCtrl());

    outTraceFile.println("NETWORK ACCESS...");
    // NETWORK ACCESS
    if ((fromSite.getNETWORKACCESS()!=null) && ((toSite.getNETWORKACCESS()!=null)||calculatePrior(fromPrior,toPrior,priorOnNetworkAccess,globalPrior))) {
      if(fromSite.getNETWORKACCESS().isEthernet())
      {
    	  outTraceFile.println("NetworkAccess in FromSite Ethernet");
    	  NETWORKACCESS netacc = new NETWORKACCESS();
    	  netacc.setEthernet(true);
    	  toSite.setNETWORKACCESS(netacc);
      }
      if(fromSite.getNETWORKACCESS().getISDN()!=null)
      {
    	  outTraceFile.println("NetworkAccess in FromSite ISDN");
    	  NETWORKACCESS netacc = new NETWORKACCESS();
    	  netacc.setISDN(fromSite.getNETWORKACCESS().getISDN());
      }
      if(fromSite.getNETWORKACCESS().getModem()!=null)
      {
    	  outTraceFile.println("NetworkAccess in FromSite Modem");
    	  NETWORKACCESS netacc = new NETWORKACCESS();
    	  netacc.setModem(fromSite.getNETWORKACCESS().getModem());
      }
    }
    
    outTraceFile.println("SITE ADDRESS...");    
    // SITE ADDRESS
    if ((fromSite.getSiteAddress()!=null) && ((toSite.getSiteAddress()==null)||calculatePrior(fromPrior,toPrior,priorOnSiteAddress,globalPrior))) {
      LinkedList siteAddressList = fromSite.getSiteAddress();
      LinkedList toSiteAddressList = toSite.getSiteAddress();
      for (int i = 0; i < siteAddressList.size(); i++) {
        String siteAddress = String.valueOf(siteAddressList.get(i));
        outTraceFile.println("siteAddress " + i + ": " + siteAddress);
        // Si ya existia, la quitamos para meterla la primera...
        for (int j = 0; j < toSiteAddressList.size(); j++) {
          outTraceFile.println("siteAddress: " + siteAddress + " and toSiteAddress: " + String.valueOf(toSiteAddressList.get(j)));
          if (String.valueOf(toSiteAddressList.get(j)).equals(siteAddress)) outTraceFile.println("SITE result: " +toSite.deleteSiteAddress(siteAddress));
        }
        if(!siteAddress.equals(""))
        {
        	toSite.insertSiteAddress(siteAddress,true);
        }
      }
    }


    outTraceFile.println("CONNECTION MODE...");
    // Miramos que connection modes hay en FROM
    boolean cmodeInFrom = false;
    boolean mcastInFrom = false;
    boolean unicastInFrom = false;
    boolean mcuInFrom = false;
    if(fromSite.getCONNECTIONMODE()!=null)
    {
    	cmodeInFrom = true;
    	if(fromSite.getCONNECTIONMODE().getMulticast()!=null)
    	{
    		mcastInFrom = true;
            outTraceFile.println("Found connection mode MULTICAST in SOURCE SITE");
    	}
    	if(fromSite.getCONNECTIONMODE().getMcu()!=null)
    	{
    		mcuInFrom = true;
            outTraceFile.println("Found connection mode MCU in SOURCE SITE");
    	}
    	if(fromSite.getCONNECTIONMODE().getUnicast()!=null)
    	{
    		unicastInFrom = true;
            outTraceFile.println("Found connection mode UNICAST in SOURCE SITE");
    	}
    }
    
    
    // Miramos que connection modes hay en TO
    boolean cmodeInTo = false;
    boolean mcastInTo = false;
    boolean mcuInTo = false;
    boolean unicastInTo = false;
    if(fromSite.getCONNECTIONMODE()!=null)
    {
    	cmodeInTo = true;
    	if(toSite.getCONNECTIONMODE().getMulticast()!=null)
    	{
    		mcastInTo = true;
            outTraceFile.println("Found connection mode MULTICAST in DEST SITE");
    	}
    	if(toSite.getCONNECTIONMODE().getMcu()!=null)
    	{
    		mcuInTo = true;
            outTraceFile.println("Found connection mode MCU in DEST SITE");
    	}
    	if(toSite.getCONNECTIONMODE().getUnicast()!=null)
    	{
    		unicastInTo = true;
            outTraceFile.println("Found connection mode UNICAST in DEST SITE");
    	}
    }
    
    
    // 1.- Miramos si from tiene prioridad en connection mode
    if (calculatePrior(fromPrior,toPrior,priorOnConnectionMode,globalPrior)) {
      outTraceFile.println("SOURCE SITE has PRIOR on CONNECTION MODE");
      // Si FROM tiene prioridad, solo se permite copiar los modos de conexion que tenga FROM
      // ...por lo que hay que hay que borrar aquellos modos de conexion que esten en TO y NO esten definidos en FROM
      if (unicastInTo && !unicastInFrom && cmodeInFrom) {
        // Hay que borrar el unicastInTo porque no esta en FROM y es el que tiene prioridad
        toSite.getCONNECTIONMODE().setUnicast(null);
        outTraceFile.println("Deleting UNICAST INFO from DEST SITE...");
      }
      
      if (mcuInTo && !mcuInFrom && cmodeInFrom) {
        // Hay que borrar el mcuInTo porque no esta en FROM y es el que tiene prioridad
        //toSite.deleteMCUConnectionMode();
        toSite.deleteMCUConnectionMode();
        outTraceFile.println("Deleting MCU INFO from DEST SITE...");
      }

      if (mcastInTo && !mcastInFrom && cmodeInFrom) {
        // Hay que borrar el mcastInTo porque no esta en FROM y es el que tiene prioridad
        toSite.deleteMulticastConnectionMode();
        outTraceFile.println("Deleting MULTICAST INFO from DEST SITE...");
      }
      // Y ahora ya metemos los datos si es que hay algo que meter...
      
      // PEER ADDRESS
      if ((fromSite.getPeerAddress()!=null) && (!fromSite.getPeerAddress().equals("")) && ((toSite.getPeerAddress()==null)||calculatePrior(fromPrior,toPrior,priorOnPeerAddress,globalPrior))) {
        outTraceFile.println("Writing UNICAST INFO from SOURCE SITE...");
        LinkedList siteAddressList = fromSite.getPeerAddress();
        LinkedList toSiteAddressList = toSite.getPeerAddress();
        for (int i = 0; i < siteAddressList.size(); i++) {
          String siteAddress = String.valueOf(siteAddressList.get(i));
          outTraceFile.println("PeerAddress " + i + ": " + siteAddress);
          // Si ya existia, la quitamos para meterla la primera...
          for (int j = 0; j < toSiteAddressList.size(); j++) {
            outTraceFile.println("siteAddress: " + siteAddress + " and toSiteAddress: " + String.valueOf(toSiteAddressList.get(j)));
            if (String.valueOf(toSiteAddressList.get(j)).equals(siteAddress)) outTraceFile.println("PEER result: " +toSite.deletePeerAddress(siteAddress));
          }
          toSite.insertPeerAddress(siteAddress,true);
        }
      }

      // MCU ADDRESS
      if ((fromSite.getMCUAddress()!=null) && (!fromSite.getMCUAddress().equals("")) && ((toSite.getMCUAddress()==null)||calculatePrior(fromPrior,toPrior,priorOnMCUAddress,globalPrior))) {
        outTraceFile.println("Writing MCU INFO from SOURCE SITE...");
        LinkedList siteAddressList = fromSite.getMCUAddress();
        LinkedList toSiteAddressList = toSite.getMCUAddress();
        //outTraceFile.println("Este alvarito esta loco FROM: " + siteAddressList);
        //outTraceFile.println("Este alvarito esta loco TO: " + toSiteAddressList);
        for (int i = 0; i < siteAddressList.size(); i++) {
          String siteAddress = String.valueOf(siteAddressList.get(i));
          outTraceFile.println("MCUAddress " + i + ": " + siteAddress);
          // Si ya existia, la quitamos para meterla la primera...
          for (int j = 0; j < toSiteAddressList.size(); j++) {
            outTraceFile.println("siteAddress: " + siteAddress + " and toSiteAddress: " + String.valueOf(toSiteAddressList.get(j)));
            if (String.valueOf(toSiteAddressList.get(j)).equals(siteAddress)) outTraceFile.println("MCU result: " +toSite.deleteMCUAddress(siteAddress));
          }
          toSite.insertMCUAddress(siteAddress,true);
        }
      }

      // MULTICAST
      if ((mcastInFrom) && ((!mcastInTo)||calculatePrior(fromPrior,toPrior,priorOnMCUAddress,globalPrior))) {
        // Cogemos los parametros multicast de from y los copiamos en to
        outTraceFile.println("Writing MULTICAST INFO from SOURCE SITE...");
        toSite.addMulticastParams(fromSite.setConnectionModeMulticast());
      }
    }
    
    
    // .- Miramos si to tiene prioridad en connection mode
    if (calculatePrior(toPrior,fromPrior,priorOnConnectionMode,globalPrior)) {
      // Si TO tiene prioridad, solo se permite copiar los modos de conexion que tenga TO
      outTraceFile.println("DEST SITE has PRION on CONNECTION MODE");

      // PEER ADDRESS
      if (unicastInTo) {
        if ((fromSite.getPeerAddress()!=null) && (!fromSite.getPeerAddress().equals("")) && ((toSite.getPeerAddress()==null)||calculatePrior(fromPrior,toPrior,priorOnPeerAddress,globalPrior))) {
          outTraceFile.println("Writing UNICAST INFO from SOURCE SITE...");
          LinkedList siteAddressList = fromSite.getPeerAddress();
          LinkedList toSiteAddressList = toSite.getPeerAddress();
          for (int i = 0; i < siteAddressList.size(); i++) {
            String siteAddress = String.valueOf(siteAddressList.get(i));
            outTraceFile.println("PeerAddress " + i + ": " + siteAddress);
            // Si ya existia, la quitamos para meterla la primera...
            for (int j = 0; j < toSiteAddressList.size(); j++) {
              outTraceFile.println("siteAddress: " + siteAddress + " and toSiteAddress: " + String.valueOf(toSiteAddressList.get(j)));
              if (String.valueOf(toSiteAddressList.get(j)).equals(siteAddress)) 
            	  outTraceFile.println("PEER result: " +toSite.deletePeerAddress(siteAddress));
            }
            toSite.insertPeerAddress(siteAddress,true);
          }
        }
      }
      
      // MCU ADDRESS
      if (mcuInTo) {
        if ((fromSite.getMCUAddress()!=null) && (!fromSite.getMCUAddress().equals("")) && ((toSite.getMCUAddress()==null)||calculatePrior(fromPrior,toPrior,priorOnMCUAddress,globalPrior))) {
          outTraceFile.println("Writing MCU INFO from SOURCE SITE...");
          LinkedList siteAddressList = fromSite.getMCUAddress();
          LinkedList toSiteAddressList = toSite.getMCUAddress();
          //outTraceFile.println("Este alvarito esta loco FROM: " + siteAddressList);
          //outTraceFile.println("Este alvarito esta loco TO: " + toSiteAddressList);
          for (int i = 0; i < siteAddressList.size(); i++) {
            String siteAddress = String.valueOf(siteAddressList.get(i));
            outTraceFile.println("MCUAddress " + i + ": " + siteAddress);
            // Si ya existia, la quitamos para meterla la primera...
            for (int j = 0; j < toSiteAddressList.size(); j++) {
              outTraceFile.println("siteAddress: " + siteAddress + " and toSiteAddress: " + String.valueOf(toSiteAddressList.get(j)));
              if (String.valueOf(toSiteAddressList.get(j)).equals(siteAddress)) outTraceFile.println("MCU result: " +toSite.deleteMCUAddress(siteAddress));
            }
            toSite.insertMCUAddress(siteAddress,true);
          }
        }
      }

      // MULTICAST
      if (mcastInTo) {
        if ((mcastInFrom) && ((!mcastInTo)||calculatePrior(fromPrior,toPrior,priorOnMCUAddress,globalPrior))) {
          // Cogemos los parametros multicast de from y los copiamos en to
          outTraceFile.println("Writing MULTICAST INFO from SOURCE SITE...");
          toSite.addMulticastParams(fromSite.setConnectionModeMulticast());
        }
      }
    }

    outTraceFile.println("BANDWIDTH...");
    //BANDWIDTH
    if ((fromSite.getBandwidth()!=-1) && ((toSite.getBandwidth()==-1)||calculatePrior(fromPrior,toPrior,priorOnID,globalPrior))) 
    	toSite.setBandwidth(fromSite.getBandwidth());

    outTraceFile.println("UPWANDWIDTH...");
    //UPWANDWIDTH
    if ((fromSite.getUpBandwidth()!=-1) && ( (toSite.getUpBandwidth()==-1) ||calculatePrior(fromPrior,toPrior,priorOnUpBandwidth,globalPrior))) 
    	toSite.setUpBandwidth(fromSite.getUpBandwidth());

    outTraceFile.println("DOWNBANDWIDTH...");
    //DOWNBANDWIDTH
    if ((fromSite.getDownBandwidth()!=-1) && ( (toSite.getDownBandwidth()==-1) ||calculatePrior(fromPrior,toPrior,priorOnDownBandwidth,globalPrior))) 
    	toSite.setDownBandwidth(fromSite.getDownBandwidth());

    outTraceFile.println("ENABLE MCU...");
    //ENABLE MCU
    if ((fromSite.getEnableMCU()!=false) && ( (toSite.getEnableMCU()==false) ||calculatePrior(fromPrior,toPrior,priorOnEnableMCU,globalPrior))) 
    	toSite.setENABLEMCU(fromSite.getEnableMCU());

    outTraceFile.println("INSTITUTION...");
    // INSTITUTION
    if ((fromSite.getInstitution()!=null) && ((toSite.getInstitution()==null)||calculatePrior(fromPrior,toPrior,priorOnInstitution,globalPrior))) 
    	toSite.setInstitution(fromSite.getInstitution());

    outTraceFile.println("SITE ROLE...");
    //SITE ROLE
    if ((fromSite.getSITEROLE()!=null) && ((toSite.getSITEROLE()==null)||calculatePrior(fromPrior,toPrior,priorOnSiteRole,globalPrior))) 
    	toSite.setSITEROLE(fromSite.getSITEROLE());


    outTraceFile.println("USER ID...");
    //USER ID
    if ((fromSite.getUserID()!=null) && ((toSite.getUserID()==null)||calculatePrior(fromPrior,toPrior,priorOnUserID,globalPrior))) 
    	toSite.setUserID(fromSite.getUserID());
    

    outTraceFile.println("ADDITIONAL PARAMS...");
    // ADDITIONAL PARAMS
    if ((fromSite.getSITEADDITIONALPARAMS()!=null) && ((toSite.getSITEADDITIONALPARAMS()==null)||calculatePrior(fromPrior,toPrior,priorOnAdditionalParams,globalPrior))) 
    	toSite.setSITEADDITIONALPARAMS(fromSite.getSITEADDITIONALPARAMS());
    
    outTraceFile.println("IPSEC...");
    // IPSEC CONFIGURATION
    if ((fromSite.getIPSECCONF()!=null) && ((toSite.getIPSECCONF()==null)||calculatePrior(fromPrior,toPrior,priorOnIpsec,globalPrior))) 
    	toSite.setIPSECCONF(fromSite.getIPSECCONF());

    outTraceFile.println("FWDN - ID...");
    // IPSEC CONFIGURATION - DN
    if ((fromSite.getDN()!=null) && ((toSite.getDN()==null)||calculatePrior(fromPrior,toPrior,priorOnDN,globalPrior))) 
    	toSite.setDN(fromSite.getDN());
   
    outTraceFile.println("IPSEC - FQDN...");
    // IPSEC CONFIGURATION - FQDN
    if ((fromSite.getFQDN()!=null) && ((toSite.getFQDN()==null)||calculatePrior(fromPrior,toPrior,priorOnFQDN,globalPrior))) 
    	toSite.setFQDN(fromSite.getFQDN());
    
    outTraceFile.println("FEC...");
    // FEC CONFIGURATION
    // Hamming Protection Mode
    int[] fromFecHammingParams = fromSite.getHammingProtectionModeParams();
    boolean fecHammingInFrom = !((fromFecHammingParams[0] ==0) && (fromFecHammingParams[1] ==0));
    int[] toFecHammingParams = toSite.getHammingProtectionModeParams();
    boolean fecHammingInTo = !((toFecHammingParams[0] ==0) && (toFecHammingParams[1] ==0));
    if ((fecHammingInFrom) && ((!fecHammingInTo)||calculatePrior(fromPrior,toPrior,priorOnHammingProtectionMode,globalPrior))) 
    	toSite.setHammingProtectionMode(fromFecHammingParams[0],fromFecHammingParams[1]);
    // Parity Protection Mode
    int[] fromFecParityParams = fromSite.getParityProtectionModeParams();
    boolean fecParityInFrom = !((fromFecParityParams[0] ==0) && (fromFecParityParams[1] ==0));
    int[] toFecParityParams = toSite.getParityProtectionModeParams();
    boolean fecParityInTo = !((toFecParityParams[0] ==0) && (toFecParityParams[1] ==0));
    if ((fecParityInFrom) && ((!fecParityInTo)||calculatePrior(fromPrior,toPrior,priorOnParityProtectionMode,globalPrior))) 
    	toSite.setParityProtectionMode(fromFecParityParams[0],fromFecParityParams[1]);

    
    return true;
  }
  



  /**
   * Mezcla todos los XEDLs pertenecientes a una sesion generando como resultado un XEDL global.
   * @param session Indica el nombre de la sesion de la que se quiere construir el XEDL.
   * @param writeFile Si es true, el XEDl se graba en fichero
   * @return XEDL si no hubo problemas en la mezcla; null en caso contrario
   * @trhows XEDLMixerException, Exception si se produjo un error en la mezcla.
   * @Author lailoken
   */
  public static XEDL xedlMixer (String session, boolean writeFile) throws XEDLMixerException {

	// Nos aseguramos de que existen los directorios
    File xlimLogsWorkDir = new File(Constants.XLIM_LOGS_WORK_DIR);
    if (!xlimLogsWorkDir.exists()) xlimLogsWorkDir.mkdirs();
    File xlimTmpWorkDir = new File(Constants.XLIM_TMP_WORK_DIR);
    if (!xlimTmpWorkDir.exists()) xlimTmpWorkDir.mkdirs();

    System.out.println("XEDL Mixer");
    System.out.println("Mixing partial configuration files to compose Global XEDL file...");

    // Especificamos por donde sacamos las trazas.
    PrintWriter outTraceFile;
    outTraceFile = Constants.inicializa_trazas_jaxb();
    
    //  PrintWriter outTraceFile = new PrintWriter (System.out,true);

    //Creamos el objeto para manejar la Agenda.
    Agenda IsabelAgenda = null;
    try {
      if (AGENDA_ROOT_DIR != null) IsabelAgenda = new Agenda (AGENDA_ROOT_DIR);
      else IsabelAgenda = new Agenda (Constants.ISABEL_DIR);
    } catch (Exception e) {
      throw new XEDLMixerException("Problem found opening Agenda: " + e);
    }
    
    // Inicializamos las variables de prioridad (leemos el fichero de prioridades que esta en .isabel/connectto.cfg
    outTraceFile.println("Empieza la fiesta...");
    outTraceFile.println("Initializing priorvars...");
    if (initPriorVars ()==false) {
      outTraceFile.println("Error initializing priorvars...");
      //ENRIQUE, lo comento, de aqui no pasaba porque no inicizaliza bien las variables de prioridad
      //return null;
      return null;
    }
    
    // Solo podemos generar un XEDL si la session es local
    if (IsabelAgenda.isLocalSession(session)){
      
      // Variables que indican si existen los XEDLs
      boolean existOriginalXEDL = false;
      boolean existLocalXEDL = false;
      boolean existFullXEDL = false;
      boolean existPartialXEDL = false;
      
      // Contiene los acronimos de todos los SITES que tienen XEDL parcial
      String[] partialNames;
      try {
        partialNames = IsabelAgenda.getSitesNames(session);
      } catch (IOException ioe) {
        throw new XEDLMixerException("Problems found obtaining subscribed SITES names: " + ioe);
      }
    
      // Fichero que contiene el XEDL original (el que crea en accept Connections. 
      File originalXEDLFile;
      try {
        originalXEDLFile = IsabelAgenda.getOriginalXedl(session);
      } catch (Exception e) {
        return null;
      }
      outTraceFile.println("Original File: " + originalXEDLFile.getAbsolutePath());
      String originalXEDLName = originalXEDLFile.getAbsolutePath();
      XEDL originalXEDL = null;
      try {
        originalXEDL = new XEDL (originalXEDLName);
      } catch (Exception e) {
        throw new XEDLMixerException("Problem found opening file: " + originalXEDLFile.getAbsolutePath() + ". Error message: " + e);
      }
      if (originalXEDL != null) {
    	  outTraceFile.println("originalXEDL no es null, va bien");
    	  existOriginalXEDL = true;
      }
      else return null; //Si el fichero original no se puede abrir malo malo
      SiteList originalSiteList = new SiteList(originalXEDL);
      outTraceFile.println("sitelist creado");
      LinkedList originalSiteIds = originalSiteList.getSiteIds();
      outTraceFile.println("linkedlist creado");
      
      // Fichero local con informacion del master (probablemente la misma que originalXEDLFile
      File localXEDLFile;
      XEDL localXEDL;
      try {
        localXEDLFile = IsabelAgenda.getLocalXedl(session);
        String localXEDLName = localXEDLFile.getAbsolutePath();
        outTraceFile.println("Master local File: " + localXEDLFile.getAbsolutePath());
        try {
          localXEDL = new XEDL(localXEDLName);
        } catch (XEDLException xedle) {
          throw new XEDLMixerException("Problems found reading XEDL information from local XEDL file: " + xedle);
        }
        if (localXEDL != null) {existLocalXEDL = true;}
        //else return null; //si el fichero con info del master no se puede abrir, malo malo
        else {existLocalXEDL = false;};
      } catch (Exception e) {
    	  e.printStackTrace();
        existLocalXEDL = false;
        localXEDL = null;
        //throw new XEDLMixerException("Problem found opening local.xml file of session: " + session + ". Error message: " + e);
      }
      
      
      // Fichero XEDL total
      File fullXEDLFile;
      try {
        fullXEDLFile = IsabelAgenda.getFullXedl(session);
        //Borramos el fichero. No queremos nada del XEDL TOTAL antiguo
        fullXEDLFile.delete();
      } catch (Exception e) {
        throw new XEDLMixerException("Problem found opening Total XEDL file of session : " + session + ". Error message: " + e);
      }
      
      String fullXEDLName = fullXEDLFile.getAbsolutePath();
      outTraceFile.println("Total File: " + fullXEDLFile.getAbsolutePath());
      XEDL fullXEDL = null;
      SiteList fullSiteList = null;
      
      try {
    	  fullXEDL = new XEDL ();
    	  }
      catch (XEDLException xedle2) { 
    	  throw new XEDLMixerException("Problems found reading XEDL information from local XEDL file: " + xedle2);
    	  }
      
      fullXEDL.setSchemaURL(XEDL.SCHEMAURL);
      fullSiteList = new SiteList(fullXEDL);
      LinkedList fullSiteIds = fullSiteList.getSiteIds();
      outTraceFile.println("This is fullSiteIds: " + fullSiteIds.size());
      
      // Si no existen el originalXEDL y el local, mal rollo, salimos
      //if ((!existOriginalXEDL) || (!existLocalXEDL)) return null;
      if ((!existOriginalXEDL)) return null;
      
      
      // --------------------------------------------------------------
      // Metemos la informacion de la SESSION, que se encuentra en el fichero original
      // --------------------------------------------------------------
      SESSION mises = originalXEDL.getEdl().getSESSION();
      outTraceFile.println("Merging info from SESSION... (ORIGINAL XML).");
      outTraceFile.println("--------------------------------------------");
      if (mises == null) {
        outTraceFile.println("Session not defined in ORIGINAL XEDL.");
        return null;
      }
      if (fullXEDL.getEdl().getSESSION()!=null) fullXEDL.getEdl().setSESSION(null); //Si existe el eventinfo lo borramos.
      fullXEDL.getEdl().setSESSION(mises);//... y le metemos el EventInfo del ORIGINAL
      
      // --------------------------------------------------------------
      // Metemos la informacion de los SITIOS
      // --------------------------------------------------------------

      String fullSiteId = null;
      if (existLocalXEDL) {
        outTraceFile.println("Merging info from master... (local.xml).");
        // Primero añadimos al fullXEDL la informacion del master (local)
        SiteList localSiteList = new SiteList(localXEDL);
        SITE localSite = localSiteList.getSite(String.valueOf(localSiteList.getSiteIds().get(0)));
        String localSiteId = localSite.getID();
        // 1.- Comprobamos si este sitio ya esta definido en el fullXEDL
        //fullSiteList = fullXEDL.createSiteList(); // Actualizamos fullSiteList
        for (int i=0;i < fullSiteIds.size();i++) {
          String tempFullSiteId = String.valueOf(fullSiteIds.get(i));
          outTraceFile.println("This is local: " + localSiteId);
          outTraceFile.println("This is full: " + tempFullSiteId);
          if (localSiteId.equals(tempFullSiteId)) { //Si esta el sitio definido en full, hay que actualizarlo campo a campo :S
            fullSiteId = tempFullSiteId;
            break;
          }
        }
        if (fullSiteId!=null) { //Si esta el sitio definido en full, hay que actualizarlo campo a campo :S
          // 3.1- Solo nos interesa conservar lo que hay en los campos: FS y site_address.
          outTraceFile.println("SITE exists: " + localSiteId + ". just editing it.");
          try {
            copySite(localSite,fullSiteList.getSite(fullSiteId),PARTIAL,NONE, false); // 2 indica q se tienen en cuenta las prioridades del fichero
          } catch (Exception e) {
            throw new XEDLMixerException("Problems found merging info from master into FULL XEDL file: " + e);
          }
        } else { //Si no esta definido, copiamos a capon y hemos terminado
          outTraceFile.println("SITE doesn't exist: " + localSiteId + ". Copying... ");
          if (fullSiteList.addSite(localSite)==null) System.out.println("Something went wrong while copying SITE: ");
          fullSiteIds = fullSiteList.getSiteIds();// Actualizamos fullSiteIDs
          outTraceFile.println("This fullSiteIds: " + fullSiteIds.size());
        }
      }
      
      outTraceFile.println("Merging info from partial XEDLs... (PARTIAL XML).");
      outTraceFile.println("-------------------------------------------------");
      // A continuacion, para cada sitio que tenga definicion parcial de XEDL, a�adimos su info al fullXEDL.
      for (int i = 0; i <partialNames.length; i++) {
        File partialFile;
        try {
          partialFile = IsabelAgenda.getSiteXedl(session,partialNames[i]);
        } catch (Exception e) {
          return null;
        }
        String partialName = partialFile.getAbsolutePath();
        outTraceFile.println("Partial File: " + partialName);
        XEDL partialXEDL;
        try {
          partialXEDL = new XEDL (partialName);
        } catch (XEDLException xedle) {
          //borramos el partialName porque si no se queda ahí y no deja que se haga la mezcla a la siguiente
	          try{
	        	  System.out.println("We delete the partialFile that went wrong");
	        	  partialFile.delete();
	          }
	          catch (Exception e) {
	        	  e.printStackTrace();
			  }
          throw new XEDLMixerException("Problems found reading XEDL information from Partial XEDL file: " + xedle);          
        }
        if (partialXEDL != null) {
          existPartialXEDL = true;
          SiteList partialSiteList = new SiteList(partialXEDL);
          SITE partialSite = partialSiteList.getSite(String.valueOf(partialSiteList.getSiteIds().get(0)));
          String partialSiteId = partialSite.getID(); // deberia coincidir con el partialName[i]
          outTraceFile.println("----> Starting with: " + partialSiteId);
          outTraceFile.println("This is partialName[" + i + "]: " + partialName + " y del ID del partialXEDL: " + partialSiteId);
          fullSiteId = null;
          // 1.- Comprobamos si este sitio ya esta definido en el fullXEDL
          fullSiteIds = fullSiteList.getSiteIds();// Actualizamos fullSiteIDs
          outTraceFile.println("This is fullSiteIds: " + fullSiteIds.size());
          for (int j=0;j < fullSiteIds.size();j++) {
            String tempFullSiteId = String.valueOf(fullSiteIds.get(j));
            outTraceFile.println("This is partial: " + partialSiteId);
            outTraceFile.println("This is full: " + tempFullSiteId);
            if (partialSiteId.equals(tempFullSiteId)) { //Si esta el sitio definido en full, hay que actualizarlo campo a campo :S
              fullSiteId = tempFullSiteId;
              break;
            }
          }
          if (fullSiteId!=null) {
            // 3.1- Solo nos interesa conservar lo que hay en los campos: FS y site_address.
            outTraceFile.println("SITE exists: " + partialSiteId + ". Editing.");
            if (fullSiteList.getSite(fullSiteId)==null) outTraceFile.println("Something went wrong. we don't have fullSite");
            try {
              copySite(partialSite,fullSiteList.getSite(fullSiteId),PARTIAL,PARTIAL,false); // 2 indica q se tienen en cuenta las prioridades del fichero
            } catch (Exception e) {
              throw new XEDLMixerException("Problems found coping Partial XEDL file: " + e);
            }
          } else { //Si no esta definido, copiamos a capon y hemos terminado
            outTraceFile.println("SITE does not exist: " + partialSiteId + ". Copying.");
            if (fullSiteList.addSite(partialSite)==null) System.out.println("Something went wrong while copying site");
          }
        }
      }
      
      // OJOJOJOJOJOJOJOJOJOJOJO
      // Copiamos el full a un temporal..
      //Servutilities.copyFile(fullXEDLFile.getAbsolutePath(),"kk_full_tmp.xml");
      //fullXEDL.save("/tmp/kk_full_tmp.xml");
      // OJOJOJOJOJOJOJOJOJOJOJO
      
      outTraceFile.println("Merging info from ORIGINAL... (ORIGINAL XML).");
      outTraceFile.println("---------------------------------------------");
      // Por �ltimo introducimos la informacion del fichero original. En el fichero original puede haber mas de un sitio definido
      for (int i=0;i < originalSiteIds.size();i++) { // Para cada uno de los sitios definidos en el fichero original
      // 1.- Comprobamos si este sitio ya esta definido en el fullXEDL
        String originalSiteId = String.valueOf(originalSiteIds.get(i));
        outTraceFile.println("----> Starting with: " + originalSiteId);
        SITE originalSite = originalSiteList.getSite(originalSiteId);
        fullSiteId = null;
        fullSiteIds = fullSiteList.getSiteIds();// Actualizamos fullSiteIds
        outTraceFile.println("This is fullSiteIds: " + fullSiteIds.size());
        for (int j=0;j < fullSiteIds.size();j++) {
          String tempFullSiteId = String.valueOf(fullSiteIds.get(j));
          outTraceFile.println("This is original: " + originalSiteId);
          outTraceFile.println("This is full: " + tempFullSiteId);
          if (originalSiteId.equals(tempFullSiteId)) { //Si esta el sitio definido en full, hay que actualizarlo campo a campo :S
            fullSiteId = tempFullSiteId;
            break;
          }
        }
        if (fullSiteId!=null) {
          // 3.1- Solo nos interesa conservar lo que hay en los campos: FS y site_address.
          outTraceFile.println("SITE exists: " + originalSiteId + ". Editing.");
          try {
            copySite(originalSite,fullSiteList.getSite(fullSiteId),ORIGINAL,PARTIAL,false); // 2 indica q se tienen en cuenta las prioridades del fichero
          } catch (Exception e) {
        	  e.printStackTrace(outTraceFile);
            throw new XEDLMixerException("Problems found copying site " + originalSite + " :" +  e);
          }
        } else { //Si no esta definido, copiamos a capon y hemos terminado
          outTraceFile.println("SITE does not exist: " + originalSiteId + ". Don't touch.");
          //if (fullSiteList.addSite(originalSite)==null) System.out.println("Something went wrong while copying site");
          if (fullSiteList.addSite(originalSite)==null) throw new XEDLMixerException("Problems found coping Admin-Defined Site: " + originalSiteId);
        }
      }

      // REGLAS PARA MEZCLAR
      //* El identificador (ID), el PUBLIC_NAME y el media_control (DISPLAYS) se toman de las definiciones parciales.
      //* En cuanto al SITE_ADDRESS, prevalece la informaci�n del organizador (aunque no conviene fijarla en el organizador)
      //* En los campos SITE-ROLE, CONNECTION-MODE, NETWORK-ACCESS, connection-parameters, CONTROL-CAPABILITIES y ADDITIONAL PARAMS prevalece la informaci�n del organizador.

      // Si writeFile es true, lo salva a fichero
      if (writeFile)
		try {
			fullXEDL.save(fullXEDLName);
		} catch (XEDLException e) {
			throw new XEDLMixerException("Problems found saving Full XEDL file: " + e);
		}
      

      //devolvemos el XEDL generado, que es el totalXEDL
      System.out.println("Global XEDL file composed.");
      return fullXEDL;
      
    } else {
      System.out.println ("This session is not local.");
      return null;
    }
  }





  /**
   * Genera un XEDL parcial a partir de la informacion proporcionada.
   * @param session Nombre de la sesion a la que pertenece el xedl.
   * @param id Identificador del sitio cuyo xedl parcial se quiere generar.
   * @param location lugar del sitio cuyo xedl parcial se quiere generar.
   * @param passwd Password del sitio cuyo xxedl parcial se quiere generar.
   * @param localAddress Direccion IP del sitio.
   * @param flowServer Direccion IP del nodo que hace de flow. Si el sitio es master, el flow es null
   * @param writeFile Indica si queremos grabar el xedl en fichero
   * @return el XEDL parcial generado
   * @throws XEDLMixerException si hubo algun error durante el proceso. 
   * @Author lailoken.
   */
  public static XEDL composeLocal (String session, String id, String passwd, String localAddress, String flowServer, boolean writeFile) throws XEDLMixerException {
    if (flowServer == null) flowServer = "";
    //llamamos a composelocal con location null
    return composeLocal (session,id, null, passwd, localAddress,flowServer,writeFile);
  }

  
  /**
   * Genera un XEDL parcial a partir de la informacion proporcionada.
   * @param session Nombre de la sesion a la que pertenece el xedl.
   * @param id Identificador del sitio cuyo xedl parcial se quiere generar.
   * @param passwd Password del sitio cuyo xxedl parcial se quiere generar.
   * @param localAddress Direccion IP del sitio.
   * @param flowServer Direccion IP del nodo que hace de flow. Si el sitio es master, el flow es null
   * @param writeFile Indica si queremos grabar el xedl en fichero
   * @return el XEDL parcial generado
   * @throws XEDLMixerException si hubo algun error durante el proceso. 
   * @Author lailoken.
   */
  public static XEDL composeLocal (String session, String id, String location, String passwd, String localAddress, String flowServer, boolean writeFile) throws XEDLMixerException {
	  //llamamos a compose local con nombre del profile null
	  return composeLocal(session, id, location, passwd, null, localAddress, flowServer, writeFile);
  }
  
  
  
  /**
   * Genera un XEDL parcial a partir de la informacion proporcionada.
   * @param session Nombre de la sesion a la que pertenece el xedl.
   * @param id Identificador del sitio cuyo xedl parcial se quiere generar.
   * @param passwd Password del sitio cuyo xxedl parcial se quiere generar.
   * @param profile_name nombre del profile que se usará, si es null usa el Default
   * @param localAddress Direccion IP del sitio.
   * @param flowServer Direccion IP del nodo que hace de flow. Si el sitio es master, el flow es null
   * @param writeFile Indica si queremos grabar el xedl en fichero
   * @return el XEDL parcial generado
   * @throws XEDLMixerException si hubo algun error durante el proceso. 
   * @Author lailoken.
   */
  public static XEDL composeLocal (String session, String id, String location, String passwd, String profile_name, String localAddress, String flowServer, boolean writeFile) throws XEDLMixerException {
	  
	  if (flowServer == null) flowServer = "";
	    //boolean isRemoteSession = false;
	    String deliveryPlatform = DEF_DELIVERY_PLATFR;

	    // Nos aseguramos de que existen los directorios
	    File xlimLogsWorkDir = new File(Constants.XLIM_LOGS_WORK_DIR);
	    if (!xlimLogsWorkDir.exists()) xlimLogsWorkDir.mkdirs();
	    File xlimTmpWorkDir = new File(Constants.XLIM_TMP_WORK_DIR);
	    if (!xlimTmpWorkDir.exists()) xlimTmpWorkDir.mkdirs();

	    // XLIM Notifications
	    XlimNotification xlimNotif;
	    try {
	      xlimNotif = new XlimNotification("Compose Local XEDL");
	    } catch (IOException ioe) {
	      throw new XEDLMixerException("Problems opening Xlim Services Notification file: " + ioe);
	    }
	    xlimNotif.println("Composing local XEDL file...");
	    
	    if (passwd == null) passwd = "";
	    else {
	      if (passwd.equals("\"\"")) passwd = "";
	    }
	    
	    // Especificamos por donde sacamos las trazas.
	    PrintWriter outTraceFile; 
	    try {
	      outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (LOCAL_TRACE_FILE_NAME)),true);
	    } catch (IOException ioe) {
	      throw new XEDLMixerException("Problems opening Trace file: " + ioe);
	    }

	    //-------------------------------------
	    // Para trazas horarias....
	    SimpleDateFormat timeFormatter = new SimpleDateFormat("HH:mm:ss.SSS");
	    //-------------------------------------

	    outTraceFile.println("Metodo composeLocal de XEDLMixerTools.");
	    outTraceFile.println(" deliveryPlatform: " + deliveryPlatform + ", flowServer: " + flowServer + ", session: " + session);

	    //Creamos el objeto para manejar la Agenda.
	    Agenda IsabelAgenda = null;
	    try {
	      if (AGENDA_ROOT_DIR != null) IsabelAgenda = new Agenda (AGENDA_ROOT_DIR);
	      else IsabelAgenda = new Agenda (Constants.ISABEL_DIR);
	    } catch (Exception e){
	      throw new XEDLMixerException("Error while opening Agenda. Error message: " + e);
	    }

	    outTraceFile.println (" Before opening options xml: " + timeFormatter.format(new Date(System.currentTimeMillis())));

	    // ----- Cogemos la informacion que necesitamos de OPTIONS ----
	    //ENRIQUE, hasta ahora se cogía siempre el default, vamos a coger el que usamos	    
	    File optionsFile = IsabelAgenda.getOptionsFile(profile_name);
	    String optionsPNAME = null;
	    // Role
	    String optionsRole = null;
	    // BANDWIDTH
	    int optionsBW = -1;
	    int optionsUPBW = -1;
	    int optionsDOWNBW = -1;
	    // Control Capabilities
	    String optionsCtrlCap = null;
	    // Media control
	    String optionsAppShCtrl = null;
	    String optionsAudioCtrl = null;
	    String optionsDataCtrl = null;
	    String optionsDefaultCtrl = null;
	    String optionsVideoCtrl = null;
	    // LinkedLists de Personal info
	    LinkedList optionsAdminContacts = new LinkedList();
	    LinkedList optionsTechContacts = new LinkedList();
	    // Institution
	    String optionsInstitution = null;
	    // Additional Params
	    SITEADDITIONALPARAMS addParams = null;
	    // IPSEC CONFIGURATION
	    IPSECCONF ipsecConfig = null;
	    // IPSEC CONFIGURATION CERT IDENTITY
	    String optionsDN = null;
	    String optionsFQDN = null;
	    // MULTICAST CONFIGURATION
	    MulticastParameters mcastParam = null;
	    // Indica si hay campo MCU en las options (para lo de multicast+mcu) para saber si el flow es para mcast o para mcu
	    boolean mcuInOptions = false;
	    // Indica si hay campo MULTICAST en las options (para lo de multicast+mcu) para saber si el flow es para mcast o para mcu
	    boolean mcastInOptions = false;
	    // FEC CONFIGURATION
	    int[] fecHammingParams = {0,0};
	    int[] fecParityParams = {0,0};
	    // ISDN CONFIGURATION
	    ISDNParameters isdnInOptions = null;
	    // Habria que sacar mas cosas
	    if (optionsFile.exists()) {
	      // Si existe el fichero sacamos la informacion necesaria
	      XEDL options;
	      try {
	        options = new XEDL (IsabelAgenda.getOptionsFile(profile_name).getAbsolutePath());
	      } catch (XEDLException xedle) {
	        throw new XEDLMixerException("Problems opening Options XEDL file: " + xedle);
	      }
	      SiteList optionsSiteList = new SiteList(options);
	      SITE optionsSite = optionsSiteList.getSite(optionsSiteList.getSiteIds().get(0).toString());
	      // Almacenamos todos los datos necesarios
	      optionsPNAME = optionsSite.getPUBLICNAME();
	      
	      optionsBW = optionsSite.getBandwidth();
	      optionsUPBW = optionsSite.getUpBandwidth();
	      optionsDOWNBW = optionsSite.getDownBandwidth();

	      optionsRole = optionsSite.getSITEROLE();
	      
	      optionsCtrlCap = optionsSite.getControlCapabilities();
	      
	      // Exportacion de displays
	      optionsAppShCtrl = optionsSite.getAppSharingCtrl();
	      optionsAudioCtrl = optionsSite.getAudioCtrl();
	      optionsDataCtrl = optionsSite.getDataCtrl();
	      optionsDefaultCtrl = optionsSite.getDefaultCtrl();
	      optionsVideoCtrl = optionsSite.getVideoCtrl();
	      
	      // Institution
	      optionsInstitution = optionsSite.getInstitution();
	      
	      // Personal info para admin contact y technical contact
	      LinkedList optionsAdminContactsNames = optionsSite.getAdminInfoEmails();
	      for (int i=0; i<optionsAdminContactsNames.size();i++) {
	        //outTraceFile.println("Trabajando con adminContact: " + optionsAdminContactsNames.get(i));
	        optionsAdminContacts.add(optionsSite.getAdminContact(String.valueOf(optionsAdminContactsNames.get(i))));
	      }
	      LinkedList optionsTechContactsNames = optionsSite.getTechInfoEmails();
	      outTraceFile.println("Linked list of techContacts: " + optionsTechContactsNames);
	      for (int i=0; i<optionsTechContactsNames.size();i++) {
	        //outTraceFile.println("valores de i " + i + " y de el size: "+optionsTechContactsNames.size());
	        //outTraceFile.println("Trabajando con TechContact: " + optionsTechContactsNames.get(i));
	        optionsTechContacts.add(optionsSite.getTechContact(String.valueOf(optionsTechContactsNames.get(i))));
	      }

	      // ADDITIONAL PARAMS
	      addParams = optionsSite.getSITEADDITIONALPARAMS();
	      
	      // IPSEC CONFIGURATION
	      ipsecConfig = optionsSite.getIPSECCONF();
	      
	      // IPSEC CONFIGURATION CERT IDENTITY
	      optionsDN = optionsSite.getDN();
	      optionsFQDN = optionsSite.getFQDN();
	      
	      // MULTICAST CONFIGURATION
	      if (optionsSite.getCONNECTIONMODE()!=null){
	        if (optionsSite.getCONNECTIONMODE().getMulticast()!=null) mcastInOptions = true;
	        if (optionsSite.getCONNECTIONMODE().getMcu()!=null) mcuInOptions = true;
	      }
	      if (mcastInOptions) {
	    	outTraceFile.println("Multicast en options");
	        mcastParam = optionsSite.setConnectionModeMulticast();
	        String padre = mcastParam.getMCASTROOT();
	        if(padre.equals(MulticastPanel.GATEWAY_NODE))
	        {
	        	outTraceFile.println("el padre era " + MulticastPanel.GATEWAY_NODE +", es el gateway");
	        	//padre = ""; No lo pongo a "" porque eso no es un nodo xml y no tira, getRootNode devuelve null
	        	if(flowServer.equals(""))
                {
                	//es el master
                	padre = "";
                }
	        }
	        else if(padre.equals(MulticastPanel.ROOT_NODE))
	        {
	        	outTraceFile.println("el padre era " + MulticastPanel.ROOT_NODE + " es un hijo.");
	        	padre = flowServer;
	        }
	        mcastParam.setMCASTROOT(padre);
	        //String node = mcastParam.getNode().getLocalName();
	        String nada = null;
	      }
	      
	      // FEC CONFIGURATION
	      fecHammingParams = optionsSite.getHammingProtectionModeParams();
	      fecParityParams = optionsSite.getParityProtectionModeParams();
	      
	      // ISDN CONFIGURATION
	      if (optionsSite.getNETWORKACCESS()!=null && optionsSite.getNETWORKACCESS().getISDN()!=null) {
	        // ISDN esta configurado
	        optionsSite.getNETWORKACCESS().setISDN(optionsSite.getNETWORKACCESS().getISDN());
	      }
	    }
	      
	    outTraceFile.println (" Before opening local xml: " + timeFormatter.format(new Date(System.currentTimeMillis())));
	    // ----- Cogemos de local.xml las ips que ya tuviesemos (si es que existe un local.xml para esta sesion en las sesiones remotas de la agenda)-----
	    LinkedList siteIPs = null;
	    String localID = null;
	    if ((IsabelAgenda.isRemoteSession(session)) || (IsabelAgenda.isLocalSession(session))) {
	      //isRemoteSession = true;
	      // Existe esta sesion en la agenda
	      File localXedlFile;
	      try {
	        localXedlFile = IsabelAgenda.getLocalXedl(session);
	        if (localXedlFile.exists()) {
	          try {
	            XEDL localXEDL = new XEDL (IsabelAgenda.getLocalXedl(session).getPath());
	            SiteList localSiteList = new SiteList(localXEDL);
	            localID = String.valueOf(localSiteList.getSiteIds().get(0));
	            // Obtenemos las IPs del local.xml
	            siteIPs = localSiteList.getSite(localID).getSiteAddress();
	            for (int i = 0; i < siteIPs.size(); i++) {
	              String siteIP = String.valueOf(siteIPs.get(i));
	            }
	          } catch (Exception e) {
	            xlimNotif.println("Error obtaining IPs from local.xml. Skipping local ips...");
	            //throw new XEDLMixerException("Error obtaining IPs from local.xml. Error message: " + e);
	          }
	          // Si habia passwd, la guardamos para no perderla
	          try {
	            XEDL localXEDL = new XEDL (IsabelAgenda.getLocalXedl(session).getPath());
	            SiteList localSiteList = new SiteList(localXEDL);
	            localID = String.valueOf(localSiteList.getSiteIds().get(0));
	            String oldPasswd = localSiteList.getSite(localID).getAccessControl();
	            if (oldPasswd == null) {
	              if (!passwd.equals("")) { // Si nos metieron passwd
	                // Protegemos la passwd.
	                //Generamos la Salt (la semilla de la encriptacion)
	                Random seed= new Random();
	                String salt = "" + seed.nextInt(10) + seed.nextInt(10);
	                outTraceFile.println("Salt: "+ salt);
	                //Encriptamos la passwd
	                passwd = Crypt.crypt(salt,passwd);
	              }
	            } else {
	              outTraceFile.println("old passwd: " + oldPasswd);
	              // se supone que la cadena passwd es del estilo "password PASSWD". A nosa solo nos interesa PASSWD
	              //ENRIQUE, ahora no se guarda "password PASSWD" sino sólo PASSWD
	              //passwd = oldPasswd.split(" ")[1]; // Ya esta encriptada. cogemos el segundo valor
	              passwd = oldPasswd;
	            }
	          } catch (Exception e){
	            xlimNotif.println("Error obtaining old passwd from local.xml. Skipping old passwd...");
	            if (!passwd.equals("")) { // Si nos metieron passwd
	              // Protegemos la passwd.
	              //Generamos la Salt (la semilla de la encriptacion)
	              Random seed= new Random();
	              String salt = "" + seed.nextInt(10) + seed.nextInt(10);
	              outTraceFile.println("Salt: "+ salt);
	              //Encriptamos la passwd
	              passwd = Crypt.crypt(salt,passwd);
	            }
	            //throw new XEDLMixerException("Error obtaining old Passwd from local.xml. Error message: " + e);
	          }
	        } else {
	          // No hay local.xml para esta sesion.
	          siteIPs = null;
	          // Creamos la sesion en la agenda
	          //IsabelAgenda.createSession(session,false);
	        }
	      } catch (IOException ioe) {
	        xlimNotif.println("Problems reading local XEDL file for session " + session + ". Skipping local information...");
	        //throw new XEDLMixerException("Problems reading local XEDL file for session " + session + ": " + ioe);
	      }
	    } else {
	      // No hay local.xml para esta sesion.
	      siteIPs = null;
	      // Creamos la sesion en la agenda
	      IsabelAgenda.createSession(session,flowServer.equals(""));
	    }
	    
	    outTraceFile.println (" Before creating local xml: " + timeFormatter.format(new Date(System.currentTimeMillis())));
	    // Actualizamos el fichero local.xml
	    // Obtenemos el path donde debe estar el fichero
	    String localXedlPath;
	    try {
	      localXedlPath = IsabelAgenda.getLocalXedl(session).getPath();
	    } catch (IOException ioe) {
	      throw new XEDLMixerException("Problems updating local XEDL file for session " + session + ": " + ioe);
	    }
	    File localXedlFile = new File (localXedlPath);
	    // borramos el fichero local.xml si es que existe
	    if (localXedlFile.exists()) {
	      if (!localXedlFile.delete()) {
	        throw new XEDLMixerException("Error deleting local.xml.");
	      }
	    }
	    XEDL localXedl;
	    try {
	      localXedl = new XEDL();
	    } catch (XEDLException xedle) {
	      throw new XEDLMixerException("Problems reading XEDL information from local XEDL file: " + xedle);
	    }
	    SiteList localSiteList = new SiteList(localXedl);
	    SITE localSite = localSiteList.createSite(id);
	    
	    // Passwd. 
	    /* Para version 4.8 */
	    //outTraceFile.println("Passwd: " + passwd);
	    if ((passwd != null) && (!passwd.equals(""))){
	      outTraceFile.println("Vamos a meter la passwd");
	      //ENRIQUE, ya no guardo password PASSWD sino sólo el PASSWD 
	      //localSite.setAccessControl("password " + passwd);
	      localSite.setAccessControl(passwd);
	    }
	    
	     
	    //Metemos todas las direcciones. Las que estaban en el localXEDL
	    if (siteIPs != null) {
	      for (int i = 0; i < siteIPs.size(); i++) {
	        String siteIP = String.valueOf(siteIPs.get(i));
	        if (!siteIP.equals(localAddress) && !siteIP.equals("")) {
	          localSite.insertSiteAddress (siteIP,false);
	        }
	      }
	    }
	    
	    // Y la nueva que hemos calculado	    
	    if (!localAddress.equals("::1") && !localAddress.equals(""))
	    {
	    	//si la ip que queremos meter no es ::1...
	    	localSite.insertSiteAddress(localAddress,true);
	    }	    	
	    else if (siteIPs == null) 
	    {
	    	//o si la direccion que queremos meter es una ::1 y no habia ips antiguas...
	    	//ya no meto el ::1 sino que si me pasan ::1 pongo todas las ips de los interfaces
	    	//localSite.insertSiteAddress(localAddress,true); 
	    	
	    	Enumeration<NetworkInterface> ifs = null;
			try {
				ifs = NetworkInterface.getNetworkInterfaces();
			} catch (SocketException e) {
				e.printStackTrace();
			}
		    
		    while (ifs.hasMoreElements()) {			
				NetworkInterface ni = ifs.nextElement();
				if(ni.getName().contains("lo"))
					continue;
				Enumeration<InetAddress> ias = ni.getInetAddresses();				
				while (ias.hasMoreElements()) {				    
				    InetAddress ia = ias.nextElement();	
				    String addr = ia.getHostAddress();
				    if(addr.indexOf("%")>0)
				    {
				    	//es ipv6 y tiene scope_id, lo quito
				    	addr = addr.substring(0, addr.indexOf("%"));
				    }
				    if(!addr.equals(""))
				    {
				    	localSite.insertSiteAddress(addr,true);
				    }
				}
		    }
	    }	      
	    else if ((siteIPs.size()==1) && (siteIPs.get(0).equals("::1"))) 
	    {
	    	//o si solo habia una ip antigua y era ::1
	    	localSite.insertSiteAddress(localAddress,true); 
	    }
	    
	    // Metemos el flow como mcu (dependiendo de si hay mcast y mcu definidas en options)
	    if ((!flowServer.equals("")) && (mcuInOptions || !(mcastInOptions))){
	        localSite.insertMCUAddress (flowServer,true);
	    }
	    else if((!flowServer.equals("") && mcastInOptions && mcastParam.getMCASTROOT().equals(MulticastPanel.GATEWAY_NODE)))
	    {
	    	outTraceFile.println("Es multicast y es el gateway, metemos también conexión mcu");
	    	localSite.insertMCUAddress (flowServer,true);
	    }
	    /*
	    else if (flowServer.equals("") && mcuInOptions && mcastInOptions)
	    {
	    	//soy el padre de la nube multicast pongo en MCU_ADDRESS la constante que tengo en el fichero constants
	    	//porque con lo que hay hecho ahora no puede ser cadena vacía, casca
	    	localSite.insertMCUAddress(Constants.MCU_ADDRESS_MULTICAST,true);
	    }*/

	    // Metemos el rol de la maquina
	    String siteRole = DEFAULT_SITEROLE;
	    localSite.setSITEROLE(siteRole);
	    if (optionsFile.exists()) {
	      // Estas opciones solo las escribimos si existe el fichero de options
	      localSite.setPUBLICNAME(optionsPNAME);
	      
	      if (optionsBW != -1) {
	        localSite.setBandwidth(optionsBW);
	      }
	      if (optionsUPBW != -1) {
	        localSite.setUpBandwidth(optionsUPBW);
	      }
	      if (optionsDOWNBW != -1) {
	        localSite.setDownBandwidth(optionsDOWNBW);
	      }
	        
	      localSite.setSITEROLE(optionsRole);
	      
	      localSite.setControlCapabilities(optionsCtrlCap);
	      
	      // Displays exportados
	      if (optionsAppShCtrl!=null) localSite.setAppSharingCtrl(optionsAppShCtrl);
	      if (optionsAudioCtrl!=null) localSite.setAudioCtrl(optionsAudioCtrl);
	      if (optionsDataCtrl!=null) localSite.setDataCtrl(optionsDataCtrl);
	      if (optionsDefaultCtrl!=null) localSite.setDefaultCtrl(optionsDefaultCtrl);
	      if (optionsVideoCtrl!=null) localSite.setVideoCtrl(optionsVideoCtrl);

	      // INSTITUTION
	      if (optionsInstitution != null) localSite.setInstitution(optionsInstitution);
	      // ADMIN CONTACTS
	      for (int i=0; i<optionsAdminContacts.size();i++) {
	        PersonalInformation adminPersonalInfo = (PersonalInformation)optionsAdminContacts.get(i);
	        outTraceFile.println("Working with adminContact: " + adminPersonalInfo.getEMAIL() +", " +adminPersonalInfo.getNAME() +", " + adminPersonalInfo.getMOBILENUMBER() +", " +adminPersonalInfo.getPHONENUMBER());
	        localSite.insertAdminContact(adminPersonalInfo);
	      }
	      
	      // TECH CONTACTS
	      for (int i=0; i<optionsTechContacts.size();i++) {
	        PersonalInformation techPersonalInfo = (PersonalInformation)optionsTechContacts.get(i);
	        if (techPersonalInfo == null) outTraceFile.println("This is null :S..");
	        outTraceFile.println("Working with techContact: " + techPersonalInfo.getEMAIL() +", " +techPersonalInfo.getNAME() +", " + techPersonalInfo.getMOBILENUMBER() +", " +techPersonalInfo.getPHONENUMBER());
	        localSite.insertTechContact(techPersonalInfo);
	      }

	      // ADDITIONAL PARAMS
	      if (addParams!=null) localSite.setSITEADDITIONALPARAMS(addParams);
	      
	      // IPSEC CONFIGURATION
	      if (ipsecConfig!=null) localSite.setIPSECCONF(ipsecConfig);

	      // IPSEC CONFIGURATION CERT IDENTITY
	      if (optionsDN!=null) localSite.setDN(optionsDN);
	      if (optionsFQDN!=null) localSite.setFQDN(optionsFQDN);
	      
	      // MULTICAST CONFIGURATION
	      
	      if (mcastParam!=null) {
	        // vemos si tenemos que meter el flowserver que nos dan como padre de la zona multicast..
	        //if ((!flowServer.equals("")) && (mcastInOptions) && (!mcuInOptions)){
	        //  mcastParam.setMCASTROOT(flowServer);
	        //}
	        localSite.addMulticastParams(mcastParam);
	      }
	      
	      // FEC CONFIGURATION
	      if (!((fecHammingParams[0] == 0) && (fecHammingParams[1] == 0))) {
	        localSite.setHammingProtectionMode(fecHammingParams[0], fecHammingParams[1]);
	      }
	      if (!((fecParityParams[0] == 0) && (fecParityParams[1] == 0))) {
	        localSite.setParityProtectionMode(fecParityParams[0], fecParityParams[1]);
	      }
	      
	      // ISDN CONFIGURATION
	      if (isdnInOptions!= null) {
	        localSite.setNetworkAccessISDN();
	        NETWORKACCESS netacc = new NETWORKACCESS();
	        ISDN isdn = new ISDN();
	        isdn.setISDNParameters(isdnInOptions);
	        netacc.setISDN(isdn);
	        localSite.setNETWORKACCESS(netacc);
	      } else {
	        localSite.setNetworkAccessEthernet();
	      }
	    }
	    
	    if (location != null) {
	      localSite.setPUBLICNAME(id + "." + location);
	    }
	    //añadimos el sitio
	    localXedl.getEdl().addSITE(localSite);
	    outTraceFile.println (" After creating local xml: " + timeFormatter.format(new Date(System.currentTimeMillis())));
	    // Volcamos la informacion a fichero
	    try {
	      localXedl.save(localXedlFile.getPath());
	    } catch (XEDLException e) {
	      throw new XEDLMixerException("Problems saving local XEDL file: " + e);
	    }
	    xlimNotif.println("local XEDL file composed.");
	    return localXedl;
	  }


	  public XEDLMixerTools() throws IOException {
	    //System.out.println("Inicializada....");
	  }

	  public static void main (String args[]) throws Exception {
	    if (args.length == 2) {
	      XEDLMixerTools kk = new XEDLMixerTools();
	      if (args[0].equals("composeLocal")) { // composeLocal
	        LineNumberReader newLine = new LineNumberReader(new InputStreamReader(System.in));
	        System.out.print("ID: ");
	        String id = newLine.readLine();
	        System.out.print("Location (enter if you don't want to set location): ");
	        String location = newLine.readLine();
	        if (location.equals("")) location = null;
	        System.out.print("PASSWD: ");
	        String passwd = newLine.readLine();
	        System.out.print("LOCALADDRESS: ");
	        String localAddress = newLine.readLine();
	        System.out.print("FLOWSERVER: ");
	        String flowServer = newLine.readLine();
	        kk.composeLocal (args[1], id, location, passwd, localAddress, flowServer, true);
	      }
	      if  (args[0].equals("composeXEDL")) { // composeXEDL
	        kk.xedlMixer(args[1],true);
	      }
	    } else {
	      System.out.println("Usage: services/xedlmixertools composeLocal||composeXEDL session ");
	    }
	    
	  }
  


 
  
  
}
