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
 * OptionsStrings.java
 *
 * Created on 13 de septiembre de 2003, 16:28
 */

package services.isabel.services.options;

/**
 * Esta interfaz contiene los Strings utilizados en el panel de opciones.
 * @author  Fernando Escribano
 */
public interface OptionsStrings {
    
    /***************************************************************************
     *    String utilizados por la clase OptionsPanel y el navegador de ayuda  *
     ***************************************************************************/
    /**
     * Titulo de la ventana principal.
     */
    public static final String OPTIONS_TITLE = "ISABEL Options";
    
    /**
     * Ayuda del boton de atras.
     */
    public static final String BACK_HELP = "Return to the previous panel";
    
    /**
     * Ayuda del boton de siguiente.
     */
    public static final String NEXT_HELP = "Validate data and proceed to next panel";

    /**
     * Ayuda del boton de reset
     */
    public static final String RESET_HELP = "Reset default values";
    
    /**
     * Ayuda del boton de hecho.
     */
    public static final String DONE_HELP = "Save data and close this window";
    
    /**
     * Ayuda del boton de cancel.
     */
    public static final String CANCEL_HELP = "Close without saving";
    
    /**
     * Icono del boton de atras.
     */
    public static final String BACK_ICON = "/navigation/Back16.gif";
    
    /**
     * Icono del boton de hecho.
     */
    public static final String DONE_ICON = "/actions/Save16.gif";
    
    /**
     * Icono del boton de cancelar.
     */
    public static final String CANCEL_ICON = "/actions/Delete16.gif";
    
    /**
     * Icono del boton de adelante.
     */
    public static final String FORWARD_ICON = "/navigation/Forward16.gif";
    
    /**
     * Icono del boton de home.
     */
    public static final String HOME_ICON = "/navigation/Home16.gif";
        
    /**
     * Icono de la barra de estado
     */
    public static final String STATE_BAR_ICON = "/actions/Save16.gif";
    
    /**
     * Imagen de Acerca de...
     */
    public static final String ABOUT_ISABEL_IMAGE = "/general/AboutIsabel.gif";
    
    /***************************************************************************
     *     String utilizados por la clase OptionsPanel.SiteIDPanel             *
     ***************************************************************************/
    /**
     * Pagina de ayuda del panel del identificacion del usuario.
     */
    public static final String SITE_ID_HELP_HOME = "/html/index.html";

    /**
     * Lista de nombres de roles mostrados en el selector de rol.
     */
    public static final String [] ROLE_NAMES = {"Interactive", "FlowServer (mcu)",
                                                "Recorder", "Sip Gateway", "Web Antenna", "Media Server", "Flash Gateway"};

    /**
     * Lista de roles tal y como aparecen en el xedl.
     */
    public static final String [] ROLE_XEDL_NAMES = {"interactive", "mcu",
                                                     "recorder", "sipgateway", "antenna", "MediaServer", "flashgateway"};
                                                     
    public static final String INTERACTIVE_ROLE = ROLE_NAMES[0];
    public static final String FLOWSERVER_ROLE = ROLE_NAMES[1];
    public static final String RECORDER_ROLE = ROLE_NAMES[2];
    public static final String GATEWAY_ROLE = ROLE_NAMES[3];    
    public static final String ANTENNA_ROLE = ROLE_NAMES[4];
    public static final String MEDIASERVER_ROLE = ROLE_NAMES[5];
    public static final String FLASH_GATEWAY_ROLE = ROLE_NAMES[6];

                                                
    /**
     * Icono del panel de Site Identification.
     */
    public static final String SITE_ID_ICON = "/options/site.gif";
    
    /**
     * Titulo del panel de Site Identification.
     */
    public static final String SITE_ID_TITLE = "Site Identification";
    
    /**
     * Ayuda del campo Site ID.
     */
    public static final String SITE_ID_HELP = "";
    
    /**
     * Ayuda del campo Access control.
     */
    public static final String ACCESS_CONTROL_HELP = "";
    
