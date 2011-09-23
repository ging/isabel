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
package isabel.gwsip;

import isabel.gw.AudioMode;
import isabel.gw.ConfigException;
import isabel.gw.FlowInfo;
import isabel.gw.GAPI;
import isabel.gw.GAPIFlowsEvent;
import isabel.gw.GAPIMemberEvent;
import isabel.gw.GridMode;
import isabel.gw.IGAPI;
import isabel.gw.IGAPIListener;
import isabel.gw.MemberInfo;
import isabel.gw.Result;
import isabel.gw.SessionEvent;
import isabel.gw.VideoMode;
import isabel.gw.flows.FlowTypePayloads;

import java.io.File;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Vector;
import java.util.logging.Logger;

import local.ua.RegisterAgent;
import local.ua.RegisterAgentListener;
import local.ua.UserAgent;
import local.ua.UserAgentListener;
import local.ua.UserAgentProfile;

import org.zoolu.sdp.AttributeField;
import org.zoolu.sdp.MediaDescriptor;
import org.zoolu.sdp.MediaField;
import org.zoolu.sdp.OriginField;
import org.zoolu.sdp.SessionDescriptor;
import org.zoolu.sip.address.NameAddress;
import org.zoolu.sip.provider.SipProvider;
import org.zoolu.sip.provider.SipStack;

public class SipGateway implements IGAPIListener, UserAgentListener, RegisterAgentListener
{
	/**
	 * Proveedor SIP. 
	 */
	private SipProvider sipProvider;
	/**
	 * User agent de SIP.
	 */
	private UserAgentProfile userProfile;
	/**
	 * El gateway de Isabel.
	 */
	private IGAPI gapi;	
	/**
	 * Agente registrador.
	 */
	private RegisterAgent registerAgent;
	/**
	 * Configuracion de la aplicacion.
	 */
	private SipConfiguration config;
	/**
	 * Logger.
	 */
	private Logger log;
	/**
	 * Lista de clientes que esperan aceptacion de llamada.
	 */
	private Vector<IsabelUserAgent> incomingUserAgents = new Vector<IsabelUserAgent>();
	/**
	 * Lista de clientes libres que estan a la espera de conexiones.
	 */
	private Vector<IsabelUserAgent> freeUserAgents = new Vector<IsabelUserAgent>();
	/**
	 * Lista de clientes ocupados que se encuentran atendiendo clientes SIP.
	 */
	private Vector<IsabelUserAgent> usedUserAgents = new Vector<IsabelUserAgent>();
    /**
     * El session description de nuestros clientes SIP.
     */
    private SessionDescriptor sdp;
    /**
     * Tabla de flujos activos/inactivos
     */
    private HashMap<Integer, FlowsBool> flows = new HashMap<Integer, FlowsBool>();
    
    /**
     * Isabel SSRC
     */
    private int isabelSSRC;
  
	/**
	 * Punto de entrada del programa.
	 * @param args
	 */
	public static void main(String [ ] args)
	{
		new SipGateway();
	}

	public String getName()
	{
		return "SIP_Gateway";
	}
	
	/**
	 * Constructor.
	 */
	public SipGateway( )
    {
		//Inicializamos log
		log = Logger.getLogger("isabel.gwsip.SipGateway");
		log.info("Starting Sip gateway...");
		//Cargar la configuracion
		try
		{
			config = new SipConfiguration("/etc/isabel/gw/gwsip.1.0.0.xml");
		}catch(SipConfigException e){
			log.severe("Error loading SIP configuration");
			System.exit(-1);
		}		
		//Iniciamos el gapi
		gapi = GAPI.GetGateway();
		//Nos suscribimos a los eventos
		gapi.addGAPIListener(this);
		//Inicializamos el stack de sip
		initSipStack();
		//Nos registramos en el registrados SIP
		registerAgent.loopRegister(120,120);
		//Escuchamos llamadas SIP!
		newListen();
		
		isabelSSRC = -1;
		try {
			isabelSSRC = gapi.getConfig().getIsabelSSRC();
		} catch (ConfigException e) {						
			e.printStackTrace();
		}
	}

