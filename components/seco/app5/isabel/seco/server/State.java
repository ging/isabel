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
 * State.java
 *
 */

package isabel.seco.server;

import java.io.*;
import java.util.*;
import java.util.logging.*;
import java.security.*;
import java.security.spec.*;

/**
 * La clase State es la base de datos donde se administra identificadores
 * unicos, nombres de grupos, listas de clientes, listas de mensajes, etc.
 * 
 * Concretatemte, esta clase se encarga de:
 * 
 * - asignacion de identificadores unicos de cliente. Esto solo se hace en el
 * nodo SeCo raiz.
 * 
 * - almacenar los identificadores de los clientes locales de este nodo SeCo.
 * 
 * - registrar los objetos PacketProcessor de los clientes, tanto si estan
 * conectados directamente a nuestro nodo SeCo, como si estan conectados a otro
 * nodo SeCo. Si el cliente no esta conectado directamente con nuestro nodo
 * SeCo, entonces el PacketProcessor es el del acceso a nodo SeCo que nos
 * permite llegar a ese cliente. NOTA: El PacketProcessor del objeto UpDataLink
 * esta asociado al cliente 0.
 * 
 * - almacenar los PacketProcessor de todos los nodos SeCo que se han conectado
 * a este nodo SeCo. NO se incluye el nodo SeCo superior.
 * 
 * - store group names, group codes, and clients of each group.
 * 
 * - almacenar los mensajes enviados a cada grupo para sincronizar los nuevos
 * clientes. Tambien posse metodos para enviar estos mensajes a clientes y
 * grupos.
 * 
 * - gestionar una tabla en la que se almacena el numero maximo de mensajes (a
 * grupos) de deben guardarse por cliente y grupo.
 * 
 * @author Santiago Pavon
 * @author Javier Calahorra
 */
public class State {

	// ---------------- Common part -------------------------------

	/**
	 * Logger where logs are written
	 */
	private Logger mylogger;

	/**
	 * Create a State object.
	 */
	State() {

		mylogger = Logger.getLogger("isabel.seco.server.State");
		mylogger.finer("Creating new State");

		// Este grupo es al que se apuntan los clientes interesados en
		// recibir eventos de desconexion de clientes.
		// Este grupo se registra como -1 en el constructor para que en todos
		// los nodos tenga el mismo identificador.
		declareGroup(DISCONNECTIONS, DISCONNECTIONS_GID);
	}

	/**
	 * Metodo que elimina todo rastro de un cliente dado: grupos,
	 * PacketProcessor, local, etc.
	 * 
	 * @param clientId
	 *            identificador de cliente
	 */
	synchronized void cleanupClient(int clientId) {

		delClientAsLocal(clientId);
		delClientPP(clientId);
		delGroupClient(clientId);
		destroyMsgs(clientId);
	}

	// --------------- Client id ---------------------------

	/**
	 * Almacena los identificadores de los clientes conectados directamente a un
	 * nodo SeCo.
	 */
	private HashSet myLocalClients = new HashSet();

	/**
	 * A#ade un nuevo cliente como local.
	 * 
	 * @param clientId
	 *            identificador de cliente
	 */
	synchronized void addClientAsLocal(int clientId) {
		myLocalClients.add(new Integer(clientId));
	}

	/**
	 * Borra un cliente de la lista de clientes locales.
	 * 
	 * @param clientId
	 *            identificador de cliente
	 */
	synchronized void delClientAsLocal(int clientId) {
		myLocalClients.remove(new Integer(clientId));
	}

	/**
	 * Devuelve true si el identificador de cliente dado es un cliente local del
	 * nodo.
	 * 
	 * @param clientId
	 *            identificador de cliente
	 * @return true si el cliente es local.
	 */
	synchronized boolean isClientLocal(int clientId) {
		return myLocalClients.contains(new Integer(clientId));
	}

	// --------------- Client's Packet Processor -------------

	/**
	 * Tabla donde se almacenan los PacketProcessors de los clientes locales, y
	 * de los clientes conectados a nodos SeCo por debajo de nosotros en el
	 * arbol de nodos SeCo. Los clientes conectados a nodos SeCo por encima en
	 * el arbol se asocian (se a#aden) todos con el codigo de cliente 0.
	 */
	private HashMap cliPPT = new HashMap();

	/**
	 * Registra el PacketProcessor de un cliente.
	 * 
	 * @param clientId
	 *            codigo del cliente.
	 * @param pp
	 *            El PacketProcessor del cliente.
	 */
	synchronized void addClientPP(int clientId, PacketProcessor pp) {
		cliPPT.put(new Integer(clientId), pp);
	}

