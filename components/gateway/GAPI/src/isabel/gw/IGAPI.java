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

import isabel.gw.Configuration;

/**
 * GAPI interface used by the gateway application.
 * 
 * This class offers the Gateway API commands to the Gateway application, which will
 * be able to configure any kind of conference scheme by means of this API.
 */
public interface IGAPI extends IMCUWrapper {

   
  /**
   * Connects an external node to the Isabel session.
   * @return       Result The method result.
   * @param        login User login.
   * @param        password User password.
   * @param        info User info.
   */
  public Result connect( String login, String password, MemberInfo info );
  
  /**
   * Disconnects an external node from the Isabel session.
   * @return       Result The method result.
   * @param        ssrc User identifier.
   */
  public Result disconnect( int ssrc );
  
  /**
   * Send button pressed event to the Isabel session from an external client.
   * @return 	   Result The method result.
   * @param        buttonName Name of the pressed button 
   * @param		   ssrc User identifier.
   */
  public Result buttonPressed( String buttonName, int ssrc );
  
  /**
   * Return the names of the buttons to be shown to the gateway clients
   * @return Array of button names. The first string in the array corresponds to the service
   * name and NOT to any button.
   */
  public String[] getButtonNames();
  
  /**
   * Add a IGAPIListener.
   * @return       Result
   * @param        listener
   */
  public Result addGAPIListener( IGAPIListener listener );

  /**
   * Remove IGAPIListener.
   * @return       Result
   * @param        listener
   */
  public Result removeGAPIListener( IGAPIListener listener );
  
  /**
   * Enable flow routing between origID and destID
   * @return       Result 	The method result.
   * @param        PT	  	Payloadtype, identifies the codec.
   * @param        origID	Source node user identifier (ssrc).
   * @param        destID	Destination node user identifier (ssrc).
   */
  public Result enableFlow( int PT, int origID, int destID);
  
  /**
   * Disable flow routing between origin and destiny
   * 
   * @return Result
   * @param PT
   *            RTP Playloadtype identifyer.
   * @param origID
   *            Source member identifyer.
   * @param destID
   *            Destination member identifyer.
   */
  public Result disableFlow(int PT, int origID, int destID);
  
  /**
   * Get GAPI configuration.
   * @return       Configuration.
   */
  public Configuration getConfig();
  
  /**
   * Get Mcu wrapper (allows mcu fine tunning).
   * @return mcu wrapper.
   */
  public McuWrapper getMcuWrapper();
  
  /**
   * Method used to inform of a property change in a isabel session.
   * @param ssrc User id
   * @param name Property name
   * @param value new value of the property 
   * @return Result The method result. 
   */
  public Result setParameter(int ssrc, String name, Object value);
    
}
