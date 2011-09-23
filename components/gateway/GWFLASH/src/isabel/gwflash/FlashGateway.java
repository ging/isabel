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
package isabel.gwflash;

import isabel.gw.FlowInfo;
import isabel.gw.GAPI;
import isabel.gw.GAPIFlowsEvent;
import isabel.gw.GAPIMemberEvent;
import isabel.gw.IGAPI;
import isabel.gw.IGAPIListener;
import isabel.gw.MemberInfo;
import isabel.gw.Result;
import isabel.gw.SessionEvent;
import isabel.gw.flows.FlowTypePayloads;
import isabel.igateway.IGateway;

import java.util.HashMap;
import java.util.Hashtable;
import java.util.Map;
import java.util.Vector;
import java.util.logging.Level;
import java.util.logging.Logger;

public class FlashGateway implements IGAPIListener, IFlashManagerListener 
{
	/**
	 * Lista de clientes en "espera de conexion"
	 */
	private Map<String, FlashUser> incomingUsers;
	
	/**
	 * Lista de clientes "conectados"
	 */
	private Map<String, FlashUser> connectedUsers;
	
	/**
	 * El gateway de Isabel.
	 */
	private IGAPI gapi;	
	/**
	 * Controlador de MCU
	 */
	private McuController mcuController;
	/**
	 * Logger.
	 */
	private Logger log;
	/**
	 * Punto de entrada del programa.
	 * @param args
	 */
	FlashManager flashManager;
	
	/**
	 * Constructor.
	 */
	public FlashGateway( )
    {
		//Inicializamos log
		log = Logger.getLogger("isabel.gwflash.FlashGateway");
		log.info("Starting Flash gateway...");
		
		mcuController = new McuController();		
		mcuController.initMcu();

		if (!FlashGatewayEnvConfig.disableFlash()) {
			//Listas de clientes
			incomingUsers =  new HashMap<String, FlashUser>();
			connectedUsers = new HashMap<String, FlashUser>();
						
			//Iniciamos el gapi
			gapi = GAPI.GetGateway();
		
			//Nos suscribimos a los eventos
			gapi.addGAPIListener(this);
			//Iniciamos el flash manager
		    flashManager = new FlashManager(gapi.getButtonNames());
		    //Nos suscribimos a los eventos
			flashManager.setListener(this);
		}
		//SilenceSnder
		try {
			new SilenceSender().go();
		} catch (Exception e) {
			log.log(Level.WARNING, "ERROR creating SilenceSender", e);
		}
		
		if (FlashGatewayEnvConfig.enableIGW()) {
			//IGateway
			try {
				new IGateway().go();
			} catch (Exception e) {
				log.log(Level.WARNING, "ERROR creating iGateway", e);
			}
		}
	}

	/* (non-Javadoc)
	 * @see isabel.gw.IGAPIListener#getName()
	 */
	public String getName()
	{
		return "Flash_Gateway";
	}
	
	/* (non-Javadoc)
	 * @see isabel.gwflash.IFlashManagerListener#onUserConnection(isabel.gwflash.FlashUser)
	 */
	public void onUserConnection(FlashUser user)
	{
		log.info("onUserConnection");
		// FlashUser fu = getFlashUser(user.getMemberInfo(), null);
		//A�adir usuario a la lista de "esperando conexion"
		synchronized (this) {
			incomingUsers.put(user.getMemberInfo().getNickName(), user);	
		}
		//Realizamos peticion de conexion al gateway
		gapi.connect(user.getMemberInfo().getNickName(),"" /*No password for now*/,user.getMemberInfo());
	}
	
	/* (non-Javadoc)
	 * @see isabel.gwflash.IFlashManagerListener#onUserDisconnection(isabel.gwflash.FlashUser)
	 */
	public void onUserDisconnection(FlashUser user)
	{
		//Nos desconectamos del gateway
		gapi.disconnect(user.getMemberInfo().getSSRC());
	}
	
	/* (non-Javadoc)
	 * @see isabel.gwflash.IFlashManagerListener#onButtonPressed(java.lang.String, isabel.gwflash.FlashUser)
	 */
	public void onButtonPressed(String buttonName, FlashUser user)
	{
		gapi.buttonPressed(buttonName, user.getMemberInfo().getSSRC());
		
	}
	
