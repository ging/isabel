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
 * SeCoListener.java
 */

package isabel.seco.access;

/**
 * Interfaz que deben implementar las clases capaces de procesar los mensajes
 * llegados a través del servicio SeCo.
 * @author  Fernando Escribano
 * @version 1.0
 */
public interface SeCoListener {
    
    /** Este método es invocado cuando llega un mensaje y se encarga de
     * procesarlo adecuadamente.
     * @param msg Array de bytes que forman el mensaje.
     * @param from String con el identificador de SeCo del origen.
     * @param to   String con el identificador de SeCo del destino.
     */
    void messageReceived(byte[] msg, String from, String to);
    
    /** Método que es invocado cuando se produce un error en las
     *  comunicaciones con el servidor SeCo.
     *  @param reason La razón del problema.
     */
    void serviceDead(String reason);
    
    /** Método que es un invocado cuando un cliente se desconecta.
     *  @param from El identificador SeCo del cliente desconectado.
     */
    void clientDisconnected(String from);
}
