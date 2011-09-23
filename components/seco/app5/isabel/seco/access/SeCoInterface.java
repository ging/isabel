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
 * SeCoInterface.java
 */

package isabel.seco.access;

import java.io.*;

/**
 * Interfaz que contiene los diferentes métodos de utilización de acceso al
 * servicio SeCo.
 * @author  Fernando Escribano
 * @author  Santiago Pavon
 * @author  José Carlos del Valle
 * @version 1.0
 */
public interface SeCoInterface {

    
    /**
     *  Cierra la conexión con el servidor SeCo y le dice a éste que puede liberar toda
     * la información relativa al usuario.
     * Cualquier intento posterior de enviar un mensaje o apuntarse a algún grupo
     * provocará que se lance una excepción.
     *
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación.
     */
    public void quit() throws IOException;


    /** 
     * Devuelve el identificador de cliente que nos asigna el servidor SeCo.
     * El String contiene un numero entero positivo.
     * @return String con el identificador SeCo del cliente.
     */
    public String getIdentity();


    /** 
     * Método para apuntarse a uno de los grupos de comunicación.
     * Es necesario cuando queremos recibir mensajes, pero no para enviar.
     *
     * @param group El nombre del grupo al que queremos apuntarnos.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o si
     * se ha hecho un quit() previamente.
     */
    public void joinGroup(String group) throws IOException;

    
    /**
     * Método para apuntarse a uno de los grupos locales de comunicación. Es
     * necesario cuando queremos recibir mensajes, pero no para enviar.
     * 
     * @param group
     *            El nombre del grupo local al que queremos apuntarnos.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o
     *             si se ha hecho un quit() previamente.
     */
    public void joinLocalGroup(String group) throws IOException;
    

    /** 
     * Método para borrarse de uno de los grupos de comunicación.
     * Hace que dejemos de recibir los mensajes enviados a dicho grupo.
     *
     * @param group El nombre del grupo del que queremos desapuntarnos.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o si
     * se ha hecho un quit() previamente.
     */
    public void leaveGroup(String group) throws IOException;


    /**
     * Método para borrarse de uno de los grupos locales de comunicación. Hace
     * que dejemos de recibir los mensajes enviados a dicho grupo.
     * 
     * @param group
     *            El nombre del grupo local del que queremos desapuntarnos.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o
     *             si se ha hecho un quit() previamente.
     */
    public void leaveLocalGroup(String group) throws IOException;
    

    /** 
     * Método para enviar un mensaje a un grupo sin que se guarde memoria de ese envio.
     * Esto significa que ese mensaje no será enviado a los clientes que se apunten
     * al grupo despues de que el envio se ha producido.
     * @param gname El nombre del grupo al que enviamos el mensaje.
     * @param msg El mensaje que queremos enviar.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o si
     * se ha hecho un quit() previamente.
     */
    public void sendGroup(String gname, byte [] msg) throws IOException;


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
    public void sendLocalGroup(String gname, byte[] msg) throws IOException;


    /** 
     * Método para enviar un mensaje con sincronismo a un grupo de comunicación.
     * @param gname El nombre del grupo al que queremos enviar el mensaje.
     * @param msg El mensaje que queremos enviar.
     * @param sync Boolean que indica si el mensaje es un mensaje de sincronismo o no.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o si
     * se ha hecho un quit() previamente.
     */
    public void sendGroup(String gname, byte[] msg, boolean sync) throws IOException;


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
	throws IOException;
    

    /** 
     * Método para enviar un mensaje a un cliente dado su ClientId.
     * @param dest El identificador SeCo del cliente destino.
     * @param msg El mensaje que queremos enviar.
     * @throws <tt>IOException</tt> Si surge algún problema de comunicación o si
     * se ha hecho un quit() previamente.
     */
    public void sendClient(int dest, byte[] msg) throws IOException;


    /** 
     * Apunta el listener especificado como parámetro para recibir eventos de llegada
     * de mensajes del servidor Seco.
     * Si lis es null no se lanza ninguna excepción ni se realiza ninguna acción.
     * @param lis El listener que queremos que reciba los eventos.
     */
    public void addSeCoListener(SeCoListener lis);


    /** 
     * Elimina el listener especificado como parámetro para que deje de recibir eventos
     * de llegada de mensajes del servidor SeCo.
     * Si el listener que se pasa como parámetro no había sido añadido o es null no
     * se realiza ninguna acción.
     * @param lis El listener que queremos que deje de recibir los eventos.
     */
    public void removeSeCoListener(SeCoListener lis);
    
    
    /**
     * El usuario del servicio que invoca este método fija para el grupo pasado
     * como parámetro un número máximo de paquetes a almacenar en la historia,
     * especificado como segundo parámetro.
     * @param group nombre del grupo del que vamos a fijar el tamaño del buffer.
     * @param size número máximo de mensajes a almacenar en la historia.
     * @throws <tt>IOException</tt> si surge algún problema de comunicación, o si se
     * ha hecho quit() previamente.
     */
    public void setMaxHistoryLength(String group, int size) throws IOException;
    

    /**
     * El usuario del servicio que invoca este método fija para el grupo local
     * pasado como parámetro un número máximo de paquetes a almacenar en la
     * historia, especificado como segundo parámetro.
     * 
     * @param group
     *            nombre del grupo local del que vamos a fijar el tamaño del
     *            buffer.
     * @param size
     *            número máximo de mensajes a almacenar en la historia.
     * @throws <tt>IOException</tt> si surge algún problema de comunicación, o
     *             si se ha hecho quit() previamente.
     */
    public void setLocalMaxHistoryLength(String group, int size)
	throws IOException;
    
}
