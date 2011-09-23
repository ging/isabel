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
package isabel.lib;

public class NetworkManagerConstants {
	/**
     * Puerto en el que escucha peticiones el servidor
     */
    public static final int PORT = 51016;
   

    /** puerto por defecto con el que intenta abrir el tunel el cliente
      */
    public static final int CLIENT_PORT = 53018;
 
    /**
     * Nombre del grupo de servicios que se ejecutan mediante el Limhandler.
     */
    public static final String TUNNEL_KEY = "tunnel";
    public static final String IPERF_KEY = "iperf";

    
}
