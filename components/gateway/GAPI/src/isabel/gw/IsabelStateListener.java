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

/**
 * Isabel listener.
 * The listener will receive IsabelClientEvent, ShutdownEvent and SessionEvent events .
 */
public interface IsabelStateListener {

	/**
	 * IsabelClient calls this method every time an Isabel member joins the
	 * session.
	 * 
	 * @return Result
	 * @param event
	 */
	public Result newIsabelSite(IsabelClientEvent event);

	/**
	 * IsabelClient calls this method every time a member (external or internal)
	 * is disconnected.
	 * 
	 * @return Result
	 * @param event
	 */
	public Result siteDisconnection(IsabelClientEvent event);

	/**
	 * IsabelClient call this method every time an external node connects.
	 * 
	 * @return Result
	 * @param event
	 */
	public Result siteReady(IsabelClientEvent event);

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
	public Result activeFlows(IsabelClientEvent event);

	/**
	 * IsabelClient calls this method when the connection with the isabel
	 * session is lost, or it can not be established.
	 * 
	 * @param event
	 */
	public void shutdown(ShutdownEvent event);
	
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
