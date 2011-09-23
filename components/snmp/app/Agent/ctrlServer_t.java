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
//*********************************************************************
//
//FILE: ctrlServer_t.java
//
//    Thread who is responsible of answering to the primitives that may
//    arrive to the control port. If -shell was a command line parameter
// at the execution of the monitor, it may also receive primitives from
// the console.
// The primitives are:
//
//        * snmp_nop(): to see if the program is alive. If it's alive, the
//        response will be OK.
//        * snmp_bye(): to finish the program.
//        * snmp_quit(): to finish the program.
//        * snmp_gui(0..1) : to launch or stop the monitor GUI.
//

import java.net.*;
import java.io.*;

public class ctrlServer_t extends java.lang.Thread {

    //Attributes:
    //-----------
    int                ctrlPort;
    boolean            shellControl;
    threadsManager_t   threadsManager;
    ServerSocket       ss;
    Socket             cs;
    DataInputStream    csis;
    PrintStream        csos;

    //Constructor:
    //------------
    ctrlServer_t (int ctrlPort,
                  boolean shellControl,
                  threadsManager_t threadsManager
                 ) {
        this.ctrlPort       = ctrlPort;
        this.shellControl   = shellControl;
        this.threadsManager = threadsManager;
    }

    //run method:
    //-----------
    public void run() {
        //If not shell control I must open a socket for communication:
        if (!shellControl) {

            try {
                ss = new ServerSocket(ctrlPort);
            }
            catch (Exception e) {
                System.out.println("Excepcion (0): "+e);
            }
        }

        ctrlProtocol_t ctrlProtocol =
            new ctrlProtocol_t(threadsManager);

        if (shellControl) {
            while (threadsManager.doIkeepAlive()) {
                try {
                    DataInputStream shell = new DataInputStream(System.in);
                    String command = shell.readLine();
                    String commandResponse = ctrlProtocol.analizeCommand(command);
                    System.out.println(commandResponse);
                }catch (Exception e) {
                    System.out.println("Excepcion en la lectura desde la shell: " +e );
                }
            }
        }

        else {//Start of port-control
            try {
                cs = ss.accept();
                ss.close();
                csis = new DataInputStream(cs.getInputStream());
                csos = new PrintStream(cs.getOutputStream());
            }
            catch (Exception e) {
                System.out.println("Excepcion (1): "+e);
            }
            while (threadsManager.doIkeepAlive()) {

                try {
                    String command = csis.readLine();
                    String commandResponse = ctrlProtocol.analizeCommand(command);
                    csos.println(commandResponse);
                }
                catch (Exception e) {
                    System.out.println("Excepcion (2): "+e);

		    threadsManager.killAll();
                }

            }
        }
    }

}

