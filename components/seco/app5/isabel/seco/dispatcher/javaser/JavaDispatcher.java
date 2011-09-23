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
 * JavaDispatcher.java
 *
 */

package isabel.seco.dispatcher.javaser;

import java.io.IOException;

import java.util.Map;
import java.util.Vector;
import java.util.HashMap;
import java.util.logging.*;

import isabel.seco.network.*;
import isabel.lib.Queue;

import isabel.seco.network.javaser.JavaMessage;


/**
 * Los objetos de esta clase se encargan de escuchar la llegada de mensajes
 * procedentes de la red, encolarlos y irlos entregando uno a uno a los objetos
 * encargados de procesarlos segun el tipo de mensaje.
 * Para ello utiliza una cola y una hebra que la va vaciando.
 *
 * @author  Santiago Pavon
 * @author  Fernando Escribano
 */
public class JavaDispatcher implements NetworkListener, Runnable {
    

    /**
     *  Mapa: clave -> JavaDispatcher
     *
     *  Tabla donde se almacenan los objetos JavaDispatcher creados con el metodo 
     *  create, junto con su clave asociada.
     */
    private static HashMap<String,JavaDispatcher> dispMap = new HashMap<String,JavaDispatcher>();


    /** La hebra que vacia la cola y procesa los mesajes */
    private Thread thread;
    
    /** Cola para ir almacenando los mensajes */
    private Queue queue;
    
    /** Tabla que dice a que objeto hay que entregar cada mensaje. */
    private Map<Class,Vector<JavaDispatcherListener>> routingTable;
    
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
     *  Crea un objeto JavaDispatcher, y lo asocia a una clave.
     *  @param key Clave asociada con el objeto JavaDispatcher creado.
     *  @return Devuelve el objeto JavaDispatcher creado.
     *  @throws <tt>IOException</tt> Si ocurre un error de comunicacion.
     */
    public static JavaDispatcher createDispatcher(String key) /*throws IOException*/ {
	   JavaDispatcher aux = new JavaDispatcher(key);
	   dispMap.put(key,aux);
	   return aux;
    }


    /**
     *  Devuelve el objeto JavaDispatcher que se creo con el metodo create y que se asocio a la
     *  clave dada.
     *  Si no creo dicho objeto, entonces se crea en este momento.
     *  @param key Clave para buscar el objeto JavaDispatcher asociado.
     *  @return Devuelve el objeto JavaDispatcher asociado a la clave dada.
     *  @throws <tt>IOException</tt> Si ocurre un error de comunicacion.
     */
    public static JavaDispatcher getDispatcher(String key)/* throws IOException*/ {    	
	JavaDispatcher d = dispMap.get(key);
	if (d == null) {
	    return createDispatcher(key);
	} else {
	    return d;
	}
    }

    
    /**
     *  Devuelve el objeto JavaDispatcher que se creo con el metodo create y que se asocio a la
     *  clave vacia "".
     *  Si no creo dicho objeto, entonces se crea en este momento.
     *  @return Devuelve el objeto JavaDispatcher asociado a la clave dada.
     *  @throws <tt>IOException</tt> Si ocurre un error de comunicacion.
     */
    public static JavaDispatcher getDispatcher() /*throws IOException*/ {
    	
	JavaDispatcher d = dispMap.get("");
	if (d == null) {
	    return createDispatcher("");
	} else {
	    return d;
	}
    }


    //---------------------------------------------



    /** 
     * Crea un nuevo objeto JavaDispatcher llamando al contructor sin argumentos,
     * y almacena la clave que se uso al llamar a createDispatcher. 
     */
    private JavaDispatcher(String myKey) {
	this();
	this.myKey = myKey;
    }


    /** 
     * Crea un nuevo JavaDispatcher. 
     * La tabla que indica a que objetos deben ser entregados los mensajes que
     * llegan, se inicializa a vacia, luego habra que configurarla mas tarde
     * con el metodo SetDestinyFor() antes de que lleguen mensajes, ya que sino
     * seran descartados.
     */

    public JavaDispatcher() {
	mylogger = Logger.getLogger("isabel.seco.dispatcher.javaser.JavaDispatcher");
	mylogger.fine("Creating Dispatcher object.");
	
        queue = new Queue();
        thread = new Thread(this);
        routingTable = new HashMap<Class,Vector<JavaDispatcherListener>>();
        thread.start();
    }
    
    
    /** 
     * Anade una nueva ruta: objeto al que debe pasarse el mensaje
     * de clase especificado por el parametro type.
     * @param type Objeto Class del mensaje para el que queremos establecer
     *             el objeto procesador.
     * @param to JavaDispatcherListener al que se enviara el mensaje.
     */
    public synchronized void addDestiny(Class type, JavaDispatcherListener to) {
        if (routingTable.containsKey(type)) {
	    routingTable.get(type).addElement(to);
	} else {
	    Vector<JavaDispatcherListener> v = new Vector<JavaDispatcherListener>(1);
	    v.addElement(to);
	    routingTable.put(type, v);
	}
    }
    
    
    /** 
     * Metodo llamado por el objeto Network en sus listeners cuando se
     * produce la recepcion de un mensaje.
     * @param msg El mensaje recibido. Debe ser un objeto tipo JavaMessage.
     */
    public void messageReceived(Message msg) {
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
    	JavaMessage msg;
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
		msg = (JavaMessage) queue.get();
	    }
	    dispatch(msg);
	    msg=null;
	}

	// Limpieza:
	if (myKey != null) {
	    // Elimina de la tabla el objeto JavaDispatcher asociado a la clave dada.
	    dispMap.remove(myKey);
	}
    }
    
    /** 
     *  Obtiene los escuchadores y les pasa el mensaje.
     *  @param msg El mensaje a procesar.
     */
    private synchronized void dispatch(JavaMessage msg) {
	
        // Obtiene los procesadores y se les pasa el mensaje.
        Vector<JavaDispatcherListener> processors = getProcessors(msg.getClass());

        for (int i = 0; i < processors.size(); i++) {
	    JavaDispatcherListener to = (JavaDispatcherListener) processors.elementAt(i);
	    try {
		to.processMessage(msg);
	    }
	    catch (Exception e) {
		mylogger.severe("Unexpected error processing SeCo message:: "+
				"msg=\""+msg+"\" "+
				"to=\""+to.getClass()+"\" "+
				"error=\""+e.getMessage()+"\"");
	    }
        }
    }
    
    /** 
     *  Obtiene un vector con los procesadores de un mensaje segun su clase.
     *  @param type Objeto Class del mensaje cuyos procesadores queremos obtener.
     *  @return Vector de JavaDispatcherListener. Puede estar vacio.
     */
    private synchronized Vector<JavaDispatcherListener> getProcessors(Class type) {
        if (routingTable.containsKey(type)) {
            return routingTable.get(type);
        } else {
            mylogger.fine("DISPATCHER: Recibido mensaje de clase desconocida. " + type);
            return new Vector<JavaDispatcherListener>(0);
        }
    }
}
