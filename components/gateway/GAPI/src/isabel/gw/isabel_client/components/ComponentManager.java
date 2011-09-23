
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

import isabel.gw.FlowInfo;
import isabel.gw.IsabelClient;
import isabel.gw.IsabelState;
import isabel.gw.IsabelStateListener;
import isabel.gw.SessionEvent;
import isabel.gw.flows.ComponentFlowTypes;
import isabel.gw.flows.FlowTypePayloads;
import isabel.gw.isabel_client.IsabelClientImpl;
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
     * ActiveFlowsRefresher object used to generate the active flows event.
     */
    private ActiveFlowsRefresher afr;
    
    /**
     * Attributes
     */
    private IsabelClientImpl ic;
    
    /**
     * Los objectos que representan los componentes se almacenan aqui, e
     * indexados por su nombre.
     */
    private HashMap<String, Component> components;
    

    public ComponentManager(IsabelClientImpl ic) {
	
	mylogger = Logger.getLogger("isabel.gw.isabel_client.components.ComponentManager");
	mylogger.info("Creating ComponentManager object.");
	
	this.ic = ic;
	
	components = new HashMap<String, Component>();
	
	// Create the ActiveFlowsRefresher object.
	this.afr = new ActiveFlowsRefresher(ic);
	
	createComponents();
    }
    
    /**
     * Create components.
     * 
     * This code was done inside the constructor, but it was extracted to this
     * method in order to test with EasyMock.
     */
    public void createComponents() {
	
	AudioComponent ac = null;
	VideoComponent vc = null;
	QuestionsComponent qc = null;
	ClassCtrlComponent classc = null;
	ConferenceCtrlComponent confc = null;

	if (isabelComponentSupported("Video")) {
	    vc = new VideoComponent(ic, afr);
	    components.put(vc.getName(), vc);
	}
	if (isabelComponentSupported("Audio")) {
	    ac = new AudioComponent(ic, afr);
	    components.put(ac.getName(), ac);
	}
	if (isabelComponentSupported("Questions")) {
	    qc = new QuestionsComponent(ic, ac, vc, afr);
	    components.put(qc.getName(), qc);
	}
	if (isabelComponentSupported("ClassCtrl")) {
	    classc = new ClassCtrlComponent(ic);
	    components.put(classc.getName(), classc);
	}
	if (isabelComponentSupported("ConferenceCtrl")) {
	    confc = new ConferenceCtrlComponent(ic);
	    components.put(confc.getName(), confc);
	}
	
	// Apuntarse al grupo "components"
	try {
	    getNetwork().joinGroup("components");
	} catch (IOException e) {
	    mylogger.severe(e.getMessage());
	    System.exit(1);
	}
	
	getDispatcher().addDestiny("CM", this);
	getDispatcher().addDestiny("COMP", this);
	
	mylogger.info("ComponentManager object initialized.");
    }


    /**
     * Return the Network object.
     * 
     * @ return The Network object.
     */
    Network getNetwork() {
	
	return ic.getNetworkMgr().getNetwork();
    }
    
    /**
     * Return the TclDispatcher object.
     * 
     * Note: This method was created in order to use EasyMock.
     * 
     * @ return The TclDispatcher object.
     */
    TclDispatcher getDispatcher() {
	
	return ic.getNetworkMgr().getDispatcher();
    }
    
    /**
     * Solicita al master la creacion de todos los componentes habilitados. Este
     * metodo lo invoca el SiteManager despues de haber logrado conectarse con
     * el master.
     */
    public void startSupportedComponents() {
	
	Set<String> ckeys = components.keySet();
	for (String key : ckeys) {
	    Component c = components.get(key);
	    try {
		getNetwork().sendGroup(
				       "isabel_master",
				       new TclMessage("CM StartComponent " + ic.getMySiteId() + " "
						      + c.getName()));
	    } catch (IOException ioe) {
		mylogger
		    .severe("I can't send StartComponent message to master. I get: "
			    + ioe.getMessage());
	    }
	}
    }
    
    
    /**
     * Cleanup.
     * 
     * Called when the IsabelClient leaves the session, because some resources need to
     * be freed
     */
    public void cleanup() {
	afr.kill();
    }
    

    /**
     * Procesa un mensaje.
     * 
     * @param msg
     *            Mensaje a procesar.
     */
    public void processMessage(TclMessage msg) {
	
	mylogger.info("IsabelClient::ComponentManager has received->" + msg);
	
	String cmd = msg.cmd;
	
	try {
	    StringParser sp = new StringParser(cmd);
	    
	    String type = sp.nextToken();
	    
	    if (type.equals("CM")) {
		
		String method = sp.nextToken();
		
		if (method.equals("InteractionModeNotify")) {
		    
		    // Apply the mode:
		    processInteractionModeNotifyMsg(sp);
		    
		}
		
	    } else if (type.equals("COMP")) {
		
		String compName = sp.nextToken();
		
		try {
		    Component c = components.get(compName);
		    if (c != null) {
			if (c.isRunning())
			    c.processMessage(sp);
		    }
		} catch (Exception e) {
		    mylogger.severe("Error proccesing the SeCo message: \""
				    + cmd + "\". Produces: " + e.getMessage());
		}
		
	    }
	    
	} catch (ParseException pe) {
	    mylogger.severe("ComponentManager: Internal error processing \""
			    + cmd + "\". I get: (" + pe.getMessage() + ").");
	}
    }
    





    /** 
     *  Update the component states of the given site.
     */
    public void updateComponentStates(String site_id, String states) {

	// Set with all the components defined in the service:
	Set<String> all_components = new HashSet<String>();
	try {
	    StringParser p1 = new StringParser(IsabelState.getIsabelState().getService().getComponents());
	    for (int i = 0; i < p1.size(); i++) {
		all_components.add(p1.tokenAt(i));
	    }
	} catch (Exception e) {
	    mylogger.severe("Internal error updating component states: "
			    + e.getMessage());
	}



	try {
	    StringParser allsp = new StringParser(states);
	    
	    while (allsp.hasMoreTokens()) {
		String compstate = allsp.nextToken();
		
		StringParser sp = new StringParser(compstate);
		
		String name = sp.nextToken();
		String state = sp.nextToken();
		// ignoring first and instance values.
		
		all_components.remove(name);
		
		Component c = components.get(name);
		if (c == null) continue;
		
		if (state.equals("Stopped")) {
		    if (c.isRunning() || c.isWaiting()) {
			componentIsStopped(name, site_id);
		    }
		} else if (state.equals("Waiting")) {
		    if (c.isStopped()) {
			componentIsWaiting(name, site_id);
		    } else if (c.isRunning()) {
			componentIsStopped(name, site_id);
			componentIsWaiting(name, site_id);
		    }
		} else if (state.equals("Running")) {
		    if (c.isStopped()) {
			componentIsRunning(name, site_id); 
		    } else if (c.isWaiting()) {
			componentIsRunning(name, site_id); 
		    }		    
		} else {
		    // Ignoring other cases.
		}
	    }
	} catch (Exception e) {
	    mylogger.severe("Protocol error updating component states: "
			    + e.getMessage());
	}

	// Stop components no specified in the method argument.
	for (String name : all_components) {
	    Component c = components.get(name);
	    if (c == null) continue;
	    if (c.isRunning() || c.isWaiting()) {
		componentIsStopped(name, site_id);
	    }
	}
    }
    

    /**
     * Process a StartComponentNotify message.
     * 
     * @param msg
     *            Message to process.
     */
    private void componentIsWaiting(String compName, String site_id) {
	
	String mysite_id = ic.getMySiteId();
	
	if ( ! site_id.equals(mysite_id) ) return;
	
	// El master me solicita que lance un componente en mi sitio
	
	Component c = components.get(compName);
	if (c != null) {
	    
	    c.start();
	    
	    try {
		getNetwork().sendGroup("isabel_master",
				       new TclMessage("CM StartComponentSuccess " +  mysite_id
						      + " " + compName));
	    } catch (IOException ioe) {
		mylogger.severe("I can't send StartComponentSuccess message to master. I get: "
			    + ioe.getMessage());
		try {
		    getNetwork().sendGroup("isabel_master",
					   new TclMessage("CM KillComponent " + mysite_id + " "
							  + compName));
		} catch (IOException ioe2) {
		    mylogger.severe("I can't send KillComponent message to master. I get: "
				    + ioe2.getMessage());
		}
	    }
	    return;
	}
	
	mylogger.fine("Ignoring StartComponentNotify for the unsupported or not allowed \""
		  + compName + "\" component.");
	try {
	    getNetwork().sendGroup("isabel_master",
				   new TclMessage("CM KillComponent " + mysite_id + " "
						  + compName));
	} catch (IOException ioe) {
	    mylogger.severe("I can't send KillComponent message to master. I get: "
			    + ioe.getMessage());
	}
	
    }
    
    /**
     * Process a StartComponentSuccessNotify message.
     * 
     * @param msg
     *            Message to process.
     */
    private void componentIsRunning(String compName, String site_id) {
	// 
	// Avisan de que un sitio (yo o otro) ya ha arrancado
	// satisfactoriamente un componente
	//

	Component c = components.get(compName);
	if (c != null) {
	    
	    String mysite_id = ic.getMySiteId();
	
	    if ( site_id.equals(mysite_id) ) {
		c.addMe();
	    } else {
		c.addRemoteSite(site_id);
	    }
	}
    }
    
    
    /**
     * The component is stopped.
     * 
     */
    private void componentIsStopped(String compName, String site_id) {
	
	//
	// El master informa sobre que hay que matar un componente.
	//

	Component c = components.get(compName);
	
	if (c != null) {

	    String mysite_id = ic.getMySiteId();

	    if ( site_id.equals(mysite_id) ) {
		// si soy yo, indico que no estoy ejecutando el componente.
		c.stop();
	    } else {
		c.delRemoteSite(site_id);
	    }
	} else {
	    mylogger.severe("PROTOCOL ERROR: Master request to stop a not enabled component.");
	}
	
    }
    
    /**
     * Process a InteractionModeNotify message.
     * 
     * @param msg
     *            Message to process.
     */
    private void processInteractionModeNotifyMsg(StringParser sp)
	throws ParseException {
	
	// Interaction mode name.
	String imName = sp.nextToken();
	mylogger.info("imode = " + imName);
	
	// Interaction mode sequence number.
	int nsec = Integer.parseInt(sp.nextToken());
	mylogger.info("nsec = " + nsec);
	
	// site_id of the selected sites.
	String sites = sp.nextToken();
	List<String> site_id_list;
	if ("*".equals(sites)) {
	    site_id_list = new ArrayList<String>(1);
	    site_id_list.add(sites);
	    mylogger.info("sites = *");
	} else if ("".equals(sites)) {
	    site_id_list = new ArrayList<String>(0);
	    mylogger.info("sites = \"\"");
	} else {
	    StringParser p1 = new StringParser(sites);
	    site_id_list = new ArrayList<String>(p1.size());
	    for (int i = 0; i < p1.size(); i++) {
		site_id_list.add(p1.tokenAt(i));
	    }
	    mylogger.info("sites = " + site_id_list);
	}
	
	// Map with the options configuring the interaction mode.
	HashMap<String, String> opts = new HashMap<String, String>();
	while (sp.hasMoreTokens()) {
	    opts.put(sp.nextToken(), sp.nextToken());
	}
	mylogger.info("options = " + opts);
	
	Set<String> names = components.keySet();
	for (String name : names) {
	    Component c = components.get(name);
	    try {
		if (c.isRunning())
		    c.setInteractionMode(imName, nsec, site_id_list, opts);
	    } catch (Exception e) {
		mylogger.severe("Error applying interaction mode \"" + imName
				+ "\" to " + c.getName() + " component: "
				+ e.getMessage());
	    }
	}
    }


    //--------------------------

    /**
     * Determine if the given components can be used by this client.
     * 
     * The component can be used:
     *   - if it is allowed for the client in the Service definition.
     *   - If the flows supported by the client and the component can match.
     *
     * @param component
     *            The component to determine if can be used by the site.
     * @param flows
     *            The flows supported by the client.
     * @return true if the component can be used by the isabel client site.
     */
    private boolean isabelComponentSupported(String compName) {

	ComponentFlowTypes.IsabelComponent component = ComponentFlowTypes.IsabelComponent.valueOf(compName);
	
	if ( ! IsabelState.getIsabelState().getService().getComponents().matches(".*\\b"+compName+"\\b.*") ) {
	    mylogger.info("Service does not support component \""+compName+"\".");
	    return false;
	}

	Set<ComponentFlowTypes.FlowType> fts = ComponentFlowTypes.getFlowTypes(component);
	
	// No flows are required.
	if (fts.isEmpty()) {
	    mylogger.info("Component \""+compName+"\" is supported because it does not need flows.");
	    return true;
	}

	List<FlowInfo> flows = ic.getMyFlows();

	for (ComponentFlowTypes.FlowType ft : fts) {
	    for (FlowInfo f : flows) {
		if (FlowTypePayloads.exists(ft, f.getPT())) {
		    mylogger.info("Component \""+compName+"\" supported because it matches client flows.");
		    return true;
		}
	    }
	}
	mylogger.info("Component \""+compName+"\" no supported because it does not match client flows.");
	return false;
    }

    //--------------------------

    /**
     *  getComponent is ussed to access to the specified component object. 
     *
     * @param name Name of the coomponent to access.
     * @return A reference to object implementing the specified component. Or null if the object does not exist.
     */
    public Component getComponent(String name) {

	return components.get(name);
    }
  

}