	/**
	 * Devuelve el PacketProcessor de un cliente.
	 * 
	 * @param clientId
	 *            codigo del cliente.
	 * @return El PacketProcessor del cliente.
	 */
	synchronized PacketProcessor getClientPP(int clientId) {
		return (PacketProcessor) cliPPT.get(new Integer(clientId));
	}

	/**
	 * Elimina el PacketProcessor de un cliente.
	 * 
	 * @param clientId
	 *            codigo del cliente.
	 */
	synchronized void delClientPP(int clientId) {
		cliPPT.remove(new Integer(clientId));
	}

	// --------------- Local SeCo node's Packet Processor -------------

	/**
	 * Almacena los PacketProcessor de todos los nodos SeCo que conectados
	 * directamente a nuestro nodo. Nota: el nodo SeCo superior (si es que
	 * existe) no se almacena aqui, sino como el cliente 0 en la lista de
	 * PacketProcessor de los clientes.
	 */
	private HashSet<PacketProcessor> secoPPSet = new HashSet<PacketProcessor>();

	/**
	 * A#ade el PacketProcessor de acceso a uno de nuestros nodo SeCo vecinos.
	 * 
	 * @param pp
	 *            PacketProcessor del nodo SeCo vecino.
	 */
	synchronized void addLocalSeCoPP(PacketProcessor pp) throws IOException {
		secoPPSet.add(pp);
	}

	/**
	 * Elimina el registro del PacketProcessor de un nodo SeCo.
	 * 
	 * @param pp
	 *            PacketProcessor del nodo SeCo que se desconecta
	 */
	synchronized void delLocalSeCoPP(PacketProcessor pp) {
		secoPPSet.remove(pp);
	}

	/**
	 * Devuelve un Iterator con los PacketProcessor de todos los nodos SeCo
	 * vecinos de nuestro nodo.
	 * 
	 * @return Iterator de acceso a los PacketProcessor de todos los nodos SeCo
	 *         vecinos.
	 */
	synchronized Iterator<PacketProcessor> getAllLocalSeCoPPs() {

		Set set = (HashSet) secoPPSet.clone();
		return set.iterator();
	}

	// ------------ Groups: mapping name-id -----------------------------

	/**
	 * Name of the group used to inform all the clients when a client
	 * disconnection occurs.
	 */
	public static final String DISCONNECTIONS = "Disconnections";

	/**
	 * Id of the group used to inform all the clients when a client
	 * disconnection occurs.
	 */
	public static final int DISCONNECTIONS_GID = -1;

	/**
	 * This hash table store a unique int code for each defined group name.
	 */
	private Hashtable groupCodes = new Hashtable();

	/**
	 * This hash table store a unique int code for each defined local group
	 * name.
	 */
	private Hashtable localGroupCodes = new Hashtable();

	/**
	 * Get the identifier of a declared group.
	 * 
	 * @param group
	 *            The name of the group.
	 * @return the group id if the group was declared, or 0 if not.
	 */
	synchronized int getGroupId(String groupName) {

		Integer groupId = (Integer) groupCodes.get(groupName);
		if (groupId == null) {
			return 0;
		}
		return groupId.intValue();
	}

	/**
	 * Get the identifier of a declared local group.
	 * 
	 * @param group
	 *            The name of the local group.
	 * @return the group id if the local group was declared, or 0 if not.
	 */
	synchronized int getLocalGroupId(String groupName) {

		Integer groupId = (Integer) localGroupCodes.get(groupName);
		if (groupId == null) {
			return 0;
		}
		return groupId.intValue();
	}

	/**
	 * Declare a group name.
	 * 
	 * @param groupName
	 *            The name of the group.
	 * @param groupId
	 *            The identifier of the group.
	 */
	synchronized void declareGroup(String groupName, int groupId) {

		groupCodes.put(groupName, new Integer(groupId));
	}

	/**
	 * Declare a local group name.
	 * 
	 * @param groupName
	 *            The name of the group.
	 * @param groupId
	 *            The identifier of the group.
	 */
	synchronized void declareLocalGroup(String groupName, int groupId) {

		localGroupCodes.put(groupName, new Integer(groupId));
	}

	// -------------- Groups: joined clients ----------------------------------

	/**
	 * This table stores who clients are in each group. The key of this table is
	 * a group Id, and the value is a HashSet with the client Ids.
	 */
	private HashMap groupMembers = new HashMap();

	/**
	 * Add the given client into the given group.
	 * 
	 * @param clientId
	 *            The client Id.
	 * @param groupId
	 *            The group Id.
	 */
	synchronized void addGroupClient(int clientId, int groupId) {

		HashSet hs = (HashSet) groupMembers.get(new Integer(groupId));
		if (hs == null) {
			hs = new HashSet();
			groupMembers.put(new Integer(groupId), hs);
		}
		hs.add(new Integer(clientId));
	}

