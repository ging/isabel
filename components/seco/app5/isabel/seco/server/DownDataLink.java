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
 * DownDataLink.java
 */

package isabel.seco.server;

import isabel.lib.Queue;

import isabel.lib.reliableudp.RUDPSocket;
import java.net.*;
import java.io.*;
import java.util.*;
import java.util.logging.*;

/**
 * Esta clase se encarga de gestionar las conexion de datos que
 * establecen los procesos clientes u otros nodos SeCo con nuestro nodo.
 * @author Santiago Pavon
 * @author Javier Calahorra
 */
class DownDataLink extends Thread {
    
    /**
     *  Objeto utilizado para enviar y recibir por el socket.
     */
    private PacketProcessor myPP;


    /** 
     *  True si estamos atendiendo a un cliente.
     *  False si estamos atendiendo a otro nodo SeCo.
     */
    private boolean client;
    
    /** 
     *  Si estamos atendiendo a un cliente, este es su identificador
     *  de cliente.
     */
    private int myClientId;
    
    /** 
     *  Si atendemos a un cliente, estos son sus permisos.
     */
    private byte permissions;
    
    /** 
     *  Cola de DownDataLink pendientes de identificador de cliente.
     */
    private Queue clientIdQueue;
    
    /** 
     *  Lista de los clientes alcanzables desde nuestro PacketProcessor.
     */
    private ArrayList myClients;
    
    /** 
     *  Almacena DownDataLink esperando permisos de su cliente
     */
    private HashMap permissionList;
    
    /** 
     *  Objeto Permissions que unicamente posee el nodo SeCo raiz. 
     */
    private Permissions permObj;
    
    /** 
     *  Objeto IdGenerator.
     *  El nodo raiz lo usa para generar los identificadores de los clientes
     *  y de los grupos globales y locales.
     *  Los demas nodos lo usan para los identificadores de los grupos locales.
     */
    private IdGenerator idGenObj;
    
    /**
     *  True if the created DownDataLink object belongs to the root SeCo node.
     */
    private boolean isRootNode;

    /** 
     *  Logger where logs are written.
     */
    private Logger mylogger;
    
    /** 
     *  Objeto que almacena el estado del nodo.
     */
    private isabel.seco.server.State state;
    

    /**
     *  Constructor de un objeto DownDataLink para el nodo SeCo raiz.
     *  @param s socket de acceso a cliente o nodo SeCo al que atendemos.
     *  @param state objeto que almacena el estado del nodo
     *  @param idGenObj Objeto para generar identificadores unicos.
     *  @param per objeto de calculo de permisos
     *  @throws IOException algo fue mal
     */
    DownDataLink(Socket s, isabel.seco.server.State state, IdGenerator idGenObj, Permissions per) throws IOException {
        this(s,state,idGenObj,null,null,per,true);
    }


    /**
     *  Constructor de un objeto DownDataLink para un nodo SeCo que no es raiz,
     *  @param s socket de acceso a cliente o nodo SeCo al que atendemos.
     *  @param state objeto que almacena el estado del nodo
     *  @param idGenObj Objeto para generar identificadores de los grupos locales.
     *  @param q cola que almacena los objetos DownDataLink en espera de un identificador de cliente.
     *  @param list almacen de temporal de objetos DownDataLink pendientes de permisos.
     *  @throws IOException algo fue mal
     */ 
    DownDataLink(Socket s, isabel.seco.server.State state,
		 IdGenerator idGenObj, Queue q, HashMap list) throws IOException {
	this(s, state, idGenObj, q, list, null, false);
    }


    /**
     *  Constructor de objetos DownDataLink
     *  @param s socket de acceso a cliente o nodo SeCo al que atendemos.
     *  @param q cola que almacena los objetos DownDataLink en espera de un identificador de cliente.
     *  @param state objeto que almacena el estado del nodo
     *  @param idGenObj Objeto para generar identificadores unicos.
     *  @param list almacen de temporal de objetos DownDataLink pendientes de permisos.
     *  @param per objeto de calculo de permisos
     *  @param isRootNode true para el nodo SeCo raiz.
     *  @throws IOException
     */
    private DownDataLink(Socket s, isabel.seco.server.State state, IdGenerator idGenObj, Queue q, HashMap list, Permissions per, boolean isRootNode) throws IOException {
	
        mylogger=Logger.getLogger("isabel.seco.server.DownDataLink");

        mylogger.finer("Creating new DownDataLink object.");

        myClientId  = 0;
        permissions = Permissions.IN_PROCESS;
        client      = false;
	
        myPP   = new PacketProcessor(s);

        clientIdQueue  = q;
        this.state     = state;
        this.idGenObj  = idGenObj;
        permissionList = list;
        permObj        = per;

        this.isRootNode = isRootNode;
    }


    /**
     *  Constructor de un objeto DownDataLink para el nodo SeCo raiz.
     *  @param s socket de acceso a cliente o nodo SeCo al que atendemos.
     *  @param state objeto que almacena el estado del nodo
     *  @param idGenObj Objeto para generar identificadores unicos.
     *  @param per objeto de calculo de permisos
     *  @throws IOException algo fue mal
     */
    DownDataLink(RUDPSocket s, isabel.seco.server.State state, IdGenerator idGenObj, Permissions per) throws IOException {
        this(s,state,idGenObj,null,null,per,true);
    }


