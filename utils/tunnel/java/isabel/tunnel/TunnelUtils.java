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
 * 
 * Utilidades comunes para servidor y cliente de tuneles
 * 
 * @author David Prieto Ruiz
 * @version 0.1
 *
 */

package isabel.tunnel;

import isabel.lib.NetworkManagerConstants;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FilenameFilter;
import java.net.InetAddress;
import java.net.MalformedURLException;
import java.net.NetworkInterface;
import java.net.URL;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Map;
import java.util.Vector;

public class TunnelUtils implements FilenameFilter{

	/*
	 * Status files characters
	 */
	
	/**
	 * Allocation character in status files
	 */
	public final static String allocationString = " ";
	
	/*
	 * Directories
	 */ 
		
	/**
	 * Directory where executable files are situated
	 */
	public final static String binDir = System.getProperty("isabel.dir") + "/bin/";
	
	/**
	 * 	Configuration directory where configuration files are situated
	 */
	public final static String confDir = System.getProperty("isabel.dir") + "/lib/tunnel/conf/";
	
	/**
	 * Directory where sync files are saved
	 */
	public final static String runDir = "/tmp/";
	
	/**
	 * Directory where OpenVPN log files are saved
	 */
	public final static String logsDir = System.getProperty("isabel.user.dir") + "/logs/";
	
	/*
	 * isabel_openvpn
	 */
	
	/**
	 * isabel_openvpn 
	 */
	public final static String isabelOpenvpnFilePath = binDir + "isabel_openvpn";
	
	/*
	 * Configuration files
	 */
	
	/**
	 * OpenVPN server basic configuration file.
	 */
	public final static String basicServerTunnelConfigurationFilePath = confDir + "server_basic.conf";

	/**
	 * OpenVPN server basic configuration file.
	 */
	public final static String basicClientTunnelConfigurationFilePath = confDir + "client_basic.conf";
	
	/**
	 * File with the list of ports where the server of tunnels listens and where the client of tunnels tries to connect.
	 */
	public final static String portsFilePath = confDir + "ports";
	
	/**
	 * File with the timeout correction factor for each attempt of connection of a client tunnel
	 */
	public final static String correctionFactorFilePath = confDir + "client_timeout_correction_factor";
	
	
	/*
	 * Log files
	 */
	
	/**
	 * OpenVPN server tunel log file
	 */
	public final static String serverTunnelLogFilePath = logsDir + "ServerTunnel.log";

	/**
	 * OpenVPN server tunel log file
	 */
	public final static String serverTunnelStatusLogFilePath = logsDir + "StatusTunnel.log";
	
	/**
	 * OpenVPN server tunel log file
	 */
	public final static String clientTunnelLogPrefixFilePath = logsDir + "ClientTunnel";
	
	/*
	 * Status files
	 */
	
	/**
	 * Server tunnel status file.
	 */
	public final static String serverStatusFilePath = runDir + "serverTunnelStatus";
	
	/**
	 * Client tunnel status file prefix.
	 */
	public final static String clientStatusFilePrefix = "clientTunnelStatus";
	
	
	/**
	 * Sync file for isabel_openvpn
	 */
    public final static String alternativePortsFilePath = runDir + "serverTunnelAlternativePorts";
    
    /*
     * PID files
     */
    
    public final static String pidExtension = ".pid2";
    
    /**
     * Server OpenVPN PID file
     */
    public final static String serverPidFilePath = System.getProperty("isabel.tmp.dir") + "/serverTunnel" + pidExtension;
    
    /**
     * Server OpenVPN PID file
     */
    public final static String clientPidPrefixFilePath = System.getProperty("isabel.tmp.dir") + "/clientTunnel";
    
	/*
	 * Default values (if there is a problem trying to read configuration files)
	 */
    
    /**
	 * 	Port where the server of tunnels listens (if there is a problem trying to read configuration files)
	 */
	public final static int defaultPortServer = 53018;
	
	/*
	 * Fixed values
	 */
	
	/**
	 *	Mask for private tunnel networks.
	 */
    public final static String mask = "255.255.255.0";
	
   
	/**
     * Directorio que guarda la configuracion de la aplicacion isabel.
     */
    public static final String ISABEL_CONFIG_DIR = System.getProperty("isabel.config.dir");
    
//  File separator
    public static final String FILE_SEPARATOR = System.getProperty("file.separator");

	/**
	 * Method which parse a status file into a hash table
	 * @param file
	 * @return Hastable with the file info
	 * @throws Exception
	 */
	public static Hashtable<String, String> parseStatusFile(File file) throws Exception{
		if (!file.exists()){
			throw new Exception ("TunnelUtils - File " + file.getName() + "doesn't exist.");
		}
		
		Hashtable<String, String> variables = new Hashtable<String, String>();
		BufferedReader fileReader = new BufferedReader(new FileReader(file));
		
		String line;
		int index;
		while ((line=fileReader.readLine())!=null){
			line = line.trim();
			//if (line.startsWith(comentaryString)){
			//	continue;
			//}
			if (!line.contains(allocationString)){
				System.out.println("TunnelUtils - WARNING - " + file.getName() + " contents a bad format line.");
				continue;
			}
			index = line.indexOf(allocationString);
			variables.put(line.substring(0,index), line.substring(index + 1));
		}
		return variables;
	}

    
    /**
     * Return the first free tunnel interface.
     * 
     * @return Interface name.
     */
    
    public static String getFreeTunnelInterface(){
    	try{
	    	Boolean free=false;
	    	int i;
	    	for (i=0;;i++){
	    		NetworkInterface interfaz;
	    		free = true;
	    	    for (Enumeration interfaces = NetworkInterface.getNetworkInterfaces(); interfaces.hasMoreElements();){
	    			interfaz=(NetworkInterface)interfaces.nextElement();
	    			if (interfaz.getName().equals("tun" + i)){
	    			    free = false;
	    			}
	    	    }
	    	    if(free){
	    	    	return "tun"+i;
	    	    }
	    	}
	    	
    	}catch (Exception e){
    		System.out.println("TunnelUtils - Error getting next free tun interface.");
    		return null;
    	}
    }
    
	/**
	 * Filename filter for client status files.
	 */
	public boolean accept(File dir, String fileName) {
		if ((!dir.equals(new File(runDir)))||(!fileName.startsWith(clientStatusFilePrefix))){
			return false;
		}else{
			return true;
		}
	}
	
    /**
     * Convierte la direccion de red en el rango de direcciones para el caso particular mascara=255.255.255.0
     * @param actual
     * @return
     * @throws UnknownHostException
     */
    public static InetAddress direccion2pool(InetAddress actual) throws UnknownHostException{
	byte[] byteDireccion = actual.getAddress();
	byteDireccion[3]=0;
	return InetAddress.getByAddress(byteDireccion); 
    }
               
		
}
