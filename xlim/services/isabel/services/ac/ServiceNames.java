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
 * ServiceNames.java
 *
 * Created on 19 de enero de 2004, 12:27
 */

package services.isabel.services.ac;

import java.io.*;

import java.util.Map;
import java.util.HashMap;
import java.util.Iterator;
import java.util.concurrent.locks.ReentrantReadWriteLock.ReadLock;


/**
 * Esta clase se encarga de conseguir los nombres de los servicios. Tambien se
 * encarga de hacer las conversiones entre nombres que se presentan al usuario
 * y los nombres que aparecen en el xedl. Enumerationel resto de la clase se
 * llaman nombres p�blicos a los primeros y nombres privados a los �ltimos.
 * @author  Fernando Escribano
 */
public class ServiceNames implements AcceptConnectionsStrings{
    
    /**
     * Tabla donde se guardan los nombre privados direccionados por los publicos.
     */
    private Map publicMap;
    
    /**
     * Tabla donde se guardan los nombres p�blicos direcionados por los privados.
     */
    private Map privateMap;
    
    /**
     * Crea un nuevo objeto serviceNames.
     */
    public ServiceNames() {
        // Creo las tablas...
        publicMap = new HashMap();
        privateMap = new HashMap();
        
        // ... y las lleno.
        getServices();
    }
    
    /**
     * Este m�todo devuelve la lista de nombres de servicios disponibles para
     * mostrar al usuario.
     * @return Un array con los nombres p�blicos.
     */
    public String[] getServicesPublicNames() {
        Iterator it = publicMap.keySet().iterator();
        //Local-Test no lo añado
        String[] result = new String[publicMap.keySet().size() -1];
        int i = 0;
        while(it.hasNext()) {
        	String temp = (String)it.next();
        	//no añado el localtest
        	if(!temp.equals("Local-Test"))
        	{
        		result[i++] = temp;
        	}
        }
        
        return result;
    }
    
    /**
     * Devuelve el nombre p�blico que corresponde al nombre privado que se pasa
     * como par�metro.
     * @param name Nombre privado.
     * @return nombre publico correspondiente.
     */
    public String getPublicNameFor(String name) {
        return (String)privateMap.get(name);
    }
    
    /**
     * Devuelve el nombre privado que corresponde al nombre p�blico que se pasa
     * como par�metro.
     * @param name Nombre p�blico.
     * @return nombre privado correspondiente.
     */    
    public String getPrivateNameFor(String name) {
        return (String)publicMap.get(name);
    }
    
    /**
     * Este m�todo es llamado desde el constructor para inuicializar las tablas
     * con los nombres p�blicos y privados de los servicios.
     */
    private void getServices() {
        File dir = new File(SERVICES_PATH);
        
        if (!((dir.exists()) && (dir.isDirectory())))
            return;
        
        // La lista de servicios es todos archivos que terminan en .act
        //y además contienen la línea "#@setup@ desc NOMBRE_SERVICIO"
        
        String[] installedServices = dir.list(new FilenameFilter() {
            public boolean accept(File f, String s) {
                return s.endsWith(".act");
            }
        }
        );
        
        //ahora en installedServices tengo los archivos que terminan en .act
        //además tiene que tener la línea "#@setup@ desc NOMBRE_SERVICIO"
        
        
        // Convierto los nombres. Les a�ado Tele- al principio
        // y la primera letra en mayuscula
        for (int i = 0; i < installedServices.length; i++) {
        	File posible_service = new File(dir,installedServices[i]);
        	String publicName = getRealName(posible_service);
        	if(publicName!=null)
        	{
        		privateMap.put(installedServices[i], publicName);
                publicMap.put(publicName, installedServices[i]);            
        	}            
        }
        
        // Adem�s hay que incluir los servicios definidos en el directorio act en $HOME
        dir = new File(System.getProperty("user.home"), USER_SERVICES_DIR);
        String[] actFiles = dir.list(new FilenameFilter() {
            public boolean accept(File f, String s) {
                return (s.endsWith(".act"));
            }
        }
        );
        
        // Si no existe el directorio (listFiles() devuelve null)
        // creo un array vacio
        if (actFiles == null)
            actFiles = new String[0];
        
        for (int i = 0; i < actFiles.length; i++) {
        	File posible_service = new File(dir,actFiles[i]);
        	String publicName = getRealName(posible_service);
        	if(publicName!=null)
        	{
        		privateMap.put(actFiles[i], publicName);
                publicMap.put(publicName, actFiles[i]);            
        	}            
        }
        
             
    }
    
    /**
     * metodo que comprueba si el archivo contiene la línea #@setup@ desc NOMBRE_SERVICIO
     * y devuelve el NOMBRE_SERVICIO
     * @param servicio archivo existente a parsear
     * @return NOMBRE_SERVICIO o null si no lo contiene
     */
    private String getRealName(File servicio)
    {
    	String nombre_real = null;
    	try
    	{
    		BufferedReader reader= new BufferedReader(new FileReader(servicio)); 
    		String linea= reader.readLine(); 
    		//para no mirar más allá de la línea 15
    		int numero_linea=0;
    		while(linea!=null && numero_linea<15) {    			 
    			if(linea.startsWith(DESCRIPTOR_NOMBRE))
    			{
    				//le quito el descriptor y los espacios
    				nombre_real = linea.substring(DESCRIPTOR_NOMBRE.length());
    				nombre_real = nombre_real.trim();
    				return nombre_real;
    			}
    			linea= reader.readLine();
    			numero_linea++;
    		}
    	}
    	catch (Exception e) {
    		return null;
		}
    	return nombre_real;    	
    }
    
    
    /**
     * Metodo de prueba de la clase.
     */
    public static void main(String[] args) {
    	
        ServiceNames sn = new ServiceNames();
        System.out.println(sn.publicMap);
        System.out.println(sn.privateMap);
        
    }
}
