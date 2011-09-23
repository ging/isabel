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
import isabel.gw.isabel_client.IsabelClientImpl;
import isabel.gw.isabel_client.SmsManager;
import isabel.gw.isabel_client.SmsManagerListener;
import isabel.lib.StringParser;

import java.util.List;
import java.util.Map;
import java.util.logging.Logger;

/**
 * Esta clase representa el componente de questions. De momento sólo tiene en
 * cuenta si el questions está o no activo
 * 
 * @author Isabel Bau
 * @version 1.0
 */

public class QuestionsComponent extends Component implements SmsManagerListener {
    
    /**
     * Logs
     */
    private Logger mylogger;
    
    /**
     * Reference to my IsabelClient.
     */
    private IsabelClientImpl ic;
    
    /**
     * Reference to the Audio component.
     */
    private Component ac;
    
    /**
     * Reference to the Video component.
     */
    private Component vc;
    
    /*****************************************************************
     * Mi estado:
     *****************************************************************/
    
    /**
     * Indica si mi ventana de video esta mapeada en modo reset.
     */
    private boolean mappedInReset;
    
    /**
     * Indica si los otros videos deben mostrarse en el modo reset
     */
    private boolean resetMapOthers;
    
    /**
     * Indica si mi ventana de video esta mapeada durante las preguntas.
     */
    private boolean mappedDuringQuestions;
    
    /**
     * Indica si los otros videos deben mostrarse durane las preguntas.
     */
    private boolean mapOthers;
    
    /**
     * Indica si soy yo el sitio que pregunta.
     */
    private boolean iAmTheQuestioner;
    
    /**
     * Indica si debemos operar los canales de audio.
     */
    private boolean setAudios;
    
    // ----------------------------------------
    
    /**
     *  This attribute is true when I am requesting a question.
     */
    private boolean requesting;
    
    // ----------------------------------------
    
    /**
     * Construye el objeto encargado de atender el estado y los mensajes del
     * questions.
     * 
     * @param ic
     *            Referencia a mi IsabelClient.
     * @param ac
     *            Referencia al componente de Audio.
     * @param vc
     *            Referencia al componente de Video.
     */
    QuestionsComponent(IsabelClientImpl ic, Component ac, Component vc,
		       ActiveFlowsRefresher afr) {
	super("Questions");
	
	mylogger = Logger.getLogger("isabel.gw.isabel_client.components.QuestionsComponent");
	mylogger.info("Creating QuestionsComponent object.");
	
	this.ic = ic;
	this.ac = ac;
	this.vc = vc;
    }
    
    
    /**
     * Configure audio and video to the "making questions" state.
     */
    private void setAVLayout() {

	/*	
	// Activate videos:
	if (vc != null) {
	    
	    mylogger.info("mappedDuringQuestions = " + mappedDuringQuestions);
	    mylogger.info("mapOthers = " + mapOthers);
	    mylogger.info("iAmTheQuestioner = " + iAmTheQuestioner);
	    
	    vc.setVisible(mappedDuringQuestions || mapOthers
			  || iAmTheQuestioner);
	}
	*/

	// Update my audio state:
	if (setAudios) {
	    String mysite_id = ic.getMySiteId();
	    
	    String _capturing;
	    if (mappedDuringQuestions || iAmTheQuestioner) {
		_capturing = "1";
	    } else {
		_capturing = "0";
		
	    }

	    ic.getSmsMgr().setState("Audio_site_"+mysite_id,
				    SmsManager.SMS_OPERATION.SET,
				    "-capture "+_capturing+" -site_id "+mysite_id);
	    
	}
    }
    /**
     * Configure audio and video to the "reset or initial" state.
     */
    private void setAResetLayout() {

	/*	
	// Activate videos:
	if (vc != null) {
	    vc.setVisible(mappedInReset || resetMapOthers);
	}
	*/

	// Update my audio state:
	if (setAudios) {
	    String mysite_id = ic.getMySiteId();
	    
	    String _capturing;
	    if (mappedInReset) {
		_capturing = "1";
	    } else {
		_capturing = "0";
		
	    }
	    
	    ic.getSmsMgr().setState("Audio_site_"+mysite_id,
				    SmsManager.SMS_OPERATION.SET,
				    "-capture "+_capturing+" -site_id "+mysite_id);
	    
	}

    }
    