    /**
     *  Constructor de un objeto DownDataLink para un nodo SeCo que no es raiz,
     *  @param s socket de acceso a cliente o nodo SeCo al que atendemos.
     *  @param state objeto que almacena el estado del nodo
     *  @param idGenObj Objeto para generar identificadores de los grupos locales.
     *  @param q cola que almacena los objetos DownDataLink en espera de un identificador de cliente.
     *  @param list almacen de temporal de objetos DownDataLink pendientes de permisos.
     *  @throws IOException algo fue mal
     */
    DownDataLink(RUDPSocket s, isabel.seco.server.State state,
		 IdGenerator idGenObj, Queue q, HashMap list) throws IOException {
	this(s, state, idGenObj, q, list, null, false);
    }


    /**
     *  Constructor de objetos DownDataLink
     *  @param s socket de acceso a cliente o nodo SeCo al que atendemos.
     *  @param q cola que almacena los objetos DownDataLink en espera de un identificador de cliente.
     *  @param state objeto que almacena el estado del nodo
     *  @param idGenObj Objeto para generar identificadores unicos.
     *  @param list almacen de temporal de objetos DownDataLink pendientes de permisos.
     *  @param per objeto de calculo de permisos
     *  @param isRootNode true para el nodo SeCo raiz.
     *  @throws IOException
     */
    private DownDataLink(RUDPSocket s, isabel.seco.server.State state, IdGenerator idGenObj, Queue q, HashMap list, Permissions per, boolean isRootNode) throws IOException {

        mylogger=Logger.getLogger("isabel.seco.server.DownDataLink");

        mylogger.finer("Creating new DownDataLink object.");

        myClientId  = 0;
        permissions = Permissions.IN_PROCESS;
        client      = false;

        myPP   = new PacketProcessor(s);

        clientIdQueue  = q;
        this.state     = state;
        this.idGenObj  = idGenObj;
        permissionList = list;
        permObj        = per;

        this.isRootNode = isRootNode;
    }


    /**
     * Atiende a la fase de login tanto de cliente como de SeCoProxy.
     */
    private boolean Login() {
	
	try {
	    SeCoPacket rpkt=myPP.receive();
	    
	    if (rpkt.getCommand()==SeCoPacket.HELLO_CLIENT_COMMAND) {
		// atendemos a un cliente
		helloClient(rpkt);
		if (!authenticateClient()) return false;
		activateClient();
		return true;
	    } else if (rpkt.getCommand() == SeCoPacket.HELLO_SECO_COMMAND) {
		// atendemos a otro nodo SeCo
		return synchronizeNewSeCo();
	    } else {
		mylogger.severe("Protocol error:: Login rejected: First packet must be HELLO_CLIENT_COMMAND or HELLO_SECO_COMMAND");
		return false;
	    }
	} catch (IOException e) {
	    mylogger.severe("IOException = "+e.getMessage());
	    return false;
	}
    }
    
    
    /**
     *  Atiende a la fase de login de un cliente.
     */
    private void helloClient(SeCoPacket rpkt) throws IOException {

	client = true;

	mylogger.info("Received client hello request.");

	if (isRootNode) {
	    
	    myClientId = idGenObj.getNewClientId();
	    
	    mylogger.info("Hello client accepted: ClientId="+myClientId);
	    SeCoPacket spkt=new SeCoPacket();
	    spkt.setCommand(SeCoPacket.CLIENT_ID_COMMAND);
	    spkt.setToID(myClientId);
	    mylogger.info("Sending that hello was accepted. ClientId="+myClientId);
	    myPP.send(spkt);
	    
	} else { 

	    clientIdQueue.put(this);
	    PacketProcessor pp=state.getClientPP(0);
	    pp.send(rpkt);
	    mylogger.info("Hello client request forwarded to the root SeCo node");
	    while (true) { //bloqueado hasta recibir myClientId
		if (myClientId!=0) {
		    mylogger.info("Hello client request accepted: Assigned ClientId is "+myClientId);
		    state.addClientAsLocal(myClientId);
		    state.addClientPP(myClientId,myPP);
		    return;
		} else { 
		    Thread.yield();
		}
	    }
	}
    }


    /**
     * Metodo que atiende a la fase de autenticacion de un cliente.
     * Se realiza la autenticacion y asignacion de permisos en base a 
     * la firma digital de informacion recibida.
     * @return true si se autentica con exito, false en caso contrario.
     */
    private boolean authenticateClient(){
	try {
	    SeCoPacket rpkt = myPP.receive();
	    int fromID=rpkt.getFromID();
	    byte[] data=rpkt.getData();

	    if (fromID != myClientId) {
		mylogger.severe("Protocol error: Authenticate message with bad clientID ("+fromID+")." );
		return false;
	    }
	    
	    if (rpkt.getCommand() == SeCoPacket.LOGIN_COMMAND) {
        	if (isRootNode) {
		    
		    permissions = permObj.getPermissions(data);
		    
		    SeCoPacket spkt=new SeCoPacket();
		    spkt.setCommand(SeCoPacket.PERMISSIONS_COMMAND);
		    spkt.setToID(fromID);
		    spkt.setData(new byte[] {permissions});
		    myPP.send(spkt);
		    
		    mylogger.info("Client "+fromID+" authenticated: permissions = "+permissions);
		    
		    if (permissions==Permissions.NONE)
			return false;
		    
		    return true;	
		    
          	} else {
		    PacketProcessor pp = state.getClientPP(0);
		    permissionList.put(new Integer(fromID),this);
		    pp.send(rpkt);
		    while (true) {
              		if (permissions != Permissions.IN_PROCESS) {
			    mylogger.info("Client "+myClientId+" authenticated: permmissions=" + permissions);
			    if (permissions == Permissions.NONE)
				return false;
			    else
				return true;
                	} else 
			    Thread.yield();
		    }
		}
	    } else {
		mylogger.severe("Protocol error: Expected LOGIN_COMMAND ("+SeCoPacketTypes.LOGIN_COMMAND+"), but received "+rpkt.getCommand() );
		return false;
	    }	    
	} catch(IOException e){
	    mylogger.severe("IOException = "+e.getMessage());
	}
	return false;
    }