	@Override
	public void onFlashClientSetParameter(FlashUser user, String name,
			Object value) {
		gapi.setParameter(user.getMemberInfo().getSSRC(), name, value);
		
	}
	
		
	
	/* (non-Javadoc) 
	 * @see isabel.gw.IGAPIListener#activeFlows(isabel.gw.GAPIFlowsEvent)
	 */	
	public Result activeFlows(GAPIFlowsEvent event) {
		Hashtable<Integer,Vector<FlowInfo>> flows = event.getFlowsInfo();
//		log.info("activeFlows --> " + flows);
		//Inicializamos el mapa
		HashMap<FlashUser, FlowsBool> usersMap = new HashMap<FlashUser, FlowsBool>();
		//Buscamos cada cliente en la lista de conectados.
		for (FlashUser user : connectedUsers.values())
		{	
			for (Integer ssrc : flows.keySet())
			{
				if (user.getMemberInfo().getSSRC() == ssrc)
				{
					usersMap.put(user, new FlowsBool());
					FlowsBool flowsBool = usersMap.get(user);
					//comprobamos los flujos activos del cliente flash
					Vector<FlowInfo> userFlows = flows.get(ssrc);
					for (FlowInfo flowInfo : userFlows)
					{
						switch (FlowTypePayloads.getTypeByPayloadType(flowInfo.getPT()))
						{
						case AUDIO_SOUND:
//							log.info("activo flujo de audio de " + user.getMemberInfo().getNickName());
							flowsBool.audio = true;
							break;
						case VIDEO_IMAGE:
//							log.info("activo flujo de video de " + user.getMemberInfo().getNickName());
							flowsBool.video = true;
							break;
						}
					}
				}
			}
		}
		flashManager.activeFlows(usersMap);		
		return (new Result(Result.OK_CODE,Result.OK_DESC));
	}

	/* (non-Javadoc)
	 * @see isabel.gw.IGAPIListener#addMember(isabel.gw.GAPIMemberEvent)
	 */
	public Result addMember(GAPIMemberEvent event) {
		FlashUser fu = getFlashUser(event.getMemberInfo(), incomingUsers);
		if (fu != null)
		{
			if (event.getMemberInfo().getSSRC() < 0)
			{
				log.info("addMember : " + event.getMemberInfo().getNickName() +":: Connection error.");
				//Informar del error
				flashManager.disconnect(fu,"Connection rejected.");									
			}else{
				log.info("addMember : " + event.getMemberInfo().getNickName());
				//Actualizamos informacion del cliente
				fu.setMemberInfo(event.getMemberInfo());
				//Aceptamos llamada
				flashManager.accept(fu);
			}
			//Mover user de lista de espera a conectados.
			synchronized (this) {
				connectedUsers.put(fu.getMemberInfo().getNickName(),incomingUsers.remove(fu.getMemberInfo().getNickName()));	
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
		FlashUser fu =  getFlashUser(event.getMemberInfo(), null);
		if (fu != null)
		{
			flashManager.disconnect(fu,"User disconnected.");
			//eliminar de todas las listas
			synchronized (this) {
				incomingUsers.remove(fu.getMemberInfo().getNickName());
				connectedUsers.remove(fu.getMemberInfo().getNickName());
			}
		}	
		return (new Result(Result.OK_CODE,Result.OK_DESC));
	}
	
	/**
	 * Method used to inform of an event in the isabel session such as a mode change.
	 * This event can be used by a session recorder to annotate the video.
	 * @param event The event object
	 */	
	public void isabelChange(SessionEvent event) {
		flashManager.isabelChange(event);
	}	
	
	@Override
	public void parameterChange(String user, String name, Object value) {
		flashManager.parameterChange(user, name, value);
	}	
	
	/**
	 * Method to get the flash user from an specific list.
	 * @param memberInfo
	 * @param list
	 * @return the flash user, if not found returns "null".
	 */
	private synchronized FlashUser getFlashUser(MemberInfo memberInfo, Map<String, FlashUser> list) {
		
		FlashUser result = null;
		if (list != null) {
			result = list.get(memberInfo.getNickName());
		}
		else {
			result = incomingUsers.get(memberInfo.getNickName());
			if (result == null)
				result = connectedUsers.get(memberInfo.getNickName());
		}
		return result;
	}
		
	/**
	 * Main application method.
	 * @param args
	 */
	public static void main(String [ ] args)
	{
		new FlashGateway();		
	}
}
