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
import isabel.gw.MemberInfo;
import isabel.gw.Result;
import isabel.gw.isabel_client.components.ComponentManager;
import isabel.gw.isabel_client.components.QuestionsComponent;

import isabel.gw.isabel_client.isabel_state.*;

import isabel.seco.network.tclser.TclMessage;

import java.io.IOException;
import java.util.Vector;
import java.util.logging.Logger;

/**
 * This class is used to create simulated isabel clients.
 * 
 * These clients communicate with the isabel conference via the SeCo service.
 * 
 * A client connect with the isabel session when the connect method is called,
 * and disconnect from the conference when the disconnect method is called.
 * While connected to the conference, the client sends event to the registered
 * IsabelStateListener objects, to inform about what is happening in the
 * conference.
 */
public class IsabelClientImpl extends IsabelClient {
    
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
     * Site Manager. Se encarga de los mensajes UM: conexion y desconexion de
     * los sitios
     */
    private SiteManager siteMgr;
    
    /**
     * Component Manager. Se encarga de los mensajes CM: lanza y para los
     * componentes.
     */
    private ComponentManager compMgr;
    
    /**
     * SMS Manager Se encarga de los mensajes SMS
     */
    private SmsManager smsMgr;
    
    /**
     * Estado de la conexion con la red isabel Valores: NOCONNECTED: se ha
     * creado el objeto pero no se llamo al metodo connect. CONNECTED: se llamo
     * al metodo connect, y estoy conectado a la red isabel. DISCONNECTED: se
     * termino la conexion con la red isabel.
     */
    private static enum ConnectionState {
	NOCONNECTED, CONNECTED, DISCONNECTED
	    };
    
    /**
     * Estado de la conexion.
     */
    private ConnectionState ccstate;
    
    /**
     * Values passed in the parameters of the connect method.
     */
    private String mySiteId;
    private String myAddr;
    private Vector<FlowInfo> myFlows;
    

    /**
     * Construct a new Isabel Client object.
     */
    public IsabelClientImpl() {
	
	mylogger = Logger.getLogger("isabel.gw.isabel_client.IsabelClient");
	mylogger.info("Creating a new Isabel Client.");
	
	ccstate = ConnectionState.NOCONNECTED;
    }
    
    /**
     * Realiza la conexion con la red isabel, y crea los objetos internos que
     * manejan los mensajes recibidos.
     */
    @Override
	public synchronized Result connect(MemberInfo info) {
	    
	    if (ccstate != ConnectionState.NOCONNECTED) {
		String emsg = "Prococol error: calling \"IsabelClientImpl.connect\" method several times.";
		mylogger.severe(emsg);
		return new Result(Result.ERROR_FATAL, emsg);
	    }
	    
	    this.mySiteId = info.getNickName();
	    this.myAddr   = info.getAddr();
	    this.myFlows  = info.getFlowList();
	    
	    // NetworkManager: 
	    // Internally, creates the Network and the Dispatcher objects.
	    netMgr  = new NetworkManager(this); 
	    
	    // SiteManager:
	    // It is registered in the dispatcher.
	    siteMgr = new SiteManager(this);
	    
	    // ComponentManager:
	    // It is registered in the dispatcher.
	    compMgr = new ComponentManager(this);
	    
	    // SmsManager:
	    // It is registered in the dispatcher.
	    smsMgr = new SmsManager(this);
	    
	    ccstate = ConnectionState.CONNECTED;
	    
	    mylogger.info("Isabel Client connection: " + mySiteId);
	    
	    // Conectarse a la session isabel
	    try {
		mylogger.info("Connecting to session ......");
		siteMgr.connect(mySiteId, myAddr);
	    } catch (IOException ioe) {
		
		ccstate = ConnectionState.DISCONNECTED;
		
		String reason = "Fatal error: I can't connect to the isabel session: "
		    + ioe.getMessage();
		mylogger.severe(reason);
		
		//sendListeners_Shutdown_Event(reason);
		
		return new Result(Result.ERROR_FATAL, reason);
	    }
	    
	    if (siteMgr.getMyChId() < 0) {
		mylogger.info("Client " + mySiteId + " => Connection refused.");
		
		ccstate = ConnectionState.DISCONNECTED;
		
		sendListeners_SiteReady_Event(siteMgr.getMyChId());
		
		return new Result(Result.OK_CODE, "Connection refused");
	    } else {
		mylogger.info("Client " + mySiteId + " => Connection done.");
		
		SitesInfo.declareSite(siteMgr.getMyChId(), mySiteId,
					myAddr, myFlows, true);
		
		sendListeners_SiteReady_Event(siteMgr.getMyChId());
		
		
		// Ya estoy conectado a la sesion. Lanzo mis componentes.
		compMgr.startSupportedComponents();
		
		// Apuntarse al grupo "isastate"
		try {
		    mylogger.info("-------------APUNTARSE A ISASTATE.");
		    netMgr.getNetwork().joinGroup("isastate");
		} catch (IOException e) {
		    mylogger.severe(e.getMessage());
		}
		
		return new Result(Result.OK_CODE, "Connection done");
	    }
	}
    
    /**
     * Send a siteReady message to all the listeners.
     * 
     * @param chId
     *            Method to inform to all the listeners about result of the
     *            connection request.
     */
    private void sendListeners_SiteReady_Event(int chId) {
	
	MemberInfo mi = new MemberInfo(chId, mySiteId, myAddr, myFlows);
	
	IsabelClientEvent ev = new IsabelClientEvent(mi, this);
	
	for (IsabelStateListener l : IsabelState.getIsabelState().getListeners()) {
	    l.siteReady(ev);
	}
    }
    
