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
/*
 * Contacts.java
 *
 * Created on 8 de Julio de 2004
 */

package services.isabel.lib;

import java.io.*;
import java.util.*;

import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;

import xedl.lib.jaxb.Edl;
import xedl.lib.jaxb.SITE;
import xedl.lib.jaxb.SiteDescription;
import xedl.lib.xedl.*;

/**
 * Esta clase contiene m�todos para acceder, crear y borrar el sistema de ficheros
 * que forman los contactos.
 * @author Fernando Escribano, Lailoken
 */
public class Contacts {
    
	
	/**
	 * configuración de trazas
	 */
	PrintWriter outTraceFile= null;
	
        
    /**
     * Nombre del directorio que guarda la agenda.
     */
    private static final String CONTACTS_DIRECTORY = "contacts";
    
    /**
     * Nombre del directorio donde se almacenan los ficheros temporales.
     */
    private static final String TEMP_DIRECTORY = "tmp";
        
    
    /**
     * Directorio a partir del cual se encuentra la agenda.
     */
    private File rootDir;
    
    
    /**
     * Directorio de la agenda de contactos.
     */
    private File contactsDir;
    
    
    /**
     * Directorios con los ficheros temporales.
     */
    private File tmpDir;
        
    
    /**
     * Crea un objeto tipo Contacts. En caso de que no existan los ficheros y
     * directorios necesarios para guardar los datos de los contacts son creados.
     * Los contactos se crean a partir del directorio $HOME/.isabel/.
     * @throws IOException Si surge algun problema al acceder a los ficheros o al crearlos.
     */
    public Contacts() throws IOException {
      this(getDefaultContactsRoot());
    }
    
    /**
     * Crea un objeto tipo Contacts. En caso de que no existan los ficheros y
     * directorios necesarios para guardar los datos de los contactos son creados.
     * @param root Path a partir del cual se encuentra el sistema de ficheros
     * que forman los contactos. En general este parametro debe ser ~/.isabel/
     * @throws IOException Si surge algun problema al acceder a los ficheros o al crearlos.
     */
    public Contacts(String root) throws IOException {        
      //fijamos las trazas
      outTraceFile = Constants.inicializa_trazas_jaxb();	 
      
      // Compruebo que la raiz que me han pasado en un directorio
      rootDir = new File(root);
      if (!rootDir.isDirectory())
          throw new java.io.IOException(root + " is not a directory");

      // Creo todos los directorios necesarios si no existian previamente.
      contactsDir = new File(rootDir, CONTACTS_DIRECTORY);
      contactsDir.mkdir();
      tmpDir = new File(rootDir, TEMP_DIRECTORY);
      tmpDir.mkdir();
    }

    /**
     * Crea un nuevo contacto en la agenda de contactos a partir de los datos suministrados
     * @param name Nombre del contacto (el ID del contacto). Es el nombre que se utilizara para almacenar la informacion del contacto en un fichero
     * @param contact Informacion del contacto en formato XEDL.
     * @return true si se pudo crear el contacto; false en caso contrario.
     */
    public boolean createContact(String name, SITE contact) {
      try {
    	outTraceFile.println("Creamos contacto " + name);
    	// Metemos el XEDL dado en la agenda de contactos en el fichero name.xml
        XEDL xedl = new XEDL();
        Edl miedl = xedl.getEdl();
        
        SiteDescription sitedes = new SiteDescription();
        List<SITE> lista = sitedes.getSITE();
        lista.add(contact);
        miedl.setSiteDescription(sitedes);
        xedl.save(contactsDir.getAbsolutePath() + Constants.FILE_SEPARATOR + name + "." + Constants.XEDL_EXTENSION);
        return true;
      } catch (Exception e) {
    	  e.printStackTrace();
    	  e.printStackTrace(outTraceFile);
        return false;
      }
    }
    
