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


/**
 * This listener receives Gateway events.
 * 
 * This class offers the Gateway API commands to the Gateway application, which will
 * be able to configure any kind of conference scheme by means of this API.
 */
public interface IGAPIListener {

	/**
	 * This event is received each time a member joins a session.
	 * @return       Result The method result.
	 * @param        event User information data.
	 */
	public Result addMember( GAPIMemberEvent event );


	/**
	 * This event is received when a member leaves a session.
	 * @return       Result The method result.
	 * @param        event User information data.
	 */
	public Result delMember( GAPIMemberEvent event );


	/**
	 * This event informs the gateway application about the active flows in the
	 * session. This event will be used by the gateway application to configure the external
	 * nodes to send only the required flows to the session and optimize network bandwidth
	 * usage.
	 * @return       Result 	The method result.
	 * @param        event		Hash table with user identifier key and flow info value.
	 */
	public Result activeFlows( GAPIFlowsEvent event );
	
	/**
	 * This method is used by GAPI to get a name description of the listener.
	 * @return       String     Listener name. 
	 */
	public String getName();
	
	/**
	 * Method used to inform of an event in the isabel session such as a mode change.
	 * This event can be used by a session recorder to annotate the video.
	 * @param event The event object
	 */
	public void isabelChange(SessionEvent event);
	
	/**
	 * Method used to inform of a property change in a isabel session.
	 * @param user Username
	 * @param name Property name
	 * @param value new value of the property 
	 */
	public void parameterChange(String user, String name, Object value);

}
