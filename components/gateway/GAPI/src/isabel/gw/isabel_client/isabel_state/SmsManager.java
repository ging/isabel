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

package isabel.gw.isabel_client.isabel_state;

import isabel.seco.dispatcher.tclser.TclDispatcherListener;
import isabel.seco.network.tclser.TclMessage;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/*
 * SmsManager.java
 *
 * Client side of the State Message Service:
 * This manager is used to:
 *  - Send states to the master.
 *  - Maintains a routing table. It associates sms keys with listener objects.
 *    When a sms message is received, it is forwarded to the listener objects 
 *    interested in the key os these messages.
 * 
 */
public class SmsManager implements TclDispatcherListener {
    
    /** Logger where logs are written. */
    private Logger mylogger;
    
    /**
     * Reference to my IsabelState.
     */
    private IsabelStateImpl is;

    /** Tabla que dice a que objeto listener hay que entregar cada mensaje. */
    private Map<String,Vector<SmsManagerListener>> routingTable;
    
    /**
     * Objects of this class store the counters (nsec and origin) associated to one SMS key.
     * The state's data is not stored in this class.
     */
    private class SmsStateCounter {
	
	// Contador de cambios realizados en el estado.
	// Se usa para saber cual es el ultimo estado.
	int nsec = 0;
	
	// Origin: usado para decidir si debo actualizar el estado cuando llega un mensaje 
	// con el mismo nsec: solo actualizo si el mensaje viene del mismo sitio, es decir, 
	// el valor de origin es el mismo.
	// Este valor es unico para cada cliente del servicio. Usare el chid de los sitios
	// como valor de origen.
	int origin = 0;
	
	/**
	 * Construye un nuevo objeto con los contadores del estado.
	 *
	 * @param nsec Valor para el atributo nsec.
	 * @param origin Valor para el atributo origin.
	 */
	SmsStateCounter(int nsec, int origin) {
	    this.nsec = nsec;
	    this.origin = origin;
	}
	
	/**
	 * Compares the given nsec and origin with my nsec and origin, in order
	 * to know if they are associated to a state which must be applied.
	 *
	 * @return Returns true if the given values are associated to a state which must be applied, i.e. 
	 *         nsec is greater than my nsec, or it they are equals, then origin
	 *         is equal to my origin. Otherwise, 0 is returned.
	 *
	 * There is a special case, when nsec is 0. In this case the method always returns 1.
	 * It is used mainly when sending an initial message and I do not know about the previous history.
	 */
	boolean applicable (int nsec, int origin) {
	    
	    if (nsec == 0) { return true; }
	    if (nsec > this.nsec) { return true; }
	
	    if (nsec == this.nsec) {
		if (origin == this.origin) {
		    return true;
		}
	    }
	    return false;
	}
    }

    /**
     * SMS operations
     */
    public static enum SMS_OPERATION {ADD, SET, DEL};
    
    /** Mapa que asocia cada clave son su objeto contador. */
    private Map<String,SmsStateCounter> counters;

    /**
     * Create a new object.
     */
    public SmsManager(IsabelStateImpl is) {
	
	mylogger = Logger.getLogger("isabel.gw.isabel_client.isabel_state.SmsManager");
	mylogger.fine("Creating IsabelState::SmsManager object.");
	
	this.is = is;
	
        routingTable = new HashMap<String,Vector<SmsManagerListener>>();
        counters = new HashMap<String,SmsStateCounter>();
	
	// Le indico al dispatcher que los mensajes SMSC me los entrege a mi.
	is.getNetworkMgr().getDispatcher().addDestiny("SMSC", this);
    }
    

    /**    
     * joinKey se usa para:
     *   - Crear un nuevo estado asociado a la clave $key.
     *   - Configurar SeCo para que escuche en el grupo por donde se reciben las actualizaciones 
     *     de ese estado desde el master.
     *
     * Para establer a que objeto se debe avisar cuando se reciba un mensaje de actualizacion de estado,
     * debe usarse addSmsManagerListener.
     */    
    public synchronized void joinKey (String key) {
	
	// origin is used to avoid conflict with nsec
	int origin = Integer.parseInt(is.getNetworkMgr().getNetwork().getClientID());

	if (counters.containsKey(key)) {
	    SmsStateCounter ssc = counters.get(key);
	    ssc.nsec = 0;
	    ssc.origin = origin;
	} else {
	    counters.put(key, new SmsStateCounter(0, origin));
	}
	
	// Apuntarse al grupo "isabel_sms_<key>"
	try {
	    is.getNetworkMgr().getNetwork().joinGroup("isabel_sms_"+key);
	} catch (IOException e) {
	    mylogger.severe(e.getMessage());
	    System.exit(1);
	}
    }
    
  
    /**  
     * leaveKey se usa para:
     *  - Indicar a SeCo que no escuche el grupo por donde se reciben las actualizaciones 
     *    de ese estado.
     */    
    public synchronized void leaveKey (String key) {

	// Avandonar el grupo "isabel_sms_<key>"
	try {
	    is.getNetworkMgr().getNetwork().leaveGroup("isabel_sms_"+key);
	} catch (IOException e) {
	    mylogger.severe(e.getMessage());
	    System.exit(1);
	}
    }


