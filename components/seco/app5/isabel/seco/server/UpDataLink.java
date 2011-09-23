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
 *  UpDataLink.java
 */
package isabel.seco.server;

import java.net.*;
import java.io.*;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.Vector;
import java.util.logging.*;
import java.util.Iterator;

import isabel.lib.*;
import isabel.lib.reliableudp.RUDPSocket;

/** 
 *  Esta clase se encarga de gestionar la recepcion y envio de mensajes
 *  con el nodo SeCo inmediatamente superior, es decir, al que nuestro nodo 
 *  se conecto.
 *
 *  @author Santiago Pavon
 *  @author Javier Calahorra
 */
public class UpDataLink extends Thread {

    /** 
     *  Objeto utilizado para enviar y recibir por el socket 
     */
    private PacketProcessor myPP;
    /** 
     *  Cola FIFO de DownDataLink's pendientes de identificador de cliente
     */
    private Queue clientIdQueue;
    /** 
     *  Objeto que almacena el estado del nodo.
     */
    private isabel.seco.server.State state;
    /** 
     *  Objeto para generar mensajes de log 
     */
    private Logger mylogger;
    /** 
     *  Almacena DownDataLink esperando permisos de su cliente 
     */
    private HashMap permissionList;

    /**
     *  Constructor de objetos UpDataLink de conexion con el nodo SeCo superior.
     *  @param s socket de conexion con el nodo padre
     *  @param q cola de clientes pendientes de identificador
     *  @param state estado del nodo
     *  @throws <tt>IOException</tt> si surge algun error durante la creacion
     * del objeto.
     */
    public UpDataLink(Socket s, Queue q, isabel.seco.server.State state, HashMap list) throws IOException {

        mylogger = Logger.getLogger("isabel.seco.server.UpDataLink");
        mylogger.finer("Creating UpDataLink object");

        myPP = new PacketProcessor(s);
        clientIdQueue = q;
        this.state = state;
        permissionList = list;

        Hello();
    }

    /**
     *  Constructor de objetos UpDataLink de conexion con el nodo SeCo superior.
     *  @param s socket de conexion con el nodo padre
     *  @param q cola de clientes pendientes de identificador
     *  @param state estado del nodo
     *  @throws <tt>IOException</tt> si surge algun error durante la creacion
     * del objeto.
     */
    public UpDataLink(RUDPSocket s, Queue q, isabel.seco.server.State state, HashMap list) throws IOException {

        mylogger = Logger.getLogger("isabel.seco.server.UpDataLink");
        mylogger.finer("Creating UpDataLink object");

        myPP = new PacketProcessor(s);
        clientIdQueue = q;
        this.state = state;
        permissionList = list;

        Hello();
    }

    /**
     *  Lo primero que hace un nodo SeCo intermedio es enviar a su nodo padre un SeCoPacket
     *  tipo HELLO_SECO_COMMAND mediante el PacketProcessor correspondiente,
     *  el de UpDataLink. Hasta que no se realiza este paso nuestro
     *  nodo SeCo no arranca su socket de servidor, es decir, no atiende
     *  peticiones ni de clientes ni de otros nodos SeCo.
     */
    private void Hello() throws IOException {
        mylogger.info("Sending HELLO SECO");
        SeCoPacket spkt = new SeCoPacket();
        spkt.setCommand(SeCoPacket.HELLO_SECO_COMMAND);
        myPP.send(spkt);
        state.addClientPP(0, myPP); // identificador 0 => ruta por defecto
    }

    /**
     *  Asigna el identificador unico del cliente a aquel que lleva mas tiempo
     *  en la cola de pendientes.
     *  @param pkt SeCoPacket tipo CLIENT_ID_COMMAND con el identificador
     *  de cliente.
     */
    private void AllocateClientID(SeCoPacket pkt) throws IOException {
        int clientID = pkt.getToID();
        DownDataLink attendant = (DownDataLink) clientIdQueue.get();
        PacketProcessor pp = attendant.getMyPP();
        pp.send(pkt);
        if (attendant.isClient()) {
            attendant.setMyClientId(clientID);
        } else {
            state.addClientPP(clientID, pp);
            ArrayList myClients = attendant.getMyClients();
            myClients.add(new Integer(clientID));
        }
    }

