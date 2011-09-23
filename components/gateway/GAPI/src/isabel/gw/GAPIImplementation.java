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

import isabel.gw.isabel_client.ShutdownEvent;

import java.util.Hashtable;
import java.util.Timer;
import java.util.TimerTask;
import java.util.Vector;
import java.util.logging.Level;
import java.util.logging.Logger;


enum IsabelButtonNames {
	VNC_MODE,
	CHAT_MODE,
	QUESTIONS_MODE,
	QUESTION_REQUEST
}

/**
 * This class represents the main module.
 * 
 * It implements the rest of classes of the package and manages the GAPI and
 * CIsabelClient commands.
 */
class GAPIImplementation extends GAPI implements IsabelStateListener {
	
	/**
	 * gateway configuration.
	 */
	private Configuration config;
	/**
	 * Logs
	 */
	private Logger log;
	/**
	 * Hash table <SSRC,IsabelClient>.
	 */
	Hashtable<Integer, IsabelClient> clientHash = new Hashtable<Integer, IsabelClient>();
	/**
	 * Hash table <SSRC,IsabelClient>.
	 */
	Hashtable<String, IsabelClient> nickHash = new Hashtable<String, IsabelClient>();
	/**
	 * Listeners list.
	 */
	Vector<IGAPIListener> listeners = new Vector<IGAPIListener>();
	/**
	 * Client list.
	 */
	Vector<Integer> clients = new Vector<Integer>();
	/**
	 * The mcu manager.
	 */
	static McuWrapper mcuWrapper;
	/**
	 * Timer de evento de activeflows
	 */
	private Timer timer;
	
	private String vncServer;
	private String vncPassword;
	
	private volatile boolean sendFlows = false;
	private volatile boolean sendEventNow = false;
	
	/**
	 * Evento de flujos activos cacheado.
	 */
	GAPIFlowsEvent activeFlowsEvent;
	
	Object lock = new Object();
	/**
	 * Constructor.
	 */
	public GAPIImplementation() {
		log = Logger.getLogger("isabel.gw.Gateway");
		log.info("Starting Gateway...");
		
		vncServer = System.getenv("FLASH_VNC_SERVER");
		vncPassword = System.getenv("FLASH_VNC_PASSWORD");
		if (vncServer == null) {
			vncServer = "127.0.0.1:89";
			vncPassword = "a";
		}
		try {
			activeFlowsEvent = new GAPIFlowsEvent(this);
			config = new Configuration("/etc/isabel/gw/gapi.1.0.0.xml");
			IsabelAddr = config.getIsabelAddr();
			if (mcuWrapper == null) {
				mcuWrapper = new McuWrapper(config.getMcuExec(), config
						.getMcuAddr(), config.getMcuPort());
				//Add local irouter member:
				//This member will represents the hole Isabel Sites and the local client (Audio mixer + video desktop)				
				MemberInfo irouterInfo = new MemberInfo(config.getIsabelSSRC(), //Este dato da lo mismo, solo se usa para guardar en la lista
						                                "irouter",
						                                config.getIsabelAddr(),
						                                LocalMixer.getIsabelFlowsInfo());
				mcuWrapper.addMember(irouterInfo);
			}
			
			timer = new Timer();
			timer.schedule(new TimerTask() {
				
				@Override
				public void run() {
					synchronized (activeFlowsEvent) {
					    if (sendEventNow)
					    {
							log.info("activeFlows: Sending event to IGAPIListeners! " + activeFlowsEvent.getFlowsInfo());
							for (IGAPIListener lis : listeners) 
							{
								try {
								    lis.activeFlows(activeFlowsEvent);
								}catch (Exception e) {
									log.log(Level.SEVERE, "Error sending active flows", e);
								}
							}
							log.info("activeFlows: event sent to IGAPIListeners!");
							//Este clear se pone para reiniciar la tabla de flujos activos.
							//Se tienen que suponer que los SSRC que no aparecen en la tabla no cambian
							activeFlowsEvent.getFlowsInfo().clear();
							sendFlows = false;
							sendEventNow = false;
						}
					    if (sendFlows) {
					    	log.info("activeFlows: events will be sent in next round");
						    sendEventNow = true;
					    }
					}
				}
			}, 1000, 500);
		} catch (ConfigException e) {
			log.severe("Error loading configuration");
			System.exit(-1);
		}
		
		IsabelState.getIsabelState().addIsabelStateListener(this);
	}
	
	public McuWrapper getMcuWrapper() {
		return mcuWrapper;		
	}
	
	public Configuration getConfig()
	{
		return config;
	}

