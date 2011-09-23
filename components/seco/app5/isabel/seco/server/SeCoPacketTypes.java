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
 * SeCoPacketTypes.java
 */

package isabel.seco.server;

/**
 * Esta clase define los diferentes tipos de paquetes existentes.
 * 
 * @author Fernando Escribano
 * @author Santiago Pavon
 * @author Javier Calahorra
 * @author José Carlos del Valle
 */

public interface SeCoPacketTypes {
    
    /** Comando para iniciar una sesion. */
    public static final int HELLO_CLIENT_COMMAND = 102;
    
    /** Comando para finalizar una sesion. */
    public static final int BYE_COMMAND = 121;
    
    /** Comando para enviarnos nuestro identificador */
    public static final int CLIENT_ID_COMMAND = 131;
    
    /** Comando para recibir los mensajes enviados a un grupo. */
    public static final int JOIN_GROUP_COMMAND = 103;
    
    /** Comando para dejar de recibir los mensajes anviados a un grupo. */
    public static final int LEAVE_GROUP_COMMAND = 104;
    
    /** Comando para recibir los mensajes enviados a un grupo local. */
    public static final int JOIN_LOCAL_GROUP_COMMAND = 105;
    
    /** Comando para dejar de recibir los mensajes anviados a un grupo local. */
    public static final int LEAVE_LOCAL_GROUP_COMMAND = 106;
    
    /** Comando para preguntar por el nombre de un grupo */
    public static final int GET_GROUP_ID_COMMAND = 141;
    
    /** Comando para preguntar por el nombre de un grupo */
    public static final int GET_LOCAL_GROUP_ID_COMMAND = 142;
    
    /** Comando para indicar que un cliente se ha desconectado */
    public static final int CLIENT_DISCONNECTED_COMMAND = 200;
    
    /** Comando para enviar un mensaje de no sincronismo a un grupo. */
    public static final int MSG_COMMAND = 150;
    
    /** Comando para enviar un mensaje de sincronismo a un grupo. */
    public static final int SYNC_MSG_COMMAND = 151;
    
    /** Comando para enviar un mensaje del que no se guarda historia a un grupo. */
    public static final int NO_HISTORY_MSG_COMMAND = 152;
    
    /** Comando para enviar un mensaje de no sincronismo a un grupo local. */
    public static final int LOCAL_MSG_COMMAND = 154;
    
    /** Comando para enviar un mensaje de sincronismo a un grupo local. */
    public static final int LOCAL_SYNC_MSG_COMMAND = 155;
    
    /** Comando para enviar un mensaje del que no se guarda historia a un grupo local. */
    public static final int LOCAL_NO_HISTORY_MSG_COMMAND = 156;
    
    /** Comando para enviar un mensaje a un cliente. */
    public static final int CLIENT_MSG_COMMAND = 153;
    
    /** Comando para iniciar la sesion de un SeCoProxy */
    public static final int HELLO_SECO_COMMAND = 101;
    
    /** Comando para indicar el tama#o del buffer para la historia */
    public static final int SET_MAX_HISTORY_LENGTH_COMMAND = 129;
    
    /** Comando para indicar el tama#o del buffer para la historia de un grupo local */
    public static final int SET_LOCAL_MAX_HISTORY_LENGTH_COMMAND = 128;
    
    /** Comando para peticion de autenticacion de cliente */
    public static final int LOGIN_COMMAND = 160;
    
    /** Comando para asignar permisos a clientes */
    public static final int PERMISSIONS_COMMAND = 161;
}