	/**
	 * A#ade un cliente a un grupo dado, cuando ya se conocen tanto el nombre
	 * como el codigo de grupo, es decir, en los nodos SeCo que no son la raiz
	 * del arbol.
	 * 
	 * @param clientId
	 *            identificador unico de cliente.
	 * @param groupName
	 *            nombre del grupo.
	 * @param groupId
	 *            identificador del grupo.
	 */
	synchronized void addGroupClient(int clientId, String groupName, int groupId) {

		declareGroup(groupName, groupId);

		HashSet hs = (HashSet) groupMembers.get(new Integer(groupId));
		if (hs == null) {
			hs = new HashSet();
			groupMembers.put(new Integer(groupId), hs);
		}
		hs.add(new Integer(clientId));
	}

	/**
	 * A#ade un cliente a un grupo dado, cuando ya se conocen tanto el nombre
	 * como el codigo de grupo, es decir, en los nodos SeCo que no son la raiz
	 * del arbol.
	 * 
	 * @param clientId
	 *            identificador unico de cliente.
	 * @param groupName
	 *            nombre del grupo.
	 * @param groupId
	 *            identificador del grupo.
	 */
	synchronized void addLocalGroupClient(int clientId, String groupName,
			int groupId) {

		declareLocalGroup(groupName, groupId);

		HashSet hs = (HashSet) groupMembers.get(new Integer(groupId));
		if (hs == null) {
			hs = new HashSet();
			groupMembers.put(new Integer(groupId), hs);
		}
		hs.add(new Integer(clientId));
	}

	/**
	 * Returns an Iterator with all the clients joined in the given group.
	 * 
	 * @param groupId
	 *            The group Id.
	 * @return An Iterator with the clients in the group.
	 */
	synchronized Iterator getGroupClients(int groupId) {

		HashSet hs = (HashSet) groupMembers.get(new Integer(groupId));
		if (hs == null) {
			return null;
		}
		return ((HashSet) hs.clone()).iterator();
	}

	/**
	 * Remove a client from a group.
	 * 
	 * @param clientId
	 *            The client Id.
	 * @param groupId
	 *            The group Id.
	 */
	synchronized void delGroupClient(int clientId, int groupId) {

		HashSet hs = (HashSet) groupMembers.get(new Integer(groupId));
		if (hs != null) {
			hs.remove(new Integer(clientId));
		}
	}

	/**
	 * Remove a client from all existing groups.
	 * 
	 * @param clientId
	 *            The client Id.
	 */
	synchronized void delGroupClient(int clientId) {

		Integer cId = new Integer(clientId);

		Iterator it = groupMembers.values().iterator();
		while (it.hasNext()) {
			HashSet hs = (HashSet) it.next();
			hs.remove(cId);
		}
	}

	// ------------------- History: messages and max length
	// ---------------------

	/**
	 * Esta tabla es un HashMap que almacena los mensajes enviados por cualquier
	 * cliente a cualquier grupo. La clave de esta tabla es el identificador del
	 * grupo al que se envio el mensaje, y el valor es otra HashMap. Respecto de
	 * esta segunda HashMap, la clave es el identificador del cliente que mando
	 * el mensaje, y el valor un objeto HistoryInfo.
	 */
	private HashMap history = new HashMap();

	/**
	 * Almacena un nuevo mensaje.
	 * 
	 * @param from
	 *            codigo del cliente que envio el mensaje
	 * @param group
	 *            codigo del grupo al que se envio el mensaje
	 * @param pkt
	 *            el mensaje a guardar
	 */
	synchronized void addMsg(int clientId, int groupId, SeCoPacket pkt) {

		mylogger.finer("Add group msg (group=" + groupId + ",from=" + clientId
				+ ").");

		Integer gId = new Integer(groupId);
		Integer cId = new Integer(clientId);

		HashMap hm2 = (HashMap) history.get(gId);
		if (hm2 == null) {
			hm2 = new HashMap();
			history.put(gId, hm2);
		}

		HistoryInfo hi = (HistoryInfo) hm2.get(cId);
		if (hi == null) {
			hi = new HistoryInfo(Integer.MAX_VALUE);
			hm2.put(cId, hi);
		}

		if (hi.size() >= hi.maxLength)
			hi.removeElementAt(0);
		hi.add(pkt);
	}

	/**
	 * Borra todos los mensajes que el cliente dado ha enviado al grupo dado
	 * 
	 * @param clientId
	 *            Identificador del cliente que envio los mensajes
	 * @param groupId
	 *            Identificador del grupo al que se enviaron los mensajes
	 */
	synchronized void cleanMsgs(int clientId, int groupId) {

		HashMap hm2 = (HashMap) history.get(groupId);
		if (hm2 != null) {
			HistoryInfo hi = (HistoryInfo) hm2.get(clientId);
			if (hi != null) {
				for (Object obj : hi) {
					SeCoPacket packet = (SeCoPacket) obj;
					packet.setDeprecated(true);
				}
				hi.clear();
			}
		}
	}

