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
 * serviceInterfaces.java
 *
 * Created on 6 de septiembre de 2004, 16:26
 */

package services.isabel.services.guis;

import services.isabel.lib.*;

/**
 *
 * @author  lailoken
 */
public interface ServiceDefinitions {

  // Para definir uno o dos paneles:
  public static final int ONE_PANEL_MODE = 0;
  public static final int TWO_PANEL_MODE = 1;
  
  // Valores para servicios ya definidos:
  
  // ---- DEFAULT ---- //
  public static final int DEFAULT_SERVICE = 0;
  public static final int DEFAULT_PANEL_MODE = TWO_PANEL_MODE;
  public static final String DEFAULT_ICON_NAME = "/general/isabelcscw24.gif";
  public static final String DEFAULT_INSTRUCTIONS = "   Instructions not found.";

  //------ABOUT ------//
  
  public static final String ABOUT_ICON_NAME = "/general/shark.gif";
  public static final String ABOUT_COPYRIGHT = "Affero GNU General Public License\r\nCopyright 2006-2010 Agora Systems S.A. \r\nhttp://www.agora-2000.com\r\n\r\n"; 
  
  // ---- C2C ---- //
  public static final int CONTACT_SERVICE = 1;
  public static final int CONTACT_PANEL_MODE = TWO_PANEL_MODE;
  public static final String CONTACT_ICON_NAME = "/general/isabelcscw24.gif";
  public static final String CONTACT_INSTRUCTIONS = "Easy way of calling another Isabel terminal to establish a session with him: \r\n\r\n   1.- Type an address (IP or name) or select a contact from the agenda. Manage your contact agenda by pressing the \"Agenda...\" button. \r\n\r\n   2.- Type the nickname and location that identify you in the session*. \r\n\r\n   3.- Press the \"Call!\" button and you will enter into a session with the called terminal**. \r\n\r\n*Quick Call will create a session configured with a telemeeting at 1Mb/s. \r\n\r\n**Quick Call can create a multipoint by calling several terminals one after the other.";
  //public static final String CONTACT_INSTRUCTIONS = " Start a videoconference with one of your contacts in three steps: \n\n   1.- Select a contact from the list or just type its ip address. You can manage your contact agenda by pressing the Agenda button. \n\n   2.- Type a nickname and a location that identify you in the videoconference. \n\n   3.- Press the Call! button";

  // ---- Start Session Server ---- //
  public static final int SSS_SERVICE = 2;
  public static final int SSS_PANEL_MODE = TWO_PANEL_MODE;
  public static final String SSS_ICON_NAME = "/general/isabelcscw24.gif";
  public static final String SSS_INSTRUCTIONS = "To start this terminal as a session server: \r\n\r\n   1.- Type your session name.\r\n\r\n   2.- Select the session service and quality. \r\n\r\n   3.- Type nickname and location or select an existing profile.\r\n\r\n   4.- Click \"Start Server\" to start the session.\r\n\r\n ";
  
  // ---- Connect To ---- //
  public static final int CONNECT_TO_SERVICE = 3;
  public static final int CONNECT_TO_PANEL_MODE = TWO_PANEL_MODE;
  public static final String CONNECT_TO_ICON_NAME = "/general/isabelcscw24.gif";
  public static final String CONNECT_TO_INSTRUCTIONS = "How to connect your terminal to a session:\r\n\r\n   1.- Type or select the URL of the session you would like to join. \r\n\r\n   2.- Type nickname and location or select an existing profile. \r\n\r\n   3.- Click \"Connect\" button to enter the session.";
  //public static final String CONNECT_TO_INSTRUCTIONS = "   Connect your terminal to an ISABEL session in three steps:\n\n   1.- Type or select the URL that defines the session you want to join.\n\n   2.- Type a nickname and a location that identifies your terminal. You can protect your nickname with a password.\n\n   3.- Click the Connect button to join the session and feel the ISABEL experience!";
  
