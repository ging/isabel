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

/** 
 * Event sent by the isabel client object when the connection with isabel session is lost, or can not be established.
 * 
 * @author santiago
 *
 */
public class ShutdownEvent {

	/**
	 * Reason of generating this event.
	 */
	private String reason;
	
	/**
	 * Construct a new object.
	 */
	public ShutdownEvent(String reason) {
		this.reason = reason;
	}
	
	/**
	 * Returns the reason attribute.
	 * @return The reason attribute.
	 */
	public String getReason() {
		return reason;
	}
	
}