    /**
     * Crea un nuevo contacto en la agenda de contactos a partir de los datos suministrados
     * @param xedlName Nombre del fichero XEDL de donde queremos sacar la informacion del contacto
     * @param site Sitio que queremos a�adir a los contactos
     * @return true si se pudo crear el contacto; false en caso contrario.
     */
    public boolean createContact(XEDL xedl, String siteID) {
      try {
    	outTraceFile.println("Creamos contacto " + siteID);
    	SITE contact = xedl.getEdl().getSiteDescription().getSITE(siteID);
        XEDL newxedl = new XEDL();
        Edl miedl = newxedl.getEdl();
        
        SiteDescription sitedes = new SiteDescription();
        List<SITE> lista = sitedes.getSITE();         
        lista.add(contact);
        miedl.setSiteDescription(sitedes);
        
        newxedl.save(contactsDir.getAbsolutePath() + Constants.FILE_SEPARATOR + contact.getID() + "." + Constants.XEDL_EXTENSION);
        return true;
      } catch (Exception e) {
    	  e.printStackTrace();
    	  e.printStackTrace(outTraceFile);
        return false;
      }
    }
    
    /**
     * Crea un nuevo contacto en la agenda de contactos a partir de los datos suministrados
     * @param xedlName Nombre del fichero XEDL de donde queremos sacar la informacion del contacto
     * @param site Sitio que queremos a�adir a los contactos
     * @return true si se pudo crear el contacto; false en caso contrario.
     */
    public boolean createContact(String name, String fullName, LinkedList ips) {
      try {
    	outTraceFile.println("Creamos contacto " + name + " fullname " + fullName);
    	//Site contact = xedl.createSiteList().getSite(site);
        XEDL cxedl = new XEDL();
        Edl miedl = cxedl.getEdl(); 
        
        SiteDescription sitedes = new SiteDescription();
        List<SITE> lista = sitedes.getSITE();         
        
        SITE contact = new SITE();
        contact.setID(name);
        contact.setPUBLICNAME(fullName);
        // Metemos las ips en el xedl..
        for (int i=0; i<ips.size(); i++) 
        	contact.addSITEADDRESS(ips.get(i).toString());
        //contact.setNetworkAccessEthernet();
        lista.add(contact);
        
        miedl.setSiteDescription(sitedes);
        cxedl.save(contactsDir.getAbsolutePath() + Constants.FILE_SEPARATOR + name + "." + Constants.XEDL_EXTENSION);
        return true;
      } catch (Exception e) {
    	  e.printStackTrace();
    	  e.printStackTrace(outTraceFile);
        return false;
      }
    }
    
    /**
     * Crea un nuevo contacto en la agenda de contactos a partir de los datos suministrados
     * @param name Nombre del contacto (el ID del contacto). Es el nombre que se utilizara para almacenar la informacion del contacto en un fichero
     * @param contact Informacion del contacto en formato XEDL.
     * @return true si se pudo crear el contacto; false en caso contrario.
     */
    public boolean createContact(String session, String site) {
      try {
    	outTraceFile.println("Creamos contacto para la sesion " + session);
    	// Sacamos el xedl de la sesion
        Agenda agenda;
        if (Constants.AGENDA_ROOT_DIR != null) agenda = new Agenda(Constants.AGENDA_ROOT_DIR);
        else agenda = new Agenda();
        // Metemos el XEDL dado en la agenda de contactos en el fichero name.xml
        XEDL fullXedl = new XEDL(agenda.getFullXedl(session).getAbsolutePath());
        return createContact (fullXedl, site);
      } catch (Exception e) {
    	  e.printStackTrace();
    	  e.printStackTrace(outTraceFile);
        return false;
      }
    }
    
    /**
     * Borra el contacto de la agenda de contactos. Si no existe no hace nada.
     * @param name El nombre del contacto.
     */
    public void deleteContact(String name) {
    	outTraceFile.println("Borramos contacto " + name);
    	File contact = new File(contactsDir, name + "." + Constants.XEDL_EXTENSION);
        contact.delete();
    }
    
