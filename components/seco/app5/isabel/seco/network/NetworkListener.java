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
 * NetworkListener.java
 */

package isabel.seco.network;

/**
 *  Este interfaz es el que tienen que implementar las clases que quieran recibir
 *  eventos de la red.
 *  Hay dos tipos de eventos posibles que son la recepcion de un mensaje
 *  a traves de la red y la deteccion de la muerte de la red. Hay por tanto
 *  un metodo asociado a cada tipo de evento.
 *
 *  @author  Fernando Escribano
 *  @author  Santiago Pavon
 *  @version 1.0
 */
public interface NetworkListener {

    /** 
     * Metodo llamado por el objeto Network en sus listeners cuando se
     * produce la recepción de un mensaje.
     * @param msg El mensaje recibido.
     */
    public void messageReceived(Message msg);

    /** 
     * Metodo llamado por el objeto Network en sus listeners cuando ocurre
     * un problema con la comunicación.
     * @param info String con información sobre los motivos.
     */
    public void networkDead(String info);

    /** 
     * Método que es un invocado cuando un cliente se desconecta.
     * @param clientId El identificador SeCo del cliente desconectado.
     */
    public void clientDisconnected(String clientId);
}

