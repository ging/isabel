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
 * Network.java
 */

package isabel.seco.network;

import isabel.seco.access.*;

import java.io.*;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.logging.*;

/**
 *  Clase encargada de gestionar la comunicación con la red (a través del servicio SeCo).
 *  Permite el envio y recepción de mensajes, avisa de cuando se producen problemas
 *  en la red, y cuando se desconectan otros clientes.
 *
 *  @author  Fernando Escribano
 *  @author  Santiago Pavon
 *  @version 1.0
 */
public class Network implements SeCoListener {

    
    /**
     *  Mapa: clave -> Network  
     *
     *  Tabla donde se almacenan los objetos Network creados con el metodo 
     *  createNetwork, junto con su clave asociada.
     */
    private static HashMap<String,Network> netMap = new HashMap<String,Network>();

    
    /**
     * Para guardar las referencias a los objetos que quieran escuchar eventos de
     * mensajes llegados.
     */
    private ArrayList messageListeners;
    
    
    /**
     * El objeto, que implementa SeCoInterface, a través del cual enviamos mensajes
     * al servicio SeCo
     */
    private SeCoInterface seco;
    
    
    /**
     *  Host donde corre el servidor SeCo.
     */
    private String secohost;
    
    
    /**
     *  Puerto de conexion al servidor SeCo.
     */
    private int secoport;
    
    
    /**
     *  Objeto encargado de la (des)serializacion de los mensajes.
     */ 
    private Marshaller marshaller;


    /**
     *  Logger where logs are written.
     */
    private Logger mylogger;
  
  
    /**
     *  Key con la que se almaceno este objeto en netMap y que se
     *  especifico en la llamada a createNetwork.
     *  Si no se almaceno en netMap, myKey sera null.
     */
    private String myKey;

    /** 
     *  Ruta del fichero que contiene la clave privada del cliente
     *  almacenada en su formato de codificación primario.
     */
    private String keyFile;
    

    //---------------------------------------------


    /**
     *  Crea un objeto Network, y lo asocia a una clave.
     *  @param key Clave asociada con el objeto Network creado.
     *  @param secohost Host donde corre el servidor SeCo.
     *  @param secoport Puerto de conexion al servidor SeCo.
     *  @param marshaller Objeto encargado de la serializacin de los mensajes.
     *  @return Network objeto Network ya construido.
     *  @throws <tt>IOException</tt> Si ocurre un error de comunicación.
     */
    public static Network createNetwork(String key, String secohost, int secoport, Marshaller marshaller) throws IOException {
    	Network aux = new Network(key, secohost,secoport,marshaller);
	netMap.put(key,aux);
	return aux;
    }


    /**
     *  Crea un objeto Network con soporte para autenticación de clientes basada en firma digital,
     *  y asocia el objeto con una clave.
     *  @param key Clave asociada con el objeto Network creado.
     *  @param secohost host donde corre el servidor SeCo o SeCoProxy.
     *  @param secoport puerto de conexión al servidor SeCo o SeCoProxy.
     *  @param marshaller Objeto encargado de la serializacin de los mensajes.
     *  @param keyFile ruta del fichero con la clave privada del cliente 
     *                 almacenada en su formato de codificación primario.
     *  @return Network objeto Network ya construido.
     *  @throws <tt>IOException</tt>
     */
    public static Network createNetwork(String key, String secohost, int secoport, Marshaller marshaller, String keyFile) throws IOException {
    	Network aux = new Network(key, secohost,secoport,marshaller,keyFile);
	netMap.put(key,aux);
	return aux;
    }
    

    /**
     *  Devuelve el objeto Network que se creo con el metodo createNetwork y que se asocio a la
     *  clave dada.
     *  @param key Clave para buscar el objeto Network asociado.
     *  @return Devuelve el objeto Network asociado a la clave dada.
     */
    public static Network getNetwork(String key) {
	return netMap.get(key);
    }


    /**
     *  Devuelve el objeto Network que se creo con el metodo createNetwork y que se asocio a la
     *  clave vacia.
     *  @return Devuelve el objeto Network asociado a la clave dada.
     */
    public static Network getNetwork() {
	return netMap.get("");
    }


    //---------------------------------------------
    
