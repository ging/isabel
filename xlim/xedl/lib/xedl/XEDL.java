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
package xedl.lib.xedl;


import xedl.lib.jaxb.Edl;
import xedl.lib.jaxb.ObjectFactory;

import javax.xml.bind.JAXBElement;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;

import org.xml.sax.SAXException;

import java.io.*;
import java.net.MalformedURLException;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.*;


import services.isabel.lib.Constants;

import javax.xml.validation.Schema;
import javax.xml.validation.SchemaFactory;


/**
	Class that represents an XEDL file
*/
public class XEDL 
{
 	/**
	* Location of XEDL file represented by this XEDL object
	*/  
	private String path;
	
	/**
	 * Edl Object that contains the whole xml data
	 */
	private Edl the_edl;
	  
	public PrintWriter outTraceFile= null;
	
	public static final String SCHEMAURL = "file:" + Constants.ISABEL_DIR + Constants.FILE_SEPARATOR + "lib/xedlsplitter/edl1-8.xsd";
	  
	/**
	 * Schema
	 */
	private Schema the_squema = null;
	
	//Para trazas horarias
	SimpleDateFormat timeFormatter;
	
	
	/**
    * Creates an XEDL object that represents an XEDL file named "path".
    * If "path" file does not exist, an exception is thrown.
    * "validate" parameter decides if parser will validate or not.
    */
   public XEDL(String path) throws XEDLException {
	   outTraceFile = Constants.inicializa_trazas_jaxb();	   	      
	   timeFormatter = new SimpleDateFormat("HH:mm:ss.SSS");
	   this.path=path;
	   
	   outTraceFile.println ("["+timeFormatter.format(new Date(System.currentTimeMillis()))+"]"+" Obtenemos el Unmarshaller.");
	   Unmarshaller unmarshaller = JAXBSingleton.getUnmarshaller();
	   outTraceFile.println ("["+timeFormatter.format(new Date(System.currentTimeMillis()))+"]"+" Ya.");
	   
	   boolean validate = validate();
	   
	   outTraceFile.println ("["+timeFormatter.format(new Date(System.currentTimeMillis()))+"]"+"Creando XEDL con validate a " + validate);
	   try {
	       if(validate)
	       {
	    	   outTraceFile.println ("VALIDATE A TRUE, VALIDAMOS EL XEDL!!");
		       SchemaFactory factory = SchemaFactory.newInstance("http://www.w3.org/2001/XMLSchema");
		       outTraceFile.println ("["+timeFormatter.format(new Date(System.currentTimeMillis()))+"]"+" Instancia del esquemaFactory creada");
		       Schema mi_squema = null;	
		       URL miurl = null;
		       try {
				 miurl = new URL(SCHEMAURL);
		       } catch (MalformedURLException e) {
				 e.printStackTrace();
		       }
			   mi_squema = factory.newSchema(miurl);
			   outTraceFile.println ("["+timeFormatter.format(new Date(System.currentTimeMillis()))+"]"+" Squema creado");
			   unmarshaller.setSchema(mi_squema);
	       }
	       outTraceFile.println ("["+timeFormatter.format(new Date(System.currentTimeMillis()))+"]"+" Vamos a parsear el XEDL");
	       JAXBElement<Edl> elem = (JAXBElement<Edl>) unmarshaller.unmarshal(new File(path));
	       outTraceFile.println ("["+timeFormatter.format(new Date(System.currentTimeMillis()))+"]"+" Hecho, vamos a coger el EDL (elemento raiz)");
	       the_edl = elem.getValue();
	       outTraceFile.println ("["+timeFormatter.format(new Date(System.currentTimeMillis()))+"]"+" Hecho");
	   }
	   catch (SAXException sxe) { // Error generated during parsing  
			Exception x = sxe;
			if (sxe.getException() != null) x = sxe.getException(); 
			x.printStackTrace();
			x.printStackTrace(outTraceFile);
	   }
	   catch (JAXBException jax) {
		   String mensaje = jax.getMessage();
		   jax.printStackTrace();
		   jax.printStackTrace(outTraceFile);
		   throw new XEDLException("Mensaje " + mensaje);
	   }	   
   }
	   
	
	/**
    * Creates an XEDL object that represents an XEDL file (the object will be empty)
    */
   public XEDL()throws XEDLException{
	  	outTraceFile = Constants.inicializa_trazas_jaxb();
	  	timeFormatter = new SimpleDateFormat("HH:mm:ss.SSS");
		this.path="";
	    the_edl = new Edl();	   
   }
   
   
   
   /**
    * Saves XEDL object to file.
    */
   public boolean save() throws XEDLException{
	   outTraceFile.println("vamos a salvar el xedl");
	   if(this.path==null || this.path.equals(""))
		   return false;
	   else
		   return save(this.path);
   }
   
   /**
    * Saves XEDL object to file named "path".
    */

