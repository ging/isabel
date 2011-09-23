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
import isabel.gw.IsabelStateListener;

import isabel.gw.flows.ComponentFlowTypes.FlowType;
import isabel.gw.isabel_client.IsabelClientImpl;
import isabel.gw.isabel_client.SmsManager;
import isabel.gw.isabel_client.SmsManagerListener;
import isabel.lib.StringParser;

import java.util.List;
import java.util.Map;
import java.util.logging.Logger;

/**
 * Esta clase representa el componente de audio. De momento sólo tiene en cuenta
 * si el audio está o no activo
 */

class AudioComponent extends Component implements SmsManagerListener {
    
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
     * audio.
     */
    AudioComponent(IsabelClientImpl ic, ActiveFlowsRefresher afr) {
	super("Audio");
	
	mylogger = Logger.getLogger("isabel.gw.isabel_client.components.AudioComponent");
	mylogger.info("Creating AudioComponent object.");
	
	this.ic = ic;
	this.afr = afr;
    }
    
    /**
     * Se llama para indicar que estoy ejecutando el componente.
     */
    @Override void start() {
	
	mylogger.info("Start my Audio component.");
	super.start();
	
    }
    
    /**
     * Se llama para indicar que estoy no ejecutando el componente.
     */
    @Override void stop() {
	
	mylogger.info("Stop my Audio component.");
	super.stop();

	// Limpiando SmsManager:
	String mysite_id = ic.getMySiteId();
	ic.getSmsMgr().removeSmsManagerListener("Audio_site_"+mysite_id, this);
	ic.getSmsMgr().leaveKey("Audio_site_"+mysite_id);

	updateListeners();
    }
    
    /**
     * He recibido del master el mensaje de notificacion de mi creacion
     * satisfactoria, me uno a la ejecucion del componente.
     */
    @Override void addMe() {
	
	mylogger.info("Joining me to the Audio component.");
	super.addMe();
	
	//int myChId = ic.getSiteMgr().getMyChId();
	String mysite_id = ic.getMySiteId();
	
	// Abro mi estado. Es necesario porque si alguien me mutea debo enterarme 
	// para decirle a la MCU que no use mi audio.
	ic.getSmsMgr().addSmsManagerListener("Audio_site_"+mysite_id, this);
	ic.getSmsMgr().joinKey("Audio_site_"+mysite_id);

	// Sending my state:

	String myinitmute = IsabelState.getIsabelState().getService().getProperty(getName(), "myinitmute", "1");
	if (myinitmute.equals("0")) {
	    setVisible(true);
	} else {	
	    updateListeners();
	}

	sendState();
    }
    
