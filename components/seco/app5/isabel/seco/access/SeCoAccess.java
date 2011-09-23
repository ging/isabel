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
 * SeCoAccess.java
 */

package isabel.seco.access;

import isabel.seco.server.*;

import java.net.*;
import java.io.*;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.HashMap;
import java.util.logging.*;
import java.security.*;
import java.security.spec.*;

/**
 * Esta clase se encarga de la comunicación con el servidor SeCo para el envio
 * y la recepcion de mensajes con datos de control.
 * @author  Fernando Escribano, Santiago Pavon
 * @version 1.0
 */
public class SeCoAccess extends Thread implements SeCoInterface {
    
    /**
     * Tiempo (en segundos) que se espera entre un reintento y otro
     * de conexión con el servidor SeCo.
     */
    public static final int WAIT_TIME = 1;
    
    
    /**
     * Máximo número de intentos de conexión. Transcurrido este número de
     * intentos se lanzará una excepción indicando el fallo.
     */
    public static final int MAX_TRIES = 30;
    
    
    /**
     * Tabla para traducir los nombres (String) de los grupos a sus
     * identificadores SeCo (números enteros negativos).
     */
    private HashMap groupNames;
    
    
    /**
     * Tabla para traducir los nombres (String) de los grupos locales a sus
     * identificadores SeCo (números enteros negativos).
     */
    private HashMap localGroupNames;

    
    /**
     * Mi identificador de cliente SeCo.
     * Es un numero entero positivo.
     */
    private String clientID;
    
    
    /**
     * Para guardar las referencias a los objetos que quieran escuchar eventos de
     * mensajes llegados.
     */
    private ArrayList messageListeners;
    
    
    /**
     * Sockect de conexión con el servidor SeCo.
     */
    private Socket socket;
    
    
    /**
     * PacketProcessor usado para enviar mensajes servidor Seco, y esperar a
     * recibir los mensajes que este nos envie.
     */
    private PacketProcessor pp;
    
    
    /** Indica si estamos o no conectados al servidor SeCo. */
    private boolean connected;
    
    
    /** Logger where logs are written. */
    private Logger mylogger;
    
    
    /** Crea un nuevo objeto SeCoAccess.
     * @param secohost Host donde se esta ejecutando el servidor SeCo.
     * @param secoport Puerto donde escucha el servidor SeCo.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación
     * durante la creación del objeto.
     */
    public SeCoAccess(String secohost, int secoport) throws IOException {
	
	mylogger = Logger.getLogger("isabel.seco.access.SeCoAccess");
	mylogger.fine("Creating SeCoAccess object.");
	
        groupNames = new HashMap();
		localGroupNames = new HashMap();
        messageListeners = new ArrayList();
	
        // Abrimos la conexión con el servidor SeCo, reintentando varias veces si es necesario.
        for (int i = 0 ; ; i++) {
            try {
		mylogger.fine("Trying connection with SeCo server at "+secohost+" "+secoport+".");
		socket = new Socket(secohost, secoport);
                break;
            }
	    catch (IOException ioe) {
                if (i >= MAX_TRIES)
                    throw ioe;
		try {
		    Thread.sleep(WAIT_TIME * 1000);
		}
		catch (InterruptedException ie) {
		}
            }
        }
	mylogger.fine("Successful conection with SeCo server.");
	
        pp = new PacketProcessor(socket);
	
        // Enviamos un hello_client
	mylogger.fine("Making hello ...");
        SeCoPacket packet = new SeCoPacket();
        packet.setCommand(SeCoPacketTypes.HELLO_CLIENT_COMMAND);
        pp.send(packet);
	
	mylogger.finer("Waiting for answer from SeCo server ...");
	
        // Recogemos la respuesta con el clientID y lo guardamos como un String.
        clientID = pp.receive().getToID()+"";
        mylogger.fine("Hello accepted. ClientID=" + clientID);
	
        mylogger.fine("Making login ...");
        SeCoPacket packet2 = new SeCoPacket();
        packet2.setCommand(SeCoPacketTypes.LOGIN_COMMAND);
        packet2.setFromID(Integer.parseInt(clientID));
        packet2.setData("anonymous@ISABEL".getBytes());
        pp.send(packet2);
	
	mylogger.finer("Waiting for permissions from SeCo server ...");
	
        //Esperamos la respuesta con los permisos
        byte permissions=pp.receive().getData()[0];
        mylogger.fine("Login accepted. ClientID=" + clientID+" Permissions="+ permissions);
	
        connected = true;
    }
    
