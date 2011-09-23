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
package xedl.lib.xpathParser;

import java.io.File;
import java.io.IOException;
import java.util.LinkedList;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;


/*
 * Clase que parsea los xedls usando xpath que es mucho más ligero que jaxb
 * Sólo habrá métodos get (con xpath no se puede hacer set)
 */
public class XPathParser {
	
	XPathFactory  factory = null;
	XPath xPath = null;    
	DocumentBuilder builder = null;
	Document doc = null;
	NodeList nodeSiteList = null;
	Node edl_node = null;
	
	public XPathParser(){
		factory=XPathFactory.newInstance();
		xPath=factory.newXPath();

		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		try {
			builder = factory.newDocumentBuilder();
		} catch (ParserConfigurationException e1) {
			e1.printStackTrace();
		}	
	}
	
	public void setFile(String path){
		
		try {
			doc = builder.parse(path);
		} catch (SAXException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		nodeSiteList = doc.getElementsByTagName("SITE");
		edl_node = doc.getElementsByTagName("EDL").item(0);
	}
	
	
	public void setFile(File file){		
		try {
			doc = builder.parse(file);
		} catch (SAXException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		nodeSiteList = doc.getElementsByTagName("SITE");
		edl_node = doc.getElementsByTagName("EDL").item(0);
	}

	
	/**
	 * method to get the list of sites a a string
	 * is the param that SESSION/session-info/SITES gives us
	 * it is not the list that you get parsing the edl site-description list
	 * @return
	 */
	public String getSitesList(){
		String expresion = "SESSION/session-info/SITES";
		 String result = null;
		 try {
			result = xPath.evaluate(expresion,edl_node);
		} catch (XPathExpressionException e) {
			e.printStackTrace();
		}
		return result;
	}
	
	
	/**
	 * method to get the list of ids of the registered sites as a LinkedList
	 * @return
	 */
	public LinkedList getSitesIds(){
		 String expresion = ""; 
		 LinkedList linked = new LinkedList();
		 //one by one, we get all the sites ids
		 for(int i=0; i>nodeSiteList.getLength(); i++){
			 Node site = nodeSiteList.item(i);
			 expresion = "site-identification/ID";
			 String id = null;
			 try {
			 	id = xPath.evaluate(expresion, site);
			 } catch (XPathExpressionException e) {
				e.printStackTrace();
			 }
			 linked.add(id);
		 }
		
		return linked;
	}
	
	
	/**
	 * method to know if the session is open
	 * @return
	 */
	public boolean isOpenSession(){
		String expresion = "SESSION/session-info/OPEN_SESSION";
		String result = null;
		 try {
			result = xPath.evaluate(expresion,edl_node);
		} catch (XPathExpressionException e) {
			e.printStackTrace();
		}
		if(result.equals("true"))
			return true;
		else
			return false;
	}
	
	
	/**
	 * method to get the id of the first SITE (in connect to, the xedl sent has only one site,
	 * so we use this method to get the site id)
	 * @return
	 */
	public String getFirstSiteId(){
		 String expresion = "site-identification/ID";
		 String result = null;
		 try {
			result = xPath.evaluate(expresion,nodeSiteList.item(0));
		} catch (XPathExpressionException e) {
			e.printStackTrace();
		}
		return result;
	}
	
	
	/**
	 * method to get the role of the first SITE (in connect to, the xedl sent has only one site,
	 * so we use this method to get the site id)
	 * @return
	 */
	public String getFirstSiteRole(){
		 String expresion = "SITE-ROLE";
		 String result = null;
		 try {
			result = xPath.evaluate(expresion,nodeSiteList.item(0));
		} catch (XPathExpressionException e) {
			e.printStackTrace();
		}
		return result;
	}
	
	
	/**
	 * method to get the address of the first SITE (in connect to, the xedl sent has only one site,
	 * so we use this method to get the site id)
	 * @return
	 */
	public String getFirstSitePassword(){
		 String expresion = "site-identification/ACCESS_CONTROL";
		 String result = null;
		 try {
			result = xPath.evaluate(expresion,nodeSiteList.item(0));
		} catch (XPathExpressionException e) {
			e.printStackTrace();
		}
		return result;
	}
	
	
	/**
	 * method to get the address of the first SITE (in connect to, the xedl sent has only one site,
	 * so we use this method to get the site id)
	 * @return
	 */
	public String getFirstSiteAddress(){
		 String expresion = "site-identification/SITE_ADDRESS";
		 String result = null;
		 try {
			result = xPath.evaluate(expresion,nodeSiteList.item(0));
		} catch (XPathExpressionException e) {
			e.printStackTrace();
		}
		return result;
	}
	
	
	/**
	 * method to get the addresses as a List of the first SITE (in connect to, the xedl sent has only one site,
	 * so we use this method to get the site id)
	 * @return
	 */
	public LinkedList getFirstSiteAddressList(){
		 String expresion = "";
		 LinkedList linked = new LinkedList();
		 int index = 1;
		 String address = "";
		 while(true)
		 {        			   
			   expresion = "site-identification/SITE_ADDRESS["+index+"]";
			   try {
				 address = xPath.evaluate(expresion,nodeSiteList.item(0));
			   } catch (XPathExpressionException e) {
				 e.printStackTrace();
			   }  
			   if(address.equals(""))
				   break;
			   linked.add(address);
			   index++;
		 }
		 
		return linked;
	}
	
	/**
	 * method to get the parent or flow server of the first SITE (in connect to, the xedl sent has only one site,
	 * so we use this method to get the site id)
	 * @return
	 */
	public String getFirstSiteFS(){
		 //first posibility is that the connection is to an mcu, so we get that address
		 String expresion = "CONNECTION-MODE/mcu/MCU_ADDRESS";
		 String result = null;
		 try {
			result = xPath.evaluate(expresion,nodeSiteList.item(0));
		} catch (XPathExpressionException e) {
			e.printStackTrace();
		}
		if(result!=null && !result.equals(""))
			return result;
		
		//second possibility is that the connection is to a multicast site, so we get the mcastroot address
		 expresion = "CONNECTION-MODE/multicast/multicast-parameters/MCAST_ROOT";
		 try {
			result = xPath.evaluate(expresion,nodeSiteList.item(0));
		} catch (XPathExpressionException e) {
			e.printStackTrace();
		}
		if(result!=null && !result.equals(""))
			return result;
	
		//third possibility is to unicast peer, we get that address
		expresion = "CONNECTION-MODE/unicast/PEER_ADDRESS";
		 try {
			result = xPath.evaluate(expresion,nodeSiteList.item(0));
		} catch (XPathExpressionException e) {
			e.printStackTrace();
		}
		if(result!=null && !result.equals(""))
			return result;
		
		return null;
	}
	
	
	/**
	 * method to get a site caracteristic given a sitename
	 * @param siteName the name of the site
	 * @return
	 */
	public String getSite(String siteName){
		return null;
	}
	
	
	/*main para probar
	*/
	public static void main(String args[])
	{
		XPathParser xpathParser = new XPathParser();
		
		File arch = new File("/home/ebarra/trazas_ccto/local.xedl");
		if(!arch.exists())
		{
			return;
		}
		try {
		    xpathParser.setFile(arch);
		    System.out.println("fisrtsiteid: " + xpathParser.getFirstSiteFS());
		      
		}
		catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	
}