  // ---- LoadXEDL ---- //
  public static final int LOAD_XEDL_SERVICE = 4;
  public static final int LOAD_XEDL_PANEL_MODE = TWO_PANEL_MODE;
  public static final String LOAD_XEDL_ICON_NAME = "/general/isabelcscw24.gif";
  public static final String LOAD_XEDL_INSTRUCTIONS = "   How to open an xedl file: \n\n 1.- Type or select the URL of the session you would like to view and clik the Ok button. \n\n 2.- If you know the file that describes the session, clic in the Open button and select it.";
  
  // ---- IRiS ---- //
  public static final int IRIS_SERVICE = 5;
  public static final int IRIS_PANEL_MODE = TWO_PANEL_MODE;
  public static final String IRIS_ICON_NAME = "/general/isabelcscw24.gif";
  public static final String IRIS_INSTRUCTIONS = "   Use this service to subscribe and connect a remote ISABEL terminal to an ISABEL session (defined by an URL).\n\n   The URL is composed by two fields: the first part is the IP address or domain name of the ISABEL entry point, wich is the terminal that will receive the new connection. The second part is the name of the session you want to connect.\n\n   The Nickname and password will identify the new terminal in the session.";
    
  // ---- EDIT SESSION ---- //
  public static final int EDIT_SESSION_SERVICE = 6;
  public static final int EDIT_SESSION_PANEL_MODE = ONE_PANEL_MODE;
  public static final String EDIT_SESSION_ICON_NAME = "/general/isabelcscw24.gif";
  public static final String EDIT_SESSION_INSTRUCTIONS = "   Instructions not found.";

  // ---- REMOTE CONTACT ---- //
  public static final int REMOTE_CONTACT_SERVICE = 7;
  public static final int REMOTE_CONTACT_PANEL_MODE = TWO_PANEL_MODE;
  public static final String REMOTE_CONACT_ICON_NAME = "/general/isabelcscw24.gif";
  public static final String REMOTE_CONTACT_INSTRUCTIONS = "   Instructions not found.";
  
  // ---- IPSEC LoadXEDL ---- //
  public static final int IPSEC_LOAD_XEDL_SERVICE = 8;
  public static final int IPSEC_LOAD_XEDL_PANEL_MODE = TWO_PANEL_MODE;
  public static final String IPSEC_LOAD_XEDL_ICON_NAME = null;
  public static final String IPSEC_LOAD_XEDL_INSTRUCTIONS = "   Use this service to start-stop terminals that have been subscribed to a session. There are three options:\n\n    - If you are subscribed to the session you want to manage, just select its url from the URL list.\n\n    - If you are not subscribed to the session you want to manage write the session's URL in the URL field and clic ok.\n\n    - If you know the file that describes the session, clic in the Open button and select it.";

  // ---- MGEN LoadXEDL ---- //
  public static final int MGEN_LOAD_XEDL_SERVICE = 9;
  public static final int MGEN_LOAD_XEDL_PANEL_MODE = TWO_PANEL_MODE;
  public static final String MGEN_LOAD_XEDL_ICON_NAME = null;
  public static final String MGEN_LOAD_XEDL_INSTRUCTIONS = "   Use this service to run bandwidth tests between terminals that have been subscribed to a session. There are three options:\n\n    - If you are subscribed to the session you want to test, just select its url from the URL list.\n\n    - If you are not subscribed to the session you want to test write the session's URL in the URL field and clic ok.\n\n    - If you know the file that describes the session, clic in the Open button and select it.";

  // ---- GXEDL LoadXEDL ---- //
  public static final int GXEDL_LOAD_XEDL_SERVICE = 10;
  public static final int GXEDL_LOAD_XEDL_PANEL_MODE = TWO_PANEL_MODE;
  public static final String GXEDL_LOAD_XEDL_ICON_NAME = "/general/isabelcscw24.gif";
  public static final String GXEDL_LOAD_XEDL_INSTRUCTIONS = "   Use this service to view the ISABEL network topology of a session. There are three options:\n\n    - If you are subscribed to the session you want to view, just select its url from the URL list.\n\n    - If you are not subscribed to the session you want to view write the session's URL in the URL field and clic ok.\n\n    - If you know the file that describes the session, clic in the Open button and select it.";

}
