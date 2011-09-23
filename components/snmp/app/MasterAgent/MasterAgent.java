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

//-------------------- MasterAgent.java

// This program is free software; you can redistribute it and/or
// modify it under the terms of the Affero GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details (cf. file COPYING).


import java.net.*;
import java.io.*;
import java.util.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.JOptionPane;

import sck.*;

/** Implements a master agent that re-sends SNMP messages in the local computer but to the
 *  specific ports that slave agents are listening to.
 *
 *  @author Oscar J. Martin Vega
 *
 */

public class MasterAgent extends Thread {
    // Listens to the well known SNMP port = 161, although otherwise specified
    static int serverPort = 161;

/** slaveAgentsPorts is a properties object that indicates the local port (value)
 *  where variables with a determined object identifier prefix (key) must be
 *  addressed.
 */
    public static Properties slaveAgentsPorts = new Properties();

    static InetAddress local;
    DatagramSocket server;  
          

/** Main method of the class. It constructs the object of class
 *  MasterAgent and invokes its run method.
 */
    public static void main(String args[]){
    
      if (args.length!=0) {
        if (args[0].equals("-h")|args[0].equals("-help")) {
                        
          System.err.println("\n\nUsage:   java MasterAgent [portNumber]\n");
          System.err.println("portNumber : listening port for the master agent, default 161\n");
          System.err.println("  When invoked for the first time, the master agent setup window "+
                             "will appear. \nFollow the instructions. If you want to see "+
                             "the current configuration and/or \nmodify it, execute:\n\t\t\tjava Setup");
          System.exit(0);
        }
        else serverPort = Integer.valueOf(args[0]).intValue();      
      }
      
  // Get the local IP address and load from disk the slaveAgentsPorts object.
      try {           
            local = InetAddress.getLocalHost();
            SetupClass.loadSlaveAgentsProperties();
            
            new MasterAgent().start();  // Starts the master agent
          
      }catch (UnknownHostException e) {
         System.err.println("Your system has no IP address");
      } catch (IOException e) {
         System.err.println("slaveAgentsPorts loading impossible!" + e.toString() );
        }
      
    }

/** Constructor. */
   public MasterAgent() {
     super();    
   }            
 
    
   
/** Listens to any incoming SNMP message at port 161 and constructs and starts a
 *  MasterAgentServer thread to attend each one.
 */
    public void run() {
        try {
            server = new DatagramSocket(serverPort);
            do {
                /* b and packet are created here (inside do) to avoid race conditions between several MasterAgentServerThreads
                   although it is not memory efficient, because the cost, measured in system recourses, of the presence of a
                   monitor is not justified if you take into account how improbable is thr arrival of two different SNMP messages
                   at almost the same time     */
                byte[] b = new byte[1024];
                DatagramPacket packet = new DatagramPacket(b,b.length);
                server.receive(packet); // block

                     // When an SNMP message arrives a new thread is run to attend it

                new MasterAgentServerThread(b, packet.getLength(), packet.getAddress(), packet.getPort() ).start();
            } while(true);
        } catch(IOException ex) {
            System.exit(0);
        }
    }

    protected void finalize() {
      server.close();
    }
}

