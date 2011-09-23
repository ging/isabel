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
/////////////////////////////////////////////////////////////////////////
//
// $Id: About.java 20206 2010-04-08 10:55:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

package services.isabel.services.about;
  
import services.isabel.services.*;
import services.isabel.services.guis.ServiceDefinitions;

// Para el manejo de menus
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

import xedl.lib.xerl.*;

import services.isabel.lib.*;


/**
 * Cliente del servicio About. Presenta una ventana
 * con la version del programa que se le pasa como parametro
 * @Author lailoken
 */
public class About { 
	public About(String version, String program)
	{
		//Sacamos un mensaje en una ventanita...
	    JFrame parentFrame = new JFrame();
	    Icon icono = new ImageIcon(getClass().getResource(ServiceDefinitions.ABOUT_ICON_NAME));
	    
	    JOptionPane.showMessageDialog(parentFrame, version, "About... " + program, JOptionPane.INFORMATION_MESSAGE, icono);
	    parentFrame.dispose();
	    System.exit(0);
	}
  
	
  public static void main (String args[]) throws Exception { 
    String program = args[0];
    String version = ServiceDefinitions.ABOUT_COPYRIGHT;
    version += Servutilities.getVersionFromLib("Isabel") + "\n";
    version += Servutilities.getVersionFromLib("IsabelExtras");
    About ventana = new About(version, program);
    
    return;
  }
}