    /**
     * Set a new state value.
     *
     * The nsec is updated and the new state is sent to master.
     *
     * $ope must be a SMS_OPERATION.
     */
    public synchronized void setState(String key, SMS_OPERATION ope, String args) {

	SmsStateCounter ssc;

	if (! counters.containsKey(key)) {
	    ssc = counters.get(key);
	    ssc.nsec = 0;
	    // origin is used to avoid conflict with nsec
	    ssc.origin = Integer.parseInt(is.getNetworkMgr().getNetwork().getClientID()); 
	} else {
	    ssc = counters.get(key);
	}

        // Send the state to the SmsManagerMaster.
	try {
	    if (ssc.nsec != 0) {
		ssc.nsec += 1;
	    }
	    is.getNetworkMgr().getNetwork().sendGroup("isabel_master",
						      new TclMessage("SMSM "+ key + " " + ssc.nsec + " " + ssc.origin + 
						  " " + ope.toString().toLowerCase() + " " + args));
	} catch (IOException ioe) {
	    mylogger.severe("I can't send a SMS message to master. I get: " + ioe.getMessage());
	}
    }
    

    /**
     * Force a new state value.
     *
     * This method is like setState but the sent message uses 0 as nsec to force the applicability of the state.
     */
    public synchronized void forceState(String key, SMS_OPERATION ope, String args) {

	SmsStateCounter ssc;

	if (! counters.containsKey(key)) {
	    ssc = counters.get(key);
	    ssc.nsec = 0;
	    // origin is used to avoid conflict with nsec
	    ssc.origin = Integer.parseInt(is.getNetworkMgr().getNetwork().getClientID()); 
	} else {
	    ssc = counters.get(key);
	}

        // Send the state to the SmsManagerMaster.
	try {
	    is.getNetworkMgr().getNetwork().sendGroup("isabel_master",
				   new TclMessage("SMSM "+ key + " 0 " + ssc.origin + 
						  " " + ope.toString().toLowerCase() + " " + args));
	} catch (IOException ioe) {
	    mylogger.severe("I can't send a SMS message to master. I get: " + ioe.getMessage());
	}
    }

    /** 
     * Anade una nueva redireccion: los mensajes sms con la clave dada deben pasarse al
     * objeto dado.
     * @param key Clave de los mensajes SMS a redirigir.
     * @param lis Objeto al que debe redirigirse el mensaje.
     */
    public synchronized void addSmsManagerListener(String key, SmsManagerListener lis) {
	
        if (routingTable.containsKey(key)) {
	    routingTable.get(key).addElement(lis);
	} else {
	    Vector<SmsManagerListener> v = new Vector<SmsManagerListener>(1);
	    v.addElement(lis);
	    routingTable.put(key, v);
	}
    }
    
    /** 
     * Remove am added listener.
     * @param key Clave de los mensajes SMS a redirigir.
     * @param lis Objet to remove.
     */
    public synchronized void removeSmsManagerListener(String key, SmsManagerListener lis) {
	
        if (routingTable.containsKey(key)) {
	    routingTable.get(key).removeElement(lis);
	}
    }
    
    /**
     * Este metodo se invoca para procesar un mensaje SMS recibido del master, y
     * que contiene el valor de un estado.
     *
     * El mensaje contiene los siguientes campos:
     *   - La clave del estado.
     *   - El tipo de mensaje. Si es "state" indica que a continuacion viene el
     *     numero de secuencia actual del estado y los pares opcion/valor. Si es "nsec", 
     *     indica que lo unique que viene a continuacion es el numero de secuencia.
     *   - El numero de secuencia actual del estado.
     *   - Lista con los pares {$opcion $valor} que formaran el nuevo estado.
     *
     * Si nos han enviado los valores del estado, se avisa a los listeners registrads
     * pasando como argumento la clave y los valores del estado.
     * 
     * @param msg Message to process.
     */
    public synchronized void processMessage(TclMessage msg) {
	
	// Contenido del mensaje:
	String cmd = msg.cmd;

	mylogger.info("SmsManager: Received a TCL message: \"" + cmd +"\".");
	
	// Obtener la clave, el tipo, el nsec, y los datos del mensaje
	Pattern p = Pattern.compile("^\\s*SMSC\\s+(\\S+)\\s+(\\S+)\\s+(\\d+)\\s*(.*)$");
	Matcher m = p.matcher(cmd);
	
	if (!m.lookingAt()) {
	    mylogger.severe("SmsManager: Received a unexpected SMS message: \"" + msg +"\".");
	    return;
	}
	String key  = m.group(1);
	String type = m.group(2);
	int nsec = Integer.parseInt(m.group(3));
	String data = m.group(4);

	if (! counters.containsKey(key)) return;

	SmsStateCounter ssc = counters.get(key);
	
	// Update the state counter:
	if (nsec > ssc.nsec) {
	    ssc.nsec = nsec;
	}

	if ("state".equals(type)) {
	    // Obtiene los escuchadores:
	    Vector<SmsManagerListener> listeners = getListeners(key);
	    
	    // Pasar el mensaje a los escuchadores.
	    for (int i = 0; i < listeners.size(); i++) {
		SmsManagerListener to = listeners.elementAt(i);
		try {
		    mylogger.info("Processing sms message: "+msg);
		    to.processSmsMessage(key, data);
		}
		catch (Exception e) {
		    mylogger.severe("Unexpected error processing SMS message:: "+
				    "msg=\""+msg+"\" "+
				    "to=\""+to+"\" "+
				    "error=\""+e.getMessage()+"\"");
		}
	    }
	}
    }
    

    /** 
     * Obtiene un vector con los escuchadores de los mensajes SMS de la clave dada.
     * @param key Clave de los mensajes SMS para buscar los listeners.
     * @return Vector de SmsManagerListener interesados en la clave dada. Puede estar vacio.
     */
    private synchronized Vector<SmsManagerListener> getListeners(String key) {
        if (routingTable.containsKey(key)) {
            return routingTable.get(key);
        } else {
            mylogger.fine("SmsManager: Recibido mensaje sms de clave desconocida: " + key);
            return new Vector<SmsManagerListener>(0);
        }
    }
    
}

