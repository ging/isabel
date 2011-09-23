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
package isabel.gwsip;

import isabel.gw.MemberInfo;
import local.ua.UserAgent;

public class IsabelUserAgent {

	private UserAgent  ua;
	private MemberInfo mi;
	
	public IsabelUserAgent(UserAgent ua, MemberInfo mi) {
		this.ua = ua;
		this.mi = mi;
	}
	
	public MemberInfo getMemberInfo(){ return mi; }
	public UserAgent  getUserAgent(){ return ua; }
	public void setMemberInfo(MemberInfo mi){ this.mi = mi; }
	public void setUserAgent(UserAgent ua){ this.ua = ua; }
}
