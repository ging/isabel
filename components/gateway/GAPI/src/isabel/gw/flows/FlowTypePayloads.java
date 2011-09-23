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
package isabel.gw.flows;

import isabel.gw.flows.ComponentFlowTypes.FlowType;

import java.util.Map;
import java.util.HashMap;
import java.util.List;
import java.util.ArrayList;
import java.util.Set;
import java.util.HashSet;

/**
 * This class contains a map to declare which payload types can be used by each
 * flow type.
 * 
 */
public class FlowTypePayloads {

	/**
	 * Map to store the payloads used by each flow type.
	 * 
	 * The map key is a flowtype, and the associated value is a list of payload
	 * types. The fist value in the list is considered the default value.
	 */
	private static Map<ComponentFlowTypes.FlowType, List<Integer>> flowType_payloads = new HashMap<ComponentFlowTypes.FlowType, List<Integer>>();

	/**
	 * Static constructor:
	 * 
	 * - to fill the component_flowTypes map.
	 * 
	 * - to fill the flowType_payloads map.
	 */
	static {
		// video:
		add(ComponentFlowTypes.FlowType.VIDEO_IMAGE, 25);
		add(ComponentFlowTypes.FlowType.VIDEO_IMAGE, 26);
		add(ComponentFlowTypes.FlowType.VIDEO_IMAGE, 32);
		add(ComponentFlowTypes.FlowType.VIDEO_IMAGE, 96);
		add(ComponentFlowTypes.FlowType.VIDEO_IMAGE, 34);
		add(ComponentFlowTypes.FlowType.VIDEO_IMAGE, 98);
		//add(ComponentFlowTypes.FlowType.VIDEO_IMAGE, 102);
		add(ComponentFlowTypes.FlowType.VIDEO_IMAGE, 118);
		add(ComponentFlowTypes.FlowType.VIDEO_IMAGE, 118);

		// audio:
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 3);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 107);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 115);

		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 116);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 117);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 121);

		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 14);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 15);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 16);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 17);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 18);

		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 100);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 102);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 108);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 11);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 119);

		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 0);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 8);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 103);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 109);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 112);

		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 9);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 104);

		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 101);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 105);
		add(ComponentFlowTypes.FlowType.AUDIO_SOUND, 124);

		// vumeter:
		add(ComponentFlowTypes.FlowType.AUDIO_VUMETER, 22);
	}

	/**
	 * Stores the given flow type and the given payload type into the
	 * flowType_payloads map.
	 * 
	 * @param flowType
	 *            a FlowType value.
	 * @param payloadType
	 *            a payload type which can be used by the given flow type.
	 */
	private static void add(ComponentFlowTypes.FlowType flowType,
			Integer payloadType) {

		List<Integer> payloads = flowType_payloads.get(flowType);

		if (payloads == null) {
			payloads = new ArrayList<Integer>();
			flowType_payloads.put(flowType, payloads);
		}

		if (!payloads.contains(payloadType)) {
			payloads.add(payloadType);
		}
	}

	public static FlowType getTypeByPayloadType(Integer payloadType)
	{
		FlowType type;
		type = FlowType.UNKNOWN_TYPE;
		for (Map.Entry<FlowType, List<Integer>> flow :  flowType_payloads.entrySet())
		{
			if (flow.getValue().contains(payloadType))
			{
				type = flow.getKey();
			}
		}		
		return type;
	}
	/**
	 * Checks if a pair <flow type, payload type> has been added to the
	 * flowtype_payloads map.
	 * 
	 * @return true is the given flow type and payload type has been added to
	 *         the flowtype_payloads map.
	 */
	public static boolean exists(ComponentFlowTypes.FlowType flowType,
			Integer payloadType) {

		List<Integer> payloads = flowType_payloads.get(flowType);

		if (payloads == null)
			return false;

		return payloads.contains(payloadType);
	}

	/**
	 * Gets the payload type to be used by the given flow type when the given
	 * codec is selected.
	 * 
	 * @param flowType
	 *            A flow type value.
	 * @param codecName
	 *            A codec name.
	 * 
	 * @return Returns the payload type associated to the given flow type and
	 *         the given codec.
	 * 
	 *         It the flow type has not registered payloads, then -1 is
	 *         returned.
	 * 
	 *         If the codec is unknown, then the deault payload type is
	 *         returned.
	 * 
	 *         It the payload of the given codec is not registered by the given
	 *         flow type, then the method returns the default payload type of
	 *         the given flow type.
	 */
	public static int getPayloadType(ComponentFlowTypes.FlowType flowType,
			String codecName) {

		List<Integer> payloads = flowType_payloads.get(flowType);
		if (payloads == null)
			return -1;

		if (payloads.size() == 0)
			return -1;

		int default_pt = payloads.get(0);

		int pt = CodecPayload.getPayloadType(codecName);

		if (pt == -1)
			return default_pt;

		if (payloads.contains(pt))
			return pt;
		else
			return default_pt;
	}

	/**
	 * Gets the default payload type to be used by the given flow type.
	 * 
	 * @param flowType
	 *            A flow type value.
	 * 
	 * @return Returns the default payload type of the given flow type.
	 * 
	 *         It the flow type has not registered payloads, then -1 is
	 *         returned.
	 */
	public static int getPayloadType(ComponentFlowTypes.FlowType flowType) {

		List<Integer> payloads = flowType_payloads.get(flowType);
		if (payloads == null)
			return -1;

		if (payloads.size() == 0)
			return -1;

		return payloads.get(0);
	}

	/**
	 * Returns a set with all the declared payload types.
	 * 
	 * @return a set with all the declared payload types.
	 */
	public static Set<Integer> getPayloadTypes() {

		Set<Integer> res = new HashSet<Integer>();

		for (FlowType flowType : flowType_payloads.keySet()) {
			List<Integer> payloads = flowType_payloads.get(flowType);
			for (Integer payload : payloads) {
				res.add(payload);
			}
		}

		return res;
	}

	/**
	 * Returns a set with all the declared payload types for the given FlowType.
	 * 
	 * @return a set with all the declared payload types for the given FlowType.
	 */
	public static Set<Integer> getPayloadTypes(
			ComponentFlowTypes.FlowType flowType) {

		Set<Integer> res = new HashSet<Integer>();

		List<Integer> payloads = flowType_payloads.get(flowType);
		for (Integer payload : payloads) {
			res.add(payload);
		}

		return res;
	}

}