    /**
     * Ayuda del campo Public Name.
     */
    public static final String PUBLIC_NAME_HELP = "";
    
    /**
     * Ayuda del campo User ID.
     */    
    public static final String USER_ID_HELP = "";
    
    /**
     * Ayuda del campo e-mail.
     */
    public static final String E_MAIL_HELP = "";
    
    /**
     * Ayuda del campo URL.
     */
    public static final String URL_HELP = "";
    
    /**
     * Ayuda del campo Site Address.
     */
    public static final String SITE_ADDRESS_HELP = "";
    
    /**
     * Ayuda del campo Role.
     */
    public static final String SITE_ROLE_HELP = "";

    /***************************************************************************
     *                   STRING DE LA BARRA DE MENUS Y SUS ACCIONES            *
     ***************************************************************************/
    
    /**
     * Nombre del menu de perfiles
     */
    public static final String PROFILE_MENU_NAME = "Profiles";
    
    /**
     * Nombre del menuitem de nuevo perfil
     */
    public static final String NEW_PROFILE_ITEM_NAME = "New profile...";
    
    /**
     * Nombre del menuitem de cargar perfil
     */
    public static final String LOAD_PROFILE_ITEM_NAME = "Load profile...";
    
    /**
     * Nombre del menuitem de salvar perfil
     */
    public static final String SAVE_PROFILE_ITEM_NAME = "Save";
    
    /**
     * Nombre del menuitem de salvar como ..
     */
    public static final String SAVEAS_PROFILE_ITEM_NAME = "Save as...";
    
    /**
     * Nombre del menuitem de borrar perfil
     */
    public static final String DELETE_PROFILE_ITEM_NAME = "Delete";
    
    
    /**
     * Nombre del menu de vistas.
     */
    public static final String VIEW_MENU_NAME = "View";
    
    /**
     * Nombre del menuitem de ver opciones avanzadas.
     */
    public static final String VIEW_AD_ITEM_NAME = "Show Advanced Options";
    
    /**
     * Nombre del menuitem de ver opciones ipsec.
     */
    public static final String VIEW_IPSEC_ITEM_NAME = "Show IPSec Options";
    
    /**
     * Nombre del menu de ayuda.
     */
    public static final String ABOUT_MENU_NAME = "About";
    
    /**
     * Nombre del menuitem de sobre isabel...
     */
    public static final String ABOUT_ISABEL_ITEM_NAME = "About Isabel";    
    
    
    /***************************************************************************
     *     String utilizados por la clase OptionsPanel.CertifiedIdentityPanel  *
     ***************************************************************************/
    /**
     * Pagina de ayuda del panel de acceso de red
     */
    public static final String CERT_ID_HELP_HOME = "/html/index.html";
    
    /**
     * Icono del panel Network access.
     */
    public static final String CERT_ID_ICON = "/options/certid.gif";
       
    /**
     * Titulo del panel network access.
     */
    public static final String CERT_ID_TITLE = "Certified Identity";
    
    
    /***************************************************************************
     *     String utilizados por la clase OptionsPanel.NetworkAccessPanel      *
     ***************************************************************************/
    
    /**
     * Pagina de ayuda del panel de acceso de red
     */
    public static final String NETWORK_HELP_HOME = "/html/index.html";
    
    /**
     * Icono del panel Network access.
     */
    public static final String NETWORK_ACCESS_ICON = "/options/access.gif";
       
    /**
     * Titulo del panel network access.
     */
    public static final String NETWORK_ACCESS_TITLE = "Network Access";
    
    /**
     * Ayuda del campo Active ISDN channels.
     */
    public static final String ACTIVE_ISDN_CHANNELS_HELP = "";
        
    /***************************************************************************
     *     String utilizados por la clase OptionsPanel.BandwidthPanel          *
     ***************************************************************************/
    /**
     * Pagina de ayuda del panel del bandwidth
     */
    public static final String BANDWIDTH_HELP_HOME = "/html/index.html";
    
