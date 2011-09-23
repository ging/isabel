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
package isabel.gw.isabel_client;

import java.util.*;
import isabel.gw.*;

/**
 * Stores information about the Isabel sites.
 * 
 * - sites: Map of MemberInfo objects for the connected sites.
 * 
 */
public class SitesInfo {

	/**
	 * Map with the information of the sites.
	 * 
	 * The key is the site_id of the site.
	 * 
	 * The associated value is a MemberInfo object.
	 */
	private static HashMap<String, MemberInfo> sites = new HashMap<String, MemberInfo>();

	/**
	 * Set containing the site_id of the GatewayClients
	 */
	private static Set<String> gwClients = new HashSet<String>();

	/**
	 * Declare a new connected site.
	 * 
	 * @return true if the site was not declared previously, so it has been
	 *         added now. false is the site was already declared.
	 */
	static synchronized boolean declareSite(int chId, String site_id,
			String addr, Vector<FlowInfo> flows, boolean isGatewayClient) {

		MemberInfo mi = sites.get(site_id);

		if (mi != null) {
			return false;
		}

		if (isGatewayClient)
			gwClients.add(site_id);

		mi = new MemberInfo(chId, site_id, addr, flows);
		sites.put(site_id, mi);
		return true;
	}

	/**
	 * Delete information of a disconnected site.
	 * 
	 * @return true if the site exists and it is removed now. false if the site
	 *         does not exists, maybe it was already removed.
	 */
	static synchronized boolean removeSite(String site_id) {

		gwClients.remove(site_id);

		return sites.remove(site_id) != null;
	}

	/**
	 * Returns true if the site is a GatewayClient
	 */
	static synchronized boolean isGatewayClient(String site_id) {

		return gwClients.contains(site_id);
	}

	/**
	 * Returns true if the site has been declared.
	 */
	static synchronized boolean isDeclared(String site_id) {

		return sites.containsKey(site_id);
	}

    /**
     * Returns a Set with the site_id of all the declared sites.
     */
    static synchronized Set<String> getDeclaredSites() {
	
	return sites.keySet();
    }
    
    /**
     * Returns the MemberInfo object associated to the given chId.
     */
    static synchronized MemberInfo getMemberInfo(String site_id) {
	return sites.get(site_id);
    }
    
	/**
	 * Returns the base ChId of the site with the given site_id. 
	 * If the site is unknown returns -1.
	 */
	public static synchronized int getChId(String site_id) {
		MemberInfo mi = sites.get(site_id);
		if (mi == null) {
			return -1;
		}
		return mi.getSSRC();
	}

	/**
	 * Returns the address of the site with the given site_id.
	 * If the site is unknown returns null.
	 */
	static synchronized String getAddr(String site_id) {
		MemberInfo mi = sites.get(site_id);
		if (mi == null) {
			return null;
		}
		return mi.getAddr();
	}
}
