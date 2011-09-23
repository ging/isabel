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
/**
 * Clase encargada de gestionar clientes isabel_openvpn.
 * 
 * @author David Prieto Ruiz
 * @version 0.3
 */

package isabel.tunnel;

import isabel.lib.NetworkManagerConstants;
import isabel.lib.tasks.Task;
import isabel.lib.tasks.TaskListener;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.UnknownHostException;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Random;
import java.util.Vector;


public class ClientTunnel extends TunnelUtils implements ActivePortTesterLauncher{

	/**
	 * Tamaño del array de threads
	 */
	private final int MAX_PORTS=50;
	
	/*
	 * Estados:
	 *  0 - Arrancando
	 *  1 - Arrancado
	 *  2 - Finalizado
	 */
    private int status=0;
    private String name;
    private InetAddress serverPublicAddress;
    private int port;
    private String iface;
    private String clientPidFilePath;
    private float correctionFactor; 
    
    private Vector<Integer> activePorts;
    
    /**
     * Crea y lanza un cliente de túneles contra la direccion indicada.
     * 
     * @param Como primer argumento se le pasa la direccion del servidor de tuneles al que se conecta.
     */
    public static void main(String[] args) {
    	Random numberGenerator = new Random();
    	String name = Integer.toString(numberGenerator.nextInt(10000));
    	while(getClientInfo(name)!=null){
    		name = Integer.toString(numberGenerator.nextInt(10000));
    	}
    	ClientTunnel clientTunnel = new ClientTunnel(args[0], NetworkManagerConstants.CLIENT_PORT, name);
    	int codigo=clientTunnel.launch();
		if (codigo==1){
			System.out.println("Client tunnel launcher has finished correctly.");
		}else{
			System.out.println("Client tunnel launcher has finished because an error has happened.");
		}
    }