	/**
	 * Inicializa el stack de SIP.
	 */
	private void initSipStack()
	{
		//Creamos directorio "Log", donde SipProvider guarda sus logs.
		(new File("log")).mkdir();		
		//Iniciamos stack
		SipStack.init();		
		//Iniciamos Sip provider
		sipProvider = new SipProvider(config.getClientAddr(),config.getRegisterPort());		
		//Inicializamos nuestro perfil
		userProfile = new UserAgentProfile();
		userProfile.nickname = config.getNickName();
		userProfile.username = config.getClientName();		
		userProfile.from_url = config.getClientName() + "@" + config.getRegisterAddr();
		userProfile.contact_url = config.getClientName() + "@" + config.getClientAddr();
		userProfile.do_register = true;
		userProfile.realm = config.getRealm();
		userProfile.passwd = config.getPassword();
		userProfile.initContactAddress(sipProvider);
		//Iniciamos nuestro SDP
		sdp =new SessionDescriptor(userProfile.from_url,sipProvider.getViaAddress());
		//Establecemos los flujos que manejamos (por ahora ponemos estos de prueba)
		//TODO : Rellenar los medios de Isabel de verdad, estos estan cableados
		sdp.addMediaDescriptor(new MediaDescriptor(new MediaField("audio",12345,0,"RTP/AVP","0 8 3"),null));
		sdp.getMediaDescriptor("audio").addAttribute(new AttributeField("rtpmap","3 GSM/8000"));
		sdp.getMediaDescriptor("audio").addAttribute(new AttributeField("rtpmap","3 GSM/8000/1"));
		sdp.getMediaDescriptor("audio").addAttribute(new AttributeField("rtpmap","0 PCMU/8000"));
		sdp.getMediaDescriptor("audio").addAttribute(new AttributeField("rtpmap","0 PCMU/8000/1"));
		sdp.getMediaDescriptor("audio").addAttribute(new AttributeField("rtpmap","8 PCMA/8000"));
		sdp.getMediaDescriptor("audio").addAttribute(new AttributeField("rtpmap","8 PCMA/8000/1"));
		
		sdp.addMediaDescriptor(new MediaDescriptor(new MediaField("video",12345,0,"RTP/AVP","34"),null));
		sdp.getMediaDescriptor("video").addAttribute(new AttributeField("rtpmap","34 H263/90000"));
		sdp.getMediaDescriptor("video").addAttribute(new AttributeField("rtpmap","34 H263/90000/1"));		
		sdp.getMediaDescriptor("video").addAttribute(new AttributeField("rtpmap","34 h263-1998/90000"));
		
		//Nos registramos en el registrador.
		registerAgent = new RegisterAgent(sipProvider,userProfile.from_url,userProfile.contact_url,
				                          userProfile.nickname,userProfile.username,userProfile.realm,userProfile.passwd,this);
		registerAgent.run();
	}
	
	/**
	 * Obtiene un Isabel user agent de la lista especificada 
	 * a partir del UserAgent.
	 * @param ua UserAgent que buscamos.
	 * @param list Lista donde se busca.
	 * @return El Isabel User Agent si se encuentra, 'null' si no lo encontramos.
	 */
	static private IsabelUserAgent getIsabelUserAgent(UserAgent ua, Vector<IsabelUserAgent> list)
	{
		IsabelUserAgent iua = null;
		for (IsabelUserAgent isabelUA : list)
		{
			if (isabelUA.getUserAgent() == ua)
			{
				iua = isabelUA; 
			}
		}		
		return iua;
	}
	
	/**
	 * Obtiene un Isabel user agent de la lista especificada a partir 
	 * de la informacion del usuario.
	 * @param info Informacion de usuario.
	 * @param list Lista donde se busca.
	 * @return El Isabel User Agent si se encuentra, 'null' si no lo encontramos.
	 */
	static private IsabelUserAgent getIsabelUserAgent(MemberInfo info, Vector<IsabelUserAgent> list)
	{
		IsabelUserAgent iua = null;
		for (IsabelUserAgent isabelUA : list)
		{
			if (isabelUA.getMemberInfo()!=null && isabelUA.getMemberInfo().getNickName() == info.getNickName())
			{
				iua = isabelUA; 
			}
		}		
		return iua;
	}


