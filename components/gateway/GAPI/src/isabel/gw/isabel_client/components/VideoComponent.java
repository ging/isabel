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

import isabel.gw.IsabelState;
import isabel.gw.flows.ComponentFlowTypes.FlowType;
import isabel.gw.isabel_client.IsabelClientImpl;
import isabel.gw.isabel_client.SmsManagerListener;
import isabel.lib.StringParser;

import java.util.List;
import java.util.Map;
import java.util.logging.Logger;

/**
 * Esta clase representa el componente de video. De momento sólo tiene en cuenta
 * si el video está o no activo
 */
class VideoComponent extends Component implements SmsManagerListener {
    
    /**
     * Logs
     */
    private Logger mylogger;
    
    /**
     * Reference to my IsabelClient.
     */
    private IsabelClientImpl ic;
    
    /**
     * Thread to send periodically an active flows event.
     */
    private ActiveFlowsRefresher afr;
    
    // ----------------------------------------
    
    /**
     * Construye el objeto encargado de atender el estado y los mensajes del
     * video.
     */
    VideoComponent(IsabelClientImpl ic, ActiveFlowsRefresher afr) {
	super("Video");
	
	mylogger = Logger.getLogger("isabel.gw.isabel_client.components.VideoComponent");
	mylogger.info("Creating VideoComponent object.");
	
	this.ic = ic;
	this.afr = afr;
    }
    
    /**
     * Se llama para indicar que estoy ejecutando el componente.
     */
    @Override void start() {
	
	mylogger.info("Start my Video component.");
	super.start();
	
	//	updateListeners();
    }
    
    /**
     * Se llama para indicar que estoy no ejecutando el componente.
     */
    @Override void stop() {
	
	mylogger.info("Stop my Video component.");
	super.stop();

	// Limpiando SmsManager:
	ic.getSmsMgr().removeSmsManagerListener("Video_Cameras", this);
	ic.getSmsMgr().leaveKey("Video_Cameras");

	updateListeners();
    }
    
    /**
     * He recibido del master el mensaje de notificacion de mi creacion
     * satisfactoria, me uno a la ejecucion del componente.
     */
    @Override void addMe() {
	
	mylogger.info("Joining me to the Video component.");
	super.addMe();

	// Abro el estado video_Cameras para saber si mi sitio debe tener el video visible o no.
	ic.getSmsMgr().addSmsManagerListener("Video_Cameras", this);
	ic.getSmsMgr().joinKey("Video_Cameras");
    }
    
    /**
     * Set the visible status of the component. When the component is running, it
     * can be visible or not.
     * 
     * @param visible
     *            true if the component is visible.
     */
    @Override void setVisible(boolean visible) {
	
	mylogger.info("My Video component - Visible = " + visible);
	super.setVisible(visible);
	updateListeners();
    }



    /**
     * Procesa un mensaje de tipo COMP.
     * 
     * @param sp
     *            StringParser con el mensaje a procesar.
     */
    void processMessage(StringParser sp) {

    }


    /**
     * Procesa un estado SMS.
     * @param key Clave del estado.
     * @param data lista de opciones y valores del estado.
     */
    public void processSmsMessage(String key, String data) {
	
	mylogger.info("Processing SMS Message: "+key+" => "+data);

	if (key != null && key.equals("Video_Cameras")) {
	    
	    StringParser datasp;
	    try {
		datasp = new StringParser(data);
	    } catch (Exception exc) {
		setVisible(false);
		return;
	    }
	    
	    String siteIdList = datasp.getOptionValue("-sites","");
	    boolean mapOthers = datasp.getOptionValue("-map_others","0").equals("1");
	    
	    if (mapOthers) {
		mylogger.fine("Other videos mapped.");
		setVisible(true);
		return;
	    }

	    if ("*".equals(siteIdList)) {
		mylogger.fine("All the videos must be visible (SMS).");
		setVisible(true);
	    } else if ("".equals(siteIdList)) {
		mylogger.fine("No videos must be visible (SMS).");
		setVisible(false);
	    } else {
		try {
		    StringParser p1 = new StringParser(siteIdList);
		    String mySiteId = "" + ic.getMySiteId();
		    boolean act = p1.indexOf(mySiteId) != -1;
		    mylogger.fine("Mi video must be activated (SMS) = " + act);
		    setVisible(act);
		} catch (Exception e) {
		    mylogger.severe("Unexpected exception is Video_Cameras SMS message: "
				    + e.getMessage());
		    setVisible(false);
		}
	    }
	}
    }
    
    
    /**
     * Applies a interaction mode configuration.
     * 
     * @param imName
     *            Interaction mode name.
     * @param nsec
     *            Interaction mode secuence number.
     * @param site_id_list
     *            - site_id of the selected sites.
     *            - "*" to select all sites.
     * @param options
     *            List of option-value pairs (ex: -opt1 v1 -opt v2 ...)
     *            configuring the interaction mode.
     */
    @Override void setInteractionMode(String imName, int nsec,
			    List<String> site_id_list, 
			    Map<String, String> options) {
	
    }    
    
    /**
     * Actualizar el objeto IsabelClientListener con el estado de los videos.
     */
    private void updateListeners() {
	
	if (getVisible()) {
	    afr.update(FlowType.VIDEO_IMAGE);
	} else {
	    afr.delete(FlowType.VIDEO_IMAGE);
	}
    }

}
