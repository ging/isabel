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
 * AcceptConnectionsStrings.java
 *
 * Created on 3 de diciembre de 2003, 15:38
 */

package services.isabel.services.ac;

import services.isabel.lib.Constants;

/**
 * Este interfaz contiene todos los Strings empleados en la
 * aplicacion AccptConnections.
 * @author  Fernando Escribano
 */
public interface AcceptConnectionsStrings {
    
    // Textos de ayuda
    public static final String SESSION_NAME_HELP_TEXT = "Unique name to identify this session";
    public static final String ID_HELP_TEXT = "Short name to be used in a session. Use 5 letters of digits maximum";
    public static final String SERVICE_HELP_TEXT = "Activity to be performed";
    public static final String QUALITY_HELP_TEXT = "Session bandwidth to be used";
    public static final String AUTOSTART_HELP_TEXT = "Select to launch this session at startup";
    public static final String ADVANCED_BUTTON_HELP_TEXT = "Start session editor";
    public static final String START_BUTTON_HELP_TEXT = "Save and launch this session";
    public static final String CANCEL_BUTTON_HELP_TEXT = "Quit without saving";
    
    // Textos de las etiquetas y botones
    public static final String SESSION_LABEL_TEXT = "Session Name";
    public static final String SERVICE_LABEL_TEXT = "Session Service";
    public static final String QUALITY_LABEL_TEXT = "Session Quality";
    public static final String ID_LABEL_TEXT = "Nickname";
    public static final String AUTOSTART_LABEL_TEXT = "Enable Autostart";
    
    public static final String ADVANCED_BUTTON_TEXT = "Advanced";
    public static final String START_BUTTON_TEXT = "Start";
    public static final String CANCEL_BUTTON_TEXT = "Cancel";
    
    
    // Paths a programas externos
    public static final String ADVANCED_EDITOR = "xterm -e vi";
    public static final String ISABEL_LAUNCHER = "isabel -xedlconf";
    
    /**
     * Titulo de la ventana principal.
     */
    public static final String FRAME_TITLE = "ISABEL Start Session";
    
    /**
     * Path al directorio donde estan los nombres de los servicios
     * disponibles.
     */
    public static final String SERVICES_PATH = Constants.ISABEL_DIR + Constants.FILE_SEPARATOR +  "lib";
    public static final String USER_SERVICES_DIR = "act";
    
    /**
     * descriptor que tienen que tener los archivos de actividades antes del nombre del servicio
     * 
     */
    public static final String DESCRIPTOR_NOMBRE = "#@setup@ desc";
    /**
     * Posibles calidades que para seleccionar.
     */
    public static final String [] QUALITY_NAMES = {"128K", "128/256K","256K", "300K/1M", "384K",
                                                    "512K", "1M", "2M", "10M"};    
   
    /**
     * Calidad elejida por defecto.
     */
    public static final String DEFAULT_QUALITY = QUALITY_NAMES[5]; 

}