	/**
	 * Pone en escucha un cliente sip. Comprobamos si tenemos
	 * clientes libres en cola, si no tenemos ninguno, lo creamos
	 * y lo ponemos a la escucha. 
	 */
	private void newListen()
	{
		if (freeUserAgents.size() == 0) //No hay clientes libres
		{
			log.info("Creating new user agent.");
			UserAgent userAgent = new UserAgent(sipProvider, userProfile, this);
			userAgent.setReceiveOnlyMode(true);
			userAgent.setSessionDescriptor(sdp.toString());
			userAgent.listen();
			IsabelUserAgent isabelUA = new IsabelUserAgent(userAgent,null);
			freeUserAgents.add(isabelUA);
		}else{
			log.info("Remains " + String.valueOf((freeUserAgents.size())) + " user agents free.");
		}
		for (IsabelUserAgent ua : freeUserAgents)
		{
			ua.getUserAgent().hangup();
			ua.getUserAgent().listen();
		}
	}
	
	/**
	 * Deja pendiente en cola de entrada un agente de usuario.
	 * @param isabelUA Cliente Isabel user agent.
	 */
	private void incomingListen(IsabelUserAgent isabelUA)
	{
		//Pasamos de la lista de libres a las de llamadas entrantes
		if (freeUserAgents.remove(isabelUA)==false)
		{ 
			log.severe("Can't find isabelUA"); 
		}
	    incomingUserAgents.add(isabelUA);
	}
	
	/**
	 * Bloquea un cliente, esto indica que el cliente esta ocupado
	 * atendiendo una llamada.
	 * @param isabelUA Cliente Isabel user agent.
	 */
	private void blockListen(IsabelUserAgent isabelUA)
	{
		//Pasamos de la lista de llamadas entrantes a la de clientes conectados
	    if(incomingUserAgents.remove(isabelUA)==false)
	    { 
	    	log.severe("Can't find isabelUA"); 
	    }
		usedUserAgents.add(isabelUA);
	}
	/**
	 * Libera un cliente isabel a partir de su user agent.
	 * @param ua Cliente SIP User agent.
	 */
	private void freeListen(UserAgent ua)
	{		
		//Buscamos entre los clientes ocupados
		Vector<IsabelUserAgent> userAgentVector = usedUserAgents;
		IsabelUserAgent isabelUA = getIsabelUserAgent(ua,usedUserAgents);
		if (isabelUA == null)
		{
			//Si no esta en la lista de clientes bloqueados,
			//comprobamos si es llamada entrante
			isabelUA = getIsabelUserAgent(ua, incomingUserAgents);
			userAgentVector = incomingUserAgents;
		}
		if (isabelUA != null)
		{
			//Nos desconectamos del gateway
			gapi.disconnect(isabelUA.getMemberInfo().getSSRC());
			//Eliminamos de la lista incoming o used
			if (userAgentVector.remove(isabelUA)==false)
			{
				log.info("Can't find isabelUA");
			}
			//Reseteamos informacion de cliente
			isabelUA.setMemberInfo(null);
			//Reseteamos el sdp
			isabelUA.getUserAgent().setSessionDescriptor(sdp.toString());
			//Colgamos, por si acaso
			isabelUA.getUserAgent().hangup();
			//Nos ponemos a la escucha
			isabelUA.getUserAgent().listen();
			//Anyadimos a la lista de libres
			freeUserAgents.add(isabelUA);			
		}
	}
	
	/**
	 * Anyade un medio al sdp.
	 * @param media Tipo de medio: "audio", "video", etc, ...
	 * @param port Puerto del medio.
	 * @param avp Campo Payloadtype del protocolo RTP.
	 * @param codec Nombre del codec: "MPEG4", "PCMU", etc,...
	 * @param rate Unidad de timestamp: 90000 para video, frecuencia de muestreo para audio.
	 */
	private void addMediaDescriptor(String media, int port, int avp, String codec, int rate)
    { 
		String attr_param=String.valueOf(avp);
		if (codec!=null) attr_param+=" "+codec+"/"+rate;
		sdp.addMedia(new MediaField(media,port,0,"RTP/AVP",String.valueOf(avp)),new AttributeField("rtpmap",attr_param));
    }	
	
