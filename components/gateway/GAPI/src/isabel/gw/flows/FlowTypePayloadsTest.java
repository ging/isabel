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

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;

import org.junit.Test;

import static org.junit.Assert.*;

public class FlowTypePayloadsTest {

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {
	}

	@After
	public void tearDown() throws Exception {
	}

	/**
	 * Checks that the method "exists" can find existing payload type of a
	 * flowtype.
	 */
	@Test
	public void existingPayloadTypeIsFound() {

		assertTrue(FlowTypePayloads.exists(
				ComponentFlowTypes.FlowType.VIDEO_IMAGE, 100));
		assertTrue(FlowTypePayloads.exists(
				ComponentFlowTypes.FlowType.VIDEO_IMAGE, 102));
	}

	/**
	 * Checks that the method "exists" can not find non-existing payload type of a
	 * flowtype.
	 */
	@Test
	public void noExistingPayloadTypeIsNotFound() {

		assertFalse(FlowTypePayloads.exists(
				ComponentFlowTypes.FlowType.VIDEO_IMAGE, 200));
		assertFalse(FlowTypePayloads.exists(
				ComponentFlowTypes.FlowType.VIDEO_IMAGE, 400));

	}

	/**
	 * Checks that the method "getPayloadType" get the right payload type of a
	 * codec supported by a flow type.
	 * 
	 * @param flowType
	 * @param codecName
	 * @return
	 */
	@Test
	public void foundPayloadTypeOfSupportedCodec() {

		assertTrue(FlowTypePayloads.getPayloadType(
				ComponentFlowTypes.FlowType.VIDEO_IMAGE, "MJPEG") == 102);
	}

	/**
	 * Checks that the default payloads type is returned when asking for a codec
	 * not supported by a flow type.
	 */
	@Test
	public void defaultPayloadTypeForUnsupportedCodec() {

		assertTrue(FlowTypePayloads.getPayloadType(
				ComponentFlowTypes.FlowType.VIDEO_IMAGE, "SOME_UNKNOWS_CODEC") == 100);
	}

	/**
	 * Checks that the dafeult payloadtype is returned if no codec is given.
	 */
	@Test
	public void defaultPayloadTypeWhenNoCodec() {

		assertTrue(FlowTypePayloads.getPayloadType(
				ComponentFlowTypes.FlowType.VIDEO_IMAGE) == 100);
	}
	
}
