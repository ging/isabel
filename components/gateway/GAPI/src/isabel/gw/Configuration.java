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
package isabel.gw;

import javax.xml.parsers.DocumentBuilderFactory;
import java.util.logging.Logger;
import java.io.File;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.NodeList;

/**
 * This class loads from the specified xml file the gateway configuration.
 * 
 * Available fields:
 * -mcuAddr : Mcu ip address.
 * -mcuPort : Mcu control port.
 * -mcuExec : Mcu executable file.
 * 
 * Xml configuration example file:
 * <GAPI 
 *     mcuExec="./mcu.exe" 
 *     mcuPort="1234" 
 *     mcuAddr="127.0.0.1"
 *     isabelAddr="192.168.131.132"
 *     >
 * </GAPI>
 */
public class Configuration {
	
	/**
	 * Mcu Ip address label. 
	 */
	private final static String MCU_ADDR = "mcuAddr";
	/**
	 * Mcu control port label.
	 */
	private final static String MCU_PORT = "mcuPort";
	/**
	 * Mcu executable file label.
	 */
	private final static String MCU_EXEC = "mcuExec";
	/**
	 * Isabel address.
	 */
	private final static String ISABEL_ADDR = "isabelAddr";
	/**
	 * Isabel SSRC for mixed audio and desktop video.
	 */
	private final static String ISABEL_SSRC = "isabelSSRC";
	/**
	 * GAPI address.
	 */
	private final static String GAPI_ADDR = "gapiAddr";
	/**
	 * Mcu control port.
	 */
	private int mcuPort;
	/**
	 * Mcu ip address.
	 */
	private String mcuAddr;
	/**
	 * Mcu executable file.
	 */
	private String mcuExec;
	/**
	 * Isabel address.
	 */
	private String isabelAddr;
	/**
	 * Isabel ssrc.
	 */
	private int isabelSSRC;
	/**
	 * GAPI address.
	 */
	private String gapiAddr;
	/**
	 * Logger.
	 */
	private Logger log;
	/**
	 * This boolean is 'true' if configuration is successfully loaded, otherwise false.
	 */
	private boolean ready;
		
	/**
	 * Constructor.
	 * @param fileName Xml configuration file.
	 * @throws ConfigException
	 */
	public Configuration(String fileName) throws ConfigException
	{
		ready = false;
		log = Logger.getLogger("isabel.gw.Configuration");
		try
		{		
			log.info("Loading " + fileName + "...\n");
			Document xml = DocumentBuilderFactory.newInstance().newDocumentBuilder().parse(new File(fileName));
			NodeList nodes = xml.getChildNodes();			
			for (int i = 0; i < nodes.getLength();++i)
			{
				Node node = nodes.item(i);
				log.info("Processing node " + node.getNodeName() + "\n");
				// Fix for GPL comment in config file
				if (node.getNodeType() == Node.COMMENT_NODE) {
					log.info("Skipping comment node");
					continue;
				}
				
			    NamedNodeMap attrs = node.getAttributes();
			    for (int j = 0; j < attrs.getLength();++j)
			    {			    	
			    	Node attr = attrs.item(j);
			    	log.info("Processing attr " + attr.getNodeName());
			    	if (attr.getNodeName() == MCU_ADDR){
			    		mcuAddr = attr.getNodeValue();
			    	}else if (attr.getNodeName() == MCU_PORT){
			    		mcuPort = Integer.parseInt(attr.getNodeValue());
			    	}else if (attr.getNodeName() == MCU_EXEC){
			    		mcuExec = attr.getNodeValue();
			    	}else if (attr.getNodeName() == ISABEL_ADDR){
			    		isabelAddr = attr.getNodeValue();
			    	}else if (attr.getNodeName() == ISABEL_SSRC){
			    		isabelSSRC = Integer.parseInt(attr.getNodeValue());
			    	}else if (attr.getNodeName() == GAPI_ADDR){
			    		gapiAddr = attr.getNodeValue();
			    	}
			    }
			    if (mcuAddr.length() > 0 && mcuPort > 0 && mcuExec.length() > 0 && 
			    		isabelAddr.length() > 0 && gapiAddr.length() > 0 && isabelSSRC > 0){
			    	ready = true;
			    }
			}			
		}catch(Exception e){
			log.severe(e.getMessage());			
			ready = false;
		}
		if (ready == true)
		{
			log.info("Configuration loaded successfully.\n");
		}else{
			log.info("Error loaded successfully.\n");
			throw new ConfigException();
		}
	}
	
	
	/**
	 * @return 'true' if configuration has been successfully loaded.
	 */
	public boolean isReady()
	{
		return ready;
	}
	
	/**
	 * @return Mcu control port. 
	 * @throws ConfigException
	 */
	public int getMcuPort() throws ConfigException
	{
		if (!ready){ throw new ConfigException(); }
		return mcuPort;
	}
	
	/**
	 * @return Mcu ip address.
	 * @throws ConfigException
	 */
	public String getMcuAddr() throws ConfigException
	{
		if (!ready){ throw new ConfigException(); }
		return mcuAddr;
	}				
	
	/**
	 * @return Mcu executable file name.
	 * @throws ConfigException
	 */
	public String getMcuExec() throws ConfigException
	{
		if (!ready){ throw new ConfigException(); }
		return mcuExec;
	}
	
	/**
	 * @return Isabel address.
	 * @throws ConfigException
	 */
	public String getIsabelAddr() throws ConfigException
	{
		if (!ready){ throw new ConfigException(); }
		if (mcuAddr.equalsIgnoreCase("localhost") || mcuAddr.startsWith("127.")|| mcuAddr.equals("::1"))
		{
			String aux = System.getenv("SIP_IPV6");
			if (aux != null)
				return "::1";
			else
				return "127.0.0.1";
			
		}
		else
		{
			return isabelAddr;
		}
	}
	
	/**
	 * @return Gapi address.
	 * @throws ConfigException
	 */
	public String getGapiAddr() throws ConfigException
	{
		if (!ready){ throw new ConfigException(); }
		return gapiAddr;
	}
	
	/**
	 * @return Isabel ssrc.
	 * @throws ConfigException
	 */
	public int getIsabelSSRC() throws ConfigException
	{
		if (!ready){ throw new ConfigException(); }
		return isabelSSRC;
	}
}
