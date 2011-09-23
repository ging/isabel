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

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;


public class ConfigurationTest {

	Configuration config;
	
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {
		try
		{
		    config = new Configuration("config.xml");
		}catch(ConfigException e){			
		}	
	}

	@After
	public void tearDown() throws Exception {
	}

	@Test
	public void testConfiguration() {
		config = null;
		try
		{
			config = new Configuration("kk");
		}catch(ConfigException e){			
		}
		assertNull(config);
		try
		{
		    config = new Configuration("config.xml");
		}catch(ConfigException e){			
		}
		assertTrue(config.isReady());
	}
	
	@Test
	public void testMcuPort() {
		int port = 0;
		try
		{
			port = config.getMcuPort();
		}catch(ConfigException e){			
		}
		assertEquals(1234, port);		
	}
	
	@Test
	public void testMcuAddr() {
		String addr = "";
		try
		{
			addr = config.getMcuAddr();
		}catch(ConfigException e){			
		}
		assertEquals("127.0.0.1",addr);
	}
	
	@Test
	public void testMcuExec() {
		String exec = "";
		try
		{
			exec = config.getMcuExec();
		}catch(ConfigException e){			
		}
		assertEquals("./mcu.exe",exec);
	}
}
