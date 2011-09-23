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
package isabel.gw.isabel_client.simulator2;

import isabel.gw.isabel_client.NetworkManager;
import isabel.gw.isabel_client.ShutdownEvent;
import isabel.gw.*;

import isabel.seco.network.Message;
import isabel.seco.network.Network;
import isabel.seco.network.NetworkListener;
import isabel.seco.network.tclser.TclMarshaller;

import java.io.IOException;

import java.util.logging.*;

class SeCoMonitor {

	/**
	 * Logs
	 */
	private Logger mylogger;
	
	/**
	 * The IsabelStateListener object to notify about network shutdown.
	 */
	private IsabelStateListener listener;
	
	/**
	 * Construct the a object.
	 */
	SeCoMonitor(IsabelStateListener listener) {

		mylogger = Logger
		.getLogger("isabel.gw.isabel_client.simulator.SeCoMonitor");
	
	this.listener = listener;
	}

	/**
	 * Nos conectamos al SeCo para detectar cuando este se muere, y asi matar el
	 * simulador.
	 */
	void killOnSeCoExit() {
		// Create a Network object.
		try {
			mylogger.fine("Creando Network.");
			String secohost = "localhost";
			Network net = new Network(secohost, NetworkManager.SECO_PORT,
					new TclMarshaller());

			// Inner class to detect networks downs.
			net.addNetworkListener(new NetworkListener() {
				public void messageReceived(Message msg) {
				}

				public void networkDead(String info) {
					mylogger.severe("Network daemon dead event received:\n"
							+ info + "\nExiting ISABEL.");

					ShutdownEvent ev = new ShutdownEvent("SeCo: Network daemon dead event received.");
					listener.shutdown(ev);
				}

				public void clientDisconnected(String clientId) {
				}
			});
		} catch (IOException ioe) {
			mylogger.severe("Fatal error: I can't start network: "
					+ ioe.getMessage());
			ShutdownEvent ev = new ShutdownEvent("SeCo: Fatal error: I can't start network.");
			listener.shutdown(ev);
		}
	}
}