    /**
     * Icono del panel Connection Mode.
     */
    public static final String BANDWIDTH_ICON = "/options/cmode.gif";
    
    /**
     * Titulo del panel connection mode.
     */
    public static final String BANDWIDTH_TITLE = "Session Bandwidth";

    /***************************************************************************
     *     String utilizados por la clase OptionsPanel.ConnectionModePanel     *
     ***************************************************************************/
    /**
     * Pagina de ayuda del panel del modo de conexion
     */
    public static final String CMODE_HELP_HOME = "/html/index.html";

    /**
     * Icono del panel Connection Mode.
     */
    public static final String CONNECTION_MODE_ICON = "/options/cmode.gif";
    
    /**
     * Titulo del panel connection mode.
     */
    public static final String CONNECTION_MODE_TITLE = "Connection Mode";
   
    /**
     * Ayuda del boton de configuracion de grupos multicast.
     */
    public static final String CONFIG_MULTICAST_HELP = "";
    
    /**
     * Ayuda del campo Peer Address.
     */
    public static final String PEER_ADDRESS_HELP = "";
   
    /**
     * Ayuda del campo MCU address.
     */
    public static final String MCU_ADDRESS_HELP = "";
    
    /**
     * Ayuda del campo TTL.
     */
    public static final String MULTICAST_TTL_HELP = "";

    /**
     * Posibles anchos de banda mostrados en los selectores de ancho de banda.
     */
    public static final String[] AVALAIBLE_BANDWIDTHS = {"56 K", "128 K", "256 K",
                                                         "384 K", "512 K", "1 M", "2 M" };
    
    /**
     * Ancho de banda de subida por defecto.
     */                                                         
    public static final String DEFAULT_UP_BW = AVALAIBLE_BANDWIDTHS[5];                                                         
    
    /**
     * Ancho de banda de bajada por defecto.
     */                                                         
    public static final String DEFAULT_DOWN_BW = AVALAIBLE_BANDWIDTHS[5];                                                         
    
    /***************************************************************************
     *     String utilizados por la clase OptionsPanel.AdminInfoPanel          *
     ***************************************************************************/
    /**
     * Pagina de ayuda del panel del informacion administrativa.
     */
    public static final String ADMIN_HELP_HOME = "/html/index.html";
    
    /**
     * Icono del panel Admin Info
     */
    public static final String ADMIN_INFO_ICON = "/options/admin.gif";
    
    /**
     * Titulo del panel Admin Info
     */
    public static final String ADMIN_INFO_TITLE = "Administrative Information";

    /***************************************************************************
     *     String utilizados por la clase OptionsPanel.IPSecPanel              *
     ***************************************************************************/
    /**
     * Pagina de ayuda del panel del informacion administrativa.
     */
    public static final String IPSEC_HELP_HOME = "/html/index.html";
    
    /**
     * Icono del panel Admin Info
     */
    public static final String IPSEC_ICON = "/options/ipsec.gif";
    
    /**
     * Titulo del panel Admin Info
     */
    public static final String IPSEC_TITLE = "IPSEC Configuration";
    /***************************************************************************
     *     String utilizados por la clase OptionsPanel.ErrorProtectionPanel    *
     ***************************************************************************/
    /**
     * Pagina de ayuda del panel del informacion administrativa.
     */
    public static final String FEC_HELP_HOME = "/html/index.html";
    
    /**
     * Icono del panel Admin Info
     */
    public static final String FEC_ICON = "/options/fec2.gif";
    
    /**
     * Titulo del panel Admin Info
     */
    public static final String FEC_TITLE = "Error Protection";
    
    /**
     * Titulo del panel Admin Info
     */
    public static final String PARITY_TITLE = "F.E.C Overhead";
    
