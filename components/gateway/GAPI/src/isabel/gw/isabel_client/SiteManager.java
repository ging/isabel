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
package isabel.gw.isabel_client;

import isabel.gw.FlowInfo;
import isabel.gw.IsabelClient;
import isabel.gw.IsabelState;
import isabel.gw.IsabelClientEvent;
import isabel.gw.IsabelStateListener;
import isabel.gw.LocalMixer;
import isabel.gw.MemberInfo;
import isabel.lib.StringParser;
import isabel.seco.dispatcher.tclser.TclDispatcherListener;
import isabel.seco.network.tclser.TclMessage;

import java.io.IOException;
import java.text.ParseException;
import java.util.HashSet;
import java.util.Set;
import java.util.Vector;
import java.util.logging.Logger;

public class SiteManager implements TclDispatcherListener {

	public static final int TIMEOUT = -11;
	public static final int WAITING = -10;
	public static final int DISCONNECTED = -9;
	public static final int REJECTED = -1;

	/**
	 * Reference to my IsabelClient.
	 */
	private IsabelClientImpl ic;


	private int myChId = WAITING;


	Logger mylogger;

	/**
	 * Construct the SiteManager object.
	 */
	SiteManager(IsabelClientImpl ic) {

		mylogger = Logger.getLogger("isabel.gw.isabel_client.SiteManager");
		mylogger.info("Creating the SiteManager object.");

		this.ic = ic;

		ic.getNetworkMgr().getDispatcher().addDestiny("UM", this);
	}

	/**
	 * Returns the chId (ssrc) assigned to this connected site.
	 * @return The chId of this connected site.
	 */
	public int getMyChId() {
		return myChId;
	}

	/**
	 * Send a SiteConnection request to the master to connect this site.
	 * 
	 * @param siteid
	 *            Nickname of the new site.
	 * @param addr
	 *            Address of the new site.
	 * @throws IOException
	 */
	void connect(String siteid, String addr) throws IOException {

		// Mensaje de solicitud de conexion
		String cmd = "UM " + "SiteConnection " + "GatewayClient "
		    + System.getenv("ISABEL_VERSION") + " "
		    + System.getenv("ISABEL_RELEASE") + " "
		    + System.getenv("ISABEL_PASSWORD") + " "
		    + siteid + " " + "0 " + "127.0.0.1 "
		    + siteid + " "
		    + ic.getNetworkMgr().getNetwork().getClientID() + " "
		    + "127.0.0.1 " + "127.0.0.1 " 
		    + "0 0 0 0 0 " + "Linux";

		// enviar mensaje
		ic.getNetworkMgr().getNetwork()
				.sendGroup("isabel_master", new TclMessage(cmd));

		// Esperar respuesta a la solicitud de conexion
		int maxTries = 30;
		for (int i = 0;; i++) {
			if (i < maxTries) {
				if (myChId == WAITING) {
					mylogger.info("Waiting for Connection Response message.");
					try {
						Thread.sleep(1000);
					} catch (InterruptedException ie) {
					}
				} else {
					mylogger
							.info("Connection Response message has been received.");
					break;
				}
			} else {

				myChId = TIMEOUT;

				mylogger
						.warning("I can't receive Connection Response message. Timeout expired. Canceling connection.");
				throw new IOException(
						"I can't receive Connection Response message. Timeout expired. Canceling connection.");
			}
		}
	}

	/**
	 * Send a site disconnection request to master. Note: I don't wait for the
	 * response.
	 */
	void disconnect() {

		if (myChId < 0)
			return;

		try {
			ic.getNetworkMgr().getNetwork().sendGroup("isabel_master",
								  new TclMessage("UM SiteDisconnection " + ic.getMySiteId()));
		} catch (IOException ioe) {
			mylogger
					.severe("I can't send SiteDisconnection message to master. I get: "
							+ ioe.getMessage());
		}

	}

    /**
     * Process a message received from the dispatcher object.
     * 
     * @param msg
     *            Message to process.
     */
    public void processMessage(TclMessage msg) {
	
	String cmd = msg.cmd;
	
	try {
	    StringParser sp = new StringParser(cmd);
	    
	    sp.nextToken(); // type
	    String method = sp.nextToken();
	    
	    if (method.equals("SiteResponse")) {
		processSiteResponseMsg(sp);
	    } else if (method.equals("ConnectedSitesState")) {
		processSitesState(sp);
	    } else {
		mylogger.severe("Mensaje no soportado aun: \"" + cmd + "\"");
	    }
	} catch (ParseException pe) {
	    mylogger.severe("SiteManager: Invalid command => \"" + cmd
			    + "\". I get: (" + pe.getMessage() + ").");
	}
    }

