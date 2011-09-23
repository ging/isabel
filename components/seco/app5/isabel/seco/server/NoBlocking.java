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
package isabel.seco.server;

import isabel.lib.reliableudp.RUDPSocket;
import java.util.HashMap;
import java.net.Socket;
import java.io.IOException;

import java.util.logging.*;

/**
 * This class is used to close no responding (blocked) connections.
 * 
 * This implementation of the SeCo server is based on java.net and threads, i.e.
 * it uses thread to attend to all the connected clients. The problem with this
 * implementation is that the server hangs when a socket write operation does
 * not finish (because the out queue is full).
 * 
 * The write operations are registered in this class, and they are checked
 * periodically to see if the are blocked. When this happens, the socket is
 * closed.
 */
class NoBlocking extends Thread {

	/**
	 * This thread checks for blocked socked at this period. Note that the time
	 * to detect a blocked socket is between CHECKING_PERIOD and
	 * 2*CHECKING_PERIOD.
	 */
	private final static int CHECKING_PERIOD = 60000;

	/**
	 * Logger where logs are written.
	 */
	private Logger mylogger;

	/**
	 * Constructor
	 */
	public NoBlocking() {

		mylogger = Logger.getLogger("isabel.seco.server.NoBlocking");
		mylogger.info("Creating NoBlocking object.");

		setDaemon(true);
		start();
	}

	/**
	 * A new object is created for each new period. The registered socked are
	 * associated with this object, so that blocked socket will be associated
	 * with older objects.
	 */
	private Object floor = new Object();

	/**
	 * Map with the registered sockets. The key is the socket object and the
	 * value is the associated floor object.
	 */
	private HashMap<Object, Object> map = new HashMap<Object, Object>();

	/**
	 * Register a socket. This socket is now involved in a write operation. When
	 * this operation finish, the socket will be unregistered.
	 * 
	 */
	synchronized void add(Object socket) {

		map.put(socket, floor);
	}

	/**
	 * Unregister the given socket. Its write operation has finish.
	 * 
	 * @param socket
	 *            the socket to unregister
	 */
	synchronized void del(Object socket) {

		map.remove(socket);
	}

	/**
	 * Checks periodically if there are any blocked operation. It closes the
	 * blocked sockets.
	 */

	public void run() {

		while (true) {

			mylogger.fine("NoBlocking: COMPROBANDO BLOQUEOS");

			// Checks sockets registered in the map to detect which are blocket
			// (associated with an old floor.
			synchronized (this) {
				for (Object socket : map.keySet()) {
					if (map.get(socket) != floor) {
						try {
							mylogger.warning("SeCo NoBlocking check: closing no responding socket.");

                            if (socket instanceof Socket)
                                ((Socket)socket).close();
                            else if (socket instanceof RUDPSocket) {
                                ((RUDPSocket)socket).closeOut();
                                ((RUDPSocket)socket).close(2000);
                            }
						} catch (IOException ioe) {

						} finally {
							map.remove(socket);
						}
					}
				}
			}

			// Create a new floor for the next period:
			floor = new Object();

			// Sleep until the next period.
			try {
				sleep(CHECKING_PERIOD);
			} catch (Exception e) {
			}
		}
	}
}