    /***************************************************************************
     *  String utilizados por la clase OptionsPanel.AditionalParametersPanel   *
     ***************************************************************************/
    /**
     * Pagina de ayuda del panel del informacion administrativa.
     */
    public static final String AD_PARAMS_HELP_HOME = "/html/index.html";
    
    /**
     * Icono del panel Admin Info
     */
    public static final String AD_PARAMS_ICON = "/options/adparams.gif";
    
    /**
     * Titulo del panel Admin Info
     */
    public static final String AD_PARAMS_TITLE = "Local Parameters";
    
    /**
     * Parametros adicionales por defecto.
     */
    public static final String[] DEFAULT_AD_PARAMS = {"ISABEL_AUDIOMIXER=true",
                                                     "ISABEL_CTRL_SITES=",
                                                     "ISABEL_AUDIO_BYPASS="};
    /***************************************************************************
     *     String utilizados por la clase OptionsPanel.RolePanel          *
     ***************************************************************************/
    /**
     * Pagina de ayuda del panel del rol
     */
    public static final String ROLE_HELP_HOME = "/html/index.html";
    
    /**
     * Paths a los iconos del panel del rol
     */
    public static final String ROLE_ICON_PATH = "/options/roles/";
    
    /**
     *Extension de los ficheros de iconos de roles.
     */
    public static final String ROLE_ICON_EXT = ".gif";
    
    /**
     * Titulo del panel ROLE
     */
    public static final String ROLE_TITLE = "Site Role";
    
    /**
     * Texto de ayuda del rol interactivo.
     */
    public static final String INTERACTIVE_ROLE_HELP = "Participate in the event";
    
    /**
     * Texto de ayuda del rol antenna.
     */
     public static final String ANTENNA_ROLE_HELP = "Broadcast the session for Web browsers";
     
    /**
     * Texto de ayuda del rol recorder
     */
     public static final String RECORDER_ROLE_HELP = "Record the session in an AVI file";
     
    /**
     * Texto de ayuda del rol gateway
     */
     public static final String GATEWAY_ROLE_HELP = "SIP clients access point";
     
    /**
     * Texto de ayuda del rol mediaserver
     */
     public static final String MEDIASERVER_ROLE_HELP = "SIP clients access point";
     
    /**
     * Texto de ayuda del rol flowserver.
     */
     public static final String FLOWSERVER_ROLE_HELP = "Multipoint Control Unit";
     
    /***************************************************************************
     *     String utilizados por la clase OptionsPanel.MulticastGroupsPanel    *
     ***************************************************************************/
    /**
     * Pagina de ayuda del panel de grupos multicast
     */
    public static final String MULTICAST_HELP_HOME = "/html/index.html";

    /**
     * Icono del panel de grupos multicast.
     */
    public static final String MULTICAST_GROUPS_ICON = "/options/multicast.gif";
    
    /**
     * Titulo del panel de grupos multicast.
     */
    public static final String MULTICAST_GROUPS_TITLE = "Multicast Groups";
    
    /**
     * Titulo del panel de multicast.
     */
    public static final String MULTICAST_TITLE = "Multicast Configuration";
        
    /**
     * Ayuda del campo audio group.
     */
    public static final String AUDIO_GROUP_HELP = "";
    
    /**
     * Ayuda del campo video group.
     */
    public static final String VIDEO_GROUP_HELP = "";
    
    /**
     * Ayuda del campo ftp group.
     */
    public static final String FTP_GROUP_HELP = "";
        
    /**
     * Ayuda del campo pointer group.
     */
    public static final String POINTER_GROUP_HELP = "";
    
    /**
     * Ayuda del campo ctrl wp group.
     */
    public static final String VUMETER_GROUP_HELP = "";
    
    /**
     * Ayuda del campo app sharing group.
     */
    public static final String APP_SHARING_GROUP_HELP = "";
    
    
    /***************************************************************************
     *     String utilizados por la clase OptionsPanel.MediaControlsPanel      *
     ***************************************************************************/
    /**
     * Pagina de ayuda del panel de displays de control.
     */
    public static final String MEDIA_CONTROLS_HELP_HOME = "/html/index.html";
    
