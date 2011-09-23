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
package isabel.gw.isabel_client.isabel_state;

import isabel.seco.dispatcher.tclser.TclDispatcher;
import isabel.seco.network.Message;
import isabel.seco.network.Network;
import isabel.seco.network.NetworkListener;
import isabel.seco.network.tclser.TclMarshaller;

import java.io.IOException;
import java.util.logging.Logger;

/*
 * This class implements the Network Manager.
 * 
 * Each NetworkManager object creates a Dispatcher object, a Network objects, and connect with the SeCo daemon.
 *  
 */
public class NetworkManager {


	/**
	 * Puerto al que debemos conectarnos para acceder al servidor de SeCo.
	 */
	public static final int SECO_PORT = 53023;

	/**
	 * Logs
	 */
	private Logger mylogger;

	/**
	 * Despachador de los mensajes recibidos de la red SeCo.
	 */
	private TclDispatcher dispatcher;

	/**
	 * Acceso a la red SeCo
	 */
	private Network net;

	/**
	 * Construct the object.
	 * 
	 */
	public NetworkManager() {

		mylogger = Logger.getLogger("isabel.gw.isabel_client.isabel_state.NetworkManager");
		mylogger.info("Creating IsabelState::NetworkManager object.");

		// Crear el despachador de mensajes
		dispatcher = new TclDispatcher();
		
		createNetwork();

	}
	
	/**
	 * Create the Network object.
	 */
	private void createNetwork() {
		try {
			mylogger.fine("Creating NetworkManager.");
			String secohost = "localhost";
			net = new Network(secohost, SECO_PORT, new TclMarshaller());

			// Inner class to detect networks downs.
			net.addNetworkListener(new NetworkListener() {
				public void messageReceived(Message msg) {
				}

				public void networkDead(String info) {
					mylogger.severe("Network daemon dead event received:\n"
							+ info + "\nExiting ISABEL.");

					System.exit(1);
				}

				public void clientDisconnected(String clientId) {
				}
			});

			// Enviar mensajes al despachador de mensajes
			net.addNetworkListener(dispatcher);

		} catch (IOException ioe) {
			mylogger.severe("Fatal error: I can't start network: "
					+ ioe.getMessage());

			System.exit(1);
		}
	}
	

	/**
	 * Disconnect from the SeCo daemon.
	 * 
	 */
	public void disconnect() {

		mylogger.info("NetworkManager disconnection.");

		net.removeNetworkListener(dispatcher);
		dispatcher.quit();
		dispatcher = null;

		try {
			net.quit();
		} catch (IOException e) {
		} finally {
			net = null;
		}
	}


	// -----------------------------------------------------------------------
	// GETTERS
	// -----------------------------------------------------------------------


	/**
	 * Get a reference to the Network attribute.
	 * 
	 * @return Returns a reference to the Network attribute.
	 */
	public Network getNetwork() {
		return net;
	}

	/**
	 * Get a reference to the Dispatcher attribute.
	 * 
	 * @return Returns a reference to the Dispatcher attribute.
	 */
	public TclDispatcher getDispatcher() {
		return dispatcher;
	}

}