	/* (non-Javadoc)
	 * @see isabel.gw.IGAPIListener#activeFlows(isabel.gw.GAPIFlowsEvent)
	 */
	
	public Result activeFlows(GAPIFlowsEvent event) {
		Hashtable<Integer,Vector<FlowInfo>> newFlows = event.getFlowsInfo();
		
		for (Integer ssrc : newFlows.keySet())
		{
			//comprobamos los flujos activos del cliente sip
			Vector<FlowInfo> userFlows = newFlows.get(ssrc);
			FlowsBool flowsBool = new FlowsBool(false, false); 
			for (FlowInfo flowInfo : userFlows)
			{
				switch (FlowTypePayloads.getTypeByPayloadType(flowInfo.getPT()))
				{
				case AUDIO_SOUND:
					flowsBool.audio = true;
					break;
				case VIDEO_IMAGE:
					flowsBool.video = true;
					break;
				}
			}
			if (!(flowsBool.audio == flows.get(ssrc).audio)){
				if(flowsBool.audio){
					activateAudio(ssrc);
				}else{
					deactivateAudio(ssrc);
				}
			}
			if (!(flowsBool.video == flows.get(ssrc).video)){
				if (flowsBool.video){
					activateVideo(ssrc);
				}else{
					deactivateVideo(ssrc);
				}
			}
		}
		
		return (new Result(Result.OK_CODE,Result.OK_DESC));
	}
	
	/**
	 * Activa los flujos de audio de un cliente SIP
	 * @param ssrc Identificador del cliente SIP
	 */
	private void activateAudio (int ssrc) {
		log.info("Activate Audio ssrc: " + ssrc);
		// Cliente SIP --> Isabel
		enableAllAudioPT(ssrc, isabelSSRC);
		
					                                     									
         // Cliente SIP --> Clientes antiguos
		for (IsabelUserAgent client: usedUserAgents)
		{
			if (client.getMemberInfo().getSSRC() != ssrc)
			{
				// Cliente SIP --> Antiguos
				enableAllAudioPT(ssrc, client.getMemberInfo().getSSRC());
					
				// Clientes antiguos --> Cliente SIP (solo si tiene audio)
				for (FlowInfo flow: client.getMemberInfo().getFlowList())
				{
					if (flow.getPT() == 0 || flow.getPT()==3 || flow.getPT()==8)
					{
						enableAllAudioPT(client.getMemberInfo().getSSRC(), ssrc);
						break;
					}
				}
			}
		}
		
		// Lo guardo en la tabla
		FlowsBool f = flows.get(ssrc);
		if (f != null) {
			f.audio = true;
		}
		else {
			flows.put(ssrc, new FlowsBool(true, false));
		}
	}
		
	/**
	 * Desactiva los flujos de audio de un cliente SIP
	 * @param ssrc Identificador del cliente SIP
	 */	
	private void deactivateAudio(int ssrc)
	{
		log.info("Deactivate Audio ssrc: " + ssrc);

		// Cliente SIP --> Isabel
		disableAllAudioPT(ssrc, isabelSSRC);
		
					                                     									
         // Cliente SIP --> Clientes antiguos
		for (IsabelUserAgent client: usedUserAgents)
		{
			if (client.getMemberInfo().getSSRC() != ssrc)
			{
				// Cliente SIP --> Antiguos
				disableAllAudioPT(ssrc, client.getMemberInfo().getSSRC());
					
				// Clientes antiguos --> Cliente SIP (solo si tiene audio)
				for (FlowInfo flow: client.getMemberInfo().getFlowList())
				{
					if (flow.getPT() == 0 || flow.getPT()==3 || flow.getPT()==8)
					{
						disableAllAudioPT(client.getMemberInfo().getSSRC(), ssrc);
						break;
					}
				}
			}
		}
		
		// Lo guardo en la tabla
		FlowsBool f = flows.get(ssrc);
		if (f != null) {
			f.audio = false;
		}
		else {
			flows.put(ssrc, new FlowsBool(false, false));
		}
	}
	
