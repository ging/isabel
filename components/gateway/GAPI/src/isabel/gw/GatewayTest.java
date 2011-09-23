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



import java.util.Vector;
import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import isabel.gw.isabel_client.IsabelClientImpl;

public class GatewayTest {

	private class GWListener implements IGAPIListener
	{
		public GAPIMemberEvent e1 = null;
		public GAPIMemberEvent e2 = null;
		public GAPIFlowsEvent  e3 = null;
		public String getName(){ return "GWListener"; }
		public Result addMember( GAPIMemberEvent event ){e1 = event; return new Result(Result.OK_CODE,Result.OK_DESC);}
		public Result delMember( GAPIMemberEvent event ){e2 = event; return new Result(Result.OK_CODE,Result.OK_DESC);}
		public Result activeFlows( GAPIFlowsEvent event ){e3 = event; return new Result(Result.OK_CODE,Result.OK_DESC);}
		public void isabelChange(SessionEvent event) {}
		@Override
		public void parameterChange(String user, String name, Object value) {
			// TODO Auto-generated method stub
			
		}
	}

	GAPIImplementation gateway = new GAPIImplementation();
	GWListener listener;
	
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
					
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {
		 listener = new GWListener();
	}

	@After
	public void tearDown() throws Exception {
	}
	
	@Test
	public void testConnect() {
		fail("Not yet implemented");
	}
	
	@Test
	public void testDisconnect() {
		fail("Not yet implemented");
	}

	@Test
	public void testAddGAPIListener() {
		Vector<FlowInfo> flowList = new Vector<FlowInfo>();
		flowList.add(new FlowInfo(11, 1234, 1235, 1024));
		flowList.add(new FlowInfo(12, 1234, 1235, 1024));
		flowList.add(new FlowInfo(13, 1234, 1235, 1024));
		MemberInfo info = new MemberInfo(5556, "testMember", "138.2.3.4", flowList);		
		IsabelClient isabelClient = new IsabelClientImpl();
		gateway.addGAPIListener(listener);
		IsabelClientEvent event = new IsabelClientEvent(info, isabelClient);
		gateway.newIsabelSite(event);
		
		assertTrue(listener.e1.getGateway()==gateway);
		assertTrue(listener.e1.getMemberInfo()==info);		
	}

	@Test
	public void testRemoveGAPIListener() {
		Vector<FlowInfo> flowList = new Vector<FlowInfo>();
		flowList.add(new FlowInfo(11, 1234, 1235, 1024));
		flowList.add(new FlowInfo(12, 1234, 1235, 1024));
		flowList.add(new FlowInfo(13, 1234, 1235, 1024));
		MemberInfo info = new MemberInfo(5556, "testMember", "138.2.3.4", flowList);		
		IsabelClient isabelClient = IsabelClient.create();
		gateway.removeGAPIListener(listener);
		IsabelClientEvent event = new IsabelClientEvent(info, isabelClient);
		gateway.newIsabelSite(event);
		
		assertNull(listener.e1);
		assertNull(listener.e2);
		assertNull(listener.e3);
	}

	@Test
	public void testNewIsabelSite() {
		testAddGAPIListener();		
	}

	@Test
	public void testSiteDisconnection() {
		Vector<FlowInfo> flowList = new Vector<FlowInfo>();
		flowList.add(new FlowInfo(11, 1234, 1235, 1024));
		flowList.add(new FlowInfo(12, 1234, 1235, 1024));
		flowList.add(new FlowInfo(13, 1234, 1235, 1024));
		MemberInfo info = new MemberInfo(5556, "testMember", "138.2.3.4", flowList);		
		IsabelClient isabelClient = IsabelClient.create();
		gateway.addGAPIListener(listener);
		IsabelClientEvent event = new IsabelClientEvent(info, isabelClient);
		gateway.siteDisconnection(event);
		
		assertTrue(listener.e2.getGateway()==gateway);
		assertTrue(listener.e2.getMemberInfo()==info);		
	}

	@Test
	public void testSiteReady() {
		Vector<FlowInfo> flowList = new Vector<FlowInfo>();
		flowList.add(new FlowInfo(11, 1234, 1235, 1024));
		flowList.add(new FlowInfo(12, 1234, 1235, 1024));
		flowList.add(new FlowInfo(13, 1234, 1235, 1024));
		MemberInfo info = new MemberInfo(5556, "testMember", "138.2.3.4", flowList);		
		IsabelClient isabelClient = IsabelClient.create();
		gateway.addGAPIListener(listener);
		IsabelClientEvent event = new IsabelClientEvent(info, isabelClient);
		gateway.siteReady(event);
		
		assertTrue(listener.e1.getGateway()==gateway);
		assertTrue(listener.e1.getMemberInfo()==info);		
	}

	@Test
	public void testEnableFlow() {
		Vector<FlowInfo> flowList = new Vector<FlowInfo>();
		flowList.add(new FlowInfo(11, 1234, 1235, 1024));
		flowList.add(new FlowInfo(12, 1234, 1235, 1024));
		flowList.add(new FlowInfo(13, 1234, 1235, 1024));
		MemberInfo info1 = new MemberInfo(5557, "testMember1", "138.2.3.5", flowList);		
		MemberInfo info2 = new MemberInfo(5556, "testMember2", "138.2.3.6", flowList);
		IsabelClient isabelClient = IsabelClient.create();
		IsabelClientEvent event;
		event = new IsabelClientEvent(info1, isabelClient);
		gateway.siteReady(event);
		event = new IsabelClientEvent(info2, isabelClient);
		gateway.siteReady(event);
		Result ret = gateway.enableFlow(11, 5556, 5557);
		assertTrue(ret.succeded());
	}

	@Test
	public void testDisableFlow() {
		Result ret = gateway.disableFlow(11, 5556, 5557);
		assertTrue(ret.succeded());
	}

	@Test
	public void testSetupVideo() {
		Result ret = gateway.setupVideo(5556, VideoMode.COMMON_GRID_MODE, 96, -1, 30, -1, 320, 240, 5556, GridMode.GRID_AUTO);
		assertTrue(ret.succeded());
	}

	@Test
	public void testSetupAudio() {
		Result ret = gateway.setupAudio(5556, AudioMode.MIX_MODE, 1, 30);
		assertTrue(ret.succeded());
	}

}
