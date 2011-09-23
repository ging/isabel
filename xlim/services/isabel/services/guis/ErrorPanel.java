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
package services.isabel.services.guis;

import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JOptionPane;

import services.isabel.lib.Servutilities;
import services.isabel.services.*;
import services.isabel.services.about.About;


/**
 * Presenta una ventana con un mensaje de error
 * si no se pudo leer el archivo donde se guardan
 * los valores del campo quality de la ventana
 * Start Sever
 * @Author Sara
 */

public class ErrorPanel {
	
	public ErrorPanel(String errorMessage, String file)
	{
		//Sacamos un mensaje en una ventanita...
	    JFrame parentFrame = new JFrame();
	    
	    JOptionPane.showMessageDialog(parentFrame, "Error reading file "+file+":"+"\n\n\n"+errorMessage, "Error", JOptionPane.INFORMATION_MESSAGE);
	    parentFrame.dispose();
	}
	
}