    /**
     * Icono del panel Media Controls.
     */
    public static final String MEDIA_CONTROLS_ICON = "/options/mcontrols.gif";
    
    /**
     * Titulo del panel Media control.
     */
    public static final String MEDIA_CONTROLS_TITLE = "Media Controls";
        
    /**
     * Ayuda del campo video display.
     */
    public static final String VIDEO_CONTROL_DISPLAY_HELP = "";
    
    /**
     * Ayuda del campo audio display.
     */
    public static final String AUDIO_CONTROL_DISPLAY_HELP = "";
    
    /**
     * Ayuda del campo default display.
     */
    public static final String DEFAULT_CONTROL_DISPLAY_HELP = "";
    
    /**
     * Ayuda del campo app sharing display.
     */
    public static final String APP_SHARING_CONTROL_DISPLAY_HELP = "";
    
    /**
     * Ayuda del campo data display.
     */
    public static final String DATA_CONTROL_DISPLAY_HELP = "";
    
    /**
     * Ayuda del campo vnc display.
     */
    public static final String VNC_SERVER_DISPLAY_HELP = "";

    /***************************************************************************
     *                   STRING UTILIZADOS EN VARIOS PANELES                   *
     ***************************************************************************/

    /**
     * Icono de ayuda.
     */
    public static final String HELP_ICON = "/actions/Help16.gif";
    
    /**
     * Flecha hacia arriba de 16 x 16.
     */
    public static final String SMALL_UP_ICON = "/navigation/Up16.gif";
    
    /**
     * Flecha hacia abajo de 16 x 16.
     */
    public static final String SMALL_DOWN_ICON = "/navigation/Down16.gif";
    
    /**
     * Icono de la ventana.
     */
    public static final String WINDOW_ICON = "/general/shark.gif";
    
    /**
     * Fuente empleada en los titulos de los paneles.
     */
    public static final String TITLE_FONT = "serif";
    
    /**
     * Color del titulo. No es un String es un objeto tipo Color.
     */
    public static final java.awt.Color TITLE_COLOR = java.awt.Color.BLACK;
    
    /***************************************************************************
     *             EXPRESIONES REGULARES PARA VALIDAR LOS CAMPOS               *
     ***************************************************************************/
    
    /**
     * Expresion regular que deben cumplir las direcciones de email.
     */
    public static final String EMAIL_REGEX = "^[a-zA-Z0-9]+([_\\-\\.]*[a-zA-Z0-9])*@([a-zA-Z0-9_\\-]+\\.)+[a-zA-Z]{2,4}$";
    
    /**
     * Expresion regular que deben cumplir los nombre de los contactos.
     * Son validos cualquier combinacion de mayusculas, minusculas, numeros,
     * guiones, subrayados, puntos y espacios de hasta 20 caracteres.
     */
    public static final String NAME_REGEX = "^[a-zA-Z����������0-9_\\-\\.\\ ]{1,20}$";
    
    /**
     * Expresion regular que deben cumplir los public names.
     * Son validos cualquier combinacion de mayusculas, minusculas, numeros,
     * guiones, subrayados, puntos y espacios de hasta 20 caracteres.
     */
    public static final String PUBLIC_NAME_REGEX = "^[a-zA-Z����������0-9_\\-\\.\\ ]{1,20}$";
    
    /**
     * Expresion regular que deben cumplir los nombre de instituciones
     * Son validos cualquier combinacion de mayusculas, minusculas, numeros,
     * guiones, subrayados, puntos y espacios de hasta 20 caracteres.
     */
    public static final String INSTITUTION_NAME_REGEX = "^[a-zA-Z����������0-9_\\-\\.\\ ]{1,20}$";
    
    /**
     * texto de la profileLabel
     */
    public static final String PROFILE_LABEL_TEXT = "Profile: ";
    
    
}