    /**
     * Create a new ClientTunnel
     * 
     * @param serverAddress	IP address or domain name of the tunnel server.  
     * @param name			Name of the client tunnel. It is used to differentiate the client tunnels among them.
     * 						It is not possible to have two clients launched with the same name. 
     * @param port the port to create the tunnel 
     */
    public ClientTunnel(String serverAddress, int port, String name){
    	
    	activePorts = new Vector<Integer>();
    	
    	try {
	    	if(getClientInfo(name)!=null){
	    		throw new Exception ("Name already used");
	    	}
	    	this.name=name;
	    	this.port = port;
	    	clientPidFilePath = clientPidPrefixFilePath +  this.name + pidExtension; 
			serverPublicAddress = InetAddress.getByName(serverAddress);
			
		} catch (UnknownHostException e) {
			System.out.println("Client Tunnel - " + name + " - Invalid server Address.");
			e.printStackTrace();
		} catch (Exception e){
			System.out.println("Client Tunnel - " + name + " - Name already used.");
		}
    
		BufferedReader correctionFactorFileReader;
		try {
			correctionFactorFileReader = new BufferedReader(new FileReader(correctionFactorFilePath));
			correctionFactor = Float.parseFloat(correctionFactorFileReader.readLine().trim());
			correctionFactorFileReader.close();
		} catch (Exception e){
			correctionFactor = 1;
			System.out.println("Client Tunnel - " + name + " - WARNING - Unable to set correction factor for client attempt timeout from the file " + correctionFactorFilePath);
		}
		
    
    }
    
	

    
    /**
     * Start a client tunnel.
     * 
     * @return	status:
	 *  		- 1 - Started.
	 *  		- 2 - Ended (Error).
     */
    public int launch() {
	
	try{
		/*
	     * Eliminamos el fichero que almacena el pid
	     */
		File pidFile= new File(clientPidFilePath);
	    if (pidFile.exists()){
	    	pidFile.delete();
	    }
		
		/*
		 * Interface
		 */
		iface=getFreeTunnelInterface();
		
        boolean lanzado = launchClient(port);

	    if (!lanzado)
	    	throw new Exception ("Client Tunnel - Client tunnel CANNOT be launched.");
        
	    /*
	     * Obtenemos la dirección de cliente que tenemos 
	     */
	    NetworkInterface interfaz;
	    InetAddress direccionCliente = null; 
	    for (Enumeration interfaces = NetworkInterface.getNetworkInterfaces(); interfaces.hasMoreElements();){
		interfaz=(NetworkInterface)interfaces.nextElement();
		if (interfaz.getName().equals(iface)){
		    int numeroDireccionesTun = 0;
		    for (Enumeration direcciones = interfaz.getInetAddresses(); direcciones.hasMoreElements();){
			direccionCliente = (InetAddress)direcciones.nextElement();
			numeroDireccionesTun++;
			if (numeroDireccionesTun > 1)
			    throw new Exception ("Client Tunnel - ClientTunnel interface has more than one address");
		    }
		}
	    }

	    /*
	     * Comprobamos que la dirección obtenida no se solapa con la del servidor de tuneles. Si es asi, reiniciamos el servidor de tuneles.
	     */
	    if (((String)ServerTunnel.getStatus()).equals("STARTED") && direccion2pool(direccionCliente).equals(direccion2pool(InetAddress.getByName((String)ServerTunnel.getPrivateIP())))){
	    	System.out.println("Client and Server IP are in the same range. Restarting ServerTunnel...");
	    	if (ServerTunnel.stop()){
	    		Thread.sleep(1000);
	    		ServerTunnel.launch();
	    	}
	    }
	    
	    
	    /*
	     * Salvamos los datos en el fichero de estado
	     */
	    //Recuperamos el pid del proceso openvpn
	    while (!pidFile.exists()){
	    	Thread.sleep(10);
	    }
	    String pid = "";
	    try{
	    	BufferedReader pidFileReader = new BufferedReader(new FileReader(pidFile));
	    	pid = pidFileReader.readLine().trim();
	    }catch (Exception e){
	    	System.out.println("ServerTunnel - WARNING - There was a problem with the PID file.");
	    }

	    //Calculamos la dirección del servidor de túneles
	    String stringServerPrivateIP = unsignedByte(direccionCliente.getAddress()[0]) + "." + unsignedByte(direccionCliente.getAddress()[1]) + "." + unsignedByte(direccionCliente.getAddress()[2]) + "." + 1;
	    
	    //Salvamos
	    File clientStatusFile = new File(runDir + clientStatusFilePrefix + name);
	    if (clientStatusFile.exists()){
	    	clientStatusFile.delete();
	    }
	    clientStatusFile.createNewFile();
	    FileWriter clientStatusFileWriter = new FileWriter(clientStatusFile);
	    clientStatusFileWriter.write("INTERFACE"  + allocationString + iface + "\n");
	    clientStatusFileWriter.write("LOCAL_PRIVATE_IP"  + allocationString + direccionCliente.getHostAddress() + "\n");
	    clientStatusFileWriter.write("SERVER_PRIVATE_IP"  + allocationString + stringServerPrivateIP + "\n");
	    clientStatusFileWriter.write("SERVER_PUBLIC_IP"  + allocationString + serverPublicAddress.getHostAddress() + "\n");
	    clientStatusFileWriter.write("NAME"  + allocationString + name + "\n");
	    clientStatusFileWriter.write("PID"  + allocationString + pid + "\n");
	    clientStatusFileWriter.close();
	    
	    // Esperamos a que se levante la ruta del tunel	    
	    boolean routeUp = false;
	    Process route;
	    
	    for (int i=1;i<20;i++){
		    route = Runtime.getRuntime().exec(new String[] {"/bin/sh", "-c", "/sbin/route -n | grep " + stringServerPrivateIP + " > /dev/null"});
		    route.waitFor();
			if (route.exitValue()==0){
				routeUp=true;
				System.out.println("Route up");
				break;
			}
			else
			{
				System.out.println("Route is not up");
			}
			Thread.sleep(50);
	    }
	    
	    if (!routeUp){
	    	throw new Exception ("Route is not up");
	    }
	    
		status=1;
		
	} catch (Exception e){
	    System.out.println(e);
	    status=2;
	}
    	return status;	    
    }
    
    /**
     * Return the client tunnel interface.
     * @return The name of the network interface of the client if it is launched or null if it is not launched.
     */
    public String getInterface(){
    	return iface;
    }
    
    /**
     * Return the private IP address of the client tunnel.
     * @return The private IP address of the client tunnel or null if it is not launched. 
     */
    public String getLocalPrivateAddress(){
    	return (String) getClientInfo(name).get("LOCAL_PRIVATE_IP");
    }
    
    /**
     * Return the private IP address of the server tunnel.
     * @return The private IP address of the server tunnel or null if it is not launched.
     */
    public String getServerPrivateAddress(){
    	return (String) getClientInfo(name).get("SERVER_PRIVATE_IP");
    }
    
    /**
     * Return the public server address
     * @return The public server address (even if the client is not launched).
     */
    public String getServerPublicAddress(){
    	return serverPublicAddress.getHostAddress();
    }
    
