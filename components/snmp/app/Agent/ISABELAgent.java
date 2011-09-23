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
//************************************************************************
//ISABELAgent.java:
//  Source code of the executable of the ISABEL component ISABELAgent.
//  This agent is able to recopilate information about the rest of the
//  components of an ISABEL application.
//
//     Throught its graphical interface it's also capable of showing information
//     about ISABEL traffic in the local mechine.
//
//  This program comunicates with three sockets:
//        a) One for the control protocol.
//        b) One for comunnicating with and SNMP Manager or a SNMP multiplexer
//           of agents
//        c) One for comunicating with the component of ISABEL
//
// The invocation command line of this program will be:
//    java ISABELAgent -SNMPPort x -ctrlPort x -shell -printStats
//

import java.io.*;

public class ISABELAgent {

    public static void usage() {
        System.out.println ("Usage: SNMPAgent [-SNMPPort] [-ctrlPort] [-dataPort] [-shell]");
        System.out.println ("    SNMPPort: puerto de comunicación con el gestor SNMP, por defecto 161");
        System.out.println ("    ctrlPort: puerto para control del componente ISABEL");
        System.out.println ("    dataPort: puerto al cual comunican sus datos el resto de módulos de Isabel");
        System.out.println ("    shell: para controlar desde la propia shell");
        System.exit(0);
    }

    public static void main (String[] argv) {
        //Default values:
        int SNMPPort         = 161;
        int ctrlPort         = 5000;
        int dataPort         = 51000;
        boolean shellControl = false;
        boolean visualization= false;
        boolean noexitbutton = false;

        //A.- Command line parser:
        //------------------------
            int i=-1;
            try {
                while (++i < argv.length) {
                    if (argv[i].equals("-h"))    {
                        // Asking for help on screen
                        usage();
                    } else if (argv[i].equals("-SNMPPort")) {
                        // Change the default SNMPPort
                        SNMPPort= Integer.valueOf(argv[++i]).intValue();
                    } else if (argv[i].equals("-ctrlPort")){
                        // Change the default CtrlPort
                        ctrlPort  = Integer.valueOf(argv[++i]).intValue();
                    } else if (argv[i].equals("-dataPort")){
                        // Change the default dataPort
                        dataPort  = Integer.valueOf(argv[++i]).intValue();
                    } else if (argv[i].equals("-shell")){
                        // Activate the shell control
                        shellControl=true;
                    } else if (argv[i].equals("-printStats")){
                        // Activate the GUI
                        visualization=true;
                    } else if (argv[i].equals("-noexitbutton")){
                        // Do not exit the application
                        noexitbutton=true;
                    } else {
                        // Unknown option
                        System.out.println("Unknown option \""
                                           + argv[i] + "\"");
                        usage();
                    }
                }
            } catch (Exception e){
                System.out.println(e);
                usage();
            }

        //Initializations:
        //----------------

            //1.-Initialization of the DataBase:
            DataBaseHandler_t DataBaseHandler = new DataBaseHandler_t ();

            //2.-Creation of threads Manager:
            threadsManager_t threadsManager =
                new threadsManager_t(DataBaseHandler, noexitbutton);

            //3.- Start of the InternalServer:
            try {
                internalCommsServer_t InternalServer =
                    new internalCommsServer_t (dataPort,
                                               threadsManager,
                                               DataBaseHandler
                                              );
                InternalServer.start();
                threadsManager.setInternalServer (InternalServer);
                System.out.println("InternalServer lanzado  [ OK ]");
            } catch (java.net.SocketException e) {
               System.err.println("NO SE PUDO CREAR EL SOCKET PARA " +
                                  "COMUNICACION INTERNA EN EL PUERTO: " +
                                  dataPort);
               System.out.println("Excepcion capturada:"+e);
               System.exit(0);
            }

            //4.- Start of the SNMPServer:
            try {
                SNMPServer_t SNMPServer =
                    new SNMPServer_t (SNMPPort,
                                      threadsManager,
                                      DataBaseHandler
                                     );
                SNMPServer.start();
                threadsManager.setSNMPServer (SNMPServer);
                System.out.println("SNMPServer lanzado    [ OK ]");
            }catch (java.net.SocketException e) {
                System.err.println("NO SE PUDO CREAR EL SOCKET PARA " +
                                   "SNMP EN EL PUERTO: " +
                                   SNMPPort);
                System.out.println("Excepcion capturada:"+e);
                System.exit(0);
            }

            //5.- Start of the ControlServer:
            ctrlServer_t ctrlServer =
                new ctrlServer_t(ctrlPort, shellControl, threadsManager);
             //if (!shellControl) ctrlServer.setDaemon(true);
            ctrlServer.start();
            threadsManager.setCtrlServer (ctrlServer);
            System.out.println("crtlServer lanzado    [ OK ]");

            //6.- Possible start of the GUI:
            if (visualization) {
                MonitorGUI_t MonitorGUI =
                    new MonitorGUI_t(DataBaseHandler, threadsManager);
                threadsManager.setMonitorGUI (MonitorGUI);
                System.out.println("Monitor GUI lanzada [ OK ]");
            }

    }

}

