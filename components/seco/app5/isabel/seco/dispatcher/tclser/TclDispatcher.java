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
 * TclDispatcher.java
 *
 */

package isabel.seco.dispatcher.tclser;

import java.io.IOException;

import java.util.Map;
import java.util.Vector;
import java.util.HashMap;
import java.util.logging.*;
import java.util.regex.*;

import isabel.seco.network.*;
import isabel.lib.Queue;

import isabel.seco.network.tclser.TclMessage;


/**
 * Los objetos de esta clase se encargan de escuchar la llegada de mensajes
 * procedentes de la red, encolarlos y irlos entregando uno a uno a los objetos
 * encargados de procesarlos segun el tipo de mensaje.
 * Para ello utiliza una cola y una hebra que la va vaciando.
 *
 * @author Santiago Pavon
 * @author Fernando Escribano
 */
public class TclDispatcher implements NetworkListener, Runnable {
    

    /**
     *  Mapa: clave -> TclDispatcher
     *
     *  Tabla donde se almacenan los objetos TclDispatcher creados con el metodo 
     *  create, junto con su clave asociada.
     */
    private static HashMap<String,TclDispatcher> dispMap = new HashMap<String,TclDispatcher>();



    /** La hebra que vacia la cola y procesa los mesajes */
    private Thread thread;
    
    /** Cola para ir almacenando los mensajes */
    private Queue queue;
    
    /** Tabla que dice a que objeto hay que entregar cada mensaje. */
    private Map<String,Vector<TclDispatcherListener>> routingTable;
    
    /** Logger where logs are written. */
    private Logger mylogger;


    /**
     *  Key con la que se almaceno este objeto en dispMap y que se
     *  especifico en la llamada a createDispatcher.
     *  Si no se almaceno en dispMap, myKey sera null.
     */
    private String myKey;


    /**
     *  Flag que indica cuando debe terminar el thread.
     */
    private boolean mustStop = false;

    //---------------------------------------------


    /**
     *  Crea un objeto TclDispatcher, y lo asocia a una clave.
     *  @param key Clave asociada con el objeto TclDispatcher creado.
     *  @return Devuelve el objeto TclDispatcher creado.
     *  @throws <tt>IOException</tt> Si ocurre un error de comunicacion.
     */
    public static TclDispatcher create(String key) throws IOException {
	TclDispatcher aux = new TclDispatcher(key);
	dispMap.put(key,aux);
	return aux;
    }


    /**
     *  Devuelve el objeto TclDispatcher que se creo con el metodo create y que se asocio a la
     *  clave dada.
     *  Si no creo dicho objeto, entonces se crea en este momento.
     *  @param key Clave para buscar el objeto TclDispatcher asociado.
     *  @return Devuelve el objeto TclDispatcher asociado a la clave dada.
     */
    public static TclDispatcher get(String key) throws IOException {
	TclDispatcher d = dispMap.get(key);
	if (d == null) 
	    return create(key);
	else
	    return d;
    }


    //---------------------------------------------

    
    /** 
     * Crea un nuevo objeto TclDispatcher llamando al contructor sin argumentos,
     * y almacena la clave que se uso al llamar a createDispatcher. 
     */
    private TclDispatcher(String myKey) {
	this();
	this.myKey = myKey;
    }


