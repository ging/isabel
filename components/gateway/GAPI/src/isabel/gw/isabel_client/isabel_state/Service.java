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
package isabel.gw.isabel_client.isabel_state;

import isabel.lib.StringParser;

import java.util.Hashtable;
import java.util.logging.Logger;

/**
 *  Clase encargada de construir un objeto Service a partir de un String procedente de un mensaje Tcl
 *  @author Isabel Bau
 *
 */
public class Service {
    
    /**
     *  Logs
     */
    private Logger mylogger;
    
    
    /**
     *  Nombre del servicio. 
     */
    private String serviceName;
    
    /**
     *  Lista de acciones definidas.
     */
    private String actions;
    
    /**
     *   Lista con la entradas de menu.
     */
    private String menuItems;
    
    /**
     *  Lista con los roles definidos.
     */
    private String roles;
    
    /**
     *  Lista con los nombres de los componentes definidos.
     */
    private String components;
    
    /**
     *  Lista con los nombres de los modos de interaccion.
     */
    private String interactionModes;
    
    /**
     *  Tabla donde se guardan el valor de las propiedades de los recursos definidos en el servicio.
     *  La clave es la cadena: "resurso.propiedad".
     *  El valor asociado es el valor de "recurso.propiedad".
     */
    private Hashtable<String,String> resources = new Hashtable<String,String>(); 


    /**
     *  Crea un objeto Service a partir de unos Strings definiendo cada parte del servicio.
     *  @param serviceName Nombre del servicio. 
     *  @param actions Lista de acciones definidas.
     *  @param menuItems Lista con la entradas de menu.
     *  @param roles Lista con los roles definidos.
     *  @param components Lista con los nombres de los componentes definidos.
     *  @param interactionModes Lista con los nombres de los modos de interaccion.
     *  @param resources Lista de "resource.property: valor" para todos los parametros de todos los
     *                   recursos del servicio.
     */ 
    public Service(String serviceName, String actions, String menuItems, String roles,
		   String components, String interactionModes, String resources) {

	mylogger = Logger.getLogger("isabel.gwsip.isabel_client.isabel_state.Service");
	mylogger.fine("Creating IsabelState service.");
	
	this.serviceName      = serviceName;
	this.actions          = actions;
	this.menuItems        = menuItems;
	this.roles            = roles;
	this.components       = components;
	this.interactionModes = interactionModes;
	
	try {
	    StringParser p1 = new StringParser(resources);
	    while(p1.hasMoreTokens()) {  
		String kv = p1.nextToken();

		StringParser p2 = new StringParser(kv);
		String key   = p2.nextToken();
		String value = p2.nextToken();
		mylogger.fine("Resource= "+key+": "+value);
		this.resources.put(key, value);				    	 
	    }
	} catch (Exception e) {			 
	    mylogger.severe("Invalid service definition: "+e.getMessage());
	}
    }


    /**
     * Devuelve el nombre del servicio.
     * @return el nombre del servicio.
     */
    public String getServiceName() {
	return serviceName;
    }


    /**
     * Devuelve el nombre de las acciones definidas en el servicio.
     * @return el nombre de las acciones definidas en el servicio.
     */
    public String getActions() {
	return actions;
    }


    /**
     * Devuelve el nombre de las entradas de menu definidas en el servicio.
     * @return el nombre de las entradas de menu definidas en el servicio.
     */
    public String getMenuItems() {
	return menuItems;
    }
    

    /**
     * Devuelve los nombres de los roles definidos en el servicio.
     * @return los nombres de los roles definidos en el servicio.
     */
    public String getRoles() {
	return roles;
    }


    /**
     * Devuelve el nombre de los componentes definidos en el servicio.
     * @return el nombre de los componentes definidos en el servicio.
     */
    public String getComponents() {
	return components;
    }
    

    /**
     * Devuelve el nombre de los modos de interaccion definidos en el servicio.
     * @return el nombre de los modos de interaccion definidos en el servicio.
     */
    public String getInteractionModes() {
	return interactionModes;
    }
    


    /** 
     *  Method to obtain the value of the given resource and property.
     *  @param name The name of a resource.
     *  @param property The name of a property.
     *  @return Returns the value of name.property, or null if it doesn't exist.
     */
    public String getProperty(String name, String property) {
        
	return resources.get(name+","+property);
    }


    /** 
     *  Methods to obtain the value of the given resource and property.
     *  @param name The name of a resource.
     *  @param property The name of a property.
     *  @param def Value to return if the name.property does not exist.
     *  @return Returns the value of name.property, or def if it doesn't exist.
     */
    public String getProperty(String name, String property, String def) {
        String value = getProperty(name,property);
        if (value != null)
            return value;
        else
            return def;
    }

}
