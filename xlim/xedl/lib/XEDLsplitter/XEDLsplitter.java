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
package xedl.lib.XEDLsplitter;

import javax.xml.parsers.DocumentBuilderFactory;  
 
import org.w3c.dom.Document;

import services.isabel.lib.Constants;


// For XEDL management
import xedl.lib.jaxb.SESSION;
import xedl.lib.xedl.*;

import java.io.*;
import java.util.Properties;

public class XEDLsplitter{
    // Global value so it can be ref'd by the tree-adapter
    static Document document;
	 

    public static void main (String argv []){
        if (argv.length != 2 && argv.length !=3) {
            System.err.println ("Usage: java -cp <path_to_XEDL.jar> -jar xedlsplitter <site_id> <filename_in>");
            System.exit (1);
        }

        System.out.println("Entramos en XEDLSplitter");
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        //factory.setNamespaceAware(true);
        //factory.setValidating(true);   
 
        try {
		  	
                String PARTICIPANTS_XSLT = System.getProperty("xedl2part");
                                if (PARTICIPANTS_XSLT == null) PARTICIPANTS_XSLT = Constants.ISABEL_DIR + Constants.FILE_SEPARATOR + "lib//xedlsplitter//xedl2part.xsl";
				String SITE_XSLT = System.getProperty("xedl2site");
                                if (SITE_XSLT == null) SITE_XSLT = Constants.ISABEL_DIR + Constants.FILE_SEPARATOR + "lib//xedlsplitter//xedl2site.xsl";
				
				String XEDL_FILE = argv[1];
				String SITE_ID = argv[0];
		        String private_ip = null;
				
				if(argv.length==3)
				{
					private_ip = argv[2];
				}
				
		  		//Creating XEDL object to represent file
				XEDL xedl= new XEDL(XEDL_FILE);
				
				//Getting session name
				SESSION mises = xedl.getEdl().getSESSION();
				String SessionName = mises.getID();
				System.out.println("Nombre de sesión: " + SessionName);
				
				
				//Windows
				/*String SESSION_DIR = SETUP_PATH + "\\" + SessionName;				
				String PARTICIPANTS_FILE = SESSION_DIR + "\\" + "participants";				
				String SITE_FILE = SESSION_DIR + "\\" + "config." + SITE_ID;*/
				
				//Linux
				String SESSION_DIR = Constants.SETUP_PATH + "//" + SessionName;				
				String PARTICIPANTS_FILE = SESSION_DIR + "//" + "participants";				
				String SITE_FILE = SESSION_DIR + "//" + "config." + SITE_ID;
				
				System.out.println("Session dir: " + SESSION_DIR);
				
				//Creating dir named as the session
				File sessionDir = new File(SESSION_DIR);
				//If it does not exist, it is created
				if(!sessionDir.exists()){
					sessionDir.mkdirs();
				}//if
				
				//Parameters array
				String[] params = {PARTICIPANTS_XSLT,XEDL_FILE,PARTICIPANTS_FILE};
				
				//Creating participants file
				JavaParser.main(params);
				
				
				params = new String[] {SITE_XSLT,XEDL_FILE,SITE_FILE,SITE_ID};

				System.out.println("Vamos a crear el site file");
				//Creating site file
				JavaParser.main(params);
		  		System.out.println("creado");
			
		  		//Creamos la tabla con la configuracion del sitio:
	            Properties siteConfig = null;
	            try {
	            	siteConfig = new Properties();
	                siteConfig.load(new FileInputStream(SITE_FILE));
	                System.out.println("siteconfig cargado");
	            }
	            catch (IOException e) {
	            	System.out.println (new String[] {
	                    "I can not load the site configuration file.",
	                    "I get: "+e.getMessage()});
	            }
	            String ccto = null;
	            if(private_ip!=null && !private_ip.equals(""))
	            {
	              ccto = private_ip;	
	            }
	            else
	            {
	              ccto = (String) siteConfig.get("ISABEL_CONNECT_TO");
	            }
	            System.out.println("Añadimos connect_to_ip " + ccto);
	            siteConfig.put("ISABEL_CONNECT_TO_IP", ccto );
	            siteConfig.store(new FileOutputStream(SITE_FILE), "Modificado para incluir ISABEL_CONNECT_TO_IP");
        } catch (XEDLException xedle) {
		  		// XEDLException
				xedle.printStackTrace();		  
        } catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}

    } // main  

}