    /**
     *  Metodo para registrar el nuevo cliente en el Estado (State).
     */
    private void activateClient() {

	state.addClientAsLocal(myClientId);
	
	mylogger.finest("Registering PacketProcessor of ClientId="+myClientId);
	state.addClientPP(myClientId,myPP);
    }


    /**
     *  Metodo para synchronizar el nuevo nodo SeCo que se nos ha conectado.
     *  Se actializan algunos atributos y se envia toda la historia.
     *  Notar que hasta que no se termina de enviar toda la historia no se
     *  atiende a ningun otro comando ya que no ejecutamos el metodo run aun.
     *  @return true si el protocolo va bien, false en caso contrario.
     */
    private boolean synchronizeNewSeCo() {

	mylogger.finest("Sending history");
	
	myClients = new ArrayList();
	
	try {
	    synchronized (state) { 
		state.addLocalSeCoPP(myPP);
		
		// se actualiza la historia
		
		//Obtenemos todos los mensajes de historia 
		Vector allPkts = state.getAllMsgs();
		//Enviamos la historia completa
		Iterator messages = allPkts.iterator();
		while (messages.hasNext()) {
		    SeCoPacket packet = (SeCoPacket) messages.next();
		    myPP.send(packet);
		}
	    }
	    
	    return true;	
	} catch(IOException e){
	    mylogger.severe("IOException = "+e.getMessage());
	}
	return false;
    }	
	


    /**
     *  Metodo que procesa los paquetes tipo HELLO_CLIENT_COMMAND procedentes de
     *  clientes. 
     *
     *  Si este DownDataLink pertenece a un nodo SeCo intermedio, se reenvia el
     *  paquete hacia el nodo SeCo superior y se introduce en la cola de pendientes. 
     *  Si pertenece al nodo SeCo raiz, asigna un identificador unico, lo encapsula en un 
     *  paquete tipo CLIENT_ID_COMMAND y lo envia de vuelta.
     *  @param pkt paquete HELLO_CLIENT_COMMAND que solicita identificador unico.
     *  @throws <tt>IOException</tt> Problemas de comunicacion con el cliente.
     */
    private void processHello(SeCoPacket pkt) throws IOException {

	mylogger.finest("Processing remote Hello request.");
	
    	if (isRootNode) {    		
	    int toID = idGenObj.getNewClientId();
	    SeCoPacket spkt=new SeCoPacket();
	    spkt.setCommand(SeCoPacket.CLIENT_ID_COMMAND);
	    spkt.setToID(toID);
	    myPP.send(spkt);
	    state.addClientPP(toID,myPP);
	    myClients.add(new Integer(toID));		       	
      	} else {
	    clientIdQueue.put(this);
            PacketProcessor pp=state.getClientPP(0);
            pp.send(pkt);
        }
    }
    
    /**
     *  Metodo que procesa los paquetes tipo LOGIN_COMMAND de cliente 
     *  asignando los permisos correspondientes. 
     *
     *  Si este DownDataLink pertenece a un nodo SeCo intermedio, simplemente 
     *  se reenvia el paquete al nodo SeCo superior. 
     *  Si pertenece al nodo SeCo raiz, se analiza el texto recibido en el paquete
     *  y en funcion de esto se le asignan al cliente unos permisos u otros
     *  encapsulado en un paquete tipo PERMISSIONS_COMMAND.
     *  @param pkt SeCoPacket tipo LOGIN_COMMAND a procesar.
     *  @throws <tt>IOException</tt> Problemas de comunicacion con el cliente.
     */
    private void processLogin(SeCoPacket pkt) throws IOException{

	mylogger.finest("Processing remote Login request.");

     	byte[] data=pkt.getData();
      	int fromID=pkt.getFromID();
	
     	if (isRootNode) {
	    byte permData = permObj.getPermissions(data);
	    
	    SeCoPacket spkt=new SeCoPacket();
	    spkt.setCommand(SeCoPacket.PERMISSIONS_COMMAND);
	    spkt.setToID(fromID);
	    spkt.setData(new byte[] {permData});
	    myPP.send(spkt);
    	} else {
	    PacketProcessor pp = state.getClientPP(0);
	    pp.send(pkt);
      	}
    }
    
    /**
     *  Coge un paquete con un mensaje que se acaba de recibir y lo reencamina al
     *  cliente destino.
     *  @param pkt El paquete a reencaminar.
     */
    private void routeToClient(SeCoPacket pkt) throws IOException {

    	int to = pkt.getToID();

	mylogger.finest("Sending a packet to the client "+to);

	PacketProcessor pp = state.getClientPP(to);
	if (isRootNode) {
	    if (pp!=null) {
		pp.send(pkt);
	    } else {
		mylogger.warning("Destination client \""+to+"\" does not exist.");
	    }
	} else {
	    if (pp==null) { 
		// ese cliente no esta unido directamente a nuestro nodo, ni a ninguno
		// de los nodos SeCo que tenemos por debajo. Lo enviamos hacia arriba.
		pp=state.getClientPP(0);
		pp.send(pkt);
	    } else { //si es nuestro o de un SeCoProxy hijo
		pp.send(pkt);
	    }
	}
    }
    
