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
 * The isabelClient event class.
 */
public class IsabelClientEvent {

	/**
	 * Client information.
	 */
	MemberInfo   memberInfo;
	/**
	 * The IsabelClient sender.
	 */
	IsabelClient isabelClient;

	/**
	 * Constructor.
	 * @param        memberInfo
	 * @param        isabelClient
	 */
	public IsabelClientEvent( MemberInfo memberInfo, IsabelClient isabelClient )
	{
		this.isabelClient = isabelClient;
		this.memberInfo   = memberInfo;
	}

	/**
	 * Return the CMemberInfo object.
	 * @return       MemberInfo
	 */
	public MemberInfo getMemberInfo(  )
	{
		return memberInfo;
	}

	/**
	 * Returns the isabel client object event sender.
	 * @return       IsabelClient
	 */
	public IsabelClient getIsabelClient(  )
	{
		return isabelClient;
	}
}