    /**
     *  Procesa la union de un cliente a un grupos.
     *  Se le envia el identificador del grupo, y todos
     *  los mensajes de la historia de ese grupo.
     *  @param pkt paquete con el identificador del grupo.
     */
    private void processJoinGroup(SeCoPacket pkt) throws IOException {

        int toID = pkt.getToID();
        String group = new String(pkt.getData());

        PacketProcessor pp = state.getClientPP(toID);

        mylogger.info("Received \"" + group + "\" group ID (" + pkt.getFromID() + ") for client " + toID);

        synchronized (state) {
            if (state.isClientLocal(toID)) {
                // el cliente es local de este nodo SeCo

                mylogger.finest("Received \"" + group + "\" group ID (" + pkt.getFromID() + ") for my client " + toID);
                int groupCode = pkt.getFromID(); //identificador de grupo en fromID
                state.addGroupClient(toID, group, groupCode);
                SeCoPacket pkt2 = new SeCoPacket();
                pkt2.setCommand(SeCoPacket.JOIN_GROUP_COMMAND);
                pkt2.setToID(groupCode);
                pkt2.setData(pkt.getData());
                pp.send(pkt2);

                //  enviamos historia
                sendClientHistory(toID, groupCode);

            } else {
                // si el cliente es de otro nodo SeCo
                pp.send(pkt);
            }
        }

    }

    /**
     *  Se emplea para enviar toda la historia de un grupo a un cliente.
     *  @param to identificador de cliente destino
     *  @param groupId identificador del grupo para obtener todos los mensajes de historia.
     */
    private void sendClientHistory(int to, int groupId) {

        mylogger.finest("Sending to client " + to + " the \"" + groupId + "\" group history.");

        PacketProcessor pp = state.getClientPP(to);
        if (pp != null) {
            Iterator i = state.getMsgs(groupId);
            while (i.hasNext()) {
                SeCoPacket pkt = (SeCoPacket) i.next();
                try {
                    mylogger.finest("Sending history message to client " + to);
                    pp.send(pkt);
                } catch (IOException e) {
                    mylogger.severe(e.getMessage());
                }
            }
        }
    }

    /**
     *  Informa a un cliente sobre cual es el identificador de un grupo.
     *  @param pkt paquete con el identificador del grupo.
     */
    private void processGetGroupID(SeCoPacket pkt) throws IOException {

        int toID = pkt.getToID();
        String group = new String(pkt.getData());

        PacketProcessor pp = state.getClientPP(toID);

        mylogger.finest("Received \"" + group + "\" group ID (" + pkt.getFromID() + ") for client " + toID);

        synchronized (state) {
            if (state.isClientLocal(toID)) {
                // el cliente es local de este nodo SeCo
                mylogger.finest("Received \"" + group + "\" group ID (" + pkt.getFromID() + ") for my client " + toID);
                int groupCode = pkt.getFromID(); //identificador de grupo en fromID
                state.declareGroup(group, groupCode);
                SeCoPacket pkt2 = new SeCoPacket();
                pkt2.setCommand(SeCoPacket.GET_GROUP_ID_COMMAND);
                pkt2.setToID(groupCode);
                pkt2.setData(pkt.getData());
                pp.send(pkt2);

            } else {
                // el cliente no es local de este nodo SeCo
                pp.send(pkt);
            }
        }

    }

    /**
     *  Reencamina el paquete que acaba de recibir hacia el
     *  cliente destino.
     *  @param pkt paquete tipo CLIENT_MSG_COMMAND con destino un cliente.
     */
    private void RouteToClient(SeCoPacket pkt) {

        int to = pkt.getToID();
        PacketProcessor pp = state.getClientPP(to);
        if (pp != null) {
            try {
                pp.send(pkt);
            } catch (IOException e) {
                mylogger.severe("IOException = " + e.getMessage());
            }
        }
    }