    /**
     *  Coge un paquete con un mensaje que se acaba de recibir y lo reencamina a
     *  todos los clientes que escuchan en un grupo.
     *  @param pkt El paquete a reencaminar.
     *  @param sync indica si el mensaje es de sincronismo.
     *  @param keep Indica si se debe guardar historia del mensaje o si no.
     */
    private void routeToGroup(SeCoPacket pkt, boolean sync, boolean keep) {
	
	int toID    = pkt.getToID();
	int fromID  = pkt.getFromID();

	mylogger.finest("Sending a packet to group "+toID);
	
	synchronized(state) {
	    if (sync) {
		mylogger.finest("Cleanup history messages of the "+toID+" group");
		state.cleanMsgs(fromID,toID);
            }
	    if (keep) {
		mylogger.finest("Saving message in the history of the "+toID+" group");
		state.addMsg(fromID,toID,pkt);
	    }
	    mylogger.finest("Sending message to group "+toID);
	    sendGroup(toID,pkt);
	}
    }

    
    /**
     * Coge un paquete con un mensaje que se acaba de recibir y lo reencamina a
     * todos los clientes que escuchan en un grupo local.
     * 
     * @param pkt
     *            El paquete a reencaminar.
     * @param sync
     *            indica si el mensaje es de sincronismo.
     * @param keep
     *            Indica si se debe guardar historia del mensaje o si no.
     */
    private void routeToLocalGroup(SeCoPacket pkt, boolean sync, boolean keep) {
	
	int toID = pkt.getToID();
	int fromID = pkt.getFromID();
	
	mylogger.finest("Sending a packet to local group " + toID);
	
	synchronized (state) {
	    if (sync) {
		mylogger.finest("Cleanup history messages of the " + toID + " group");
		state.cleanMsgs(fromID, toID);
	    }
	    if (keep) {
		mylogger.finest("Saving message in the history of the " + toID + " group");
		state.addMsg(fromID, toID, pkt);
	    }
	    mylogger.finest("Sending message to group " + toID);
	    sendLocalGroup(toID, pkt);
	}
    }
    

    
    /**
     *  Propaga un mensaje de desconexion de cliente, y limpia todas las estructuras de ese cliente.
     * @param pkt El paquete a reencaminar.
     */
    private void sendClientDisconnection(SeCoPacket pkt) {
	
	int fromID  = pkt.getFromID();
	
	mylogger.info("Sending disconnection message: client="+fromID);

	synchronized(state) {
	    state.cleanupClient(fromID);

	    sendGroup(state.DISCONNECTIONS_GID,pkt);
	}
    }


    /**
     * El algoritmo empleado para enviar mensajes a grupos sera la inundacion.
     * 
     * @param group
     *            identificador de grupo destino de los mensajes
     * @param pkt
     *            mensaje a enviar
     * 
     */
    private void sendGroup(int group, SeCoPacket pkt) {
	
	/*
	 * 1. Enviamos el mensaje de grupo a todos los clientes locales nuestros
	 * que esten apuntados a ese grupo. Notar que si un cliente es el que
	 * envio el mensaje, entonces se le reenvia tambien a el.
	 */
	
	// Calculo los clientes locales mios, o locales a nodos seco por debajo
	// de mi que
	// esten unido al grupo dado.
	// ¿¿¿LOCALES A NODOS SECO??? Creo que esto lo hace el paso 2...
	Iterator i = state.getGroupClients(group);
	if (i != null) { // ningun cliente en el grupo
	    while (i.hasNext()) {
		int clientId = ((Integer) i.next()).intValue();
		if (state.isClientLocal(clientId)) { // hace falta para SeCo
		    // -> Groups global
		    PacketProcessor pp = state.getClientPP(clientId);
		    try {
			if (pp != null) {
			    mylogger.finest("Sending message to local client");
			    pp.send(pkt);
			}
		    } catch (IOException e) {
			mylogger.severe(e.getMessage());
		    }
		}
	    }
	}
	
	/*
	 * 2. Enviamos el mensaje a todos los nodos SeCo que se conectaron a
	 * nuestro nodo, excepto por la que se recibio (si es el caso de que el
	 * mensaje procedia de alguno de estos nodos). Nota: solo enviamos una
	 * copia a cada nodo SeCo conectado con nosotros, aunque por esa ruta se
	 * llegue a varios clientes.
	 */
	Iterator<PacketProcessor> it = state.getAllLocalSeCoPPs();
	while (it.hasNext()) {
	    PacketProcessor pp = it.next();
	    try {
		if (pp != myPP) {
		    mylogger.finest("Sending message to a local SeCo node.");
		    pp.send(pkt);
		}
	    } catch (IOException e) {
		mylogger.severe(e.getMessage());
	    }
	    
	}
	
	/*
	 * 3. Enviamos el mensaje hacia el nodo SeCo al que nos unimos (si ese
	 * fue el caso) Es la ruta 0.
	 */
	try {
	    PacketProcessor pp = state.getClientPP(0);
	    if (pp != null) {
		mylogger.finest("Sending message by default route (up)");
		pp.send(pkt);
	    }
	} catch (IOException e) {
	    mylogger.severe(e.getMessage());
	}
    }
    
    /**
     * Mandar mensaje a un grupo local.
     * 
     * @param group
     *            identificador de grupo destino de los mensajes
     * @param pkt
     *            mensaje a enviar
     * 
     */
    private void sendLocalGroup(int group, SeCoPacket pkt) {
	
	/*
	 * 1. Enviamos el mensaje de grupo a todos los clientes locales nuestros
	 * que esten apuntados a ese grupo. Notar que si un cliente es el que
	 * envio el mensaje, entonces se le reenvia tambien a el.
	 */
	Iterator i = state.getGroupClients(group);
	if (i != null) { // ningun cliente en el grupo
	    while (i.hasNext()) {
		int clientId = ((Integer) i.next()).intValue();
		if (state.isClientLocal(clientId)) { // hace falta para SeCo
		    // ->
		    // Groups global
		    PacketProcessor pp = state.getClientPP(clientId);
		    try {
			if (pp != null) {
			    mylogger.finest("Sending message to local client");
			    pp.send(pkt);
			}
		    } catch (IOException e) {
			mylogger.severe(e.getMessage());
		    }
		}
	    }
	}
    }