    /**
     * Informo al componente de que el sitio indicado se ha unido a la ejecucion
     * del componente.
     * 
     * @param site_id
     *            Sitio que se ha unido a la ejecucion del componente.
     */
    @Override void addRemoteSite(String site_id) {
	
	super.addRemoteSite(site_id);
	
	// No hago nada. No me importa el estado de los sitios remotos ya que la 
	// MCU manda a los clientes de la pasarela el audio sumado de toda la sesion.
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

	String mysite_id = ic.getMySiteId();

	if (key != null && key.equals("Audio_site_"+mysite_id)) {
	    
	    StringParser datasp;
	    try {
		datasp = new StringParser(data);
	    } catch (Exception exc) {
		setVisible(false);
		return;
	    }
	    
	    // Capture or mute
	    boolean v = datasp.getOptionValue("-capture","0").equals("1");
	    setVisible(v);

	    // Input gain
	    String sgain = datasp.getOptionValue("-gain","");
	    if ( ! sgain.equals("")) {

		Integer gain;
		try {
		    gain = new Integer(sgain);
		} 
		catch (NumberFormatException nfe) {
		    gain = 0;
		}
		List<IsabelStateListener> ll = IsabelState.getIsabelState().getListeners();
		for (IsabelStateListener l : ll) {
		    l.parameterChange(ic.getMySiteId(), "Audio_InputGain", gain);
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
				      List<String> site_id_list, Map<String, String> options) {
	
	String imservice = IsabelState.getIsabelState().getService().getProperty(imName,
								       getName(), "");
	mylogger.finest("Service definition: " + imName + "." + getName()
			+ " = " + imservice);
	
	StringParser imssp;
	try {
	    imssp = new StringParser(imservice);
	} catch (Exception exc) {
	    setVisible(false);
	    sendState();
	    return;
	}
	
	// Si -active es cero entonces no estoy activo.
	if (imssp.getOptionValue("-active", "0").equals("0")) {
	    mylogger.fine("Audio not active in this interaction mode.");
	    setVisible(false);
	    sendState();
	    return;
	}
	
	// Si la opcion -openall esta a 1 entonces estoy activo.
	if (imssp.getOptionValue("-openall", "0").equals("1")) {
	    mylogger
		.fine("All the audios are actives in this interaction mode.");
	    setVisible(true);
	    sendState();
	    return;
	}
	
	// Si la opcion -closeall esta a 1 entonces no estoy activo.
	if (imssp.getOptionValue("-closeall", "0").equals("1")) {
	    mylogger.fine("No audios active in this interaction mode.");
	    setVisible(false);
	    sendState();
	    return;
	}
	
	// Si la opcion -setmutes es 0 entonces no hago nada.
	if (imssp.getOptionValue("-setmutes", "0").equals("0")) {
	    mylogger.fine("Skip audios in this interaction mode.");
	    return;
	}
	
	// ------
	
	if (options.containsKey("-audiosites")) {
	    
	    String audioSites = options.get("-audiosites");
	    
	    if ("*".equals(audioSites)) {
		mylogger
		    .fine("All the audios are actived in this interaction mode.");
		setVisible(true);
		sendState();
	    } else if ("".equals(audioSites)) {
		mylogger.fine("No audios active in this interaction mode.");
		setVisible(false);
		sendState();
	    } else {
		try {
		    StringParser p1 = new StringParser(audioSites);
		    String mysite_id = "" + ic.getMySiteId();
		    boolean act = p1.indexOf(mysite_id) != -1;
		    mylogger
			.fine("Mi audio (-audiosites) is selected in this interaction mode = "
			      + act);
		    setVisible(act);
		    sendState();
		} catch (Exception e) {
		    mylogger
			.severe("Malformed -audiosites option this interaction mode.");
		    setVisible(false);
		    sendState();
		}
	    }
	    
	} else {
	    
	    String mysite_id = "" + ic.getMySiteId();
	    boolean act = site_id_list.contains(mysite_id);
	    mylogger.fine("Mi audio is selected in this interaction mode = "
			  + act);
	    setVisible(act);
	    sendState();
	}
    }

    /**
     * Set the visible status of the component. When the component is running, it
     * can be visible or not.
     * 
     * @param visible
     *            true if the component is visible.
     */
    @Override void setVisible(boolean visible) {
	
	mylogger.info("My Audio component - Visible = " + visible);
	super.setVisible(visible);
	updateListeners();
    }


    /**
     * Send my state to the SMS system.
     * 
     */
    void sendState() {
	
	String mysite_id = ic.getMySiteId();
	ic.getSmsMgr().forceState("Audio_site_"+mysite_id,
				  SmsManager.SMS_OPERATION.ADD,
				  "-site_id "+mysite_id+" -capture "+(getVisible()?"1":"0"));
    }


    private void updateListeners() {
	
	if (getVisible()) {
	    afr.update(FlowType.AUDIO_SOUND);
	    //afr.update(FlowType.AUDIO_VUMETER);
	} else {
	    afr.delete(FlowType.AUDIO_SOUND);
	    //afr.delete(FlowType.AUDIO_VUMETER);
	}
    }

}