    /**
     * Return the name of the client tunnel.
     * @return Name of the client tunnel.
     */
    public String getName(){
    	return name;
    }
    
	/**
	 * Close server tunnel
	 * @return true if the tunnel has been closed correctly.  
	 */
    public boolean stop(){
    	if(closeClientTunnel(name)){
    		iface=null;
    		return true;
    	}else{
        	return false;
    	}
    }
    
    /**
     * Used by ActivePortTester to notify an active port
     * @param port
     */
    public synchronized void addActivePort (int port){
    	activePorts.add(new Integer(port));
    }
/*
 * METODOS ESTÁTICOS PÚBLICOS
 */
   
    /**
     * Information about the clients status. The keys of the Hastable are:
     *  - NUMBER_OF_CLIENTS 	- Number of tunnel clients
     *  - INTERFACE#n 			- Name of the client tunnel interface.
     *  - LOCAL_PRIVATE_IP#n 	- private IP address of client tunnel.
     *  - NAME#n 				- Client tunnel name.
     *  - SERVER_PRIVATE_IP#n 	- Server private IP address.
     *  - SERVER_PUBLIC_IP#n 	- Server public IP address.
     *  - PID#n 				- Proccess ID
     *    
     * @return Hashtable with the clients tunnel status information.
     */
	public static Hashtable<String,String> getAllClientsInfo() {
		File[] fileArray = (new File(runDir)).listFiles(new TunnelUtils());
		
		Hashtable<String, String> allClients = new Hashtable<String, String>();
		Hashtable<String, String> client;
		int numberOfClients=0;
		allClients.put("NUMBER_OF_CLIENTS", Integer.toString(numberOfClients));
		for (int i = 0; i < fileArray.length; i++){
			try{
				client = parseStatusFile(fileArray[i]);
				if(!checkClientStatusFile(client)){
					throw new Exception ("Status file is corrupt.");
				}
				
				//Pasamos los valores a la tabla hash definitiva.
				String key;
				for (Enumeration e = client.keys() ; e.hasMoreElements() ;) {
					key = (String) e.nextElement();
					allClients.put((key + i), client.get(key));
				}
				
				//Actualizamos el numero total de clientes lanzados.
				numberOfClients++;
				allClients.put("NUMBER_OF_CLIENTS",Integer.toString(numberOfClients));
					
			}catch (Exception e){
				System.out.println("TunnelUtils - WARNING - There were problems with a client status file(" + fileArray[i].getPath() + ")");
				//e.printStackTrace();
				continue;
			}

		}
		return allClients;
		
		
	}
	
	/**
     * Information about the specific client tunnel. The keys of the Hastable are:
     *  - INTERFACE - String - Name of the client tunnel interface.
     *  - LOCAL_PRIVATE_IP - String - private IP address of client tunnel.
     *  - NAME - String - Client tunnel name.
     *  - SERVER_PRIVATE_IP - String - Server private IP address.
     *  - SERVER_PUBLIC_IP - String - Server public IP address.
     *  - PID - Integer - Proccess ID
     *    
     * @return Hashtable with the client tunnel status information or null if it does not exist.
     */
	public static Hashtable<String, String> getClientInfo(String name) {
		File[] fileArray = (new File(runDir)).listFiles(new TunnelUtils());
		
		Hashtable<String, String> client;
		for (int i = 0; i < fileArray.length; i++){
			try{
				client = parseStatusFile(fileArray[i]);
				if(!checkClientStatusFile(client)){
					throw new Exception ("Status file is corrupt.");
				}
				
				if (name.equals(client.get("NAME"))){
					return client;
				}
				
			}catch (Exception e){
				System.out.println("TunnelUtils - WARNING - There were problems with a client status file(" + fileArray[i].getPath() + ")");
				continue;
			}

		}
		return null;
		
	}
	
	/**
	 * Close client tunnel
	 * @return true if the tunnel has been closed correctly.  
	 */
	public static boolean closeClientTunnel(String name){
		boolean ok=false;
		try{
			Hashtable<String, String> client = getClientInfo(name);
			String pid = client.get("PID");
			System.out.println("We will try to kill the tunnel by killin the process with pid: " + pid);
			Process kill = Runtime.getRuntime().exec("/bin/kill " + pid);
			kill.waitFor();
			if (kill.exitValue()==0){
				ok=true;
				System.out.println("Tunnel killed ok");
			}
			else
			{
				System.out.println("Could not kill the tunnel");
			}
		}catch (Exception e){
			System.out.println("TunnelUtils - Unable to close client tunnel " + name + ".");
			ok=false;
		}
		return ok;
	}

/*
 * MÉTODOS PRIVADOS
 */