    /**
     * Crea un objeto SeCoAccess con soporte para autenticación de clientes basada en 
     * la firma digital de cierta información (resumen + cifrado con clave
     * privada). En el servidor se verificará, con la clave pública de cliente,
     * la autenticidad de la firma quedando el cliente autenticado o no.
     * @param secohost host donde se está ejecutando el servicio SeCo o SeCoProxy.
     * @param secoport puerto donde escucha el servidor SeCo o SeCoProxy. 
     * @param keyFile ruta del fichero que contiene la clave privada requerida
     * para firmar digitalmente ciertos datos de control.
     * @throws <tt>IOException</tt> si surge algún problema de comunicación
     * durante la creación del objeto.
     */
    public SeCoAccess(String secohost, int secoport, String keyFile) throws IOException {
	
        mylogger = Logger.getLogger("isabel.seco.access.secoaccess");
        mylogger.fine("Creating SeCoAccess object.");
	
        groupNames = new HashMap();
		localGroupNames = new HashMap();
        messageListeners = new ArrayList();
	
        // Abrimos la conexión con el servidor SeCo, reintentando varias veces si es necesario.
        for (int i = 0; i < MAX_TRIES; i++) {
            try {
                mylogger.fine("Trying connection with SeCo server at "+secohost+" "+secoport+".");
                socket = new Socket(secohost, secoport);
                break;
            }
            catch (IOException ioe) {
                if (i >= (MAX_TRIES - 1))
                    throw ioe;
                try {
                    Thread.sleep(WAIT_TIME * 1000);
                }
                catch (InterruptedException ie) {
                }
            }
        }
        mylogger.fine("Successful conection with SeCo server.");
	
        pp = new PacketProcessor(socket);
	
        // Enviamos un hello_client
        mylogger.warning("Making hello ...");
        SeCoPacket packet = new SeCoPacket();
        packet.setCommand(SeCoPacketTypes.HELLO_CLIENT_COMMAND);
        pp.send(packet);
	
        mylogger.warning("Waiting for answer from SeCo server ...");
	
        // Recogemos la respuesta con el clientID y lo guardamos como un String.
        clientID = pp.receive().getToID()+"";
        mylogger.warning("Hello accepted. ClientID=" + clientID);
	
        mylogger.warning("Making login ...");
        SeCoPacket packet2 = new SeCoPacket();
        packet2.setCommand(SeCoPacketTypes.LOGIN_COMMAND);
        packet2.setFromID(Integer.parseInt(clientID));
        ByteArrayOutputStream baos=new ByteArrayOutputStream();
        baos.write((keyFile.substring(keyFile.lastIndexOf("/")+1,keyFile.indexOf("_"))).getBytes());
        baos.write('@');
        baos.write(signText(keyFile,"ISABEL".getBytes()));
        byte[] baosArray=baos.toByteArray();
        mylogger.warning("Client data= "+new String(baosArray));
        mylogger.warning("Client data length= "+baosArray.length);
        packet2.setData(baosArray);
        pp.send(packet2);
	
        mylogger.warning("Waiting for permissions from SeCo server ...");
	
        //Esperamos la respuesta con los permisos
        byte permissions=pp.receive().getData()[0];
        mylogger.warning("Login accepted. ClientID=" + clientID+" Permissions="+ permissions);
	
        connected = true;
	
    }
    
    
    /**
     *  Cierra la conexión con el servidor SeCo y le dice a éste que puede liberar toda
     * la información relativa al usuario.
     * Cualquier intento posterior de enviar un mensaje o apuntarse a algún grupo
     * provocará que se lance una excepción.
     *
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación.
     */
    public void quit() throws IOException {
        mylogger.fine("Desconecting form SeCo server: quit()");
        if (connected) {
            connected = false;
            // Enviamos el bye y cerramos el socket.
            SeCoPacket packet = new SeCoPacket();
            packet.setCommand(SeCoPacketTypes.BYE_COMMAND);
            pp.send(packet);
            pp.close();
        }
    }
    
    
    /**
     * Devuelve el identificador de cliente que nos asigna el servidor SeCo.
     * El String contiene un numero entero positivo.
     * @return String con el identificador SeCo del cliente.
     */
    public String getIdentity() {
        return clientID;
    }
    
    
    
