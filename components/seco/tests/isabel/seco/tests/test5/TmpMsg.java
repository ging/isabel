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
package isabel.seco.tests.test5;

import isabel.seco.network.Network;
import isabel.seco.network.javaser.JavaMessage;

public class TmpMsg implements JavaMessage {

	public long id;
	private int from;
	
	public byte[] data;

	public TmpMsg(long id) {
		this.id = id;
	}
	
	
	public TmpMsg(long id, int from) {
		this.id = id;
		this.from = from;
	}
	
	protected long getId(){
		return id;
	}

	public int getFrom(){
		return from;
	}
	
	public void setId (long id){
		this.id =id;
	}
	public void setData(byte[] data){
		this.data = data;
	}

}
