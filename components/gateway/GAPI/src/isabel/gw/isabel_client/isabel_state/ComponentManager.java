
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

import isabel.gw.SessionEvent;
import isabel.gw.IsabelClient;
import isabel.gw.IsabelState;
import isabel.gw.IsabelStateListener;
import isabel.lib.StringParser;
import isabel.seco.dispatcher.tclser.TclDispatcher;
import isabel.seco.dispatcher.tclser.TclDispatcherListener;
import isabel.seco.network.Network;
import isabel.seco.network.tclser.TclMessage;

import java.io.IOException;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.logging.Logger;

public class ComponentManager implements TclDispatcherListener {

    /**
     * Logs
     */
    private Logger mylogger;
    
    /**
     * Attributes
     */
    private IsabelStateImpl is;
    

    public ComponentManager(IsabelStateImpl is) {
	
	mylogger = Logger.getLogger("isabel.gw.isabel_client.isabel_state.ComponentManager");
	mylogger.info("Creating IsabelState::ComponentManager object.");
	
	this.is = is;

	// Apuntarse al grupo "components"
	try {
	    getNetwork().joinGroup("components");
	} catch (IOException e) {
	    mylogger.severe(e.getMessage());
	    System.exit(1);
	}
	
	getDispatcher().addDestiny("CM", this);
    }
    
    /**
     * Return the Network object.
     * 
     * @ return The Network object.
     */
    Network getNetwork() {
	
	return is.getNetworkMgr().getNetwork();
    }
    
    /**
     * Return the TclDispatcher object.
     * 
     * Note: This method was created in order to use EasyMock.
     * 
     * @ return The TclDispatcher object.
     */
    TclDispatcher getDispatcher() {
	
	return is.getNetworkMgr().getDispatcher();
    }
    
    
    /**
     * Procesa un mensaje.
     * 
     * @param msg
     *            Mensaje a procesar.
     */
    public void processMessage(TclMessage msg) {
	
	mylogger.info("IsabelState::ComponentManager has received->" + msg);
	
	String cmd = msg.cmd;
	
	try {
	    StringParser sp = new StringParser(cmd);
	    
	    String type = sp.nextToken();
	    
	    if (type.equals("CM")) {
		
		String method = sp.nextToken();
		
		if (method.equals("InteractionModeNotify")) {

		    // Notifies to the listeners:
		    sendListeners_InteractionModeChange_Event(cmd);

		} 
		
	    }
	    
	} catch (ParseException pe) {
	    mylogger.severe("ComponentManager: Internal error processing \""
			    + cmd + "\". I get: (" + pe.getMessage() + ").");
	}
    }
    


    /**
     * Send a SessionEvent message to all the listeners informing about the interaction mode change.
     * 
     * @param imode Description of the applied interaction mode.
     */
    private void sendListeners_InteractionModeChange_Event(String imode) {
	
	SessionEvent se = new SessionEvent(imode,new java.util.Date());
	
	for (IsabelStateListener l : IsabelState.getIsabelState().getListeners()) {
	    try {
		l.isabelChange(se);
	    } catch (Exception e) {
		mylogger.severe("IsabelState::ComponentManager: "+
				"An IsabelStateListener has raised an exception during a call to isabelChange. "+
				"The raised exception was: \"" + e.getMessage() + "\". "+
				"The SeCo message being processed was \""+imode+"\".");
	}
	}
    }
    
    //--------------------------

}