    /** 
     * Crea un nuevo objeto Network llamando al contructor Network(secohost, secoport, marshaller).
     * y almacena la clave que se uso al llamar a createNetwork.
     */
    private Network(String myKey, String secohost, int secoport, Marshaller marshaller) throws IOException {
	this(secohost, secoport, marshaller);
	this.myKey = myKey;
    }

    
    /**
     *  Crea un nuevo objeto Network.
     *  @param secohost Host donde corre el servidor SeCo.
     *  @param secoport Puerto de conexion al servidor SeCo.
     *  @param marshaller Objeto encargado de la serializacin de los mensajes.
     *  @throws <tt>IOException</tt> Si ocurre un error de comunicación.
     */
    public Network(String secohost, int secoport, Marshaller marshaller) throws IOException {
	this(secohost, secoport, marshaller, null);
    }

    
    /** 
     * Crea un nuevo objeto Network llamando al contructor Network(secohost, secoport, marshaller, keyFile).
     * y almacena la clave que se uso al llamar a createNetwork.
     */
    private Network(String myKey, String secohost, int secoport, Marshaller marshaller, String keyFile) throws IOException {
	this(secohost, secoport, marshaller, keyFile);
	this.myKey = myKey;
    }

    /**
     *  Crea un nuevo objeto Network con soporte para
     *  autenticación de clientes basada en firma digital.
     *  @param secohost host donde corre el servidor SeCo o SeCoProxy.
     *  @param secoport puerto de conexión al servidor SeCo o SeCoProxy.
     *  @param marshaller Objeto encargado de la serializacin de los mensajes.
     *  @param keyFile ruta del fichero con la clave privada del cliente 
     *                 almacenada en su formato de codificación primario.
     *  @throws <tt>IOException</tt> Si ocurre un error de comunicación.
     */
    public Network(String secohost, int secoport, Marshaller marshaller, String keyFile) throws IOException {
    		mylogger = Logger.getLogger("isabel.seco.network.Network");
	mylogger.info("Creating Network object ("+secohost+","+secoport+","+marshaller.getClass()+","+keyFile+").");

        this.secohost   = secohost;
        this.secoport   = secoport;
	this.marshaller = marshaller;
        this.keyFile    = keyFile;
	
        messageListeners = new ArrayList();
	
	mylogger.fine("Creating SeCoAccess object.");

	SeCoAccess access;
	if(keyFile==null)
	    access = new SeCoAccess(secohost,secoport);
	else
	    access = new SeCoAccess(secohost,secoport,keyFile);
	
	access.addSeCoListener(this);
	
	seco = access;
	
	access.start();
    }
    
    
    /**
     * Método para apuntarse a uno de los grupos de comunicación de SeCo.
     * @param group El nombre del grupo al que queremos apuntarnos.
     * @throws <tt>IOException</tt> Si ocurre un error de comunicación.
     */
    public void joinGroup(String group) throws IOException {
	if (seco == null) return ;

        mylogger.fine("Request to join to group: " + group);
        seco.joinGroup(group);
    }

    
    /**
     * Método para apuntarse a uno de los grupos locales de comunicación de
     * SeCo.
     * 
     * @param group
     *            El nombre del grupo al que queremos apuntarnos.
     * @throws <tt>IOException</tt> Si ocurre un error de comunicación.
     */
    public void joinLocalGroup(String group) throws IOException {
    if (seco == null) return ;
    
	mylogger.fine("Request to join to group: " + group);
	seco.joinLocalGroup(group);
    }
    
    
    /**
     * Método para desapuntarse de uno de los grupos de comunicación de SeCo.
     * @param group El nombre del grupo del que queremos desapuntarnos.
     * @throws <tt>IOException</tt> Si ocurre un error de comunicación.
     */
    public void leaveGroup(String group) throws IOException {
	if (seco == null) return ;

        seco.leaveGroup(group);
    }

    
    /**
     * Método para desapuntarse de uno de los grupos locales de comunicación de
     * SeCo.
     * 
     * @param group
     *            El nombre del grupo del que queremos desapuntarnos.
     * @throws <tt>IOException</tt> Si ocurre un error de comunicación.
     */
    public void leaveLocalGroup(String group) throws IOException {
    if (seco == null) return ;
    
	seco.leaveLocalGroup(group);
    }
    