    /**
     *  propaga un mensage de actualizacion de tama#o de historia.
     */
    private void sendMaxHistoryLengthUpdate(SeCoPacket pkt) {
	
	int fromID  = pkt.getFromID();  // id del cliente
	int toID    = pkt.getToID();    // id del grupo
	
	DataInputStream dis = new DataInputStream(new ByteArrayInputStream(pkt.getData()));
	try {
	    int size = dis.readInt();

	    state.setMaxLength(fromID,toID,size);
	    
	    synchronized (state) {
		mylogger.fine("Propagating max history length request: Client="+fromID+" group="+toID+" length="+size);
		propagateMaxHistoryLengthUpdate(pkt);
	    }
	} catch (IOException ioe) {
	    mylogger.severe(ioe.getMessage());
	}
    }

    /**
     *  Propaga una peticion de actualizacion de longitud de la historia por toda la red.
     * @param pkt mensaje a enviar
     */
     private void propagateMaxHistoryLengthUpdate(SeCoPacket pkt) {

	/*  Enviamos el mensaje a todos los nodos SeCo que se conectaron 
	 *  a nuestro nodo, excepto por la que se recibio
	 *  (si es el caso de que el mensaje procedia de alguno de estos nodos). 
	 *  Nota: solo enviamos una copia a cada nodo SeCo conectado
	 *          con nosotros.
	 */
	Iterator<PacketProcessor> it = state.getAllLocalSeCoPPs();
	while (it.hasNext()){
	    PacketProcessor pp = it.next();
	    try {
		if (pp != myPP) {
		    mylogger.finest("Sending MaxHistoryLength message to a local SeCo node.");
		    pp.send(pkt);
		}
	    } catch(IOException e) {
		mylogger.severe(e.getMessage());
	    }
	}
	
	/*  Enviamos el mensaje hacia el nodo SeCo al que nos unimos (si ese fue el caso)
	 *  Es la ruta 0.
	 */
	try {
	    PacketProcessor pp = state.getClientPP(0);
	    if (pp != null) {
		mylogger.finest("Sending message MaxHistoryLength to the upper SeCo node.");
		pp.send(pkt);
	    }
	} catch(IOException e) {
	    mylogger.severe(e.getMessage());
	}
    }

    
    /**
     * propaga un mensage de actualizacion de tama#o de historia.
     */
    private void sendLocalMaxHistoryLengthUpdate(SeCoPacket pkt) {
	
	int fromID = pkt.getFromID(); // id del cliente
	int toID = pkt.getToID(); // id del grupo
	
	DataInputStream dis = new DataInputStream(new ByteArrayInputStream(pkt.getData()));
	try {
	    int size = dis.readInt();
	    
	    state.setMaxLength(fromID, toID, size);
	    
	    synchronized (state) {
		mylogger.fine("Propagating max history length request: Client="
			      + fromID + " group=" + toID + " length=" + size);
		propagateLocalMaxHistoryLengthUpdate(pkt);
	    }
	} catch (IOException ioe) {
	    mylogger.severe(ioe.getMessage());
	}
    }
    
    /**
     * Propaga una peticion de actualizacion de longitud de la historia por toda
     * la red.
     * 
     * @param pkt
     *            mensaje a enviar
     */
    private void propagateLocalMaxHistoryLengthUpdate(SeCoPacket pkt) {
	
	/*
	 * Enviamos el mensaje a todos los nodos SeCo que se conectaron a
	 * nuestro nodo, excepto por la que se recibio (si es el caso de que el
	 * mensaje procedia de alguno de estos nodos). Nota: solo enviamos una
	 * copia a cada nodo SeCo conectado con nosotros.
	 */
	Iterator<PacketProcessor> it = state.getAllLocalSeCoPPs();
	while (it.hasNext()) {
	    PacketProcessor pp = it.next();
	    try {
		if (pp != myPP) {
		    mylogger.finest("Sending MaxHistoryLength message to a local SeCo node.");
		    pp.send(pkt);
		}
	    } catch (IOException e) {
		mylogger.severe(e.getMessage());
	    }
	}
    }
    