    /** 
     * Crea un nuevo TclDispatcher. 
     * La tabla que indica a que objetos deben ser entregados los mensajes que
     * llegan, se inicializa a vacia, luego habra que configurarla mas tarde
     * con el metodo SetDestinyFor() antes de que lleguen mensajes, ya que sino
     * seran descartados.
     */
    public TclDispatcher() {
	mylogger = Logger.getLogger("isabel.seco.dispatcher.tclser.Dispatcher");
	mylogger.fine("Creating TclDispatcher object.");
        queue = new Queue();
        thread = new Thread(this);
        routingTable = new HashMap<String,Vector<TclDispatcherListener>>();
        thread.start();
    }
    
    
    /** 
     * Anade una nueva ruta: objeto al que debe pasarse los mensajes del tipo
     * especificado por el parametro type.
     * @param type Tipo de mensaje para el que queremos asociar un objeto procesador.
     * @param to TclDispatcherListener al que se enviara el mensaje.
     */
    public synchronized void addDestiny(String type, TclDispatcherListener to) {
        if (routingTable.containsKey(type)) {
	    routingTable.get(type).addElement(to);
	} else {
	    Vector<TclDispatcherListener> v = new Vector<TclDispatcherListener>(1);
	    v.addElement(to);
	    routingTable.put(type, v);
	}
    }
    
    
    /** 
     * Metodo llamado por el objeto Network en sus listeners cuando se
     * produce la recepcion de un mensaje.
     * @param msg El mensaje recibido.
     */
    public void messageReceived(Message msg) {

	mylogger.finest("Message received: "+(TclMessage)msg);

        synchronized(thread) {
	    // Se guarda el mensaje en la cola
	    queue.put(msg);
	    
	    // Se avisa a la hebra de que hay mensajes pendientes
	    thread.notify();
	}
    }
    
    
    /** 
     * Metodo llamado por el objeto Network en sus listeners cuando ocurre
     * un problema con la comunicacion.
     * @param info String con informacion sobre los motivos.
     */
    public void networkDead(String info) {
	// nada que hacer
    }
    
    
    /** 
     * Metodo que es un invocado cuando un cliente se desconecta.
     * @param clientId El identificador SeCo del cliente desconectado.
     */
    public void clientDisconnected(String clientId) {
	// nada que hacer
    }
    
    
    /**
     *  Indica al thread que debe terminar.
     */
    public void quit() {
	mustStop = true;
    }


    /** Metodo que va vaciando la cola y repartiendo los mensajes */
    public void run() {
    	TclMessage msg;
        while (!mustStop) {
	    synchronized(thread) {
		if (queue.isEmpty()) {
		    try {
			thread.wait();
		    } catch (InterruptedException e) {
			mylogger.severe(e.getMessage());
                    	return;
		    }
		}
		msg = (TclMessage) queue.get();
	    }
	    dispatch(msg);
	    msg=null;
	}
	
	// Limpieza:
	if (myKey != null) {
	    // Elimina de la tabla el objeto TclDispatcher asociado a la clave dada.
	    dispMap.remove(myKey);
	}
    }
    
    
    /** 
     *  Obtiene los escuchadores  y les pasa el mensaje.
     *  @param msg El mensaje a procesar.
     */
    private synchronized void dispatch(TclMessage msg) {
	
	// Contenido del mensaje:
	String body = msg.cmd;

	// Obtener tipo de mensaje
	Pattern p = Pattern.compile("(\\S*)");
	Matcher m = p.matcher(body);

	if (!m.lookingAt()) {
            mylogger.warning("DISPATCHER: Recibido mensaje de clase no identificable: \"" + msg +"\".");
	    return;
	}
	String type = m.group(1);

        // Obtiene los procesadores 
        Vector<TclDispatcherListener> processors = getProcessors(type);

	// Pasar el mensaje a los procesadores.
        for (int i = 0; i < processors.size(); i++) {
	    TclDispatcherListener to = processors.elementAt(i);
	    try {
		mylogger.finest("Processing message: "+msg);
		to.processMessage(msg);
	    }
	    catch (Exception e) {
		mylogger.severe("Unexpected error processing SeCo message:: "+
				"msg=\""+msg+"\" "+
				"to=\""+to+"\" "+
				"error=\""+e.getMessage()+"\"");
	    }
        }
    }
    
    /** 
     *  Obtiene un vector con los procesadores de un mensaje segun su clase.
     *  @param type Tipo de mensaje cuyos procesadores queremos obtener.
     *  @return Vector de TclDispatcherListener. Puede estar vacio.
     */
    private synchronized Vector<TclDispatcherListener> getProcessors(String type) {
        if (routingTable.containsKey(type)) {
            return routingTable.get(type);
        } else {
            mylogger.fine("DISPATCHER: Recibido mensaje de clase desconocida. " + type);
            return new Vector<TclDispatcherListener>(0);
        }
    }
}
