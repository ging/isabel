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

import isabel.gw.flows.*;
import java.util.*;

/**
 * This class is used to get a Vector<FlowInfo> object describing the mixing of the
 * video and audio flows (of the real isabel sites) to be used by the MCU to
 * send them to the external (SIP, ..) clients.
 * 
 * @author santiago
 * 
 */
public class LocalMixer {

	/**
	 * Local Video port - (the mcu or gateway side).
	 */
	private final static int LOCAL_VIDEO_PORT = 61017;

	/**
	 * Local audio port - (the mcu or gateway side).
	 */
	private final static int LOCAL_AUDIO_PORT = 51001;

	/**
	 * Remote Video port - (the irouter or isabel side).
	 */
	private final static int REMOTE_VIDEO_PORT = 53025;

	/**
	 * Remote Video port2 - (video from isabel video component).
	 */
	private final static int REMOTE_VIDEO_PORT2 = 51005;

	/**
	 * Remote audio port - (the irouter or isabel side).
	 */
	private final static int REMOTE_AUDIO_PORT = 51002;


	
	/**
	 * Cache used to stores the isabel FlowInfo vector.
	 */
	private static Vector<FlowInfo> isabelFlowInfo;


	/**
	 * Returns the flowInfo objects used by the isabel sites.
	 */
	public static Vector<FlowInfo> getIsabelFlowsInfo() {

		if (isabelFlowInfo == null) {
			isabelFlowInfo = new Vector<FlowInfo>();

			// Video Flows:
			Set<Integer> vpayloads = FlowTypePayloads
					.getPayloadTypes(ComponentFlowTypes.FlowType.VIDEO_IMAGE);
			for (Integer payload : vpayloads) {
				isabelFlowInfo.add(new FlowInfo(payload, LOCAL_VIDEO_PORT, REMOTE_VIDEO_PORT, -1));
				isabelFlowInfo.add(new FlowInfo(payload, LOCAL_VIDEO_PORT, REMOTE_VIDEO_PORT2, -1));
			}

			// Audio Flows:
			Set<Integer> apayloads = FlowTypePayloads
					.getPayloadTypes(ComponentFlowTypes.FlowType.AUDIO_SOUND);
			for (Integer payload : apayloads) {
				isabelFlowInfo.add(new FlowInfo(payload, LOCAL_AUDIO_PORT, REMOTE_AUDIO_PORT, -1));
			}
		}

		return isabelFlowInfo;
	}
}