	/**
	 * Process a SiteResponse message.
	 * 
	 * @param msg
	 *            Message to process.
	 */
	private void processSiteResponseMsg(StringParser sp) {

		if (myChId == TIMEOUT) {
			mylogger.warning("Connection was cancelled by timeout.");
			return;
		}

		String _mySiteId = sp.nextToken();
		if ( ! _mySiteId.equals(ic.getMySiteId())) {
			return;
		} 

		myChId = Integer.parseInt(sp.nextToken());

		mylogger.info("SiteResponse message received: " + myChId);

		if (myChId == REJECTED) {
			mylogger.warning("Connection rejected: " + sp.nextToken());
			return;
		}

		/**
		 * Thread usada para enviar pings al master.
		 */
		Thread keepalive = new Thread() {
			
			public void run() {
			    
			    while (getMyChId() != DISCONNECTED) {
				try {
				    ic.getNetworkMgr().getNetwork().sendGroup(
									      "isabel_master",
									      new TclMessage("XS KeepAlive Ping site-"
											     + ic.getMySiteId()));
				} catch (IOException ioe) {
				    mylogger
					.severe("I can't send keepalive message to master. I get: "
						+ ioe.getMessage());
				    break;
				}
				try {
				    sleep(10000);
				} catch (Exception e) {
				}
			    }
			}
		    };
		keepalive.start();
		
	}
    
    
    /**
     * Process a ConnectedSitesState message.
     */
    private void processSitesState(StringParser parser ) {
	
	Set<String> new_connected = new HashSet<String>();
	
	while (parser.hasMoreTokens()) {
	    try {
		StringParser site_sp = new StringParser(parser.nextToken());
 		
	 	String tool = site_sp.tokenAt(0); // tool
		int chId = Integer.parseInt(site_sp.tokenAt(1)); // userid
		String site_id = site_sp.tokenAt(2); // site_id
		String publicName = site_sp.tokenAt(8); // location
		String compStates = site_sp.tokenAt(18); // component states
		
		
		new_connected.add(site_id);
		
		if ( ! SitesInfo.isDeclared(site_id)) {
		    // It is a new connected site.
		    declareSite(tool, chId, site_id, publicName); 
		}
		
		// Procesar estado de componentes
		ic.getComponentMgr().updateComponentStates(site_id, compStates);
	    } 
	    catch (Exception e) {
		mylogger.severe("Protocol error: Processing Global Sites state.");
	    }
	}
	
	// Look for disconnected sites.
	for (String _site_id : SitesInfo.getDeclaredSites()) {
	    if ( ! new_connected.contains(_site_id)) {
		// Site _site_id has disconnected.

		ic.getComponentMgr().updateComponentStates(_site_id, "");

		siteDisconnection(_site_id, "Disconnection");
	    }
	}
	
    }
    
    
    
    
    /**
     * Declare a site.
     * 
     */
    private void declareSite(String tool, int chId, String site_id, String publicName) {
	
	mylogger.info("New Site Connection: " + site_id + " (" + tool + ")");
	
	Vector<FlowInfo> flows = null;
	
	if ( ! tool.equals("GatewayClient")) {
	    
	    flows = LocalMixer.getIsabelFlowsInfo();
	    
	    if (SitesInfo.declareSite(chId, site_id, "127.0.0.1", flows, false)) {
		
		// Notify to the isabel client listeners:
		
		MemberInfo mi = new MemberInfo(chId, site_id, "127.0.0.1", flows);
		
		IsabelClientEvent ev = new IsabelClientEvent(mi, ic);
		
		for (IsabelStateListener l : IsabelState.getIsabelState().getListeners()) {
		    l.newIsabelSite(ev);
		}
	    }
	}
    }
    
    /**
     * Process a Site Disconnection
     * 
     */
    private void siteDisconnection(String site_id, String endMsg) {
	
	mylogger.info("Site connection finished (SITE_ID=" + site_id + "): " + endMsg);
	
	// ----
	
	if (ic.getMySiteId().equals(site_id)) {
	    myChId = DISCONNECTED;
	    
	    if (ic.isConnected())
		ic.disconnect();
	    
	    ic.getComponentMgr().cleanup();
	    ic.getNetworkMgr().disconnect();
	    
	    // Notify isabel client listeners:
	    
	    MemberInfo mi = SitesInfo.getMemberInfo(site_id);
	    
	    IsabelClientEvent ev = new IsabelClientEvent(mi, ic);
	    
	    for (IsabelStateListener l : IsabelState.getIsabelState().getListeners()) {
		l.siteDisconnection(ev);
	    }
	    
	    SitesInfo.removeSite(site_id);
	    
	} else {
	    
	    if (!SitesInfo.isGatewayClient(site_id)) {
		
		// Notify isabel client listeners:
		
		MemberInfo mi = SitesInfo.getMemberInfo(site_id);
		
		if (mi != null && SitesInfo.removeSite(site_id)) {
		    IsabelClientEvent ev = new IsabelClientEvent(mi, ic);
		    
		    for (IsabelStateListener l : IsabelState.getIsabelState().getListeners()) {
			l.siteDisconnection(ev);
		    }
		    
		}
	    }
	}
    }
    
}
