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
import isabel.gw.flows.ComponentFlowTypes.IsabelComponent;

import java.util.Set;

import org.junit.Test;

import static org.junit.Assert.*;

public class ComponentFlowTypesTest {


	/**
	 * Check that returns the flow types used by the component.
	 */
	@Test
	public void checkReturnedUsedFlowTypes() {

		Set<FlowType> flows = ComponentFlowTypes.getFlowTypes(IsabelComponent.Audio);
		
		assertTrue(flows.contains(FlowType.AUDIO_SOUND));
		assertTrue(flows.contains(FlowType.AUDIO_VUMETER));
		}
	

	/**
	 * Check that does not return flow types not used by the component.
	 */
	@Test
	public void checkNoReturnedUnusedFlowTypes() {

		Set<FlowType> flows = ComponentFlowTypes.getFlowTypes(IsabelComponent.Audio);
		
		assertFalse(flows.contains(FlowType.VIDEO_IMAGE));
	}
	
}