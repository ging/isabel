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
package isabel.tunnel;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketTimeoutException;

/**
 * Hebra que comprueba si es posible el uso de túneles en un puerto
 * 
 * @author david
 *
 */
public class ActivePortTester extends Thread{
	
	private int port;
	private InetAddress address;
	private ActivePortTesterLauncher launcher;
	private int timeout;
	private boolean stop = false;
	
	/**
	 * Crea un ActivePortTester
	 * @param group		Grupo de hebras del que forma parte la hebra
	 * @param launcher	Objeto donde se notifican los resultados de la prueba
	 * @param port		Puerto para el que se realiza la prueba
	 * @param address	Dirección IP contra la que se realiza la prueba
	 * @param timeout	Tiempo que se espera la respuesta.
	 */
	public ActivePortTester(ThreadGroup group, ActivePortTesterLauncher launcher, int port, InetAddress address, int timeout){
		super(group, "test" + port);
		this.port = port;
		this.address = address;
		this.launcher = launcher;
		this.timeout = timeout;
	}
	
	public void run(){
			
		DatagramSocket socket;
		try {
			//byte[] openvpnData = {56,52,-128,53,14,-26,112,-72,-68,0,0,0,0,0}; 
			byte[] openvpnData = {56,96,-106,63,-113,-43,-42,117,78,0,0,0,0,0};
			socket = new DatagramSocket();
			DatagramPacket sentPacket = new DatagramPacket (openvpnData, openvpnData.length, address,port);

			byte msg[] = new byte [1024];
			DatagramPacket receivedPacket = new DatagramPacket (msg, msg.length);
			socket.setSoTimeout(timeout);
			boolean received=false;
			for(int i=0; i<10 ; i++){
				if (stop){
					throw new Exception ("Other port is active.");
				}
                                socket.send(sentPacket);
				try{
				socket.receive(receivedPacket);
				received=true;
				continue;
				}
				catch(SocketTimeoutException e){
				}
			}

			if(received){
				launcher.addActivePort(port);
			}else{
				throw new Exception("Timeout");
			}
			
			socket.close();
			
		} catch (Exception e) {
			System.out.println("ActivePortTester - " + e);
			//e.printStackTrace();
		}
		return;
	}
	
	/**
	 * Notifica a la qhebra que la prueba ha terminado.
	 *
	 */
	public void end(){
		stop = true;
	}
	

}
