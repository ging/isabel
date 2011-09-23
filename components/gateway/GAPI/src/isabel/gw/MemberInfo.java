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

/**
 * This class will store member information and is composed of following data
 * types.
 */
public class MemberInfo {

	/**
	 * Member identifier.
	 */
	int ssrc;
	/**
	 * Member name.
	 */
	String name;
	/**
	 * Member IP address.
	 */
	String addr;
	/**
	 * Member flow information vector.
	 */
	Vector<FlowInfo> flowList;

	/**
	 * Constructor
	 * 
	 * @param ssrc
	 *            Member identifier.
	 * @param name
	 *            Member name.
	 * @param addr
	 *            Member IP address.
	 * @param flowList
	 *            Member flow information vector.
	 */
	public MemberInfo(int ssrc, String name, String addr,
			Vector<FlowInfo> flowList) {
		this.ssrc = ssrc;
		this.name = name;
		this.addr = addr;
		this.flowList = flowList;
	}

	/**
	 * Returns the SSRC.
	 * 
	 * @return int
	 */
	public int getSSRC() {
		return ssrc;
	}

	/**
	 * Returns the NickName.
	 * 
	 * @return String
	 */
	public String getNickName() {
		return name;
	}

	/**
	 * Returns ip address.
	 * 
	 * @return String
	 */
	public String getAddr() {
		return addr;
	}

	/**
	 * Returns the client data flow list.
	 */
	public Vector<FlowInfo> getFlowList() {
		return flowList;
	}

	/**
	 * Returns a String representing this object
	 */
	public String toString() {
		return "MI(" + ssrc + "," + name + "," + addr + "," + flowList + ")";
	}
}
