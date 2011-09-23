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
import isabel.gw.MemberInfo;
import isabel.gw.SessionEvent;
import isabel.gwflash.red5client.IGWRed5UserManagerListener;
import isabel.gwflash.red5client.Red5UserManager;

import java.net.MalformedURLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Vector;
import java.util.logging.Logger;

public class FlashManager implements IGWRed5UserManagerListener {
		
	HashMap<String, FlashUser> users;	
	Red5UserManager flashUsersManager;	
	IFlashManagerListener listener;
	IsabelEventManager eventManager;
	private Logger log = Logger.getLogger("isabel.gwflash.FlashManager");

	//HashMap<FlashUser, FlowsBool> currentActiveFlows;
		
	
	/**
	 * Crea una nueva instancia de FlashManager
	 * 
	 */
	public FlashManager(String[] buttonNames){
		users = new HashMap<String, FlashUser>();
		eventManager = new IsabelEventManager();
		try {
			
			String flashURL = FlashGatewayEnvConfig.getFlashServerURL();			
			log.info("FLASH SERVER URL : " + flashURL);
			flashUsersManager = new Red5UserManager(flashURL, buttonNames);
		} catch (MalformedURLException e) {
			e.printStackTrace();
			throw new RuntimeException(e.getMessage());
		}
		flashUsersManager.setListener(this);
	
	}
		
	/**
	 * Fija el Listener
	 * @param listener
	 */
	public void setListener(IFlashManagerListener listener) {
		this.listener = listener;
		
	}
	
	public void activeFlows(HashMap<FlashUser, FlowsBool> usersMap)
	{
		flashUsersManager.updateStreams(usersMap);
	}
	
	public void onFlashClientConnect(String Username, String audio, String video) { 
		
		FlashUser user = new FlashUser();
		Vector<FlowInfo> flows = new Vector<FlowInfo>();
		if (!audio.equals("none")){
			flows.add(new FlowInfo(102,1234,1234,-1)); //Audio
		}
		if (!video.equals("none")){
			flows.add(new FlowInfo(96,1234,1234,-1));  //Video
		}
		MemberInfo info = new MemberInfo(-1, Username, "127.0.0.3", flows);		
		//users.lastElement().setMemberInfo(info);
		user.setMemberInfo(info);
		users.put(Username, user);		
		listener.onUserConnection(users.get(Username));
		
	}
	
	public void onFlashClientDisconnect(String Username) {
		FlashUser user = users.get(Username);
		if (users.get(Username)==null){
			System.out.println("Usuario " + Username + " a desconectar no encontrado");
			return;
		}
		log.info("Desconectando a " + Username);
		listener.onUserDisconnection(user);	
	}
	
	public void onFlashClientButtonPressed(String buttonName, String Username)
	{
		FlashUser user = users.get(Username);
		if (users.get(Username)==null){
			log.info("Usuario " + Username + " tiene dudas existenciales y de gran importancia, pero no lo encuentro en mi lista!");
			return;
		}
		log.info("Pregunta existencial desde " + Username);
		listener.onButtonPressed(buttonName, user);		
	}
	
	public void accept(FlashUser fu)	{		
		flashUsersManager.newUser(fu.getMemberInfo().getNickName(), fu.getMemberInfo().getSSRC());	    
	}
	
	public void disconnect(FlashUser fu, String reason)
	{
		flashUsersManager.disconnectUser(fu.getMemberInfo().getNickName(), reason);
		System.out.println("Parando medios de " + fu.getMemberInfo().getNickName());
		System.out.println("Desconectando a " + fu.getMemberInfo().getNickName());
	}
	
	/**
	 * Method used to inform of an event in the isabel session such as a mode change.
	 * This event can be used by a session recorder to annotate the video.
	 * @param event The event object
	 */	
	public void isabelChange(SessionEvent event) { 
		log.info("EVENTO------ > " + event);

		// Obtengo la lista de escritorios activos y filtro los usuarios
		// locales
		List<String> list = new ArrayList<String>();

		for (String s: event.getActiveDesktops()) {
			if (users.containsKey(s))
				list.add(s);
		}
			
		log.info("EVENTO------ > Active Desktops: " + list);
		flashUsersManager.activateDesktop(list);
		
		// Paso el evento al gestor de eventos
		eventManager.isabelEvent(event);
		
	}
	
	public void parameterChange(String user, String name, Object value) {
		flashUsersManager.parameterChange(user, name, value);
		
	}

	@Override
	public void onFlashClientSetParameter(String userName, String name,
			Object value) {
		FlashUser user = users.get(userName);
		if (users.get(userName)==null){
			log.info("Usuario " + userName + " ha cambiado algo pero no lo encuentro en mi lista!");
			return;
		}
		listener.onFlashClientSetParameter(user, name, value);
	}
}
