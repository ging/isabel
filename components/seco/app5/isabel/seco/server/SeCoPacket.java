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
 * SeCoPacket.java
 */

package isabel.seco.server;

import java.io.*;

/** 
 *  Esta clase mantiene la informacion necesaria para construir un paquete con los 
 *  datos (datos de control de las aplicaciones) que el servidor SeCo reencamina entre los clientes.
 *
 *  El formato de linea de un paquete es el siguiente:
 *         ----------------   
 *        | int fromID     |    ClientId del origen
 *        |----------------|  
 *        | int toID       |    ClientId/GroupId del destino
 *        |----------------|   
 *        | int type       |    tipo de primitiva
 *        |----------------|  
 *        | int dataSize   |    longitud de los datos
 *        |----------------|  
 *        |                |   
 *        |                |   
 *        /   char *data   /    datos
 *        /                /   
 *        |                |   
 *        |                |   
 *         ----------------   
 *  @author  Fernando Escribano
 *  @author  Santiago Pavon
 */
public class SeCoPacket implements SeCoPacketTypes {
    
    /** Numero de bytes que ocupa la cabecera de los paquetes */
    public static final int HEAD_SIZE = 16;
    
    /** Identificador de la fuente. */
    private int fromID;
    
    /** Identificador del destino. */
    private int toID;
    
    /** Tipo de primitiva. */
    private int command;
    
    /** Tama#o de los datos. Este campo se maneja internamente. */
    private int dataSize;
    
    /** Datos del mensaje. */
    private byte[] data;
    
    /** Indicates if the Packet is deprecated */
    private boolean deprecated;

    /** Crea un nuevo paquete con todos los campos a cero */
    public SeCoPacket() {
	fromID    = 0;
	toID      = 0;
	command   = 0;
	dataSize  = 0;
	data      = null;
	deprecated = false;
    }
    
    /** 
     *  Crea un nuevo objeto SeCoPacket a partir de los bytes
     *  que forman la cabecera.
     *  @param header Array de HEAD_SIZE bytes que forman la cabecera.
     *  @throws <tt>IOException</tt> Si surge algun problema.
     */
    public SeCoPacket(byte[] header) throws IOException {
	// A partir del array obtengo los diferentes campos del paquete.
	DataInputStream din = new DataInputStream(
	    new ByteArrayInputStream(header));
	fromID    = din.readInt();
	toID      = din.readInt();
	command   = din.readInt();
	dataSize  = din.readInt();
    }
    
    /** 
     *  Metodo para obtener el identificador de la fuente
     *  del paquete.
     *  @return Entero que identifica la fuente.
     */
    public int getFromID() {
	return fromID;
    }
    
    /** 
     *  Metodo para obtener el identificador del destino
     *  del paquete.
     *  @return Entero que identifica el destino
     */
    public int getToID() {
	return toID;
    }   
    
    /** 
     *  Metodo para obtener el tipo de primitiva del paquete.
     *  @return Entero que indica el tipo de primitiva.
     */
    public int getCommand() {
	return command;
    }   
    
    /** 
     *  Metodo para obtener la longitud de los datos del paquete.
     *  @return Entero que indica la longitud de la parte de datos.
     */
    public int getDataSize() {
	return dataSize;
    }
    
    /** 
     *  Metodo para obtener los datos del paquete.
     *  @return Array de bytes que contiene los datos.
     */
    public byte[] getData() {
	return data;
    }
    
    /** 
     *  Metodo para colocar el tipo de primitiva del paquete.
     *  @param type Integer que indica el tipo de primitiva.
     */
    public void setCommand(int type) {
	command = type;
    }
    
    /** 
     *  Metodo para colocar los datos del paquete.
     *  @param dat Array de bytes con los datos.
     */
    public void setData(byte[] dat) {
	data = dat;
	dataSize = data.length;
    }
    
    /** 
     *  Metodo para colocar el campo fromID del paquete.
     *  @param id Array de dos enteros que forman el identificador.
     */
    public void setFromID(int id) {
	fromID = id;
    }
    
    /** 
     *  Metodo para colocar el campo toID del paquete.
     *  @param id Array de dos enteros que forman el identificador.
     */
    public void setToID(int id) {
	toID = id;
    }
    