    /**
     * Lanza el cliente de tuneles contra el servidor indicado en el puerto indicado 
     * @param stringDireccionServidor Dirección del servidor al que nos queremos conectar
     * @param port Puerto del servidor
     * @return true si consigue lanzar el tunel
     * @throws Exception
     */
    private boolean launchClient(int port) throws Exception{
		 Task clienteTunel = new Task("client_tunnel_" + name,
				 	 isabelOpenvpnFilePath +
				 	 " " + "client" +
				 	 " " + basicClientTunnelConfigurationFilePath + 
					 " " + clientTunnelLogPrefixFilePath + name + ".log"+
					 " " + System.getProperty("user.name") +
					 " " + serverPublicAddress.getHostAddress() +
					 " " + port +
					 " " + clientPidFilePath +
					 " " + iface,
					 false,
					 System.out,
					 System.err);

		clienteTunel.setDestroyOnKill(false);

		clienteTunel.addTaskListener(new TaskListener() {
		   
		   public void deadTask(){
			System.out.println("Client Tunnel - " + name + " -  Client tunnel has finished");
			status=2;
		   }
		});
		clienteTunel.start();
		
		//Comprobamos que se ha levantado la interfaz y devolvemos false si no es asi (time out o error al levantarse)
		// Time out: 4s para el primer intento y 2s para los siguientes
		// modificable mediante el factor de corrección
		for (int i=1; NetworkInterface.getByName(iface)==null;i++){
			//System.out.println("Status=" + status + "   i=" + i);			
			if (status==2 ||  i==Math.round(50*correctionFactor)){
				System.out.println("Client Tunnel -" + name + " Client tunnel cannot be opened at port " + port);
				return false;
			}
			Thread.sleep(200);
		}
		
		//Si se ha levantado correctamente, devolvemos true
		System.out.println("Client Tunnel - " + name + " - Client tunnel opened at port " + port);
		return true;
    }

	/**
	 * Used to kill a client tunnel which had not started correctly 
	 * 
	 */
	private boolean killZombie(){
		boolean ok=false;
		try{
			BufferedReader pidFileReader = new BufferedReader(new FileReader(clientPidFilePath));
	    	String pid = pidFileReader.readLine().trim();
	    	pidFileReader.close();
			Process kill = Runtime.getRuntime().exec("/bin/kill " + pid);
			kill.waitFor();
			if (kill.exitValue()==0){
				ok=true;
				System.out.println("Tunnel zombie killed ok");
			}
			else
			{
				System.out.println("Could not kill the tunnel zombie");
			}
		}catch (Exception e){
			System.out.println("TunnelUtils - Unable to close client tunnel zombie " + name + ".");
			ok=false;
		}
		return ok;
	}
    
	/**
	 * Return true if the values of the status files are correct.
	 * @param variables
	 * @return
	 */
	private static boolean checkClientStatusFile(Hashtable<String, String> variables){

		boolean parseOK = false;
		try{
		
		//Comprobamos que direccion e interface corresponden con la realidad
		NetworkInterface interfaz;
	    InetAddress parsedAddress = InetAddress.getByName(variables.get("LOCAL_PRIVATE_IP"));
	    InetAddress address;
	    for (Enumeration interfaces = NetworkInterface.getNetworkInterfaces(); interfaces.hasMoreElements();){
			interfaz=(NetworkInterface)interfaces.nextElement();
			if (interfaz.getName().equals(variables.get("INTERFACE"))){
			    for (Enumeration direcciones = interfaz.getInetAddresses(); direcciones.hasMoreElements();){
			    	address = (InetAddress)direcciones.nextElement();
			    	if (address.equals(parsedAddress)){
			    		parseOK=true;
			    	}
				}
			}
	    }

	    //Comprobamos que la direccion publica del servidor tiene un formato correcto.
	    InetAddress.getByName(variables.get("SERVER_PUBLIC_IP"));
	    
		}catch (Exception e){
			parseOK=false;
		}
	    return parseOK;

	}

    public void killThread(Thread thread) {
        Thread moribund = thread;
        thread = null;
        moribund.interrupt();
    }
    /**
     * Simple conversion de byte a int como número positivo 
     * @param b
     * @return
     */
    private static int unsignedByte (byte b){
	return (b & 0xff);
    }
}
