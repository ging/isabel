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
/**
 * 
 */
package isabel.gw;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

/**
 * @author sirvent
 *
 */
public class ResultTest {


	Result result;
	/**
	 * @throws java.lang.Exception
	 */
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	/**
	 * @throws java.lang.Exception
	 */
	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp() throws Exception {
	}

	/**
	 * @throws java.lang.Exception
	 */
	@After
	public void tearDown() throws Exception {
	}

	/**
	 * Test method for {@link isabel.gw.Result#Result(int, java.lang.String)}.
	 */
	@Test
	public void testResult() {
		result = new Result(Result.OK_CODE,Result.OK_DESC);
		assertNotNull(result);
	}

	/**
	 * Test method for {@link isabel.gw.Result#succeded()}.
	 */
	@Test
	public void testSucceded() {
		result = new Result(Result.OK_CODE,Result.OK_DESC);
		assertTrue(result.succeded());
		result = new Result(Result.ERROR,Result.ERROR_DESC);
		assertFalse(result.succeded());
	}

	/**
	 * Test method for {@link isabel.gw.Result#getCode()}.
	 */
	@Test
	public void testGetCode() {
		result = new Result(Result.OK_CODE,Result.OK_DESC);
		assertEquals(Result.OK_CODE,result.getCode());
	}

	/**
	 * Test method for {@link isabel.gw.Result#getDescription()}.
	 */
	@Test
	public void testGetDescription() {
		result = new Result(Result.OK_CODE,"Descripcion de prueba");
		assertEquals("Descripcion de prueba",result.getDescription());
	}

}
