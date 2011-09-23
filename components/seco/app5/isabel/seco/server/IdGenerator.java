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
 *  IdGenerator.java
 */

package isabel.seco.server;

/**
 * This class is used in the root SeCo node to generate uniq client and group
 * identifier.
 * 
 * Client identifiers are positive numbers.
 * 
 * Group identifiers are negative numbers. NOTE: State.DISCONNECTIONS_GID (-1)
 * is reserved for the DISCONNECTIONS group.
 */
class IdGenerator {
    
    /**
     * Contador usado para asignar identificadores de cliente unicos. Seran
     * numeros positivos.
     */
    private int clientCont = 0;
    
    /**
     *  Counter user to generate group identifiers. 
     *  Values are negative numbers and they are assigned 
     *  starting at State.DISCONNECTIONS_GID-1.
     */
    private int groupCont = State.DISCONNECTIONS_GID;
    
    /**
     *  Counter user to generate local group identifiers. 
     *  Values are negative numbers and they are assigned 
     *  starting at -1000000000. (2^30 more or less).
     */
    private int localGroupCont = -1000000000;
    
    /**
     * Lock object for the client id counter
     */
    private Object cLock = new Object();
    
    /**
     * Lock object for the group id counter
     */
    private Object gLock = new Object();
    
    /**
     * Get a new client ID
     * 
     * @return a new client ID.
     */
    int getNewClientId() {
	
	synchronized (cLock) {
	    return ++clientCont;
	}
    }
    
    /**
     * Get a new group ID
     * 
     * @return a new group ID.
     */
    int getNewGroupId() {
	
	synchronized (gLock) {
	    return --groupCont;
	} 
    }
    
    /**
     * Get a new local group ID
     * 
     * @return a new group ID.
     */
    int getNewLocalGroupId() {
	
	synchronized (gLock) {
	    return --localGroupCont;
	}
    }
}
