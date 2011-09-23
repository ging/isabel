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
import java.util.Enumeration;
import java.util.Hashtable;


/**
 * This class handles the request of the RPC tunnel server.
 */
public class TunnelHandler {
	//private Hashtable<String, Integer> allTunnels; 
	
	//private boolean use_tunnels = true;
	
	/*public TunnelHandler(){
		allTunnels = new Hashtable<String, Integer>();
		this.use_tunnels = true;
	}
	
	
	public TunnelHandler(boolean use_tunnels){
		allTunnels = new Hashtable<String, Integer>();
		this.use_tunnels = use_tunnels;
	}*/
	
	
	/**
	 * method to create a tunnel with a server
	 * @param server the name or ip of the server
	 * @param port the number of port to start the tunnel
	 * @param use the purpose for this tunnel, this param is just for logging
	 * @return hashtable with the information of the tunnel
	 */
	public Hashtable<String, String> createTunnel(String server, int port, String use){
		  /*if(!use_tunnels)
		  {
			  System.out.println("We don't use the tunnels, return hashtable vacía");
			  return new Hashtable<String, String>();
		  }
		  System.out.println("We create a tunnel with \"" + server + "\" and will be used for \"" + use +"\"");
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
		  ClientTunnel cliente = new ClientTunnel(server, name);
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
		  if(hash==null)
			  return new Hashtable<String, String>();
		  else
			  return hash;	*/
		
		return Tunnelator.getTunnelator().createTunnel(server,port, use);
		
	}
	 
	/**
	 * method to stop or delete a tunnel with a server
	 * @param server the server
	 */
	public boolean deleteTunnel(String server){
		/*if(!use_tunnels)
		{
			  System.out.println("We don't use the tunnels, return true");
			  return true;
		}
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
		return true;*/
		
		return Tunnelator.getTunnelator().deleteTunnel(server);
	}
	
	
	/**
	 * 
	 * @return the list with the tunnels created
	 */
	public  Hashtable<String, Integer> getListOfTunnels(){
		return Tunnelator.getTunnelator().getListOfTunnels();
	}
	
	/**
	 * method to increase the Integer belonging to the tunnel in the hashtable alltunnels
	 * @param name the name of the tunnel to increase
	 */
	/*private void increase(String name){
		Integer number_of_tunnels = allTunnels.get(name);
		if(number_of_tunnels==null)
		{
			allTunnels.put(name, new Integer(1));
			System.out.println("1 tunnel stablished with " + name);
		}
		else
		{
			int new_number = number_of_tunnels +1;
			allTunnels.remove(name);
			allTunnels.put(name, new_number);
			System.out.println(new_number + " users of this tunnel with " + name);
		}
		
	}*/
	
	
	/**
	 * method to decrease the Integer belonging to the tunnel in the hashtable alltunnels
	 * @param name the name of the tunnel to increase
	 */
	/*private void decrease(String name){
		Integer number_of_tunnels = allTunnels.get(name);
		if(number_of_tunnels!=null && number_of_tunnels==1)
		{
			System.out.println("this was the last one using the tunnel, we close it");
			//hay que cerrar el tunel
			ClientTunnel.closeClientTunnel(name);
			allTunnels.remove(name);
		}
		else
		{
			int new_number = number_of_tunnels -1;
			allTunnels.remove(name);
			allTunnels.put(name, new_number);
			System.out.println(new_number + " users of this tunnel with " + name);
		}
		
	}*/
	
	
	/**
	 * metodo para parar todos los tuneles que se crearon con el manager
	 *
	 */
	public void closeAllTunnels(){
		/*if(!use_tunnels)
		{
			  System.out.println("We don't use the tunnels, return");
			  return;
		}
		System.out.println("Method closeAllTunnels");
		Hashtable<String, Integer> allTunnels = getListOfTunnels();
		System.out.println("Method closeAllTunnels, there are " + allTunnels.size() + " tunnels");
		Enumeration<String> lista_nombres = allTunnels.keys();
		
		while(lista_nombres.hasMoreElements())
		{			
			String name = lista_nombres.nextElement();
			System.out.println("Closing tunnel with: " + name);
			deleteTunnel(name);
		}*/
		
		Tunnelator.getTunnelator().closeAllTunnels();
	}
}