    // -----------------------------------------------------------------------
    
    /**
     * Disconnect me from the isabel session
     * 
     */
    @Override
	public synchronized Result disconnect() {
	    
	    if (ccstate != ConnectionState.CONNECTED) {
		mylogger.warning("IsabelClient is already disconnected.");
		return new Result(Result.ERROR,
				  "IsabelClient is already disconnected.");
	    }
	    
	    mylogger.info("Isabel Client \"" + mySiteId + "\" disconnection.");
	    
	    ccstate = ConnectionState.DISCONNECTED;
	    
	    siteMgr.disconnect();
	    
	    return new Result(Result.OK_CODE, "Disconnection done");
	    
	}
    
    /**
     * Este metodo debe enviar el evento de pregunta a Isabel.
     */
    @Override
	public Result question() {

	QuestionsComponent qc = (QuestionsComponent) compMgr.getComponent("Questions");

	if (qc != null) {
	    qc.toggleSite(mySiteId);
	    return new Result(Result.OK_CODE, "Question (un)requested.");
	} else {
	    return new Result(Result.ERROR, "Questions component is not running.");
	}
    }
    
    /**
     * Returns if I am connected.
     */
    public boolean isConnected() {
	
	return ccstate == ConnectionState.CONNECTED;
	
    }
    
    // -----------------------------------------------------------------------
    // SHUTDOWN
    // -----------------------------------------------------------------------
    
    /**
     * Send a shutdown event to all the listeners.
     * 
     * @param reason
     *            Reason explaining why this event was generated.
     */
    private void sendListeners_Shutdown_Event(String reason) {
	
	ShutdownEvent ev = new ShutdownEvent(reason);
	
	for (IsabelStateListener l : IsabelState.getIsabelState().getListeners()) {
	    l.shutdown(ev);
	}
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
     * Get a reference to the SiteManager attribute.
     * 
     * @return Returns a reference to the SiteManager attribute.
     */
    public SiteManager getSiteMgr() {
	return siteMgr;
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
    
    /**
     * Get Channel identifier.
     * 
     * @return Returns the channel identifier of this isabel client.
     */
    public int getChId() {
	
	return siteMgr.getMyChId();
    }
    
    /**
     * Get MySiteId attribute.
     * 
     * @return Returns the mySiteId attribute.
     */
    public String getMySiteId() {
	return mySiteId;
    }
    
    /**
     * Get myAddr attribute.
     * 
     * @return Returns the myAddr attribute.
     */
    public String getMyAddr() {
	return myAddr;
    }
    
    /**
     * Get valid flows.
     * 
     * @return Returns the flow list of this isabel client.
     */
    public Vector<FlowInfo> getMyFlows() {
	
	return myFlows;
    }

	@Override
	public Result setChatMode() {
		// Mensaje de solicitud de conexion
		String cmd = "CM InteractionModeRequest chatmode *"; 

		// enviar mensaje
		try {
			getNetworkMgr().getNetwork().sendGroup("isabel_master", new TclMessage(cmd));
			return new Result(Result.OK_CODE, "Chatmode requested");
		} catch (IOException e) {
			e.printStackTrace();				
			return new Result(Result.ERROR, e.getMessage());
		}
	}

	@Override
	public Result setQuestionsMode() {
		// Mensaje de solicitud de conexion
		String cmd = "CM InteractionModeRequest quesmode " + this.getMySiteId() + 
		" -ctrlsites *"; 

		// enviar mensaje
		try {
			getNetworkMgr().getNetwork().sendGroup("isabel_master", new TclMessage(cmd));
			return new Result(Result.OK_CODE, "Chatmode requested");
		} catch (IOException e) {
			e.printStackTrace();				
			return new Result(Result.ERROR, e.getMessage());
		}
	}
	
	@Override
	public Result setVNCMode(String server, String password) {
		// Mensaje de solicitud de conexion
		String cmd = "CM InteractionModeRequest appsharingmode " + 
		this.getMySiteId() + " -vnccfg " +
		"\"NeReDa " + System.getenv("ISABEL_SITE_ID") + 
		" server " + server + " " + password +"\""; 

		// enviar mensaje
		try {
			getNetworkMgr().getNetwork().sendGroup("isabel_master", new TclMessage(cmd));
			return new Result(Result.OK_CODE, "VNCmode requested");
		} catch (IOException e) {
			e.printStackTrace();				
			return new Result(Result.ERROR, e.getMessage());
		}
	}

	@Override
	public Result setParameter(String name, Object value) {
	
	    if ("Audio_Capture".equals(name)) {

		smsMgr.forceState("Audio_site_"+mySiteId,
				  SmsManager.SMS_OPERATION.ADD,
				  "-site_id " + mySiteId +
				  " -capture " + (((Boolean)value).booleanValue() ? "1" : "0")
				  );

	    } else if ("Audio_InputGain".equals(name)) {

		smsMgr.forceState("Audio_site_"+mySiteId,
				  SmsManager.SMS_OPERATION.ADD,
				  "-site_id " + mySiteId +
				  " -gain " + ((Integer)value).intValue()
				  );

	    } else {

		return new Result(Result.ERROR, "Unknown parameter name: "+name);

	    }

	    return new Result(Result.OK_CODE, "Parameter set");
	}
    
    // -----------------------------------------------------------------------
    
}