    /**
     *  Procesa un paquete con una solicitud de union a un grupo.
     *  Se contesta al cliente con un mensaje indicandole cual es el
     *  identificador del grupo al que se ha unido.
     *  Tambien se le envian todos los mensajes enviados a ese grupo
     *  desde el ultimo sincronismo de cada cliente origen.
     *  @param pkt Paquete con la solicitud de union al grupo.
     *  @throws <tt>IOException</tt> Problemas de comunicacion con el cliente.
     */
    private void joinGroup(SeCoPacket pkt) throws IOException {
	
	String groupName = new String(pkt.getData());

	mylogger.fine("Processing request to join to group "+groupName);
	
	if (isRootNode) {
	    
	    synchronized(state) {
		// identificador del grupo
		int id = state.getGroupId(groupName);
		if (id == 0) {
		    id = idGenObj.getNewGroupId();
		    state.declareGroup(groupName,id);
		}

		SeCoPacket pkt2 = new SeCoPacket();
		pkt2.setCommand(SeCoPacket.JOIN_GROUP_COMMAND);
		pkt2.setData(pkt.getData());

		if (myClientId!=0) {
		    // estamos atendiendo un cliente
		    state.addGroupClient(myClientId,id);
		    pkt2.setToID(id);
		    myPP.send(pkt2);

		    // Enviar la historia del grupo		    
		    sendClientHistory(myClientId,id);
		} else {
		    // estamos atendiendo a un nodo SeCo
		    int clientId=pkt.getFromID();
		    pkt2.setFromID(id);
		    pkt2.setToID(clientId);
		    myPP.send(pkt2);
		}
	    }

	} else {

	    // identificador del grupo
	    int id = state.getGroupId(groupName);
	    if (id == 0) {
		// No conozco el grupo, reenvio hacia arriba la peticion
		PacketProcessor pp=state.getClientPP(0);
		pp.send(pkt);
	    } else {
		// es un grupo conocido
		SeCoPacket pkt2 = new SeCoPacket();
		pkt2.setCommand(SeCoPacket.JOIN_GROUP_COMMAND);
		pkt2.setData(pkt.getData());

		if (myClientId!=0) {
		    // estamos atendiendo un cliente
		    state.addGroupClient(myClientId,id);
		    pkt2.setToID(id);
		    myPP.send(pkt2);

		    // Enviar la historia del grupo		    
		    sendClientHistory(myClientId,id);
		} else {
		    // estamos atendiendo a un nodo SeCo
		    int clientId=pkt.getFromID();
		    pkt2.setFromID(id);
		    pkt2.setToID(clientId);
		    myPP.send(pkt2);
		}
	    }
	}
    }


    /**
     * Procesa un paquete con una solicitud de union a un grupo. Se contesta al
     * cliente con un mensaje indicandole cual es el identificador del grupo
     * local al que se ha unido. Tambien se le envian todos los mensajes
     * enviados a ese grupo local desde el ultimo sincronismo de cada cliente
     * origen.
     * 
     * @param pkt
     *            Paquete con la solicitud de union al grupo local.
     * @throws <tt>IOException</tt> Problemas de comunicacion con el cliente.
     */
    private void joinLocalGroup(SeCoPacket pkt) throws IOException {
	
	String groupName = new String(pkt.getData());
	
	mylogger.fine("Processing request to join local group " + groupName);
	
	// En este caso da igual ser Root o no pues solo es en local.
	
	synchronized (state) {
	    // identificador del grupo
	    int id = state.getLocalGroupId(groupName);
	    if (id == 0) {
		id = idGenObj.getNewLocalGroupId();
		state.declareLocalGroup(groupName, id);
	    }
	    
	    SeCoPacket pkt2 = new SeCoPacket();
	    pkt2.setCommand(SeCoPacket.JOIN_LOCAL_GROUP_COMMAND);
	    pkt2.setData(pkt.getData());
	    
	    if (myClientId != 0) {
		// estamos atendiendo un cliente
		state.addGroupClient(myClientId, id);
		pkt2.setToID(id);
		myPP.send(pkt2);
		
		// Enviar la historia del grupo
		sendClientHistory(myClientId, id);
	    } else {
		// estamos atendiendo a un nodo SeCo
		int clientId = pkt.getFromID();
		pkt2.setFromID(id);
		pkt2.setToID(clientId);
		myPP.send(pkt2);
	    }
	}
    }
    
    
    /**
     *  Se emplea para enviar toda la historia de un grupo a un cliente.
     *  @param to identificador de cliente destino
     *  @param groupId identificador del grupo cuya historia hay que enviar al cliente
     */
     private void sendClientHistory(int to, int groupId) {

	 mylogger.finest("Sending history messages of the "+groupId+" group");

	 PacketProcessor pp = state.getClientPP(to);
	 if (pp != null) {
	     Iterator i = state.getMsgs(groupId);
	     while (i.hasNext()) {
		 SeCoPacket pkt=(SeCoPacket) i.next();
		 try{
		     mylogger.finest("Sending history message to client "+to);
		     pp.send(pkt);
		 } catch(IOException e){
		     mylogger.severe(e.getMessage());
		 }
	     }
	 }
     }
    