    /**
     * Devuelve una lista con los nombres de todos los contactos almacenados.
     * @return Una LinkedList con los nombres de todos los contactos almacenados.
     */
    public LinkedList getContacts() {
      outTraceFile.println("Metodo getContacts de la clase Contacts");
      String[] filesList =  contactsDir.list();
      LinkedList contactList = new LinkedList();
      for (int i=0;i<filesList.length;i++) {
        try {
          contactList.add(filesList[i].substring(0, filesList[i].lastIndexOf(".")));
        } catch (Exception e) {
        }
      }
      return contactList;
    }
    
    
    /**
     * Dice si el contacto cuyo nombre se pasa esta definido en la agenda.
     * @param name Nomnbre del contacto.
     * @return True si el contacto esta definido en la agenda de contactos.
     */
    public boolean exists(String name) {
      File contact = new File(contactsDir,name+"." + Constants.XEDL_EXTENSION);
      return contact.exists();
    }
    
    
    
    
    /**
     * Devuelve el fichero que contiene la definicions del contacto indicado.
     * @param name Nombre del contacto.
     * @return Objeto File del fichero XEDL con la definicion del contacto.
     * @throws IOException Si la session no esta definida.
     */
    public File getContact(String name) throws IOException {
    	outTraceFile.println("getcontact " + name);
    	return new File(contactsDir,name + "." + Constants.XEDL_EXTENSION);
    }
            
    
    /***************************************************************************
     *                            METODOS PRIVADOS                             *
     **************************************************************************/
    
    /**
     * Borra todos los ficheros contenidos en un directorio.
     * @param f El directorio a borrar.
     */
    private void deleteDirectory(File f) {
        // Listo los ficheros del directorio
        File[] contents = f.listFiles();
        
        // Borro cada fichero contenido
        // utilizando deleteDirectory si es un directorio.
        for (int i = 0; i < contents.length; i++) {
            if(contents[i].isDirectory())
                deleteDirectory(contents[i]);
            contents[i].delete();
        }
        // Finalmente borro el propio directorio
        f.delete();
    }

    /**
     * Este m�todo devuelve una cadena de texto que es el path al directorio
     * base de la agenda que se emplea normalmente.
     * Si el directorio no existe lo crea.
     */
    private static String getDefaultContactsRoot() {
        String root = Constants.ISABEL_USER_DIR;
        new File(root).mkdir();
        return root;
    }
    

    
    /**
     * Metodo de prueba de la clase.
     * @param args Argumentos de la line de comando. No se usan.
     */
    public static void main (String[] args) throws Exception {
      // Probamos la agenda de contactos.....
      BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
      Unmarshaller	unmarshaller = null;
      unmarshaller = JAXBSingleton.getUnmarshaller();
      
      // Abrimos un fichero XEDL
      System.out.println("Abrimos el ficehro XEDL");
      XEDL xedl = new XEDL(args[0]);
      Contacts contacts = new Contacts();
      
      System.out.println("creamos un contacto a partir de nombre y ips");
      LinkedList<String> ips = new LinkedList<String>();
      ips.add("127.0.0.1");
      ips.add("otro.dit.upm.es");
      contacts.createContact("otro", "otro.dit.upm.es",ips );
      System.out.println("A ido bien.\nPulse Enter para continuar...");
      in.readLine();
      
      
      System.out.println("cogemos todos los sites...");
      Edl tempo = xedl.getEdl();
      List<SITE> lista = xedl.getEdl().getSiteDescription().getSITE(); 
            
      for (int i = 0; i<lista.size(); i++) {
        SITE sitio = lista.get(i);
        System.out.println("Metemos en los contactos al sitio: " + sitio.getID());
        contacts.createContact(xedl, sitio.getID());
      }
      System.out.println("Pulse Enter para continuar...");
      in.readLine();
      
      System.out.println("Probamos getContacts"); 
      LinkedList contactsInContacts = contacts.getContacts();
      for (int i = 0; i<contactsInContacts.size(); i++) {
        System.out.println("Encontrado contacto " + contactsInContacts.get(i));
      }
      System.out.println("Pulse Enter para continuar...");
      in.readLine();
      
      System.out.println("Probamos getContact. con id local"); 
      System.out.println("Pulse Enter para continuar...");
      in.readLine();
      String id = "azul";
      XEDL xedl2 = new XEDL (contacts.getContact(id).getAbsolutePath());
      SITE sitio2 = xedl2.getEdl().getSiteDescription().getSITE(id);
      
      System.out.println("El public name es: " + sitio2.getPUBLICNAME());
      System.out.println("la ip es : " + sitio2.getSITEADDRESS());
      
      System.out.println("Probamos deleteContact. Dame un ID de un contacto que quieras borrar."); 
      id = in.readLine();
      contacts.deleteContact(id);
      
      System.out.println("Probamos exists. Dame el id de un contacto que quieras saber si existe."); 
      id = in.readLine();
      if (contacts.exists(id)) System.out.println("Existe el contacto.");
      else System.out.println("NO Existe el contacto.");
      
      
    }
}