	/**
	 * Activa los flujos de video de un cliente SIP
	 * @param ssrc Identificador del cliente SIP
	 */
	private void activateVideo(int ssrc) {
		log.info("Activate Video ssrc: " + ssrc);

		// Tiene en cuenta que los PT que se usa es el 34
		gapi.enableFlow(34, ssrc, isabelSSRC);
		// Lo guardo en la tabla
		FlowsBool f = flows.get(ssrc);
		if (f != null) {
			f.video = true;
		}
		else {
			flows.put(ssrc, new FlowsBool(false, true));
		}
	}

	/**
	 * Desactiva los flujos de video de un cliente SIP
	 * @param ssrc Identificador del cliente SIP
	 */
	private void deactivateVideo(int ssrc) {
		log.info("Deactivate Video ssrc: " + ssrc);
		// Tiene en cuenta que los PT que se usa es el 34
		// Ademas no hace na pq la MCU no tira
		// gapi.disableFlow(34, ssrc, isabelSSRC);
		// Lo guardo en la tabla
		FlowsBool f = flows.get(ssrc);
		if (f != null) {
			f.video = false;
		}
		else {
			flows.put(ssrc, new FlowsBool(false, false));
		}
	}
	
	private void enableAllAudioPT(int ssrc1, int ssrc2) {
		// Tiene en cuenta que los PT que se usan son 0,3 y 8
		gapi.enableFlow(0, ssrc1, ssrc2);
		/*
		gapi.enableFlow(3, ssrc1, ssrc2);
		gapi.enableFlow(8, ssrc1, ssrc2);
		*/
	}

	private void disableAllAudioPT(int ssrc1, int ssrc2) {
		// Tiene en cuenta que los PT que se usan son 0,3 y 8
		gapi.disableFlow(0, ssrc1, ssrc2);
		/*
		gapi.disableFlow(3, ssrc1, ssrc2);
		gapi.disableFlow(8, ssrc1, ssrc2);
		*/
	}


	/* (non-Javadoc)
	 * @see isabel.gw.IGAPIListener#addMember(isabel.gw.GAPIMemberEvent)
	 */
	public Result addMember(GAPIMemberEvent event) {
		if (event.getMemberInfo().getSSRC() < 0)
		{
			log.info("addMember : " + event.getMemberInfo().getNickName() +":: Connection error.");
			IsabelUserAgent ua = getIsabelUserAgent(event.getMemberInfo(),incomingUserAgents); 
			if (ua != null)
			{
				freeListen(ua.getUserAgent());
			}
		}else{
			log.info("addMember : " + event.getMemberInfo().getNickName());
			//Comprobamos si el cliente es el de test del gateway
			if (event.getMemberInfo().getNickName() == this.getName())
			{
				log.info("addMember : " + this.getName() + " connected");
			}else if (event.getMemberInfo().getAddr() == GAPI.IsabelAddr)
			{
				//Comprobamos si el cliente es el host local del gateway
				log.info("addMember : Isabel gateway registered!");				
			}else{		
				//Comprobamos si estamos esperando aceptacion de llamada
				IsabelUserAgent ua = getIsabelUserAgent(event.getMemberInfo(),incomingUserAgents); 
				if (ua != null)
				{
					//Actualizamos informacion del cliente
					ua.setMemberInfo(event.getMemberInfo());
					//Aceptamos llamada
					ua.getUserAgent().accept();		
					//Ahora esta ocupado el cliente
					blockListen(ua);
					//Ponemos un nuevo cliente a la escucha
					newListen();
					//configuramos flujos de medios

					if (isabelSSRC > 0)
					{
						SessionDescriptor uaSdp = new SessionDescriptor(ua.getUserAgent().getSessionDescriptor());
						Vector<MediaDescriptor> mediaList = uaSdp.getMediaDescriptors();
						boolean audioConfigured = false;
						boolean videoConfigured = false;
						for (MediaDescriptor media : mediaList)
						{
							Vector<String> formats = media.getMedia().getFormatList(); 
							if (formats.size() > 0)
							{
								if (media.getMedia().getMedia().contains("audio") && !audioConfigured)
								{
									audioConfigured = true;
									//Configuramos audio (Cliente SIP --> Gateway)
									//Unicamente cambiamos el SSRC que nos llega por
									//el que espera isabel.
									gapi.setupAudio(isabelSSRC, 
										        AudioMode.MIX_MODE, 0, -50);
									//Configuramos audio (Gateway --> Cliente SIP)
									//Transcodificamos el audio del gateway al primer formato de la 
									//lista de sdp, sin importar el SSRC .
									gapi.setupAudio(event.getMemberInfo().getSSRC(),
											        AudioMode.MIX_MODE, Integer.parseInt(formats.elementAt(0)),-50);
								}else if (media.getMedia().getMedia().contains("video") && !videoConfigured)
								{
									videoConfigured = true;
									//Configuramos video (Cliente SIP --> Gateway)
									//Unicamente cambiamos el SSRC que nos llega por
									//el que espera isabel.
									gapi.setupVideo(isabelSSRC, 
										            VideoMode.SWITCH_MODE, -1, -1, -1, -1, -1, -1, 
										            event.getMemberInfo().getSSRC(), 
										            GridMode.GRID_AUTO);
									//Configuramos video (Gateway --> Cliente SIP)
									gapi.setupVideo(event.getMemberInfo().getSSRC(),
											        VideoMode.SWITCH_MODE,
											        -1, -1, -1, -1, -1, -1, isabelSSRC, GridMode.GRID_AUTO);
								}
								//Habilitamos los payloads en la MCU.
								for (String format: formats)
								{	
									//Gateway --> Cliente SIP
									gapi.enableFlow(Integer.parseInt(format),
											isabelSSRC,
											ua.getMemberInfo().getSSRC());
								}
								flows.put(event.getMemberInfo().getSSRC(), new FlowsBool(false, false));
							}
						}
					}
				}
			}
		}
		return (new Result(Result.OK_CODE,Result.OK_DESC));
	}
	
