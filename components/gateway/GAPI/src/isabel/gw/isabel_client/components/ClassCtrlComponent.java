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

package isabel.gw.isabel_client.components;

import isabel.gw.isabel_client.IsabelClientImpl;
import isabel.lib.StringParser;

import java.util.List;
import java.util.Map;
import java.util.logging.Logger;

/**
 * Esta clase representa el componente de ClassCtrl.
 *
 */
public class ClassCtrlComponent extends Component {

    
    /**
     *  Logs
     */
    private Logger mylogger;


    /**
     *  Reference to my IsabelClient.
     */
    private IsabelClientImpl ic;

    
    //----------------------------------------

    /**
     *  Flag indicando si ya he recibido el mensaje SYNC
     */
    private boolean syncReceived;


    /**
     *  Construye el objeto encargado de atender el estado y los
     *  mensajes del componente ClassCtrl.
     */
    public ClassCtrlComponent(IsabelClientImpl ic){  
	super("ClassCtrl");
	
    	mylogger = Logger.getLogger("isabel.gw.isabel_client.components.ClassCtrlComponent");
    	mylogger.info("Creating ClassCtrlComponent object.");
    
	this.ic = ic;
    }
    
    
    /**
     *  Synchronization message
     *  @param msg StringParser con el mensaje SYNC a procesar.
     */
    private void sync(StringParser msg) {
	
    }


    /**
     * Procesa un mensaje de tipo COMP.
     * 
     * @param sp StringParser con el mensaje a procesar.
     */
    public void processMessage(StringParser sp) {

	String method = sp.nextToken();
	
	if (method.equals("Sync")) {
	    sync(sp);
	} else if (method.equals("Cmd")) {
	    // Nothing to do
	} else {
	    mylogger.severe("Message not supported yet: \"" + method + "\"");
	}    
    }
    

    /** 
     *  Applies a interaction mode configuration.
     *  @param imName   Interaction mode name.
     *  @param nsec     Interaction mode secuence number.
     * @param site_id_list
     *            - site_id of the selected sites.
     *            - "*" to select all sites.
     *  @param options  List of option-value pairs (ex: -opt1 v1 -opt v2 ...) configuring
     *                  the interaction mode.
     */
@Override
    public void setInteractionMode(String imName, int nsec, List<String> site_id_list, Map<String,String> options) {

    }


    /**
     *  Se llama para indicar que estoy ejecutando el componente.
     */
    public void start() {
	
	mylogger.info("Start my ClassCtrl component.");
	super.start();

	syncReceived = false;
    }
    

    /**
     *  Se llama para indicar que estoy no ejecutando el componente.
     */
    public void stop(){
	
	mylogger.info("Stop my ClassCtrl component.");
	super.stop();
    }
    

    /**
     *  He recibido del master el mensaje de notificacion de mi creacion satisfactoria,
     *  me uno a la ejecucion del componente.
     */
    public void addMe() {

	mylogger.info("Joining me to the ClassCtrl component.");
	super.addMe();
    }


}