    /**
     * Método para apuntarse a uno de los grupos de comunicación.
     * Es necesario cuando queremos recibir mensajes, pero no para enviar.
     *
     * @param group El nombre del grupo al que queremos apuntarnos.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o si
     * se ha hecho un quit() previamente.
     */
    public void joinGroup(String group) throws IOException {
        if (connected) {
            SeCoPacket packet = new SeCoPacket();
            packet.setCommand(SeCoPacketTypes.JOIN_GROUP_COMMAND);
            packet.setFromID(Integer.parseInt(clientID));
            packet.setData(group.getBytes());
	    mylogger.finer("Sending request to join to group " + group + ": " + packet);
            pp.send(packet);
        } else {
            throw new IOException("Not connected");
        }
    }
    

    /**
     * Método para apuntarse a uno de los grupos locales de comunicación. Es
     * necesario cuando queremos recibir mensajes, pero no para enviar.
     * 
     * @param group
     *            El nombre del grupo local al que queremos apuntarnos.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o
     *             si se ha hecho un quit() previamente.
     */
    public void joinLocalGroup(String group) throws IOException {
	if (connected) {
	    SeCoPacket packet = new SeCoPacket();
	    packet.setCommand(SeCoPacketTypes.JOIN_LOCAL_GROUP_COMMAND);
	    packet.setFromID(Integer.parseInt(clientID));
	    packet.setData(group.getBytes());
	    mylogger.info("Sending request to join local group " + group
			  + ": " + packet);
	    pp.send(packet);
	} else {
	    throw new IOException("Not connected");
	}
    }


    
    /**
     * Método para borrarse de uno de los grupos de comunicación.
     * Hace que dejemos de recibir los mensajes enviados a dicho grupo.
     *
     * @param group El nombre del grupo del que queremos desapuntarnos.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o si
     * se ha hecho un quit() previamente.
     */
    public void leaveGroup(String group) throws IOException {
        mylogger.finest("LBA: leave(" + group + ")");
        if (connected) {
            SeCoPacket packet = new SeCoPacket();
            packet.setCommand(SeCoPacketTypes.LEAVE_GROUP_COMMAND);
            packet.setFromID(Integer.parseInt(clientID));
            packet.setData(group.getBytes());
            pp.send(packet);
        }
        else {
            throw new IOException("Not connected");
        }
    }
    
    
    /**
     * Método para borrarse de uno de los grupos locales de comunicación. Hace
     * que dejemos de recibir los mensajes enviados a dicho grupo.
     * 
     * @param group
     *            El nombre del grupo local del que queremos desapuntarnos.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o
     *             si se ha hecho un quit() previamente.
     */
    public void leaveLocalGroup(String group) throws IOException {
	mylogger.finest("LBA: leave(" + group + ")");
	if (connected) {
	    SeCoPacket packet = new SeCoPacket();
	    packet.setCommand(SeCoPacketTypes.LEAVE_LOCAL_GROUP_COMMAND);
	    packet.setFromID(Integer.parseInt(clientID));
	    packet.setData(group.getBytes());
	    pp.send(packet);
	} else {
	    throw new IOException("Not connected");
	}
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
    public void sendGroup(String gname, byte [] msg) throws IOException {
        mylogger.finest("Sending message to group " + gname + ".");
        if (connected) {
            // Si aun no tengo el identificador del grupo, lo pido.
            if (!groupNames.containsKey(gname)) {
                // pido el id del grupo.
                getGroupID(gname);
                //.. y espero la respuesta. (Posible problema de deadlock)
                // La hebra que envia y la que recibe no pueden ser la misma.
                while(!groupNames.containsKey(gname)) {
                    Thread.yield();
		}
            }
            // Ya tengo el id del grupo, luego envio el mensaje.
            SeCoPacket packet = new SeCoPacket();
            packet.setFromID(Integer.parseInt(clientID));
            packet.setToID(((Integer)groupNames.get(gname)).intValue());
            packet.setCommand(SeCoPacketTypes.NO_HISTORY_MSG_COMMAND);
            packet.setData(msg);
            pp.send(packet);
        } else {
            throw new IOException("Not connected");
        }
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
    public void sendLocalGroup(String gname, byte[] msg) throws IOException {
    	mylogger.finest("Sending message to local group " + gname + ".");
	if (connected) {
	    // Si aun no tengo el identificador del grupo, lo pido.
	    if (!localGroupNames.containsKey(gname)) {
		// pido el id del grupo.
		getLocalGroupID(gname);
		// .. y espero la respuesta. (Posible problema de deadlock)
		// La hebra que envia y la que recibe no pueden ser la misma.
		while (!localGroupNames.containsKey(gname)) {
		    Thread.yield();
		}
	    }
	    // Ya tengo el id del grupo, luego envio el mensaje.
	    SeCoPacket packet = new SeCoPacket();
	    packet.setFromID(Integer.parseInt(clientID));
	    packet.setToID(((Integer) localGroupNames.get(gname)).intValue());
	    packet.setCommand(SeCoPacketTypes.LOCAL_NO_HISTORY_MSG_COMMAND);
	    packet.setData(msg);
	    pp.send(packet);
	} else {
	    throw new IOException("Not connected");
	}
    }
    
    
    /**
     * Método para enviar un mensaje con sincronismo a un grupo de comunicación.
     * @param gname El nombre del grupo al que queremos enviar el mensaje.
     * @param msg El mensaje que queremos enviar.
     * @param sync Boolean que indica si el mensaje es un mensaje de sincronismo o no.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o si
     * se ha hecho un quit() previamente.
     */
    public void sendGroup(String gname, byte[] msg, boolean sync) throws IOException {
        mylogger.finest("Sending sync message to group " + gname + "," + sync + ".");
        if (connected) {
            // Si aun no tengo el identificador del grupo, lo pido.
            if (!groupNames.containsKey(gname)) {
                // pido el id del grupo.
                getGroupID(gname);
                //.. y espero la respuesta. (Posible problema de deadlock)
                // La hebra que envia y la que recibe no pueden ser la misma.
                while(!groupNames.containsKey(gname)) {
                    Thread.yield();
		}
            }
            // Ya tengo el id del grupo, luego envio el mensaje.
            SeCoPacket packet = new SeCoPacket();
            packet.setFromID(Integer.parseInt(clientID));
            packet.setToID(((Integer)groupNames.get(gname)).intValue());
            if (sync)
                packet.setCommand(SeCoPacketTypes.SYNC_MSG_COMMAND);
            else
                packet.setCommand(SeCoPacketTypes.MSG_COMMAND);
	    
            packet.setData(msg);
            pp.send(packet);
        } else {
            throw new IOException("Not connected");
        }
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
    public void sendLocalGroup(String gname, byte[] msg, boolean sync)
	throws IOException {
    mylogger.finest("Sending sync message to local group " + gname + ","
			+ sync + ".");
	if (connected) {
	    // Si aun no tengo el identificador del grupo, lo pido.
	    if (!localGroupNames.containsKey(gname)) {
		// pido el id del grupo.
		getLocalGroupID(gname);
		// .. y espero la respuesta. (Posible problema de deadlock)
		// La hebra que envia y la que recibe no pueden ser la misma.
		while (!localGroupNames.containsKey(gname)) {
		    Thread.yield();
		}
	    }
	    // Ya tengo el id del grupo, luego envio el mensaje.
	    SeCoPacket packet = new SeCoPacket();
	    packet.setFromID(Integer.parseInt(clientID));
	    packet.setToID(((Integer) localGroupNames.get(gname)).intValue());
	    if (sync)
		packet.setCommand(SeCoPacketTypes.LOCAL_SYNC_MSG_COMMAND);
	    else
		packet.setCommand(SeCoPacketTypes.LOCAL_MSG_COMMAND);
	    
	    packet.setData(msg);
	    pp.send(packet);
	} else {
	    throw new IOException("Not connected");
	}
    }
    
    
    /**
     * Método para enviar un mensaje a un cliente dado su ClientId.
     * @param dest El identificador SeCo del cliente destino.
     * @param msg El mensaje que queremos enviar.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o si
     * se ha hecho un quit() previamente.
     */
    public void sendClient(int dest, byte[] msg) throws IOException {
        mylogger.finest("Sending message to client " + dest + ".");
        if (connected) {
	    SeCoPacket packet = new SeCoPacket();
	    packet.setFromID(Integer.parseInt(clientID));
	    packet.setToID(dest);
	    packet.setCommand(SeCoPacketTypes.CLIENT_MSG_COMMAND);
	    packet.setData(msg);
	    pp.send(packet);
        } else {
            throw new IOException("Not connected");
        }
    }
    
    
    /**
     * Apunta el listener especificado como parámetro para recibir eventos de llegada
     * de mensajes del servidor Seco.
     * Si lis es null no se lanza ninguna excepción ni se realiza ninguna acción.
     * @param lis El listener que queremos que reciba los eventos.
     */
    public synchronized void addSeCoListener(SeCoListener lis) {
        if (lis == null)
            return;
        messageListeners.add(lis);
    }
    
    
    /**
     * Elimina el listener especificado como parámetro para que deje de recibir eventos
     * de llegada de mensajes del servidor SeCo.
     * Si el listener que se pasa como parámetro no había sido añadido o es null no
     * se realiza ninguna acción.
     * @param lis El listener que queremos que deje de recibir los eventos.
     */
    public synchronized void removeSeCoListener(SeCoListener lis) {
	if (lis == null)
	    return;
	messageListeners.remove(lis);
    }
    
    /** Esta hebra se dedica a escuchar por el socket y procesar los mensajes.
     */
    public void run() {
        SeCoPacket packet = new SeCoPacket();
        try {
            while(true) {
                packet = pp.receive();
                // Segun el tipo de primitiva se hacen diferentes cosas
                switch(packet.getCommand()) {
		    
                    // Si se trata de la recepción de un mensaje lo extraemos y
                    // avisamos a los listeners.
                    case SeCoPacketTypes.MSG_COMMAND:
                    case SeCoPacketTypes.SYNC_MSG_COMMAND:
                    case SeCoPacketTypes.NO_HISTORY_MSG_COMMAND:
		   			case SeCoPacketTypes.LOCAL_MSG_COMMAND:
		    		case SeCoPacketTypes.LOCAL_SYNC_MSG_COMMAND:
		    		case SeCoPacketTypes.LOCAL_NO_HISTORY_MSG_COMMAND:
                    case SeCoPacketTypes.CLIENT_MSG_COMMAND:
                        String from = packet.getFromID()+"";
                        String to   = packet.getToID()+"";
                        mylogger.finest("Received message (from="+from+",to="+to+",size="+packet.getDataSize()+").");
                        warnListenersOfMsg(packet.getData(), from , to);
                        break;
			
			// Si se trata de un identificador de grupo (contesta a un join o a un get)
			// lo guardo en la tabla.
                    case SeCoPacketTypes.JOIN_GROUP_COMMAND:
                    case SeCoPacketTypes.GET_GROUP_ID_COMMAND:
			String  gn  = new String(packet.getData());
			Integer gid = new Integer(packet.getToID());
                        mylogger.finest("Received a a group identifier: "+gn+"="+gid+".");
                        groupNames.put(gn, gid);
                        break;
			
			// Si se trata de un identificador de grupo (contesta a un join
			// o a un get)
			// lo guardo en la tabla.
		    case SeCoPacketTypes.JOIN_LOCAL_GROUP_COMMAND:
		    case SeCoPacketTypes.GET_LOCAL_GROUP_ID_COMMAND:
			String lgn = new String(packet.getData());
			Integer lgid = new Integer(packet.getToID());
			mylogger.finest("Received a local group identifier: " + lgn
					+ "=" + lgid + ".");
			localGroupNames.put(lgn, lgid);
			break;
			
			
			// Si se trata de un mensaje de aviso de desconexión aviso a los listeners
                    case SeCoPacketTypes.CLIENT_DISCONNECTED_COMMAND:
                        String from2 = packet.getFromID()+"";
                        mylogger.finest("Received notification of client disconection: "+from2);
                        warnListenersOfDisc(from2);
                        break;
			
			// Cualquier otro tipo no se atiende (por ahora)
                    default:
                        mylogger.finest("Received unknow command type: " + packet.getCommand());
                }
            }
        } catch (IOException e) {
	    mylogger.severe("Terminated SeCoAccess thread: " + e.getMessage());
	    if (connected) {
		warnListenersOfDead(e.getMessage());
	    }
        }
    }
    
    /** Método para avisar a todos los listeners de la llegada de un mensaje.
     * @param msg Array de bytes que forman el mensaje.
     * @param from String con el identificador del cliente que envio el mensaje.
     * @param to String con el identificador SeCo del destino.
     */
    private void warnListenersOfMsg(byte [] msg, String from, String to) {
        ArrayList list;
        synchronized(this) {
            list = (ArrayList)messageListeners.clone();
        }
	
        Iterator it = list.iterator();
	
        while(it.hasNext()) {
            SeCoListener listener = (SeCoListener)it.next();
            listener.messageReceived(msg, from, to);
        }
    }
    
    /** Método para avisar a todos los listeners de la desconexión de un cliente.
     * @param from String con el identificador SeCo del cliente.
     */
    private void warnListenersOfDisc(String from) {
        ArrayList list;
        synchronized(this) {
            list = (ArrayList)messageListeners.clone();
        }
	
        Iterator it = list.iterator();
	
        while(it.hasNext()) {
            SeCoListener listener = (SeCoListener)it.next();
            listener.clientDisconnected(from);
        }
    }
    
    /** Método para avisar a los listeners de un problema en la red.
     * @param reason String explicativo del problema.
     */
    private void warnListenersOfDead(String reason) {
        ArrayList list;
        synchronized(this) {
            list = (ArrayList)messageListeners.clone();
        }
	
        Iterator it = list.iterator();
	
        while(it.hasNext()) {
            SeCoListener listener = (SeCoListener)it.next();
            listener.serviceDead(reason);
        }
    }
    
    /** Método para preguntar por el identificador de un grupo.
     * El nombre se almacenará en la tabla de identificadores cuando
     * llegue la respuesta.
     * @param name El nombre del grupo cuyo Id queremos saber.
     * @throws <tt>IOException</tt> si surgen problemas de comunicación.
     */
    private void getGroupID(String name) throws IOException {
        SeCoPacket packet = new SeCoPacket();
        packet.setCommand(SeCoPacketTypes.GET_GROUP_ID_COMMAND);
	packet.setFromID(Integer.parseInt(clientID));
        packet.setData(name.getBytes());
        pp.send(packet);
    }
    

    /**
     * Método para preguntar por el identificador de un grupo. El nombre se
     * almacenará en la tabla de identificadores cuando llegue la respuesta.
     * 
     * @param name
     *            El nombre del grupo cuyo Id queremos saber.
     * @throws <tt>IOException</tt> si surgen problemas de comunicación.
     */
    private void getLocalGroupID(String name) throws IOException {
	SeCoPacket packet = new SeCoPacket();
	packet.setCommand(SeCoPacketTypes.GET_LOCAL_GROUP_ID_COMMAND);
	packet.setFromID(Integer.parseInt(clientID));
	packet.setData(name.getBytes());
	pp.send(packet);
    }
    
    
    /**
     * Método que fija para el cliente que lo invoca el tamaño máximo del
     * buffer o número máximo de paquetes que se van a almacenar en la 
     * historia para el grupo que le pasamos como parámetro.
     * @param group grupo para el que vamos a fijar el tamaño del buffer.
     * @param size número máximo de paquetes almacenados en la historia.
     * @throws <tt>IOException</tt> si surge algún problema de comunicación
     * o se ha hecho quit() previamente. 
     */
    public void setMaxHistoryLength(String group, int size) throws IOException{
	if (connected) {

            // Si aun no tengo el identificador del grupo, lo pido.
            if (!groupNames.containsKey(group)) {
                // pido el id del grupo.
                getGroupID(group);
                //.. y espero la respuesta. (Posible problema de deadlock)
                // La hebra que envia y la que recibe no pueden ser la misma.
                while (!groupNames.containsKey(group)) {
                    Thread.yield();
		}
            }

	    SeCoPacket packet = new SeCoPacket();
	    packet.setFromID(Integer.parseInt(clientID));
            packet.setToID(((Integer)groupNames.get(group)).intValue());
	    packet.setCommand(SeCoPacketTypes.SET_MAX_HISTORY_LENGTH_COMMAND);

	    ByteArrayOutputStream data = new ByteArrayOutputStream();
	    DataOutputStream data2 = new DataOutputStream(data);

	    //data.write((group+'#'+size).getBytes());

	    data2.writeInt(size);
	    data2.flush();

	    packet.setData(data.toByteArray());
	    pp.send(packet);
	} else {
	    throw new IOException("Not connected");
	}
    }
    
    
    /**
     * Método que fija para el cliente que lo invoca el tamaño máximo del buffer
     * o número máximo de paquetes que se van a almacenar en la historia para el
     * grupo local que le pasamos como parámetro.
     * 
     * @param group
     *            grupo local para el que vamos a fijar el tamaño del buffer.
     * @param size
     *            número máximo de paquetes almacenados en la historia.
     * @throws <tt>IOException</tt> si surge algún problema de comunicación o
     *             se ha hecho quit() previamente.
     */
    public void setLocalMaxHistoryLength(String group, int size)
	throws IOException {
	if (connected) {
	    
	    // Si aun no tengo el identificador del grupo, lo pido.
	    if (!localGroupNames.containsKey(group)) {
		// pido el id del grupo.
		getLocalGroupID(group);
		// .. y espero la respuesta. (Posible problema de deadlock)
		// La hebra que envia y la que recibe no pueden ser la misma.
		while (!localGroupNames.containsKey(group)) {
		    Thread.yield();
		}
	    }
	    
	    SeCoPacket packet = new SeCoPacket();
	    packet.setFromID(Integer.parseInt(clientID));
	    packet.setToID(((Integer) localGroupNames.get(group)).intValue());
	    packet
		.setCommand(SeCoPacketTypes.SET_LOCAL_MAX_HISTORY_LENGTH_COMMAND);
	    
	    ByteArrayOutputStream data = new ByteArrayOutputStream();
	    DataOutputStream data2 = new DataOutputStream(data);
	    
	    // data.write((group+'#'+size).getBytes());
	    
	    data2.writeInt(size);
	    data2.flush();
	    
	    packet.setData(data.toByteArray());
	    pp.send(packet);
	} else {
	    throw new IOException("Not connected");
	}
    }


    /**
     * Este método devuelve un array de bytes que contiene la firma digital
     * del los datos (bytes) pasados como parámetro. Para obtener esa firma
     * digital que distinga al cliente de los demás necesitamos su clave
     * privada, con lo que le pasamos como parámetro la ruta del fichero .prv
     * donde se encuentra su clave privada. La idea es que el servidor, que
     * dispone de la clave pública correspondiente a ésta privada, verifique
     * la firma digital de ese cliente, quedando éste autenticado.
     * @param keyFile ruta del fichero que contiene la clave privada del cliente.
     * @param data datos que vamos a firmar digitamente con la clave privada.
     * @return firma digital de los datos pasados como parámetro.
     */
    private byte[] signText(String keyFile,byte[] data){
	FileInputStream in=null;
	try{
	    
	    in = new FileInputStream(keyFile);
	    byte[] privKeyEnc=new byte[in.available()];
	    in.read(privKeyEnc);
	    
	    PKCS8EncodedKeySpec privKeySpec=new PKCS8EncodedKeySpec(privKeyEnc);
	    KeyFactory keyFactory=KeyFactory.getInstance("DSA");
	    PrivateKey privKey=keyFactory.generatePrivate(privKeySpec);
	    
	    Signature dsa=Signature.getInstance("SHA1withDSA");
	    dsa.initSign(privKey);
	    dsa.update(data);
	    byte[] sig=dsa.sign();
	    mylogger.warning("Signature= "+new String(sig));
	    mylogger.warning("Signature length= "+sig.length);
	    return sig;
	    
	}catch(NoSuchAlgorithmException e){
	    e.printStackTrace();
	}catch(InvalidKeySpecException e){
	    e.printStackTrace();
	}catch(InvalidKeyException e){
	    e.printStackTrace();
	}catch(SignatureException e){
	    e.printStackTrace();
	}catch(FileNotFoundException e){
	    e.printStackTrace();
	}catch(IOException e){
	    e.printStackTrace();
	}finally{
	    try{
		if (in != null) in.close();
	    }catch(IOException e){
		e.printStackTrace();
	    }
	}
	
	return null;
    }
    
    
}

