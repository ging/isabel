package isabel.lib.reliableudp;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketAddress;
import java.util.HashMap;
import java.util.Map;

public class SocketSync {
	
	private static Map<SocketAddress, Object> locks = new HashMap<SocketAddress, Object>();
	
	/**
	 * Send a packet through a socket
	 * @param sock The socket
	 * @param packet The packet to be sent
	 */
	public static void send(DatagramSocket sock, DatagramPacket packet) throws IOException
	{
		Object lock = getLock(sock);
		synchronized (lock) {
			sock.send(packet);	
		}
	}
	
	/**
	 * Get the lock for a socket
	 * @param sock The socket to get the lock for
	 * @return An Object to act as a lock
	 */
	private static Object getLock(DatagramSocket sock) {
		SocketAddress addr = sock.getLocalSocketAddress();
		if (!locks.containsKey(addr))
		{
			Object lock = new Object();
			locks.put(addr, lock);
		}
		return locks.get(addr);
	}
}