    /**
     *  Procesa un paquete con una solicitud de consulta del
     *  identificador de un grupo.
     *  Se contesta con un mensaje indicando el identificador.
     *  @param pkt Paquete con la solicitud de consulta.
     *  @throws <tt>IOException</tt> Problemas de comunicacion con el cliente.
     */
    private void getGroupId(SeCoPacket pkt) throws IOException {
	
	String groupName = new String(pkt.getData());

	mylogger.fine("Processing request to get ID of the group "+groupName);
	
	if (isRootNode) {	
	    
	    synchronized(state){
		// identificador del grupo
		int id = state.getGroupId(groupName);
		if (id == 0) {
		    id = idGenObj.getNewGroupId();
		    state.declareGroup(groupName,id);
		}

		SeCoPacket pkt2=new SeCoPacket();
		pkt2.setCommand(SeCoPacket.GET_GROUP_ID_COMMAND);
		pkt2.setData(pkt.getData());
		
		if (myClientId != 0) {
		    // estamos atendiendo un cliente
		    pkt2.setToID(id);
		    myPP.send(pkt2);
		} else {
		    // estamos atendiendo a un SeCoProxy
		    int clientId=pkt.getFromID();
		    pkt2.setFromID(id);
		    pkt2.setToID(clientId);
		    myPP.send(pkt2);
		}
	    }		

	} else {

	    // identificador del grupo
	    int id = state.getGroupId(groupName);
	    if (id == 0) {
		// No conozco el grupo, reenvio hacia arriba la peticion
		PacketProcessor pp=state.getClientPP(0);
		pp.send(pkt);
	    } else {
		// es un grupo conocido
		SeCoPacket pkt2=new SeCoPacket();
		pkt2.setCommand(SeCoPacket.GET_GROUP_ID_COMMAND);
		pkt2.setData(pkt.getData());
		
		if (myClientId != 0) {
		    // estamos atendiendo un cliente
		    pkt2.setToID(id);
		} else {
		    // estamos atendiendo a un SeCoProxy
		    int clientId=pkt.getFromID();
		    pkt2.setFromID(id);
		    pkt2.setToID(clientId);
		}
		myPP.send(pkt2);
	    }
	}
    }
    
    
    /**
     * Procesa un paquete con una solicitud de consulta del identificador de un
     * grupo local. Se contesta con un mensaje indicando el identificador.
     * 
     * @param pkt
     *            Paquete con la solicitud de consulta.
     * @throws <tt>IOException</tt> Problemas de comunicacion con el cliente.
     */
    private void getLocalGroupId(SeCoPacket pkt) throws IOException {
	
	String groupName = new String(pkt.getData());
	
	mylogger.fine("Processing request to get ID of the local group "
		      + groupName);
	
	// En este caso da igual ser Root o no pues solo es en local.
	
	synchronized (state) {
	    // identificador del grupo
	    int id = state.getLocalGroupId(groupName);
	    if (id == 0) {
		id = idGenObj.getNewLocalGroupId();
		state.declareLocalGroup(groupName, id);
	    }
	    
	    SeCoPacket pkt2 = new SeCoPacket();
	    pkt2.setCommand(SeCoPacket.GET_LOCAL_GROUP_ID_COMMAND);
	    pkt2.setData(pkt.getData());
	    
	    if (myClientId != 0) {
		// estamos atendiendo un cliente
		pkt2.setToID(id);
		myPP.send(pkt2);
	    } else {
		// estamos atendiendo a un SeCoProxy
		int clientId = pkt.getFromID();
		pkt2.setFromID(id);
		pkt2.setToID(clientId);
		myPP.send(pkt2);
	    }
	}
    }
    

    /**
     * Procesa un paquete con una solicitud de abandono de un grupo.
     * @param pkt Paquete con la solicitud de abandono.
     */
    private void leaveGroup(SeCoPacket pkt) throws IOException {
	
	// si estamos atendiendo un cliente local
	if (myClientId != 0) {
	    String groupName = new String(pkt.getData());
	    int groupId = state.getGroupId(groupName);
	    mylogger.fine("Client "+myClientId+" leaves group "+groupName+"("+groupId+").");
	    state.delGroupClient(myClientId,groupId);
	} else {
	    mylogger.severe("Protocol error:: Unexpected LEAVE_GROUP_COMMAND received.");
	}
    }
    
    
    /**
     * Procesa un paquete con una solicitud de abandono de un grupo local.
     * 
     * @param pkt
     *            Paquete con la solicitud de abandono.
     */
    private void leaveLocalGroup(SeCoPacket pkt) throws IOException {
	
	// si estamos atendiendo un cliente local
	if (myClientId != 0) {
	    String groupName = new String(pkt.getData());
	    int groupId = state.getLocalGroupId(groupName);
	    mylogger.fine("Client " + myClientId + " leaves group " + groupName
			  + "(" + groupId + ").");
	    state.delGroupClient(myClientId, groupId);
	} else {
	    mylogger
		.severe("Protocol error:: Unexpected LEAVE_GROUP_COMMAND received.");
	}
    }