   public boolean save(String path) throws XEDLException{
	   outTraceFile.println ("["+timeFormatter.format(new Date(System.currentTimeMillis()))+"]"+" Vamos a salvar el XEDL");
	   try {
	       Marshaller marshaller = JAXBSingleton.getMarshaller();
	       marshaller.setProperty(Marshaller.JAXB_FORMATTED_OUTPUT, new Boolean(true));
	       outTraceFile.println ("["+timeFormatter.format(new Date(System.currentTimeMillis()))+"]"+" Marshaller creado");
	       
	       ObjectFactory objFactory = new ObjectFactory();	       
		   JAXBElement<Edl> localtest = objFactory.createEDL(the_edl);
		   outTraceFile.println ("["+timeFormatter.format(new Date(System.currentTimeMillis()))+"]"+" EDL creado");
		   boolean validate = validate();
		   if(validate)
		   {
			   outTraceFile.println ("VALIDATE A TRUE, VALIDAMOS EL XEDL!!");
			   SchemaFactory factory = SchemaFactory.newInstance("http://www.w3.org/2001/XMLSchema");
		       if(the_squema == null)
		       {
				   try {				
					   URL miurl = new URL(SCHEMAURL);
					   the_squema = factory.newSchema(miurl);
					   outTraceFile.println ("["+timeFormatter.format(new Date(System.currentTimeMillis()))+"]"+" Esquema creado");
				   } catch (SAXException e) {
						e.printStackTrace();
						e.printStackTrace(outTraceFile);
				   }
				   catch (MalformedURLException e)
				   {
						e.printStackTrace();
						e.printStackTrace(outTraceFile);
				   }
		       }
			   //XXX QUITAR ENRIQUE, lo pongo para hacer pruebas, que no valide
		       outTraceFile.println ("["+timeFormatter.format(new Date(System.currentTimeMillis()))+"]"+" Vamos a fijar el esquema");
		       marshaller.setSchema(the_squema);
		       outTraceFile.println ("["+timeFormatter.format(new Date(System.currentTimeMillis()))+"]"+" Fijado, vamos a hacer el marshal");
		   }
	       marshaller.marshal(localtest, new FileOutputStream(path));
	       outTraceFile.println ("["+timeFormatter.format(new Date(System.currentTimeMillis()))+"]"+" Hecho, salimos");	       
	       setPath(path);
	       return true;
	   }
	   catch (JAXBException jax) {
		   String mensaje = jax.getMessage();
		   jax.printStackTrace();		   
		   jax.printStackTrace(outTraceFile);
		   outTraceFile.println("DANGER DANGER: We delete the file " + path + " because it is corrupted");
		   new File(path).delete();
		   throw new XEDLException("Mensaje " + mensaje);
	   }
	   catch (FileNotFoundException fnf)
	   {
		   String mensaje = fnf.getMessage();
		   fnf.printStackTrace();
		   fnf.printStackTrace(outTraceFile);
		   throw new XEDLException("Mensaje " + mensaje);
	   }

   }
   

    public void setSchemaURL(String path)
    {
    	SchemaFactory factory = SchemaFactory.newInstance("http://www.w3.org/2001/XMLSchema");
	    Schema mi_squema = null;			
		try {
			URL miurl = new URL(path);
			mi_squema = factory.newSchema(miurl);
		} catch (SAXException e) {
			e.printStackTrace();
			e.printStackTrace(outTraceFile);
		}
		catch (MalformedURLException e)
		{
			e.printStackTrace();
			e.printStackTrace(outTraceFile);
		}
	}
    
    
    //method that reads a file and check if we want to validate or no
    public boolean validate(){
    	//First of all we check the content of the file .isabel/config/tunnel.cfg to see if we will use tunnels
    	String file =  Constants.ISABEL_CONFIG_DIR + Constants.FILE_SEPARATOR + Constants.VALIDATE_XEDL;
    	File arch = new File(file);
    	boolean validate = true;  //default yes
    	if(arch.exists())
    	{
    		LineNumberReader procOutLine;
    	    String linea = "";
    		try {
    			procOutLine = new LineNumberReader(new FileReader(file));
    			linea = procOutLine.readLine();
    			while(linea!=null)
    			{
    				if(linea.startsWith(Constants.VALIDATE_KEY))
    				{
    					//System.out.println("Line: " + linea);
    					String [] trozos = linea.split(" ");
    					if(trozos.length>1 && trozos[1].equals("0"))
    					{
    						//System.out.println("We won't validate the XEDL");
    						validate = false;
    					}
    					else
    					{
    						//System.out.println("We will validate the XEDL");
    						validate=true;
    					}
    				}
    				linea = procOutLine.readLine();
    			}
    		} catch (FileNotFoundException e) {
    			e.printStackTrace();
    		} 
    		catch (IOException e) {
    			e.printStackTrace();
    		}  
    		return validate;
    	}
    	else
    	{
    		System.out.println("The file does not exist, we create it");
    		PrintWriter outTraceFile = null;
			try {
				outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (file)),true);
			} catch (IOException e) {
				e.printStackTrace();
			}
    		outTraceFile.println("#File to decide if validate the xedl(1) or not(0)");
    		outTraceFile.println(Constants.VALIDATE_KEY + " " + "1");
    		System.out.println("created");
    		return true;
    	}
    	
    }
	

	public Edl getEdl() {
		return the_edl;
	}
	
	
	public void setEdl(Edl the_edl) {
		this.the_edl = the_edl;
	}	
	
	
	public String getPath(){
	    return this.path;
	}
	
	
	public void setPath(String path) {
		this.path = path;
	}
	
	
	/**
     * Metodo de prueba de la clase.
     * @param args Argumentos de la line de comando. No se usan.
     */
    public static void main (String[] args) throws Exception {
    	XEDL xedl = new XEDL(Constants.ISABEL_DIR + Constants.FILE_SEPARATOR + "lib/sessions/localtest.xedl");
    }
		
}