    /**
     *  Toma un mensaje que acaba de recibir y lo reencamina a
     *  todos los clientes que escuchan en un grupo.
     *  @param pkt paquete con destino un grupo (sin historia, con historia o
     *             de sincronismo).
     *  @param sync indica si el mensaje a grupo es de sincronismo o no.
     *  @param keep indica si el mensaje a grupo se almacena en la historia
     *              del mismo o no.
     */
    private void RouteToGroup(SeCoPacket pkt, boolean sync, boolean keep) {

        int toID = pkt.getToID();
        int fromID = pkt.getFromID();

        synchronized (state) {
            if (sync) {
                state.cleanMsgs(fromID, toID);
            }

            if (keep) {
                state.addMsg(fromID, toID, pkt);
            }

            sendGroup(toID, pkt);
        }
    }

    /**
     *  Toma un mensaje de desconexion de cliente que acaba de recibir y lo reencamina a
     *  todos los clientes que escuchan en un grupo.
     *  @param pkt paquete con destino un grupo
     */
    private void sendClientDisconnection(SeCoPacket pkt) {

        int toID = pkt.getToID();
        int fromID = pkt.getFromID();

        synchronized (state) {
            state.cleanupClient(fromID);

            sendGroup(toID, pkt);
        }
    }

    /**
     * El algoritmo empleado para enviar mensajes a grupos será la inundacion.
     * Por defecto suponemos que el mensaje NO es solo para los clientes
     * locales.
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
         * que esten apuntados a ese grupo.
         */
        // Calculos clientes locales mios, o locales a nodos seco por debajo de
        // mi que
        // esten unido al grupo dado.
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
         * nuestro nodo. Nota: solo enviamos una copia a cada nodo SeCo
         * conectado con nosotros, aunque por esa ruta se llegue a varios
         * clientes.
         */
        Iterator<PacketProcessor> it = state.getAllLocalSeCoPPs();
        while (it.hasNext()) {
            PacketProcessor pp = it.next();
            try {
                mylogger.finest("Sending message to a local SeCo node.");
                pp.send(pkt);
            } catch (IOException e) {
                mylogger.severe(e.getMessage());
            }

        }

    }

    /**
     *  Toma un mensaje de actualizacion de tama#0 de historia que acaba de recibir y lo reencamina a
     *  todos los clientes que escuchan en un grupo.
     *  @param pkt paquete con destino un grupo.
     */
    private void sendMaxHistoryLengthUpdate(SeCoPacket pkt) {

        int fromID = pkt.getFromID();
        int toID = pkt.getToID();

        DataInputStream dis = new DataInputStream(new ByteArrayInputStream(pkt.getData()));
        try {
            int size = dis.readInt();

            synchronized (state) {
                state.setMaxLength(fromID, toID, size);

                propagateMaxHistoryLengthUpdate(pkt);
            }
        } catch (IOException ioe) {
            mylogger.severe(ioe.getMessage());
        }
    }

    /**
     *  El algoritmo empleado serÃ¡ la inundaciÃ³n, es decir, se envia una 
     *  copia a todos los nodos SeCo que se nos han conectado.
     *  @param pkt mensaje a enviar
     */
    private void propagateMaxHistoryLengthUpdate(SeCoPacket pkt) {

        /* Enviamos el mensaje a todos los nodos SeCo que se conectaron
         * a nuestro nodo.
         * Nota: solo enviamos una copia a cada nodo SeCo conectado
         *       con nosotros, aunque por esa ruta se llegue a varios
         *       clientes.
         */
        Iterator<PacketProcessor> it = state.getAllLocalSeCoPPs();
        while (it.hasNext()) {
            PacketProcessor pp = it.next();
            try {
                mylogger.finest("Sending message to a local SeCo node.");
                pp.send(pkt);
            } catch (IOException e) {
                mylogger.severe(e.getMessage());
            }
        }
    }

    /**
     * Metodo que en funcion de los permisos asignados a un cliente actualiza
     * la variable permissions del DownDataLink que lo atiende con el valor
     * correspondiente, siempre y cuando el cliente sea del mismo SeCoProxy.
     * Si no lo es, simplemente reenvia el paquete al SeCoProxy adecuado.
     * @param pkt paquete tipo PERMISSIONS_COMMAND con los permisos asignados
     * a un cliente.
     * @throws <tt>IOException</tt> si surge algun error durante el proceso.
     */
    private void SetPermissions(SeCoPacket pkt) throws IOException {

        int toID = pkt.getToID();
        byte[] data = pkt.getData();
        PacketProcessor pp = state.getClientPP(toID);
        if (state.isClientLocal(toID)) {
            /*
            Iterator it = permissionList.iterator();
            while (it.hasNext()) {
            DownDataLink da=(DownDataLink) it.next();
            if (da.getMyClientId()==toID) {
            switch (data[0]) {
            case Permissions.NONE:
            da.setPermissions(Permissions.NONE);
            break;
            case Permissions.RECEIVE_ONLY:
            da.setPermissions(Permissions.RECEIVE_ONLY);
            break;
            case Permissions.SEND_AND_RECEIVE:
            da.setPermissions(Permissions.SEND_AND_RECEIVE);
            break;
            }
            it.remove();
            pp.send(pkt);
            return;
            }
             */
            DownDataLink da = (DownDataLink) permissionList.remove(new Integer(toID));
            if (da != null) {
                da.setPermissions(data[0]);
                pp.send(pkt);
                return;
            }
        } else {
            pp.send(pkt);
        }
    }

    /**
     *  Bucle principal del thread que atiende a los mensajes que llegan del nodo SeCo superior.
     */
    public void run() {

        boolean parar = false;

        try {
            while (!parar) {
                SeCoPacket rpkt = myPP.receive();
                mylogger.finest("UpLink:: Received message from ClientId=" + rpkt.getFromID() + " :" + rpkt);

                //Segun el tipo de primitiva se hacen diferentes cosas
                switch (rpkt.getCommand()) {

                    case SeCoPacket.CLIENT_ID_COMMAND:
                        // Comando para recibir identificador de cliente
                        AllocateClientID(rpkt);
                        break;

                    case SeCoPacket.JOIN_GROUP_COMMAND:
                        // Comando para unirse a grupo, y recibir su identificador y la historia
                        processJoinGroup(rpkt);
                        break;

                    case SeCoPacket.GET_GROUP_ID_COMMAND:
                        // Comando para recibir identificador de grupo
                        processGetGroupID(rpkt);
                        break;

                    case SeCoPacket.CLIENT_MSG_COMMAND:
                        // Comando para enviar un mensaje a un cliente
                        RouteToClient(rpkt);
                        break;

                    case SeCoPacket.MSG_COMMAND:
                        // Comando para enviar mensajes
                        RouteToGroup(rpkt, false, true);
                        break;

                    case SeCoPacket.SYNC_MSG_COMMAND:
                        // Comando para enviar mensajes de sincronismo
                        RouteToGroup(rpkt, true, true);
                        break;

                    case SeCoPacket.NO_HISTORY_MSG_COMMAND:
                        // Comando para enviar mensajes sin historia
                        RouteToGroup(rpkt, false, false);
                        break;

                    case SeCoPacket.CLIENT_DISCONNECTED_COMMAND:
                        // Comando que informa de la desconexion de un cliente
                        sendClientDisconnection(rpkt);
                        break;

                    case SeCoPacket.SET_MAX_HISTORY_LENGTH_COMMAND:
                        // Comando para dar tama#o a la historia
                        sendMaxHistoryLengthUpdate(rpkt);
                        break;

                    case SeCoPacket.PERMISSIONS_COMMAND:
                        SetPermissions(rpkt);
                        break;
                }
            }
        } catch (IOException e) {
            mylogger.severe("IOException = " + e.getMessage());
        } catch (Exception e) {
            mylogger.severe("UpDataLink Unexpected exception = " + e.getMessage());
            e.printStackTrace(System.err);
        } finally {

            if (myPP != null) {
                try {
                    myPP.close();
                } catch (IOException e) {
                    mylogger.severe("IOException = " + e.getMessage());
                }
            }
            mylogger.info("UpLink is going to exit SeCo.");
            System.exit(0);
        }
    }
}