    /**
     * Procesa un mensaje de tipo COMP.
     * 
     * @param sp
     *            StringParser con el mensaje a procesar.
     */
    void processMessage(StringParser sp) {

	String prefix = sp.nextToken();
	
	if (prefix.equals("Cmd")) {
	    
	    String method = sp.nextToken();
	    
	    if (method.equals("AAAA")) {
		// ????
	    } else if (method.equals("BBBBB")) {
		// ????
	    } else {
		mylogger.severe("Message not supported yet: \"" + method + "\"");
	    }
	} else {
	    mylogger.severe("Prefix is invalid: \"" + prefix + "\". Must be \"Cmd\".");
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
	
    }
    
    /**
     * Se llama para indicar que estoy ejecutando el componente.
     */
    void start() {
	
	mylogger.info("Start my Questions component.");
	
	super.start();
	
	mappedInReset = false;
	resetMapOthers = false;
	mappedDuringQuestions = false;
	mapOthers = false;
	iAmTheQuestioner = false;
	setAudios = false;
    }
    
    /**
     * Se llama para indicar que estoy no ejecutando el componente.
     */
    void stop() {
	
	mylogger.info("Stop my Questions component.");
	super.stop();
	
	
	// Limpiando SmsManager:
	String mysite_id = ic.getMySiteId();
	ic.getSmsMgr().removeSmsManagerListener("Questions_site_"+mysite_id, this);
	ic.getSmsMgr().leaveKey("Questions_site_"+mysite_id);
	
    }
    
    /**
     * He recibido del master el mensaje de notificacion de mi creacion
     * satisfactoria, me uno a la ejecucion del componente.
     */
    void addMe() {
	
	mylogger.info("Joining me to the Questions component.");
	super.addMe();
	
	//int myChId = ic.getSiteMgr().getMyChId();
	String mysite_id = ic.getMySiteId();
	
	// Necesito recibir el estado global de las questions:
	ic.getSmsMgr().addSmsManagerListener("Questions", this);
	ic.getSmsMgr().joinKey("Questions");
	
	// Abro mi estado. Es necesario para saber si estoy solicitando pregunta o no.
	ic.getSmsMgr().addSmsManagerListener("Questions_site_"+mysite_id, this);
	ic.getSmsMgr().joinKey("Questions_site_"+mysite_id);

	// Audio: El estado de mi canal.
	// Solo llamo a joinKey (no llamo a addSmsManagerListener) para que se actuallicen los contadores SMS.
	/* 
	   ic.getSmsMgr().addSmsManagerListener("Audio_site_"+mysite_id, this);
	*/
	ic.getSmsMgr().joinKey("Audio_site_"+mysite_id);
	
    }
    
    //-----------------------------------
    
    
    
    /**
     * Procesa un estado SMS.
     * @param key Clave del estado.
     * @param data lista de opciones y valores del estado.
     */
    public void processSmsMessage(String key, String data) {
	
	mylogger.info("Processing SMS Message: "+key+" => "+data);
	
	String mysite_id = ic.getMySiteId();
	
	if (key == null) return;
	
	if (key.equals("Questions")) {
	    processQuestionsSmsMessage(data);
	} else if (key.equals("Questions_site_"+mysite_id)) {
	    processQuestionsSiteSmsMessage(data);
	}
    }
    
    
    /**
     * Procesa mensajes SMS con clave Questions_site_MYSITEID.
     */	
    private void processQuestionsSiteSmsMessage(String data) {
	
	StringParser datasp;
	try {
	    datasp = new StringParser(data);
	} catch (Exception exc) {
	    mylogger.severe("I have received a bad formatted SMS message with the key \"Questions_site_<mysiteid>\": " +
			    data);
	    return;
	}
	
	// Procesar opcion -request
	requesting = ! datasp.getOptionValue("-request", "0").equals("0");	
    }
    
    
    /**
     * Procesa mensajes SMS con clave Questions.
     */	
    private void processQuestionsSmsMessage(String data) {

	//mylogger.fine("Received Questions SMS message: "+data);
	
	String mysite_id = ic.getMySiteId();
	
	StringParser datasp;
	try {
	    datasp = new StringParser(data);
	} catch (Exception exc) {
	    mylogger.severe("I have received a bad formatted SMS message with the key \"Questions\": " +
			    data);
	    return;
	}
	
	// Si -active es cero no hago nada:
	if (datasp.getOptionValue("-active", "0").equals("0")) {
	    mylogger.fine("No active");
	    return;
	}
	
	
	// Ver si hay que mapear el otros videos mientras se esta preguntando:
	String otherMapOpt = datasp.getOptionValue("-video_other_map", "");
	if (otherMapOpt.equals("")) {
	    otherMapOpt = IsabelState.getIsabelState().getService().getProperty("Video",
										"othermap", "1");
	}
	mapOthers = otherMapOpt.equals("1");
	mylogger.fine("Map other sites = " + mapOthers);
	
	
	// Ver si hay que mapear el otros videos en el modo reset:
	String resetOtherMapOpt = datasp.getOptionValue("-video_reset_other_map",
							otherMapOpt);
	resetMapOthers = resetOtherMapOpt.equals("1");
	mylogger.fine("Map other sites in reset = " + resetMapOthers);
	
	
	// Si soy un speaker mapeare mi video durante las preguntas:
	mappedDuringQuestions = false;
	try {
	    String _ids = datasp.getOptionValue("-speakers","");
	    if (_ids.equals("*")) {
		mappedDuringQuestions = true;
	    } else { 
		mappedDuringQuestions = new StringParser(_ids).indexOf(mysite_id) != -1;
	    }
	} catch (Exception e) {
	    mylogger.severe("Unexpected error proccesing -speakers option: " + e.getMessage());
	}
	mylogger.fine("Mapped during questions = " + mappedDuringQuestions);
	
	
	// Calcular si mi video aparece en el modo reset:
	mappedInReset = false;
	try {
	    String _ids = datasp.getOptionValue("-reset_sites","");
	    if (_ids.equals("*")) {
		mappedInReset = true;
	    } else { 
		mappedInReset = new StringParser(_ids).indexOf(mysite_id) != -1;
	    }
	} catch (Exception e) {
	    mylogger.severe("Unexpected error proccesing -reset_sites option: " + e.getMessage());
	}
	mylogger.fine("Mapped in reset (1) = " + mappedInReset);
	
	
	
	// Si soy el chairman, mi video tambien debe aparecer en el modo reset:
	if (!mappedInReset) {
	    try {
		String _chair = datasp.getOptionValue("-chair","");
		if (_chair.equals("")) {
		    String _ctrls = datasp.getOptionValue("-controllers","");
		    mappedInReset = new StringParser(_ctrls).indexOf(mysite_id) == 0;
		} else {
		    mappedInReset = new StringParser(_chair).indexOf(mysite_id) != -1;
		}
	    } catch (Exception e) {
		mylogger.severe("Unexpected error processing -chair or -controllers option: "
				+ e.getMessage());
	    }
	    mylogger.fine("Mapped in reset (2) = " + mappedInReset);
	}
	
	
	// Calcular si debo operar los canales de audio:
	setAudios = datasp.getOptionValue("-audio_set", "1").equals("1");
	mylogger.fine("SetAudios = " + setAudios);
	
	
	// Who is the questioner:
	iAmTheQuestioner = datasp.getOptionValue("-question_from", "").equals(mysite_id);
	
	// Reset or no-reset mode:
	String _reset = datasp.getOptionValue("-reset_mode", "1");
	
	if (_reset.equals("1")) {
	    setAResetLayout();
	} else {
	    setAVLayout();
	}
	
    }
    
    //-----------------------------------
    
    /**
     *  Send a ToggleSite request to the isabel session.
     *
     * @param site_id Site to toggle its question request indicator.
     */
    public void toggleSite(String site_id) {
	
	String mysite_id = ic.getMySiteId();
	ic.getSmsMgr().forceState("Questions_site_"+mysite_id,
				  SmsManager.SMS_OPERATION.ADD,
				  "-site_id "+mysite_id+" -request "+(requesting?"0":"1"));
    }
}
