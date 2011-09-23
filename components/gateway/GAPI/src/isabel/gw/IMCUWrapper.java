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
 * Interface to manage the MCU application.
 * 
 * 
 */
public interface IMCUWrapper {

  /**
   * This method setups the client video flows in the MCU.
   * 
   * This optional command is used to configure video data flows. If not
   * used, the video packets are sent without modifications, the gateway will act as a common
   * network router.
   * 
   * @return       Result 		The method result.
   * @param        id 			User identifier.
   * @param        video_mode   Video mode enumerator.
   * @param        pt   		Video codec payload (-1 to keep unchanged).
   * @param        bw   		Maximum bandwidth (-1 to keep unchanged).
   * @param        fr   		Frames per second (-1 to keep unchanged).
   * @param        Q   			Quality factor (-1 to keep unchanged).
   * @param        width   		Image width (-1 to keep unchanged).
   * @param        height   	Image height (-1 to keep unchanged).
   * @param        ssrc   		RTP SSRC channel for the output flow (-1 to set random SSRC).
   * @param        grid_mode	Mosaic mode enumerator (only if video_mode = GRID_MODE).
   */
  public Result setupVideo( int id, VideoMode video_mode, int pt, int bw, int fr, int Q, int width, int height, int ssrc, GridMode grid_mode );


  /**
   * This method setups the client audio flows in the MCU.
   * 
   * This command is used to configure audio flows. As the “SetupVideo”
   * command, this command is also optional. If not used, audio data packets will be sent without
   * any modification.
   * 
   * @return       Result 		The method result.
   * @param        id	        User identifier.
   * @param        audioMode	Audio mode enumerator.
   * @param        pt  			Audio codec payload (-1 to keep unchanged).
   * @param        sl			Silence detection level (-1 to disable silence detection feature).		
   *
   */
  public Result setupAudio( int id, AudioMode audioMode, int pt, int sl );

  /**
   * Enable flow routing between “OrigID” and “DestID”.
   * @return       Result 	The method result.
   * @param        PT	  	Payloadtype, identifies the codec.
   * @param        origID	Source node user identifier.
   * @param        destID	Destination node user identifier.
   * @param        changeSSRC Change RTP SSRC.
   */
  public Result enableFlow( int PT, int origID, int destID, boolean changeSSRC );


  /**
   * Disable flow routing between “OrigID” and “DestID”..
   * @return       Result 	The method result.
   * @param        PT		Payloadtype, identifies the codec.
   * @param        origID   Source node user identifier.
   * @param        destID   Destination node user identifier.
   */
  public Result disableFlow( int PT, int origID, int destID );


}
