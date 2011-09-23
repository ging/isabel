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
package isabel.gw.isabel_client.isabel_state;

import isabel.gw.IsabelState;
import java.util.logging.Logger;

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
public class IsabelStateImpl extends IsabelState {
    
    // ------------------------ DE INSTANCIA ------------------------------
    
    /**
     * Logs
     */
    private Logger mylogger;
    
    /**
     * Network Manager. Setup the object to access the SeCo daemon.
     */
    private NetworkManager netMgr;
    
    /**
     * Component Manager. Se encarga de los mensajes CM: lanza y para los
     * componentes.
     */
    private ComponentManager compMgr;
    
    /**
     * Service Manager Se encarga de los mensajes SERVICE
     */
    private ServiceManager servMgr;
    
    /**
     * SMS Manager Se encarga de los mensajes SMS
     */
    private SmsManager smsMgr;
    
    /**
     * Construct a new Isabel State object.
     */
    public IsabelStateImpl() {
	
	mylogger = Logger.getLogger("isabel.gw.isabel_client.isabel_state.IsabelState");
	mylogger.info("Creating a new Isabel State object.");

	connect();
    }
    
    /**
     * Realiza la conexion con la red isabel, y crea los objetos internos que
     * manejan los mensajes recibidos.
     */
    private synchronized void connect() {
	
	// NetworkManager: 
	// Internally, creates the Network and the Dispatcher objects.
	netMgr  = new NetworkManager(); 
	
	// ServiceManager:
	// It is registered in the dispatcher, and wait for (it don't returns) the
	//  Activity to be received.
	servMgr = new ServiceManager(this);
	
	// ComponentManager:
	// It is registered in the dispatcher.
	compMgr = new ComponentManager(this);
	
	// SmsManager:
	// It is registered in the dispatcher.
	smsMgr = new SmsManager(this);
	
	/*
	// Apuntarse al grupo "isastate"
	try {
	mylogger.info("-------------APUNTARSE A ISASTATE.");
	netMgr.getNetwork().joinGroup("isastate");
	} catch (IOException e) {
	mylogger.severe(e.getMessage());
	}
	*/
    }
    
    
    // -----------------------------------------------------------------------

    /**
     * Devuelve una referencia al objeto que almacena el servicio recibido.
     * 
     * @return Devuelve una referencia al objeto que almacena el servicio
     *         recibido.
     */
    public Service getService() {
	return servMgr.getService();
    }
    
    
    // -----------------------------------------------------------------------
    // GETTERS
    // -----------------------------------------------------------------------
    
    /**
     * Get a reference to the NetworkManager attribute.
     * 
     * @return Returns a reference to the NetworkManager attribute.
     */
    public NetworkManager getNetworkMgr() {
	return netMgr;
    }
    
    /**
     * Get a reference to the ComponentManager attribute.
     * 
     * @return Returns a reference to the ComponentManager attribute.
     */
    public ComponentManager getComponentMgr() {
	return compMgr;
    }
    
    /**
     * Get a reference to the SmsManager attribute.
     * 
     * @return Returns a reference to the SmsManager attribute.
     */
    public SmsManager getSmsMgr() {
	return smsMgr;
    }
    
    // -----------------------------------------------------------------------
    
}