	/**
	 * Borra todos los mensajes que el cliente ha enviado a todos los grupo,
	 * pero sin borra el vaalor de la longitud maxima, ni destriyendo las
	 * estructuras de datos creadas.
	 * 
	 * @param clientId
	 *            Identificador del cliente que envio los mensajes
	 */
	synchronized void cleanMsgs(int clientId) {

		Iterator it = history.values().iterator();
		while (it.hasNext()) {
			HashMap hm2 = (HashMap) it.next();
			HistoryInfo hi = (HistoryInfo) hm2.get(new Integer(clientId));
			if (hi != null) {
				for (Object obj : hi) {
					SeCoPacket packet = (SeCoPacket) obj;
					packet.setDeprecated(true);
				}
				hi.clear();
			}
		}
	}

	/**
	 * Destruye toda referencia sobre mensajes enviados, y longitud de la
	 * historia del cliente dado.
	 * 
	 * @param clientId
	 *            Identificador del cliente que envio los mensajes
	 */
	private void destroyMsgs(int clientId) {

		Integer cId = new Integer(clientId);

		Iterator it = history.values().iterator();
		while (it.hasNext()) {
			HashMap hm2 = (HashMap) it.next();
			HistoryInfo hi = (HistoryInfo) hm2.remove(cId);
			if (hi != null) {
				for (Object obj : hi) {
					SeCoPacket packet = (SeCoPacket) obj;
					packet.setDeprecated(true);
				}
				hi.clear();
			}
		}
	}

	/**
	 * Devuelve un Iterator oara acceder a todos los mensajes enviados a ese
	 * grupo por cualquier cliente, pero manteniendo el orden temporal en el que
	 * cada cliente envio sus mensajes.
	 * 
	 * @param groupId
	 *            Identificador del grupo al que se enviaron los mensajes.
	 * @return Devuelve el Iterator de acceso a todos los mensajes enviados al
	 *         grupo
	 */
	synchronized Iterator getMsgs(int groupId) {

		Vector allPkts = new Vector();

		HashMap hm2 = (HashMap) history.get(new Integer(groupId));
		if (hm2 != null) {

			Iterator it = hm2.values().iterator();
			while (it.hasNext()) {
				HistoryInfo hi = (HistoryInfo) it.next();
				allPkts.addAll(hi);
			}
		}
		return allPkts.iterator();
	}

	/**
	 * Devuelve un Vector con todos los mensajes de historia de todos los
	 * grupos, pero manteniendo el orden temporal en el que cada cliente envio
	 * sus mensajes a cada grupo.
	 * 
	 * @return Vector con todos los mensajes de historia.
	 */
	synchronized Vector getAllMsgs() {

		Vector allPkts = new Vector();

		Iterator it = history.values().iterator();
		while (it.hasNext()) {
			HashMap hm2 = (HashMap) it.next();
			Iterator it2 = hm2.values().iterator();
			while (it2.hasNext()) {
				HistoryInfo hi = (HistoryInfo) it2.next();
				allPkts.addAll(hi);
			}
		}
		return allPkts;
	}

	/**
	 * Define la longitud maxima de la histori del cliente y grupo dado.
	 * 
	 * @param clientId
	 *            identificador de cliente
	 * @param groupId
	 *            identificador de grupo
	 * @param maxLength
	 *            numero maximo de mensajes a almacenar
	 */
	synchronized void setMaxLength(int clientId, int groupId, int maxLength) {

		Integer gId = new Integer(groupId);
		Integer cId = new Integer(clientId);

		HashMap hm2 = (HashMap) history.get(gId);
		if (hm2 == null) {
			hm2 = new HashMap();
			history.put(gId, hm2);
		}

		HistoryInfo hi = (HistoryInfo) hm2.get(cId);
		if (hi == null) {
			hi = new HistoryInfo(maxLength);
			hm2.put(cId, hi);
		}

		hi.maxLength = maxLength;

		while (hi.size() > maxLength)
			hi.removeElementAt(0);
	}

	// ----------- History info ----------------------------------------------

	/**
	 * Los objetos de esta clase almacenan los mensajes que un cliente envio a
	 * un grupo. Estan ordenados cronologicamente. Tambien se almacena el numero
	 * maximo de mensajes que hay que guardar.
	 */
	private class HistoryInfo extends Vector {

		/**
		 * Maximum history length.
		 */
		int maxLength;

		/*
		 * Create a new object.
		 * 
		 * @param maxLength Maximum history length.
		 */
		HistoryInfo(int maxLength) {
			super(5);
			this.maxLength = maxLength;
		}

	}

}
