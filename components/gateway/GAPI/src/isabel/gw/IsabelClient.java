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

import isabel.gw.isabel_client.IsabelClientImpl;

/**
 * This class is used to create simulated isabel clients.
 * 
 * These clients communicate with the isabel conference via the SeCo service.
 * 
 * A client connect with the isabel session when the connect method is called,
 * and disconnect from the conference when the disconnect method is called.
 * While connected to the conference, the client sends event to the registered
 * IsabelClientListener objects, to inform about what is happening in the
 * conference.
 */
public abstract class IsabelClient {


	/**
	 * Construct a new IsabelClient instance.
	 */
	public static IsabelClient create() {
		IsabelClientImpl ic = new IsabelClientImpl();
		return ic;
	}

	/**
	 * Connect me to the isabel session.
	 * 
	 * @return Result
	 * @param info
	 *            Member informattion.
	 */
	public abstract Result connect(MemberInfo info);

	/**
	 * Disconnect me from the isabel session.
	 * 
	 * @return Result
	 */
	public abstract Result disconnect();
	
	/**
	 * Send question to isabel session.
	 * 
	 * @return Result
	 */
	public abstract Result question();
	
	/**
	 * Set the VNC mode with the configured VNC
	 * @param password 
	 * @param server 
	 * @return Result
	 */
	public abstract Result setVNCMode(String server, String password);
	
	/**
	 * Set the chat mode
	 * @return Result
	 */
	public abstract Result setChatMode();

	/**
	 * Set the chat mode
	 * @return Result
	 */
	public abstract Result setQuestionsMode();
	
	/**
	 * Method used to inform of a property change in a isabel session.
	 * @param name Property name
	 * @param value new value of the property 
	 */
	public abstract Result setParameter(String name, Object value);
	
}
