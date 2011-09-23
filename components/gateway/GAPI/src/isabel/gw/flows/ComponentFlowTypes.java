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

import java.util.Map;
import java.util.HashMap;
import java.util.Set;
import java.util.HashSet;

/**
 * This class contains a map to associate the isabel components with their used
 * flow types.
 * 
 * This class also define enumerations for the isabel component names and the
 * flow types used by these components.
 */
public class ComponentFlowTypes {

	/**
	 * Isabel components
	 */
	public enum IsabelComponent {
	    Video, Audio, Questions, ClassCtrl, ConferenceCtrl
	};

	/**
	 * Flow types
	 */
	public enum FlowType {
		VIDEO_IMAGE, AUDIO_SOUND, AUDIO_VUMETER, UNKNOWN_TYPE
	};

	/**
	 * Map to store the flow types used by each Isabel component.
	 * 
	 * The key is a IsabelComponent object, and the value is a set of flow
	 * types.
	 */
	static Map<IsabelComponent, Set<FlowType>> component_flowTypes = new HashMap<IsabelComponent, Set<FlowType>>();

	/**
	 * Static constructor to fill the component_flowTypes map.
	 */
	static {
		add(IsabelComponent.Video, FlowType.VIDEO_IMAGE);
		add(IsabelComponent.Audio, FlowType.AUDIO_SOUND);
		add(IsabelComponent.Audio, FlowType.AUDIO_VUMETER);
	}

	/**
	 * Stores the given IsabelComponent and the given flow type in the
	 * flowTypesComponents map.
	 * 
	 * @param component
	 *            an IsabelComponent object.
	 * @param flowType
	 *            a Flow type used by the given component.
	 */
	private static void add(IsabelComponent component, FlowType flowType) {

		Set<FlowType> fts = component_flowTypes.get(component);

		if (fts == null) {
			fts = new HashSet<FlowType>();
			component_flowTypes.put(component, fts);
		}
		fts.add(flowType);
	}

	/**
	 * Returns a set with the flow types of the given component.
	 * @param component a component.
	 * @return a set with the flow types of the given component.
	 */
	public static Set<FlowType> getFlowTypes(IsabelComponent component) {

		Set<FlowType> fts = component_flowTypes.get(component);

		if (fts == null) {
			fts = new HashSet<FlowType>();
		}
		
		return fts;
	}
}