    /** 
     *  Metodo para obtener el array de bytes que debe enviarse para
     *  enviar este paquete.
     *  @return Array de bytes que contiene la cabecera y los datos.
     *  @throws IOException Nunca espero
     */
    public byte[] getBytes() throws IOException {
	byte [] result = new byte[HEAD_SIZE + dataSize];
	
	// Escribo todos los datos a un byte array en el orden adecuado.
	ByteArrayOutputStream baos = new ByteArrayOutputStream();
	DataOutputStream dout = new DataOutputStream(baos);
	dout.writeInt(fromID);
	dout.writeInt(toID);
	dout.writeInt(command);
	dout.writeInt(dataSize);
	
	// Copio los bytes de cabecera al array que voy a devolver
	System.arraycopy(baos.toByteArray(),0,result,0,HEAD_SIZE);
	
	// y copio tambien los datos si los hay
	if (data != null)
	    System.arraycopy(data,0,result,HEAD_SIZE,dataSize);
	
	return result;
    }
    
  /**
     * Indicate if the packet is deprecated
     * @param dep true if it is deprecated, false if it is not.
     */
    public void setDeprecated(boolean dep){
        deprecated = dep;
    }

    /**
     * Tells whether the packet is deprecated
     * @return true if it is deprecated
     */
    public boolean isDeprecated(){
        return deprecated;
    }

    public String toString() {
	return "F="+fromID+
	    " T="+toID+
	    " CMD="+cmd2name(command)+
	    " SZ="+dataSize+
	    " D=????";
    }
    
    /**
     *  Devuelve un string significativo para el codigo de comando.
     *  @param cmd codigo del comando
     *  @return un string significativo para el codigo de comando.
     */
    private String cmd2name(int cmd) {
	
	switch (cmd) 
	    {
	    case HELLO_CLIENT_COMMAND:
		return "HELLO_CLIENT_COMMAND";
	    case LOGIN_COMMAND:
		return "LOGIN_COMMAND";
	    case PERMISSIONS_COMMAND:
		return "PERMISSIONS_COMMAND";
	    case HELLO_SECO_COMMAND:
		return "HELLO_SECO_COMMAND";
	    case BYE_COMMAND:
		return "BYE_COMMAND";
	    case CLIENT_ID_COMMAND:
		return "CLIENT_ID_COMMAND";
	    case JOIN_GROUP_COMMAND:
		return "JOIN_GROUP_COMMAND";
	    case JOIN_LOCAL_GROUP_COMMAND:
		return "JOIN_LOCAL_GROUP_COMMAND";
	    case LEAVE_GROUP_COMMAND:
		return "LEAVE_GROUP_COMMAND";
	    case LEAVE_LOCAL_GROUP_COMMAND:
		return "LEAVE_LOCAL_GROUP_COMMAND";
	    case GET_GROUP_ID_COMMAND:
		return "GET_GROUP_ID_COMMAND";
	    case GET_LOCAL_GROUP_ID_COMMAND: 
		return "GET_LOCAL_GROUP_ID_COMMAND";
	    case CLIENT_DISCONNECTED_COMMAND:
		return "CLIENT_DISCONNECTED_COMMAND";
	    case MSG_COMMAND:
		return "MSG_COMMAND";
	    case LOCAL_MSG_COMMAND:
		return "LOCAL_MSG_COMMAND";
	    case SYNC_MSG_COMMAND:
		return "SYNC_MSG_COMMAND";
	    case LOCAL_SYNC_MSG_COMMAND:
		return "LOCAL_SYNC_MSG_COMMAND";
	    case NO_HISTORY_MSG_COMMAND:
		return "NO_HISTORY_MSG_COMMAND";
	    case LOCAL_NO_HISTORY_MSG_COMMAND:
		return "LOCAL_NO_HISTORY_MSG_COMMAND";
	    case CLIENT_MSG_COMMAND:
		return "CLIENT_MSG_COMMAND";
	    case SET_MAX_HISTORY_LENGTH_COMMAND:
		return "SET_MAX_HISTORY_LENGTH_COMMAND";
	    case SET_LOCAL_MAX_HISTORY_LENGTH_COMMAND:
		return "SET_LOCAL_MAX_HISTORY_LENGTH_COMMAND";
	    default: return "???";
	}
    }
}

    