    /**
     * Devuelve el identificador de cliente que nos asigna SeCo.
     * Es un numero entero positivo.
     * @return String con el identificador.
     */
    public String getClientID() {

        return seco.getIdentity();
    }
    
    
    /**
     * Método para enviar un mensaje a un grupo sin que se guarde memoria de ese envio.
     * Esto significa que ese mensaje no será enviado a los clientes que se apunten
     * al grupo despues de que el envio se ha producido.
     * @param gname El nombre del grupo al que enviamos el mensaje.
     * @param msg El mensaje que queremos enviar.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o si
     * se ha hecho un quit() previamente.
     */
    public void sendGroup(String gname, Message msg) throws IOException {
	if (seco == null) return ;

        seco.sendGroup(gname,marshaller.marshall(msg));
    }


    /**
     * Método para enviar un mensaje a un grupo local sin que se guarde memoria
     * de ese envio. Esto significa que ese mensaje no será enviado a los
     * clientes que se apunten al grupo despues de que el envio se ha producido.
     * 
     * @param gname
     *            El nombre del grupo local al que enviamos el mensaje.
     * @param msg
     *            El mensaje que queremos enviar.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o
     *             si se ha hecho un quit() previamente.
     */
    public void sendLocalGroup(String gname, Message msg) throws IOException {
	if (seco == null) return ;
    
	seco.sendLocalGroup(gname, marshaller.marshall(msg));
    }
    
    
    /**
     * Método para enviar un mensaje con sincronismo a un grupo de comunicación.
     * @param gname El nombre del grupo al que queremos enviar el mensaje.
     * @param msg El mensaje que queremos enviar.
     * @param sync Boolean que indica si el mensaje es un mensaje de sincronismo o no.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o si
     * se ha hecho un quit() previamente.
     */
    public void sendGroup(String gname, Message msg, boolean sync) throws IOException {
	if (seco == null) return ;

        seco.sendGroup(gname, marshaller.marshall(msg), sync);
    }
    
    
    /**
     * Método para enviar un mensaje con sincronismo a un grupo local de
     * comunicación.
     * 
     * @param gname
     *            El nombre del grupo local al que queremos enviar el mensaje.
     * @param msg
     *            El mensaje que queremos enviar.
     * @param sync
     *            Boolean que indica si el mensaje es un mensaje de sincronismo
     *            o no.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o
     *             si se ha hecho un quit() previamente.
     */
    public void sendLocalGroup(String gname, Message msg, boolean sync)
	throws IOException {
	if (seco == null) return ;
    
	seco.sendLocalGroup(gname, marshaller.marshall(msg), sync);
    }
    

