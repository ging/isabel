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

import isabel.lib.StringParser;
import isabel.seco.dispatcher.tclser.*;
import isabel.seco.network.tclser.TclMessage;

import java.io.IOException;

import java.util.logging.Logger;

/**
 * Clase que se encarga de escuchar mensajes Tcl de tipo service
 * 
 * @author Isabel Bau
 * 
 */
public class ServiceManager implements TclDispatcherListener {

	/**
	 * Logs
	 */
	private Logger mylogger;

	/**
	 * Flag que indica cuando hemos recibido el servicio de isabel.
	 */
	private boolean serviceReceived = false;

	/**
	 * El servicio recibido de isabel.
	 */
	private Service service;

	/**
	 * Crea el objeto encargado de gestionar los mensajes de servicio.
	 */
	ServiceManager(IsabelStateImpl is) {

		mylogger = Logger.getLogger("isabel.gw.isabel_client.isabel_state.ServiceManager");

		is.getNetworkMgr().getDispatcher().addDestiny("SERV", this);

		// Apuntarse al grupo "service"
		try {
			is.getNetworkMgr().getNetwork().joinGroup("isabel_master_service");
		} catch (IOException e) {
			mylogger.severe(e.getMessage());
			System.exit(1);
		}

		waitForService();
	}

	/**
	 * Espera hasta recibir el servicio.
	 */
	private void waitForService() {

		for (int tries = 0; tries < 300; tries++) {
			if (serviceReceived) {
				mylogger.info("Service message has been received.");
				return;
			}
			mylogger.fine("Waiting for Service message (" + tries + ").");
			try {
				Thread.sleep(100);
			} catch (InterruptedException ie) {
			}
		}

		mylogger.severe("I can't receive service message. Timeout expired. Exiting now.");
		System.exit(1);
	}

	/**
	 * Procesa los mensajes recibidos del dispatcher.
	 */
	public void processMessage(TclMessage msg) {

		mylogger.info("ServiceManager has received the service definition.");

		String cmd = msg.cmd;

		mylogger.finest("Service ->" + cmd);

		try {
			StringParser sp = new StringParser(cmd);

			sp.nextToken(); // String type
			String method = sp.nextToken();

			if (method.equals("ServiceDefinition")) {
				// creacion del servicio

				String serviceName = sp.nextToken();
				mylogger.fine("Service name: " + serviceName);

				String actions = sp.nextToken();
				mylogger.fine("Actions: " + actions);

				String menuItems = sp.nextToken();
				mylogger.fine("Menu items: " + menuItems);

				String roles = sp.nextToken();
				mylogger.fine("Roles: " + roles);

				String components = sp.nextToken();
				mylogger.fine("Components: " + components);

				String interactionModes = sp.nextToken();
				mylogger.fine("Interaction modes: " + interactionModes);

				String resources = sp.nextToken();
				mylogger.fine("Resources: " + resources);

				service = new Service(serviceName, actions, menuItems, roles,
						components, interactionModes, resources);
				serviceReceived = true;

			} else if (method.equals("ServiceUpdate")) {
				// actualización del servicio
				mylogger.severe("Mensaje no soportado aun: \"" + cmd + "\"");

			} else {
				mylogger.severe("Mensaje no soportado aun: \"" + cmd + "\"");
			}
		} catch (Exception e) {
			mylogger.severe("Invalid service definition: " + e.getMessage());
		}
	}

	/**
	 * Devuelve una referencia al objeto que almacena el servicio recibido.
	 * 
	 * @return Devuelve una referencia al objeto que almacena el servicio
	 *         recibido.
	 */
	public Service getService() {
		return service;
	}
}