	/**
	 * Connects an external node to the session.
	 * 
	 * @return Result
	 * @param login
	 *            Member user name.
	 * @param password
	 *            Member user password.
	 * @param info
	 *            Member information data.
	 */
	public Result connect(String login, String password, MemberInfo info) {
		log.info("Connection attempt from " + info.getAddr() + ".\n");
		Result result = null;
		// Buscamos el nick no esta repetido
		IsabelClient isabelClient = nickHash.get(info.getNickName());
		if (isabelClient == null)
		{
			//Comprobamos que tampoco esta repetido el SSRC
			isabelClient = clientHash.get(info.getSSRC());			
		}
		if (isabelClient != null) {
			log.info("Client already connected.\n");
			isabelClient.disconnect();
			for (int i = 0;
		     i < 10 && (nickHash.get(info.getNickName())!=null || clientHash.get(info.getSSRC())!=null);
		     i++)
			{
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					log.severe("Wait error!");
				}
				log.info("Waiting client disconnection...");
			}
		}		
		// Creamos un nuevo cliente
		log.info("Creating new client.\n");
		isabelClient = IsabelClient.create();	
		result = isabelClient.connect(info);
		return result;
	}

	/**
	 * Disconnects an external node from the session.
	 * 
	 * @return Result
	 * @param ssrc
	 *            Member ssrc identifyer.
	 */
	public Result disconnect(int ssrc) {
		log.info("Disconnecting client " + ssrc + ".\n");
		Result result = null;
		// Buscamos el miembro en la tabla hash
		IsabelClient isabelClient = clientHash.get(ssrc);
		if (isabelClient != null) {
			result = isabelClient.disconnect();
		} else {
			log.info("Client " + ssrc + " not found.\n");
			result = new Result(Result.ERROR_CLIENT_NOT_FOUND,
					Result.ERROR_CLIENT_NOT_FOUND_DESC);
		}
		return result;
	}
	
	/* (non-Javadoc)
	 * @see isabel.gw.IGAPI#buttonPressed(java.lang.String, int)
	 */
	public Result buttonPressed(String buttonName, int ssrc) {
		IsabelButtonNames button = IsabelButtonNames.valueOf(buttonName);
		Result result = null;
		IsabelClient isabelClient = clientHash.get(ssrc);
		if (isabelClient == null) {
			log.info("Client " + ssrc + " not found.\n");
			result = new Result(Result.ERROR_CLIENT_NOT_FOUND,
					Result.ERROR_CLIENT_NOT_FOUND_DESC);			
		}
		switch(button) {
		
		case QUESTION_REQUEST:
			log.info("Question activated from ssrc = " + ssrc);
			// Buscamos el miembro en la tabla hash
		    result = isabelClient.question();
			break;
		case CHAT_MODE:
			log.info("Chat mode activated from ssrc = " + ssrc);
			result = isabelClient.setChatMode();
			break;
		case VNC_MODE:
			log.info("VNC mode activated fro ssrc = " + ssrc);
			result = isabelClient.setVNCMode(vncServer, vncPassword);
			break;
		case QUESTIONS_MODE:
			log.info("VNC mode activated fro ssrc = " + ssrc);
			result = isabelClient.setQuestionsMode();	
			break;
		default:
			result =  new Result(Result.OK_CODE, "not implemented yet");
		}
		return result;
	}
	
	/* (non-Javadoc)
	 * @see isabel.gw.IGAPI#getButtonNames()
	 */
	public String[] getButtonNames() {
		String serviceName = IsabelState.getIsabelState().getService().getServiceName();
		System.out.println("SERVICE: " + serviceName);
		String[] result = {serviceName, IsabelButtonNames.QUESTION_REQUEST.toString()};
		if (serviceName.equalsIgnoreCase("TeleMeeting")) {
			IsabelButtonNames[] names  = IsabelButtonNames.values();
			result = new String[names.length + 1];
			result [0] = serviceName;

			for (int i = 0; i < names.length; i++) {
				result[i+1] = names[i].toString();
			}
		}
		return result;
	}
	
	/**
	 * Add a IGAPIListener.
	 * 
	 * @return Result
	 * @param listener
	 */
	public Result addGAPIListener(IGAPIListener listener) {
		// Lo buscamos en la lista
		if (listeners.contains(listener) == false) {
			listeners.add(listener);											
		}
		return (new Result(Result.OK_CODE, Result.OK_DESC));
	}

	/**
	 * Remove IGAPIListener.
	 * 
	 * @return Result
	 * @param listener
	 */
	public Result removeGAPIListener(IGAPIListener listener) {
		// Lo buscamos en la lista
		if (listeners.contains(listener) == true) {
			// eliminamos el listener en la lista			
			listeners.remove(listener);
		}
		return (new Result(Result.OK_CODE, Result.OK_DESC));
	}

	/**
	 * IsabelClient calls this event every time an Isabel member joins the
	 * session.
	 * 
	 * @return Result
	 * @param event
	 */
	public Result newIsabelSite(IsabelClientEvent event) {
		synchronized (lock) {
			// Todos los sitios de isabel tienen que tener 127.0.0.1 en el campo Address
			// y los clientes externos deben tener su propia IP.
			if (!clients.contains(event.getMemberInfo().ssrc))
			{
				log.info("new site " + event.getMemberInfo().getNickName() + "\n");
				log.info("--> SSRC = " + event.getMemberInfo().getSSRC() + ".\n");
				log.info("--> ADDR = " + event.getMemberInfo().getAddr() + ".\n");
				// Obtenemos el sender
				IsabelClient isabelClient = event.getIsabelClient();
				// si todo conexion OK
				if (event.getMemberInfo().getSSRC() > 0) {
					// Lo metemos en la hashtable
					clientHash.put(event.getMemberInfo().getSSRC(), isabelClient);
					nickHash.put(event.getMemberInfo().getNickName(), isabelClient);
					// AÃ±adimos el cliente en la MCU solo si no es "127.0.0.1",
					//Es decir, los clientes externos, ya que los clientes de isabel
					//se manejan a traves del irouter local, cliente que se anyade en el constructor
					if (event.getMemberInfo().getAddr() != "127.0.0.1")
					{
						mcuWrapper.addMember(event.getMemberInfo());						
					}
				}
				// Informamos los listeners del evento
				GAPIMemberEvent memberEvent = new GAPIMemberEvent(event.getMemberInfo(), this);
				
				for (IGAPIListener lis : listeners) {
					lis.addMember(memberEvent);
				}
				clients.add(event.getMemberInfo().ssrc);
			}
		}		
		return (new Result(Result.OK_CODE, Result.OK_DESC));
	}

	/**
	 * IsabelClient calls this event every time a member (external or internal)
	 * is disconnected.
	 * 
	 * @return Result
	 * @param event
	 */
	public Result siteDisconnection(IsabelClientEvent event) {
		synchronized (lock) {
			if (clients.contains(event.getMemberInfo().ssrc))				
			{
				log.info("Site " + event.getMemberInfo().getSSRC() + " disconnected.\n");
				// Eliminamos el cliente
				clientHash.remove(event.getMemberInfo().getSSRC());
				nickHash.remove(event.getMemberInfo().getNickName());
				// Eliminamos cliente de la MCU.
				mcuWrapper.delMember(event.getMemberInfo().getSSRC());
				// Informamos los listeners del evento
				GAPIMemberEvent memberEvent = new GAPIMemberEvent(event.getMemberInfo(), this);
				for (IGAPIListener lis : listeners) 
				{
					lis.delMember(memberEvent);
				}
				clients.remove(clients.indexOf(event.getMemberInfo().getSSRC()));
			}
		}
		return (new Result(Result.OK_CODE, Result.OK_DESC));
	}

	/**
	 * IsabelClient call this event every time an external node connects.
	 * 
	 * @return Result
	 * @param event
	 */
	public Result siteReady(IsabelClientEvent event) {
		synchronized (lock) {
			if (!clients.contains(event.getMemberInfo().ssrc))
			{
				log.info("new site " + event.getMemberInfo().getNickName() + "\n");
				log.info("--> SSRC = " + event.getMemberInfo().getSSRC() + ".\n");
				log.info("--> ADDR = " + event.getMemberInfo().getAddr() + ".\n");
		
				// Obtenemos el sender
				IsabelClient isabelClient = event.getIsabelClient();
				// si todo conexion OK
				if (event.getMemberInfo().getSSRC() > 0) {
					// Lo metemos en la hashtable
					clientHash.put(event.getMemberInfo().getSSRC(), isabelClient);
					nickHash.put(event.getMemberInfo().getNickName(), isabelClient);
					// AÃ±adimos el cliente en la MCU
					mcuWrapper.addMember(event.getMemberInfo());
				}
				// Informamos los listeners del evento
				GAPIMemberEvent memberEvent = new GAPIMemberEvent(
						event.getMemberInfo(), this);
				for (IGAPIListener lis : listeners) 
				{
					lis.addMember(memberEvent);
				}
				clients.add(event.getMemberInfo().ssrc);
			}
		}
		return (new Result(Result.OK_CODE, Result.OK_DESC));
	}

	/**
	 * IsabelClient calls this method to inform the listeners about which flows
	 * should sent the external site to the isabel session
	 * 
	 * @param event
	 *            Specifies a site and the flows that should send to the isabel
	 *            session.
	 * 
	 * @return A result object.
	 */
	public Result activeFlows(IsabelClientEvent event) {

		// Informamos los listeners del evento
		MemberInfo memberInfo = event.getMemberInfo();
		log.info("activeFlows for: " + event.getMemberInfo().getSSRC() + " => " + event.getMemberInfo().getFlowList());

		synchronized (activeFlowsEvent) {
			if (memberInfo.getSSRC() > 0) {
			    activeFlowsEvent.getFlowsInfo().put(memberInfo.getSSRC(), memberInfo.getFlowList());
			    sendFlows = true;
			}
		}
		return (new Result(Result.OK_CODE, Result.OK_DESC));
	}

	/**
	 * IsabelClient calls this method when the connection with the isabel
	 * session is lost, or it can not be established.
	 * 
	 * @param event
	 */
	public void shutdown(ShutdownEvent event) {

		//TODO: to be implemented
	}
	
	public void isabelChange(SessionEvent event) {
		for (IGAPIListener lis : listeners) 
		{
			lis.isabelChange(event);
		}		
	}

	/**
	 * Enable flow routing between â€œOrigIDâ€� and â€œDestIDâ€�.
	 * 
	 * @return Result
	 * @param PT
	 *            RTP Payloadtype identifyer.
	 * @param origID
	 *            Source member identifyer.
	 * @param destID
	 *            Destination member identifyer.
	 */
	public Result enableFlow(int PT, int origID, int destID) {
		return mcuWrapper.enableFlow(PT, origID, destID, true);
	}

	/**
	 * Enable flow routing between â€œOrigIDâ€� and â€œDestIDâ€�.
	 * 
	 * @return Result
	 * @param PT
	 *            RTP Payloadtype identifyer.
	 * @param origID
	 *            Source member identifyer.
	 * @param destID
	 *            Destination member identifyer.
  	 * @param changeSSRC
	 *            Change SSRC RTP flow.
	 */
	public Result enableFlow(int PT, int origID, int destID, boolean changeSSRC) {
		return mcuWrapper.enableFlow(PT, origID, destID, changeSSRC);
	}

	/**
	 * Disable flow routing between â€œOrigIDâ€� and â€œDestIDâ€�.
	 * 
	 * @return Result
	 * @param PT
	 *            RTP Playloadtype identifyer.
	 * @param origID
	 *            Source member identifyer.
	 * @param destID
	 *            Destination member identifyer.
	 */
	public Result disableFlow(int PT, int origID, int destID) {
		return mcuWrapper.disableFlow(PT, origID, destID);
	}

	/**
	 * This method setups the client video flows in the MCU.
	 * 
	 * @return Result
	 * @param id
	 * @param video_mode
	 * @param pt
	 * @param bw
	 * @param fr
	 * @param Q
	 * @param width
	 * @param height
	 * @param ssrc
	 * @param grid_mode
	 */
	public Result setupVideo(int id, VideoMode video_mode, int pt, int bw,
			int fr, int Q, int width, int height, int ssrc, GridMode grid_mode) {
		return mcuWrapper.setupVideo(id, video_mode, pt, bw, fr, Q, width,
				height, ssrc, grid_mode);
	}

	/**
	 * This method setups the client audio flows in the MCU.
	 * 
	 * @return Result
	 * @param id
	 * @param audioMode
	 * @param pt
	 * @param sl
	 * @param ssrc
	 */
	public Result setupAudio(int id, AudioMode audioMode, int pt, int sl) {
		return mcuWrapper.setupAudio(id, audioMode, pt, sl);
	}

	@Override
	public void parameterChange(String user, String name, Object value) {
		for (IGAPIListener lis : listeners) 
		{
			lis.parameterChange(user, name, value);
		}		
	}

	@Override
	public Result setParameter(int ssrc, String name, Object value) {
		Result result = null;
		IsabelClient isabelClient = clientHash.get(ssrc);
		if (isabelClient == null) {
			log.info("Client " + ssrc + " not found.\n");
			result = new Result(Result.ERROR_CLIENT_NOT_FOUND,
					Result.ERROR_CLIENT_NOT_FOUND_DESC);			
		}
		return isabelClient.setParameter(name, value);
	}
}