    /**
     *  Bucle principal del thread que atiende los mensajes enviados por nuestros clientes, y por los nodos
     *  SeCo que se conectaron con nosotros.
     */
    public void run() {
	
	boolean parar=false;
	
	if (!Login()) parar=true;

	try {
	    while (!parar) {
		
		mylogger.finest("Waiting for a new message (ClientId="+myClientId+")");
		SeCoPacket pkt = myPP.receive();
		mylogger.finest("Received message from ClientId="+myClientId+" :"+pkt);
		
		switch (pkt.getCommand()) 
		    {
		    case SeCoPacket.HELLO_CLIENT_COMMAND:
			// Comando para pedir identificador de cliente
			processHello(pkt);
			break;
			
		    case SeCoPacket.BYE_COMMAND:
                	// Comando para finalizar una sesion
			parar=true;
			myPP.invalid();
			break;
			
		    case SeCoPacket.JOIN_GROUP_COMMAND:
                        // Comando para recibir los mensajes enviados a un grupo
			joinGroup(pkt);
			break;
			
		    case SeCoPacket.JOIN_LOCAL_GROUP_COMMAND:
			// Comando para recibir los mensajes enviados a un grupo
			// local
			joinLocalGroup(pkt);
			break;
			
		    case SeCoPacket.GET_GROUP_ID_COMMAND:
                        // Comando para preguntar por el nombre de un grupo
			getGroupId(pkt);
			break;
			
		    case SeCoPacket.GET_LOCAL_GROUP_ID_COMMAND:
			// Comando para preguntar por el nombre de un grupo local
			getLocalGroupId(pkt);
			break;
			
		    case SeCoPacket.LEAVE_GROUP_COMMAND:
                        // Comando para dejar de recibir los mensajes enviados a un grupo
			leaveGroup(pkt);
			break;
			
		    case SeCoPacket.LEAVE_LOCAL_GROUP_COMMAND:
                // Comando para dejar de recibir los mensajes enviados a un grupo
		    leaveLocalGroup(pkt);
		    break;
			
		    case SeCoPacket.CLIENT_MSG_COMMAND:
                        // Comando para enviar un mensaje a un cliente
			if(myClientId==0 || (myClientId!=0 && permissions==Permissions.SEND_AND_RECEIVE))
                            routeToClient(pkt);
			break;
			
		    case SeCoPacket.MSG_COMMAND:
                        // Comando para enviar mensajes
			if (myClientId==0 || (myClientId!=0 && permissions==Permissions.SEND_AND_RECEIVE))
                            routeToGroup(pkt,false, true);
			break;
			
		    case SeCoPacket.LOCAL_MSG_COMMAND:
			// Comando para enviar mensajes
			if (myClientId == 0 || (myClientId != 0 && permissions == Permissions.SEND_AND_RECEIVE))
			    routeToLocalGroup(pkt, false, true);
			break;
			
		    case SeCoPacket.SYNC_MSG_COMMAND:
                        // Comando para enviar mensajes de sincronismo
			if(myClientId==0 || (myClientId!=0 && permissions==Permissions.SEND_AND_RECEIVE))
                            routeToGroup(pkt,true, true);
			break;
			
		    case SeCoPacket.LOCAL_SYNC_MSG_COMMAND:
			// Comando para enviar mensajes de sincronismo
			if (myClientId == 0
			    || (myClientId != 0 && permissions == Permissions.SEND_AND_RECEIVE))
			    routeToLocalGroup(pkt, true, true);
			break;
			
		    case SeCoPacket.NO_HISTORY_MSG_COMMAND:
                        // Comando para enviar mensajes sin historia 
			if(myClientId==0 || (myClientId!=0 && permissions==Permissions.SEND_AND_RECEIVE))
                            routeToGroup(pkt,false, false);
			break;
			
		    case SeCoPacket.LOCAL_NO_HISTORY_MSG_COMMAND:
			// Comando para enviar mensajes sin historia
			if (myClientId == 0
			    || (myClientId != 0 && permissions == Permissions.SEND_AND_RECEIVE))
			    routeToLocalGroup(pkt, false, false);
			break;
			
		    case SeCoPacket.CLIENT_DISCONNECTED_COMMAND:
                        // Comando que informa de la desconexion de un cliente 
			sendClientDisconnection(pkt);
			break;
			
		    case SeCoPacket.SET_MAX_HISTORY_LENGTH_COMMAND:
                        // Comando para dar tama#o a la historia 
			sendMaxHistoryLengthUpdate(pkt);
			break;
			
		    case SeCoPacket.SET_LOCAL_MAX_HISTORY_LENGTH_COMMAND:
			// Comando para dar tama#o a la historia
			sendLocalMaxHistoryLengthUpdate(pkt);
			break;
			
		    case SeCoPacket.LOGIN_COMMAND:
                        // Comando para autenticacion 
			processLogin(pkt);
			break;
		    }
		
	    }
	} catch (IOException e) {
	    mylogger.severe("Connection failed: Terminated DownDataLink of ClientId="+myClientId+": "+e.getMessage());
	} catch(Exception e) {
	    mylogger.severe("DownDataLink (ClientId="+myClientId+"): Unexpected exception = "+e.getMessage());
	    e.printStackTrace(System.err);
	} finally {
	    try {
		myPP.close();
	    } catch(IOException e){
		mylogger.severe("Fail closing DownDataLink socket of ClientId="+myClientId+": "+e.getMessage());
	    }
	}
	
	if (myClientId!=0) {	//si atendemos a un cliente
	    mylogger.info("ClientId= "+myClientId+" leaves SeCo");
	    
	    synchronized (state) {
		mylogger.finest("Deregistering ClientId="+myClientId);
		state.cleanupClient(myClientId);
		
		SeCoPacket pkt=new SeCoPacket();
		pkt.setCommand(SeCoPacket.CLIENT_DISCONNECTED_COMMAND);
		pkt.setToID(state.DISCONNECTIONS_GID);
		pkt.setFromID(myClientId);
		mylogger.fine("Sending a disconnection message to all the clients");
		routeToGroup(pkt,false,false);
	    }

	} else { // Si atendiamos a un nodo SeCo

	    mylogger.info("SeCo node disconnection.");

	    synchronized (state) {
		Iterator it=myClients.iterator();
		while (it.hasNext()){
		    int clientId=((Integer) it.next()).intValue();
		    
		    mylogger.info("Deregistering ClientId="+clientId);
		    state.cleanupClient(clientId);
		    
		    SeCoPacket pkt=new SeCoPacket();
		    pkt.setCommand(SeCoPacket.CLIENT_DISCONNECTED_COMMAND);
		    pkt.setToID(state.DISCONNECTIONS_GID);
		    pkt.setFromID(clientId);
		    mylogger.fine("Sending a disconnection message to all the clients");
		    routeToGroup(pkt,false,false);
		}
	    }
	}
    }
    
    /**
     *  Metodo get para el campo myClientId.
     *  @return identificador del cliente al que atiende este DownDataLink.
     */
    int getMyClientId(){
    	return myClientId;
    }
    
    /**
     * Metodo set para el campo myClientId.
     * @param myClientId valor que asignamos al identificador del cliente
     * al que atiende este DownDataLink.
     */
    void setMyClientId(int myClientId){
    	this.myClientId=myClientId;
    }
    
    /**
     * Metodo get para el PacketProcessor myPP de este DownDataLink.
     * @return PacketProcessor asociado a este thread.
     */
    PacketProcessor getMyPP(){
    	return myPP;
    }
    
    /**
     * Metodo get para el campo myClients.
     * @return clientes alcanzables desde nuestro PacketProcessor.
     */
    ArrayList getMyClients() {
	return myClients;
    }
    
    /**
     * Metodo set para el campo permissions.
     * @param permissions permisos del cliente al que atiende este
     * DownDataLink.
     */
    void setPermissions(byte permissions){
	this.permissions=permissions;
    }
    
    /**
     * @return
     */
    boolean isClient() {
	return client;
    }
}






