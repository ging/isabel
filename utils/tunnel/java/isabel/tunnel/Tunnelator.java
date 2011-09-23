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

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;

public class Tunnelator {
	private static Tunnelator tunnelator = null;
	private Hashtable<String, Integer> tunnelList;
	
	/**
	 * New tunnelator
	 *
	 */
	private Tunnelator(){
		tunnelList = new Hashtable<String, Integer>();
	}
	
	/**
	 * Returns the tunnelator
	 * @return
	 */
	public static Tunnelator getTunnelator(){
		if(tunnelator == null){
			tunnelator = new Tunnelator();
		}
		return tunnelator;
	}
	
		
	/**
	 * method to create a tunnel with a server
	 * @param server the name or ip of the server
	 * @param use the purpose for this tunnel, this param is just for logging
	 * @return hashtable with the information of the tunnel
	 */
	public Hashtable<String, String> createTunnel(String server, int port, String use){
		  		  
		  System.out.println("We create a tunnel with \"" + server + "\" and port \"" + port + "\" and will be used for \"" + use +"\"");
		  String name = server;
		  try {
			  //en vez de el nombre que puede ser triton o triton.dit.upm.es o el 138.4.24.13 
			  //cojo siempre la IP para que sea un identificador unico
			  InetAddress direcc = InetAddress.getByName(server);
			  name = direcc.getHostAddress();
		  } catch (UnknownHostException e) {
			 e.printStackTrace();
		  }		  
		  		  		  
		  Hashtable<String, String> hash = ClientTunnel.getClientInfo(name);
		  if(hash!=null)
		  {
			  System.out.println("The client existed before, we return the hash, and increase the number of users of that tunnel");
			  increase(name);
			  return hash;
		  }
		  System.out.println("the client did not exist before, we create it");
		  ClientTunnel cliente = new ClientTunnel(server, port, name);
		  System.out.println("client created, we launch it");
		  int error = cliente.launch();			  
		  if(error==2)
		  {
			  System.out.println("error in the launch of the tunnel");
			  hash = null;
		  }
		  else
		  {
			  System.out.println("cliente de tuneles created and launched");
			  hash = ClientTunnel.getClientInfo(name);
			  increase(name);
		  }
		  
		  //null no esta soportado por xmlrpc, devolvemos una hash vacía
		  if(hash==null){			  
			  return new Hashtable<String, String>();
		  }else{
			  return hash;
		  }
	}
	
	/**
	 * method to stop or delete a tunnel with a server
	 * @param server the server
	 */
	public boolean deleteTunnel(String server){
		System.out.println("delete Tunnel with server " + server);  
		String name = server;
		  try {
			  //en vez de el nombre que puede ser triton o triton.dit.upm.es o el 138.4.24.13 
			  //cojo siempre la IP para que sea un identificador unico
			  InetAddress direcc = InetAddress.getByName(server);
			  name = direcc.getHostAddress();
		  } catch (UnknownHostException e) {
			 e.printStackTrace();
		  }
		decrease(name);
		return true;
	}
	
	
	/**
	 * 
	 * @return the list with the tunnels created
	 */
	public  Hashtable<String, Integer> getListOfTunnels(){
		return tunnelList;
	}
	
	/**
	 * method to increase the Integer belonging to the tunnel in the hashtable alltunnels
	 * @param name the name of the tunnel to increase
	 */
	private void increase(String name){
		Integer number_of_tunnels = tunnelList.get(name);
		if(number_of_tunnels==null)
		{
			tunnelList.put(name, new Integer(1));
			System.out.println("1 tunnel stablished with " + name);
		}
		else
		{
			int new_number = number_of_tunnels +1;
			tunnelList.remove(name);
			tunnelList.put(name, new_number);
			System.out.println(new_number + " users of this tunnel with " + name);
		}
	}
	
	
	/**
	 * method to decrease the Integer belonging to the tunnel in the hashtable alltunnels
	 * @param name the name of the tunnel to increase
	 */
	private void decrease(String name){
		Integer number_of_tunnels = tunnelList.get(name);
		if(number_of_tunnels!=null && number_of_tunnels==1)
		{
			System.out.println("this was the last one using the tunnel, we close it");
			//hay que cerrar el tunel
			ClientTunnel.closeClientTunnel(name);
			tunnelList.remove(name);
		}
		else if (number_of_tunnels!=null)
		{
			int new_number = number_of_tunnels -1;
			tunnelList.remove(name);
			tunnelList.put(name, new_number);
			System.out.println(new_number + " users of this tunnel with " + name);
		}		
		
	}
	
	
	/**
	 * metodo para parar todos los tuneles que se crearon con el manager
	 *
	 */
	public void closeAllTunnels(){
		System.out.println("Method closeAllTunnels");
		Hashtable<String, Integer> tunnelList = getListOfTunnels();
		System.out.println("Method closeAllTunnels, there are " + tunnelList.size() + " tunnels");
		Enumeration<String> lista_nombres = tunnelList.keys();
		
		while(lista_nombres.hasMoreElements())
		{			
			String name = lista_nombres.nextElement();
			System.out.println("Closing tunnel with: " + name);
			deleteTunnel(name);
		}
	}

}