	/* (non-Javadoc)
	 * @see isabel.gw.IGAPIListener#delMember(isabel.gw.GAPIMemberEvent)
	 */
	
	public Result delMember(GAPIMemberEvent event) {
		log.info("delMember : " + event.getMemberInfo().getNickName());
		//Comprobamos si estamos hablando con el cliente
		IsabelUserAgent ua = getIsabelUserAgent(event.getMemberInfo(),usedUserAgents);
		if (ua == null)
		{
			//Comprobamos si estamos esperando aceptacion de llamada
			//(Nunca se debe de recibir llamada delMember de un cliente que espera aceptacion)
			ua = getIsabelUserAgent(event.getMemberInfo(),incomingUserAgents);
			if (ua != null)
			{
				log.severe("delMember of incomingCall?? :: " + event.getMemberInfo().getNickName());
			}
		}
		if (ua != null)
		{
			//liberamos el cliente
			freeListen(ua.getUserAgent());	
		}
		flows.remove(event.getMemberInfo().getSSRC());
		return (new Result(Result.OK_CODE,Result.OK_DESC));
	}
	
	/* (non-Javadoc)
	 * @see local.ua.UserAgentListener#onUaCallAccepted(local.ua.UserAgent)
	 */
	
	public void onUaCallAccepted(UserAgent arg0) {
		log.info("onUaCallAccepted");
	}

	/* (non-Javadoc)
	 * @see local.ua.UserAgentListener#onUaCallCancelled(local.ua.UserAgent)
	 */
	
	public void onUaCallCancelled(UserAgent arg0) {
		log.info("onUaCallCancelled");
		//Liberamos cliente
		freeListen(arg0);
	}

	/* (non-Javadoc)
	 * @see local.ua.UserAgentListener#onUaCallClosed(local.ua.UserAgent)
	 */
	
	public void onUaCallClosed(UserAgent arg0) {
		log.info("onUaCallClosed");
		//Liberamos cliente
		freeListen(arg0);
	}

	/* (non-Javadoc)
	 * @see local.ua.UserAgentListener#onUaCallFailed(local.ua.UserAgent)
	 */
	
