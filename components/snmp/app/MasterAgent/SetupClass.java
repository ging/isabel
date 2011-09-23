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

//------------------------ SetupClass.java

// This program is free software; you can redistribute it and/or
// modify it under the terms of the Affero GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details (cf. file COPYING).


import java.util.Properties;
import java.util.Enumeration;
import java.util.Vector;
import java.io.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.JOptionPane;

import sck.*;

/** Prepares the Properties object that contains the ports to which each Var
*   received in an SNMP message by the Master Agent must be locally re-sent
*   based on the prefix of its object identifier. This Properties object is
*   stored on disk as a file called agentports.dat.
*
*   @author Oscar J. Martin Vega
*/

public class SetupClass {     

   /** Returns true if the file agentports.dat already exists.
    */
   static boolean existeArchivo() {
      File archivo = new File("agentports.dat");
      try {
      return archivo.canRead();
      } catch (SecurityException e2) {
         System.err.println("The security manager does not allow to acces the file agentports.dat which is needed to run the Master Agent");
         return false;
        }
   }

   /** Returns the Properties object (called slaveAgentsProperties)
    *  that holds the ports of the slave agents, using the object
    *  identifier prefix each slave agent can deal with as the key.
    */ 
   synchronized public static void loadSlaveAgentsProperties() throws IOException {
     try {       
       if (! existeArchivo() ) 
        create();         
       else {                
         FileInputStream fin = new FileInputStream("agentports.dat");
         MasterAgent.slaveAgentsPorts.load(fin);
         fin.close();  
         checkConfigurationFileContents();              
       }
     } catch (FileNotFoundException e1) {
                // There es a file system malfunction
                // Error de lectura del archivo agentports.dat
             System.err.println(e1.toString() );
             System.exit(0);                  
     } 
   }
  
  /** Checks for the correctnness of the configuration file
    * agentports.dat
    */
  synchronized private static void checkConfigurationFileContents() {    
    try {  
     
      for (Enumeration keys = MasterAgent.slaveAgentsPorts.keys();keys.hasMoreElements();) {
        String nextOne = (String)keys.nextElement();
        if (! nextOne.equals("default")) { 
      // constructs the Oid just to see if it is correctaccording to the SMI 
      // specifications; if it isn't, an exception will be launched.           
            Oid unaClave = new Oid(nextOne);            
        }
      }
    } catch(java.io.InvalidObjectException e) {
        System.out.println("\n\n\tFatal error: invalid Oid prefix in configuration file!!\n");
        System.out.println("This error has ocurred because the contents of the configuration file\nagentports.dat have been externally changed in a wrong way.\n");
        System.out.println("You should delete the file agentports.dat and run the master agent again; it\nwill ask you to introduce the configuration information again.\n\n");          
        System.exit(0);
    }
  }   
   
  /** It creates the first slaveAgentsPorts object.
   */
  synchronized private static void create() {
    JOptionPane.showMessageDialog(null,
                              "The MasterAgent Setup program has been launched because you must specify the parameters for the master agent\nthread to work. Now you must indicate  which Object Identifiers must go to which port, so SNMP messages containing\nvariables whose Object Identifier includes one of those prefixes will be locally re-sent to the desired local port where a\nslave agent will attend the message.",
                              "Master agent",
                              JOptionPane.INFORMATION_MESSAGE);                            
        
    ConfigurationFrame frame = new ConfigurationFrame();

    //Center the window
    Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
    Dimension frameSize = frame.getSize();
    frame.setLocation((screenSize.width - frameSize.width) / 2, (screenSize.height - frameSize.height) / 2);
    frame.setVisible(true);
  }

 
  /** It is invoked by any object of the class MasterAgentServerThread when a Var whose
   *  Object Identifier is not included in the current slaveAgentsPorts object,
   *  to add its Object Identifier prefix to it.
   */
  synchronized public static void setup() {    
    ConfigurationFrame frame = new ConfigurationFrame();

    //Center the window
    Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
    Dimension frameSize = frame.getSize();
    frame.setLocation((screenSize.width - frameSize.width) / 2, (screenSize.height - frameSize.height) / 2);
    frame.setVisible(true);     
  }  

} // SetupClass


