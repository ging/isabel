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
/*
 * ControlLink.java
 */

package isabel.seco.server;

import java.net.*;
import java.io.*;
import java.util.*;
import java.util.logging.*;

// ---------------------------------------

/**
 * Este thread es el encargado de gestionar un socket (tcp) de control textual.
 * Los clientes envian los mensajes de control por el socket, y este objeto
 * atiende a estos mensajes, y responde adecuadamente.
 * 
 * @author Santiago Pavon
 */
class ControlLink extends Thread {

	/**
	 * Socket de conexion con el cliente.
	 */
	private Socket s;

	/**
	 * Para poder leer líneas
	 */
	private BufferedReader myIn;

	/**
	 * Stream para enviar al socket.
	 */
	private PrintWriter myOut;

	/**
	 * Logger where logs are written.
	 */
	private Logger mylogger;

	// ---------------------------------------

	/**
	 * Construye el objeto que atiende al cliente de control.
	 * 
	 * @param s
	 *            socket de conexion con el cliente.
	 * @throws Exception
	 *             algo fue mal.
	 */
	public ControlLink(Socket s) throws Exception {

		// Inicializacion de los mensajes de log.
		mylogger = Logger.getLogger("isabel.seco.server.ControlLink");
		mylogger.finer("Creating ControlLink object.");

		this.s = s;
		myIn = new BufferedReader(new InputStreamReader(s.getInputStream()));
		myOut = new PrintWriter(new BufferedWriter(new OutputStreamWriter(s
				.getOutputStream())), true);
	}

	// ---------------------------------------

	/**
	 * Atiende al comando seco_echo(msg), respondiendo con el mismo mensaje.
	 * 
	 * @param st
	 *            StringTokenizer con los argumentos del comando.
	 */
	private void Echo(StringTokenizer st) {

		if (!st.hasMoreTokens()) {
			mylogger.warning("Command error: expected more arguments.");
			return;
		}
		String msg = st.nextToken();

		try {
			myOut.println(msg);
		} catch (Exception e) {
			mylogger.severe(e.getMessage());
		}
	}

	// ---------------------------------------

	/**
	 * Atiende al comando seco_nop(). Solo responde "OK\n".
	 */
	private void Nop() {

		try {
			myOut.println("OK");
		} catch (Exception e) {
			mylogger.severe(e.getMessage());
		}
	}

	// ---------------------------------------

	/**
	 * Bucle principal del thread.
	 */
	public void run() {

		try {
			String linea;
			while ((linea = myIn.readLine()) != null) {
				mylogger.finest("Received= " + linea);

				StringTokenizer st = new StringTokenizer(linea, "(),");

				String cmd = st.nextToken();

				if (cmd.equals("seco_nop")) {
					Nop();
				} else if (cmd.equals("seco_echo")) {
					Echo(st);
				} else if (cmd.equals("seco_quit")) {
					mylogger.info("SeCo ControlLink: seco_quit() received.");
					System.exit(0);
				} else if (cmd.equals("seco_bye")) {
					break;
				} else {
					mylogger.warning("Received invalid command: \"" + linea
							+ "\"");
				}
			}
		} catch (Exception e) {
			mylogger.severe(e.getMessage());
		}

		try {
			s.close();
		} catch (Exception e) {
			mylogger.severe(e.getMessage());
		}
	}
}

// ---------------------------------------

