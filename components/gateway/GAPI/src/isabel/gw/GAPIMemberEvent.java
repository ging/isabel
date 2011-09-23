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
 * This events is sent every time a member connects and disconnects.
 */
public class GAPIMemberEvent {

	/**
	 * Client information.
	 */
	MemberInfo memberInfo;
	/**
	 * Gateway that generated the event.
	 */
	IGAPI    gateway;

	/**
	 * Constructor.
	 * @param        memberInfo The member information.
	 * @param        gateway The IGAPI sender.
	 */
	public GAPIMemberEvent( MemberInfo memberInfo, IGAPI gateway )
	{
		this.memberInfo = memberInfo;
		this.gateway    = gateway;
	}

	/**
	 * Returns the member information.
	 * @return       MemberInfo The member information.
	 */
	public MemberInfo getMemberInfo(  )
	{
		return memberInfo;
	}


	/**
	 * Returns IGAPI interface of the event sender.
	 * @return       Gateway The IGAPI sender.
	 */
	public IGAPI getGateway(  )
	{
		return gateway;
	}
}
