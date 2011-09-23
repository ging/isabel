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
package isabel.gwsip;

import javax.xml.parsers.DocumentBuilderFactory;
import java.util.logging.Logger;
import java.io.File;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.NodeList;

/**
 * This class loads from the specified xml file the SIP gateway configuration.
 * 
 * Available fields:
 * -registerAddr : The SIP register address.
 * -registerPort : The SIP register port.
 * 
 * Xml configuration example file:
<GWSIP
    registerAddr="192.168.176.128"
    registerPort="5060"
    nickName="Isabel Gateway"
    realm="Isabel"
    password=" "
    clientAddr="192.168.131.1"
    clientName="gwsip"
    videoBW="1000" >
</GWSIP>
 */
class SipConfiguration {
	
	private final static String SIP_NODE = "GWSIP";
	/**
	 * Gateway nickName. 
	 */
	private final static String NICK_NAME = "nickName";
	/**
	 * Auth realm. 
	 */
	private final static String REALM = "realm";
	/**
	 * Auth password. 
	 */
	private final static String PASSWD = "password";
	/**
	 * Register Ip address label. 
	 */
	private final static String REGISTER_ADDR = "registerAddr";
	/**
	 * Register control port label.
	 */
	private final static String REGISTER_PORT = "registerPort";
	/**
	 * Client addr.
	 */
	private final static String CLIENT_ADDR = "clientAddr";	
	/**
	 * Client name.
	 */
	private final static String CLIENT_NAME = "clientName";	
	/**
	 * Ancho de banda del video.
	 */
	private final static String VIDEO_BW = "videoBW";
	/**
	 * Ancho de banda del video.
	 */
	private int videoBW;
	/**
	 * Register port.
	 */
	private int registerPort;
	/**
	 * Register ip address.
	 */
	private String nickName;
	/**
	 * Register ip address.
	 */
	private String registerAddr;
	/**
	 * Client address.
	 */
	private String clientAddr;
	/**
	 * Client name.
	 */
	private String clientName;
	/**
	 * Auth realm.
	 */
	private String realm;
	/**
	 * Auth password.
	 */
	private String passwd;
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
	 * @throws SipConfigException
	 */
	public SipConfiguration(String fileName) throws SipConfigException
	{
		ready = false;
		log = Logger.getLogger("isabel.gwsip.SipConfiguration");
		try
		{		
			log.info("Loading " + fileName + "...\n");
			Document xml = DocumentBuilderFactory.newInstance().newDocumentBuilder().parse(new File(fileName));
			NodeList nodes = xml.getChildNodes();			
			for (int i = 0; i < nodes.getLength();++i)
			{
				Node node = nodes.item(i);
				log.info("Processing node " + node.getNodeName() + "\n");
				if (node.getNodeName() == SIP_NODE)
				{
				    NamedNodeMap attrs = node.getAttributes();
				    for (int j = 0; j < attrs.getLength();++j)
				    {			    	
				    	Node attr = attrs.item(j);
				    	log.info("Processing attr " + attr.getNodeName());
				    	if (attr.getNodeName() == REGISTER_ADDR){
				    		registerAddr = attr.getNodeValue();
				    	}else if (attr.getNodeName() == REGISTER_PORT){
				    		registerPort = Integer.parseInt(attr.getNodeValue());
				    	}else if (attr.getNodeName() == CLIENT_ADDR){
				    		clientAddr = attr.getNodeValue();
				    	}else if (attr.getNodeName() == CLIENT_NAME){
				    		clientName = attr.getNodeValue();
				    	}else if (attr.getNodeName() == NICK_NAME){
				    		nickName = attr.getNodeValue();
				    	}else if (attr.getNodeName() == REALM){
				    		realm = attr.getNodeValue();
				    	}else if (attr.getNodeName() == PASSWD){
				    		passwd = attr.getNodeValue();
				    	}else if (attr.getNodeName() == VIDEO_BW){
				    		videoBW = Integer.parseInt(attr.getNodeValue());
				    	}				    	
				    }
				    if (registerAddr.length() > 0 && 
				    	registerPort > 0          && 
				    	clientAddr.length() > 0   && 
				    	clientName.length() > 0   &&
				    	nickName.length() > 0     &&
				    	videoBW > 0){
				    	ready = true;
				    }
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
			throw new SipConfigException();
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
	 * @return Register port. 
	 */
	public int getRegisterPort()
	{
		String aux = System.getenv("SIP_REGISTER_PORT");
		if (aux != null)
		    return Integer.parseInt(aux);
		else
		    return registerPort;
	}
	
	/**
	 * @return Register address.
	 */
	public String getRegisterAddr()
	{
		String aux = System.getenv("SIP_REGISTER_ADDRESS");
		if (aux != null)
		    return aux;
		else
    		return registerAddr;
	}			
	
	/**
	 * @return Client address.
	 */
	public String getClientAddr()
	{
		String aux = System.getenv("SIP_CLIENT_ADDRESS");
		if (aux != null)
		    return aux;
		else		
		    return clientAddr;
	}
	
	/**
	 * @return Client name.
	 */
	public String getClientName()
	{
		String aux = System.getenv("SIP_CLIENT_NAME");
		if (aux != null)
		    return aux;
		else				
		    return clientName;
	}
	
	/**
	 * @return The nick name.
	 */
	public String getNickName()
	{
		String aux = System.getenv("SIP_CLIENT_NICKNAME");
		if (aux != null)
		    return aux;
		else				
		    return nickName;
	}
	/**
	 * @return The auth realm.
	 */
	public String getRealm()
	{
		String aux = System.getenv("SIP_REALM");
		if (aux != null)
		    return aux;
		else				
		    return realm;
	}
	/**
	 * @return The nick name.
	 */
	public String getPassword()
	{
		String aux = System.getenv("SIP_CLIENT_PASSWORD");
		if (aux != null)
		    return aux;
		else				
		    return passwd;
	}
	
	/**
	 * @return The video bandwidth.
	 */
	public int getVideoBW()
	{
		String aux = System.getenv("SIP_VIDEO_BW");
		if (aux != null)
		    return Integer.parseInt(aux);
		else				
		    return videoBW;
	}
}