    /**
     * Método para enviar un mensaje a un cliente dado su ClientId.
     * @param id El identificador SeCo del cliente destino.
     * @param msg El mensaje que queremos enviar.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o si
     * se ha hecho un quit() previamente.
     */
    public void sendClient(int id, Message msg) throws IOException {
	if (seco == null) return ;

        seco.sendClient(id, marshaller.marshall(msg));
    }
    
    
    /**
     * Método para finalizar mi conexion con el servicio SeCo.
     * @throws IOException Si surge algún problema de comunicación.
     */
    public void quit() throws IOException {
	if (seco == null) return ;

	seco.removeSeCoListener(this);
        seco.quit();
	seco = null;

	if (myKey != null) {
	    // Elimina de la tabla el objeto Network asociado a la clave dada.
	    netMap.remove(myKey);
	}
    }
    
    
    /**
     * Apunta el listener especificado como parámetro para recibir eventos de llegada
     * de mensajes de la red.
     * Si lis es null no se lanza ninguna excepción ni se realiza ninguna acción.
     * @param lis El listener que queremos que reciba los eventos.
     */
    public synchronized void addNetworkListener(NetworkListener lis) {
        if (lis == null)
            return;
        messageListeners.add(lis);
    }
    
    
    /**
     * Elimina el listener especificado como parámetro para que deje de recibir eventos
     * de llegada de mensajes de la red.
     * Si el listener que se pasa como parámetro no había sido añadido o es null no
     * se realiza ninguna acción.
     * @param lis El listener que queremos que reciba los eventos.
     */
    public synchronized void removeNetworkListener(NetworkListener lis) {
	if (lis == null)
	    return;
	messageListeners.remove(lis);
    }
    
    
    /**
     * Este método es invocado cuando llega un mensaje y se encarga de
     * procesarlo adecuadamente.
     * @param msg Array de bytes que forman el mensaje.
     * @param from String con el identificador SeCo-Client del origen.
     * @param to String con el identificador SeCo del destino.
     */
    public void messageReceived(byte[] msg, String from, String to){
        try {

	    Message m = marshaller.unmarshall(msg);

            mylogger.finest("Received message: " + m);

            warnListenersOfMsg(m);
        }
	catch(Exception e) {
	    mylogger.severe("NETWORK: "+e);
	}
    }
    
    
    /**
     * Método que es invocado cuando se produce un error en las
     * comunicaciones.
     * @param reason La razón del problema.
     */
    public void serviceDead(String reason){
        mylogger.finest("SeCo service Dead. Reason: "+reason);
        warnListenersOfDead(reason);
    }
    
    
    /**
     * Método que es un invocado cuando un cliente se desconecta.
     * @param from El identificador SeCo cliente desconectado.
     */
    public void clientDisconnected(String from) {
        mylogger.finest("NETWORK: Client Disconnection. ID: " + from);
        warnListenersOfDisconnection(from);
    }
    
    
    /**
     * Método para avisar a todos los listeners de la llegada de un mensaje.
     * @param msg El mensaje recibido.
     */
    private void warnListenersOfMsg(Message msg) {
        ArrayList list;
        synchronized(this) {
            list = (ArrayList)messageListeners.clone();
        }
	
        Iterator it = list.iterator();
        while(it.hasNext()) {
            NetworkListener listener = (NetworkListener)it.next();
            listener.messageReceived(msg);
        }
    }
    
    
    /**
     * Método para avisar a los listeners de un problema en la red.
     * @param reason String explicativo del problema.
     */
    private void warnListenersOfDead(String reason) {
        ArrayList list;
        synchronized(this) {
            list = (ArrayList)messageListeners.clone();
        }
	
        Iterator it = list.iterator();
	
        while(it.hasNext()) {
            NetworkListener listener = (NetworkListener)it.next();
            listener.networkDead(reason);
        }
    }
    
    
    /**
     * Método para avisar a los listeners de la desconexion de un cliente.
     * @param clientId El cliente que se desconecta.
     */
    private void warnListenersOfDisconnection(String clientId) {
        ArrayList list;
        synchronized(this) {
            list = (ArrayList)messageListeners.clone();
        }
	
        Iterator it = list.iterator();
        while(it.hasNext()) {
            NetworkListener listener = (NetworkListener)it.next();
            listener.clientDisconnected(clientId);
	    
        }
    }
    
    /**
     * Método para que los clientes fijen el número máximo de paquetes a
     * almacenar en la historia para un grupo dado pasado como parámetro.
     * Ese número o tamaño máximo del buffer es el segundo parámetro que
     * pasamos al método.
     * @param group grupo del que vamos a fijar el tamaño del buffer para la historia.
     * @param size tamaño máximo del buffer para mensajes con historia.
     * @throws <tt>IOException</tt> si surge algún problema de comunicación
     * o si se ha hecho quit() previamente.
     */
    public void setMaxHistoryLength(String group, int size) throws IOException {
	if (seco == null) return ;

    	seco.setMaxHistoryLength(group,size);
    }
    

    /**
     * Método para que los clientes fijen el número máximo de paquetes a
     * almacenar en la historia para un grupo local dado pasado como parámetro.
     * Ese número o tamaño máximo del buffer es el segundo parámetro que pasamos
     * al método.
     * 
     * @param group
     *            grupo del que vamos a fijar el tamaño del buffer para la
     *            historia.
     * @param size
     *            tamaño máximo del buffer para mensajes con historia.
     * @throws <tt>IOException</tt> si surge algún problema de comunicación o
     *             si se ha hecho quit() previamente.
     */
    public void setLocalMaxHistoryLength(String group, int size)
	throws IOException {
	if (seco == null)
	    return;
	
	seco.setLocalMaxHistoryLength(group, size);
    }
}


