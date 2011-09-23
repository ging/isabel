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

import java.util.Hashtable;
import java.util.Vector;

/**
 * This event informs the gateway application about the current active flows in the
 * session.
 */
public class GAPIFlowsEvent {

	/**
	 * Flows hash table indexed by ssrc.
	 */
	Hashtable<Integer , Vector<FlowInfo>> flows;
	
	/**
	 * Gateway sender.
	 */
	IGAPI gateway;

	
	/**
	 * Constructor.
	 * @param        gateway The IGAPI sender.
	 */
	public GAPIFlowsEvent( IGAPI gateway )
	{
		this.flows = new Hashtable<Integer,Vector<FlowInfo>>();
		this.gateway = gateway;
	}
	
	/**
	 * Constructor.
	 * @param        flows The data flows.
	 * @param        gateway The IGAPI sender.
	 */
	public GAPIFlowsEvent( Hashtable<Integer,Vector<FlowInfo>> flows, IGAPI gateway )
	{
		this.flows = flows;
		this.gateway = gateway;
	}

	/**
	 * @return       Hashtable The flows related to each ssrc.
	 */
	public Hashtable<Integer,Vector<FlowInfo>> getFlowsInfo(  )
	{
		return flows;
	}

	/**
	 * @return       Gateway The IGAPI sender.
	 */
	public IGAPI getGateway(  )
	{
		return gateway;
	}
}
