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
package isabel.gw;

import java.util.ArrayList;
import isabel.gw.isabel_client.isabel_state.IsabelStateImpl;
import isabel.gw.isabel_client.isabel_state.Service;

/**
 * This class is used to manage the isabel session global state, which is not related with the state
 * of the connected sites. 
 * 
 * The isabel session state related with the connected clients is managed by the IsabelClient objects.
 * 
 * These objects communicate with the isabel conference via the SeCo service.
 * 
 * An IsabelState object connects with the isabel session when the connect method is called,
 * and disconnects from the conference when the disconnect method is called.
 * While connected to the conference, it sends event to the registered
 * IsabelStateListener objects, to inform about what is happening in the
 * conference.
 */
public abstract class IsabelState {
    
    /**
     *  The singleton instance.
     */
    private static IsabelStateImpl isabelState;


    /**
     * Returns the IsabelState object.
     * It is a singleton.
     */
    public static IsabelState getIsabelState() {

	if (isabelState == null) {
	    isabelState = new IsabelStateImpl();
	}
	return isabelState;
    }


    // -----------------------------------------------------------------------
    // Activity/Service definition
    // -----------------------------------------------------------------------

    /**
     * Devuelve una referencia al objeto que almacena el servicio recibido.
     * 
     * @return Devuelve una referencia al objeto que almacena el servicio
     *         recibido.
     */
    public abstract Service getService();

    // -----------------------------------------------------------------------
    // LISTENERS
    // -----------------------------------------------------------------------


    /**
     * To store the references to the listener objects.
     */
    private ArrayList<IsabelStateListener> listeners = new ArrayList<IsabelStateListener>();


    /**
     *  Add a new IsabelStateListener reference. 
     *  Note that this is a static method.
     * 
     * @return Result
     * @param listener
     */
    public void addIsabelStateListener(IsabelStateListener listener) {
	
	if (listener == null)
	    return;
	synchronized (listeners) {
	    listeners.add(listener);
	}
    }
    
    /**
     *  Remove a IsabelStateListener reference.
     *  Note that this is a static method.
     * 
     * @return Result
     * @param listener
     */
    public void removeIsabelStateListener(IsabelStateListener listener) {
	
	if (listener == null)
	    return;
	synchronized (listeners) {
	    listeners.remove(listener);
	}
    }
    
    
    /**
     * Returns references to the IsabelStateListener objects.
     * 
     * @return Returns references to the IsabelstateListener objects.
     */  
    public ArrayList<IsabelStateListener> getListeners() {
	
	ArrayList<IsabelStateListener> ll = new ArrayList<IsabelStateListener>();
	
	synchronized (listeners) {
	    ll.addAll(listeners);
	}
	
	return ll;
    }
}
