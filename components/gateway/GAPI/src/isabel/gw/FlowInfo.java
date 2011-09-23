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

/**
 * This class stores data flow related information for each session member.
 */
public class FlowInfo {

	/**
	 * RTP Payloadtype field. 
	 */
	private int pt;
	/**
	 * Flow local port.
	 */
	private int localPort;
	/**
	 * Flow remote port..
	 */
	private int remotePort;
	/**
	 * Flow bandwidth.
	 */
	private int bw;
	/**
	 * Constructor.
	 * @param        pt RTP payloadtype field.
	 * @param        localPort The local port where gateway will receive the data flow.
	 * @param        remotePort The remote port where external node will receive the data flow.
	 * @param        bw Bandwidth in bps. (a 0 value means unknown bandwidth).
	 */
	public FlowInfo( int pt, int localPort, int remotePort, int bw )
	{
		this.pt = pt;
		this.localPort = localPort;
		this.remotePort = remotePort;
		this.bw = bw;
	}

	/**
	 * Returns the RTP payloadtype field.
	 * @return       byte RTP payloadtype field.
	 */
	public int getPT(  )
	{
		return pt;
	}

	/**
	 * Returns the local port.
	 * @return       int The local port where gateway will receive the data flow.
	 */
	public int getLocalPort(  )
	{
		return localPort;
	}

	/**
	 * Returns the remote port.
	 * @return       int The remote port where external node will receive the data flow.
	 */
	public int getRemotePort(  )
	{
		return remotePort;
	}

	/**
	 * Returns the flow bandwidth.
	 * @return       int Bandwidth in bps. (a 0 value means unknown bandwidth).
	 */
	public int getBW(  )
	{
		return bw;
	}

	/**
	 * Returns a String representing this object
	 */
	public String toString() {
		return "FI(" + pt + "," + localPort + "," + remotePort + "," + bw + ")";
	}

}
