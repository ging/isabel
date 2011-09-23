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

//------------------------ Setup.java

// This program is free software; you can redistribute it and/or
// modify it under the terms of the Affero GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details (cf. file COPYING).


import java.io.*;

/** Shows the setup window for configuration purposes.
 *  @author Oscar J. Martin Vega
 */

public class Setup {
  public static void main(String[] args) {
    if (args.length==0) {
      if (!SetupClass.existeArchivo())         
        System.err.println("\nThere is no configuration information yet. Run the master agent program to\ninitialize it, by entering \"java MasterAgent [portNumber]\"");                                       
      else {  
        
        // Load from disk the slaveAgentsPorts object.
        try {          
          SetupClass.loadSlaveAgentsProperties();                                  
        } catch (IOException e) {
           System.err.println("slaveAgentsPorts loading impossible!" + e.toString() );
          }
      
        SetupClass.setup();        
      }  
    }  
    else 
      if (args[0].equals("-h")|args[0].equals("-help")) {
        System.err.println("\nUsage:    java Setup\n");
        System.err.println("  Shows the setup window for configuration purposes.");
      }   
      
  }

}