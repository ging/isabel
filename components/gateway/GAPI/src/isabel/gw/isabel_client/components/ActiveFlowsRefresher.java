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
package isabel.gw.isabel_client.components;

import isabel.gw.IsabelState;
import isabel.gw.FlowInfo;
import isabel.gw.IsabelClientEvent;
import isabel.gw.IsabelStateListener;
import isabel.gw.MemberInfo;

import isabel.gw.flows.ComponentFlowTypes.FlowType;
import isabel.gw.flows.FlowTypePayloads;
import isabel.gw.isabel_client.IsabelClientImpl;

import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.util.Vector;

import java.util.logging.Logger;

/**
 * Object of this class are associated to an IsabelClient object, and it collect
 * information for all the components in order to create a ActiveFlows event
 * with information of all the flows that this site must use. All the components
 * call to the "update" methods to update the payload types (and bandwidth) of
 * the flow types related with them. Then, an activeFlows events is send
 * periodically to the IsabelStateListener objects.
 * 
 * @author santiago
 * 
 */
class ActiveFlowsRefresher extends Thread {

	/**
	 * Logs
	 */
	private Logger mylogger;

	/**
	 * The IsabelClient who I belong to.
	 */
	private IsabelClientImpl ic;

	/**
	 * Flag to indicate when an active flows event must be sent to the
	 * listeners.
	 */
	private boolean needRefresh;

	/**
	 * flags used to kill the thread.
	 */
	private boolean killed;

	/**
	 * Map to store the flowInfo object associated to each flow type.
	 * 
	 * The keys of the map are FlowType objects.
	 * 
	 * The value associated to each key is a FlowInfo object. The FlowInfo
	 * object will contain the payload type and bandwidth of the flows. The
	 * ports are not used, so they are set to zero.
	 */
	private Map<FlowType, FlowInfo> activeFlows;

	/**
	 * Construct a new instance.
	 * 
	 * This object will integrate all the active flows queries form Audio and
	 * video component, in order to generate only one event with the active
	 * flows of all the components.
	 */
	ActiveFlowsRefresher(IsabelClientImpl ic) {

		mylogger = Logger
				.getLogger("isabel.gw.isabel_client.components.ActiveFlowsRefresher");
		mylogger.info("Creating an ActiveFlowsRefresher object.");

		this.ic = ic;
		
		this.activeFlows = new HashMap<FlowType, FlowInfo>();

		setDaemon(true);
		start();
	}

	/**
	 * Update the pair "FlowType - FlowInfo". The FlowInfo value is created
	 * using the given payload type and bandwidth. The posts are set to -1.
	 * 
	 * This method is synchronized because the attribute "activeFlows" is used
	 * by two threads: myself and the dispatcher thread.
	 * 
	 * @param flowtype
	 *            The flow type which I am configuring.
	 * @param codec
	 *            The codec to used for the given flow type.
	 * @param bandwidth
	 *            The bandwidth to use for this flow.
	 */
	synchronized void update(FlowType flowType, String codec, int bandwidth) {

		mylogger.info("AFR - updating " + flowType + " to " + codec + " ("
				+ bandwidth + ").");

		needRefresh = true;

		int payloadType = FlowTypePayloads.getPayloadType(flowType, codec);

		if (payloadType == -1) {
			activeFlows.remove(flowType);
			return;
		}

		activeFlows.put(flowType, new FlowInfo(payloadType, -1, -1, bandwidth));

		notify();
	}

	/**
	 * Update the value of the given FlowType key, with the default payload type
	 * of this flow type. The ports and bandwith are set to -1.
	 * 
	 * This method is synchronized because the attribute "activeFlows" is used
	 * by two threads: myself and the dispatcher thread.
	 * 
	 * @param flowtype
	 *            The flow type which I am configuring.
	 */
	synchronized void update(FlowType flowType) {

		mylogger.info("AFR - updating " + flowType + ".");

		needRefresh = true;

		int payloadType = FlowTypePayloads.getPayloadType(flowType);

		if (payloadType == -1) {
			activeFlows.remove(flowType);
			return;
		}

		activeFlows.put(flowType, new FlowInfo(payloadType, -1, -1, -1));

		//notify();
	}

	/**
	 * Removes the given FlowType key.
	 * 
	 * This method is synchronized because the attribute "activeFlows" is used
	 * by two threads: myself and the dispatcher thread.
	 * 
	 * @param flowtype
	 *            The flow type which I am removing.
	 */
	synchronized void delete(FlowType flowType) {

		mylogger.info("AFR - deleting " + flowType + ".");

		needRefresh = true;

		activeFlows.remove(flowType);

		//notify();
	}

	/**
	 * kill this thread
	 */
	synchronized void kill() {
		killed = true;
		notify();
	}

	/**
	 * Method "run" of the thread.
	 */
	public void run() {

		while (!killed) {

			try {
				synchronized (this) {
					if (needRefresh) {
						needRefresh = false;

						sendListeners_ActiveFlows_Message();
					}
					wait(500);
				}
			} catch (Exception e) {
				mylogger.severe("Unexpected exception: " + e.getMessage());
			}
		}
	}

	/**
	 * Send an activeFlows message to all the listeners.
	 * 
	 */
	private void sendListeners_ActiveFlows_Message() {
		
	    mylogger.info("Site "+ic.getMySiteId()+" is sending an Active Flows event.");

		ArrayList<IsabelStateListener> ll = IsabelState.getIsabelState().getListeners();

		Vector<FlowInfo> v = new Vector<FlowInfo>();

		for (FlowType ft : activeFlows.keySet()) {
			v.add(activeFlows.get(ft));
		}

		MemberInfo mi = new MemberInfo(ic.getChId(), ic.getMySiteId(), ic
				.getMyAddr(), v);

		IsabelClientEvent ev = new IsabelClientEvent(mi, ic);

		for (IsabelStateListener l : ll) {
			l.activeFlows(ev);

			FlowInfo fi = activeFlows.get(FlowType.AUDIO_SOUND);
			Boolean capture =  new Boolean( fi != null && fi.getPT() != -1 );
			l.parameterChange(ic.getMySiteId(), "Audio_Capture", capture);
		}
	}
}