	public void onUaCallFailed(UserAgent arg0) {
		log.info("onUaCallFailed");
		//Liberamos cliente
		freeListen(arg0);
	}

	/* (non-Javadoc)
	 * @see local.ua.UserAgentListener#onUaCallRinging(local.ua.UserAgent)
	 */
	
	public void onUaCallRinging(UserAgent arg0) {
		log.info("onUaCallRinging");
	}
	
	/* (non-Javadoc)
	 * @see local.ua.UserAgentListener#onUaCallTrasferred(local.ua.UserAgent)
	 */
	
	public void onUaCallTrasferred(UserAgent arg0) {
		log.info("onUaCallTrasferred");
	}

	/* (non-Javadoc)
	 * @see local.ua.UserAgentListener#onUaCallIncoming(local.ua.UserAgent, org.zoolu.sip.address.NameAddress, org.zoolu.sip.address.NameAddress)
	 */
	
	public void onUaCallIncoming(UserAgent arg0, SessionDescriptor remote_sdp, NameAddress arg1, NameAddress arg2) {
		log.info("onUaCallIncoming: " + arg1 + ":" + arg2);
		//Generamos un cliente de isabel SIP
		Vector<FlowInfo> flowList = new Vector<FlowInfo>();
		SessionDescriptor clientSdp = new SessionDescriptor(arg0.getSessionDescriptor());
		OriginField o = clientSdp.getOrigin();
		if ((o.getAddressType().equals("IP6") && System.getenv("SIP_IPV6")==null) || 
			(o.getAddressType().equals("IP4") && System.getenv("SIP_IPV6")!=null)) {
			log.warning("Wrong IP protocol from "+ arg1 + ":" + arg2 + " Type " + o.getAddressType());
			/*
			// Reject call
			arg0.setSessionDescriptor(sdp.toString());
			arg0.hangup();
			arg0.listen();
			return;
			*/
			
		}
		
		Vector<MediaDescriptor> medias = clientSdp.getMediaDescriptors();
		for (MediaDescriptor media : medias )
		{
			Vector<String> formats = media.getMedia().getFormatList();
			for (String format : formats)
			{
				log.info("onUaCallIncoming: adding media " + format);
				flowList.add(new FlowInfo(Integer.parseInt(format),
						                  media.getMedia().getPort(),
						                  remote_sdp.getMediaDescriptor(media.getMedia().getMedia()).getMedia().getPort(), -1));
			}
		}
		//Creamos la informacion del miembro
		MemberInfo info = new MemberInfo(-1/*SSRC me lo da Isabel*/,arg2.getAddress().getUserName(),o.getAddress(),flowList);
		IsabelUserAgent isabelUA = getIsabelUserAgent(arg0,freeUserAgents);
		isabelUA.setMemberInfo(info);
		incomingListen(isabelUA);
		//Realizamos peticion de conexion al gateway
		gapi.connect(info.getNickName(),"" /*No password for now*/,info);								
	}

	/* (non-Javadoc)
	 * @see local.ua.RegisterAgentListener#onUaRegistrationFailure(local.ua.RegisterAgent, org.zoolu.sip.address.NameAddress, org.zoolu.sip.address.NameAddress, java.lang.String)
	 */
	
	public void onUaRegistrationFailure(RegisterAgent arg0, NameAddress arg1,
			NameAddress arg2, String arg3) {
		log.info("onUaRegistrationFailure: " + arg1 + ":" + arg2 + ":" + arg3);
	}

	/* (non-Javadoc)
	 * @see local.ua.RegisterAgentListener#onUaRegistrationSuccess(local.ua.RegisterAgent, org.zoolu.sip.address.NameAddress, org.zoolu.sip.address.NameAddress, java.lang.String)
	 */
	
	public void onUaRegistrationSuccess(RegisterAgent arg0, NameAddress arg1,
			NameAddress arg2, String arg3) {
		log.info("onUaRegistrationSuccess: " + arg1 + ":" + arg2 + ":" + arg3);		
	}

	@Override
	public void isabelChange(SessionEvent event) {
		
	}

	@Override
	public void parameterChange(String user, String name, Object value) {
		
	}
}
