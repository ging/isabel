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
 * SeCo.java
 */
package isabel.seco.server;

import isabel.lib.Queue;
import isabel.lib.Logs;

import isabel.lib.reliableudp.RUDPServerSocket;
import isabel.lib.reliableudp.RUDPSocket;
import java.net.*;
import java.io.*;
import java.util.*;
import java.util.logging.*;

/**
 * Esta clase representa a los nodos SeCo intermedios y raiz, dependiendo de los
 * argumentos introducidos por la linea de comandos.
 * 
 * Para arrancar esta clase:
 * 
 * SeCo [opc val] [hostname [port]]
 * 
 * Opciones:
 * 
 * -ctrlport: Puerto del socket (tcp) de control textual. -dataport: Puerto del
 * socket servidor al que se conectaran otros nodos.
 * 
 * Donde todos los campos son opcionales.
 * 
 * Los clientes se pueden unir a cualquier nodo SeCo mediante un socket TCP para
 * comenzar la sesion. Los mensajes intercambiados por estos socket seran
 * objetos de la clase SeCoPacket.
 * 
 * @author Santiago Pavon
 * @author Javier Calahorra
 * @author Jose Carlos del Valle
 */
class SeCo {

    /**
     * Used to debug. When true set the send and receive buffers to the minimum size.
     *
     * This constant must be always false in production. True is only used for testing.
     */
    private static final boolean DEBUG_USE_SMALL_BUFFERS = false;

    private static class TCPAcceptor extends Thread {

        Logger mylogger;
        int serverPort;
        final isabel.seco.server.State state;
        boolean root;
        Queue queue;
        HashMap list;
        Permissions permObj;
        IdGenerator idGenObj;

        public TCPAcceptor(Logger logger, int port, final isabel.seco.server.State st, Permissions permObj, IdGenerator idGenObj) {
            mylogger = logger;
            serverPort = port;
            state = st;
            this.root = true;
            this.permObj = permObj;
            this.idGenObj = idGenObj;
        }

        public TCPAcceptor(Logger logger, int port, final isabel.seco.server.State st, Queue queue, HashMap list, IdGenerator idGenObj) {
            mylogger = logger;
            serverPort = port;
            state = st;
            this.root = false;
            this.queue = queue;
            this.list = list;
            this.idGenObj = idGenObj;
        }

        @Override
        public void run() {
            try {
                mylogger.finest("Creating TCP server socket at port " + serverPort);
                ServerSocket ss = new ServerSocket(serverPort);

                while (true) {
                    Socket s = ss.accept();
                    if (DEBUG_USE_SMALL_BUFFERS) {
                        s.setReceiveBufferSize(1);
                        s.setSendBufferSize(1);
                    }
                    mylogger.finer("Creating thread DownDataLink to manage the new connection.");
                    if (root) {
                        new DownDataLink(s, state, idGenObj, permObj).start();
                    } else {
                        new DownDataLink(s, state, idGenObj, queue, list).start();
                    }
                }
            } catch (IOException ex) {
                mylogger.log(Level.SEVERE, null, ex);
            }
        }
    }

    private static class RUDPAcceptor extends Thread {

        Logger mylogger;
        int serverPort;
        final isabel.seco.server.State state;
        boolean root;
        Queue queue;
        HashMap list;
        Permissions permObj;
        IdGenerator idGenObj;

        public RUDPAcceptor(Logger logger, int port, final isabel.seco.server.State st, Permissions permObj, IdGenerator idGenObj) {
            mylogger = logger;
            serverPort = port;
            state = st;
            this.root = true;
            this.permObj = permObj;
            this.idGenObj = idGenObj;
        }

        public RUDPAcceptor(Logger logger, int port, final isabel.seco.server.State st, Queue queue, HashMap list, IdGenerator idGenObj) {
            mylogger = logger;
            serverPort = port;
            state = st;
            this.root = false;
            this.queue = queue;
            this.list = list;
            this.idGenObj = idGenObj;
        }

        @Override
        public void run() {
            try {
                mylogger.finest("Creating RUDP server socket at port " + serverPort);
                RUDPServerSocket ss = new RUDPServerSocket(serverPort);

                while (true) {
                    RUDPSocket s = ss.accept();
                    mylogger.finer("Creating thread DownDataLink to manage the new connection.");
                    if (root) {
                        new DownDataLink(s, state, idGenObj, permObj).start();
                    } else {
                        new DownDataLink(s, state, idGenObj, queue, list).start();
                    }
                }
            } catch (Exception ex) {
                mylogger.log(Level.SEVERE, null, ex);
            }
        }
    }

    public static void main(String[] args) {

        // Inicializacion de los logs.
        Logs.configure("secoserver");
        final Logger mylogger = Logger.getLogger("isabel.seco.server.SeCo");

        // Indica si este nodo es la raiz del arbol de nodos SeCo.
        boolean isRoot = true;

        // Nodo SeCo al que nos conectamos cuando no somos el nodo raiz.
        String remoteHost = null;

        // Puerto servidor del nodo al que nos conectamos cuando no somo el nodo
        // raiz.
        int remotePort = 53023;

        // Puerto del socket servidor al que se conectaran otros nodos.
        int serverPort = 53023;

        // Puerto servidor del nodo al que nos conectamos cuando no somo el nodo
        // raiz.
        int rudpremotePort = 53000;

        // Puerto del socket servidor al que se conectaran otros nodos.
        int rudpserverPort = 53000;

        // Puerto del socket (tcp) de control textual.
        int controlPort = 51011;

        int argAt = 0;
        while (argAt < args.length) {
            String argumento = args[argAt];
            if (argumento.charAt(0) == '-' && isRoot) {
                if (argumento.equals("-dataport")) {
                    argAt++;
                    if (argAt >= args.length) {
                        mylogger.severe("Missing data port value.");
                        System.exit(1);
                    }
                    try {
                        serverPort = Integer.parseInt(args[argAt]);
                        argAt++;
                    } catch (NumberFormatException e) {
                        mylogger.severe("Wrong data port format: " + args[argAt]);
                        System.exit(1);
                    }
                } else if (argumento.equals("-ctrlport")) {
                    argAt++;
                    if (argAt >= args.length) {
                        mylogger.severe("Missing control port value.");
                        System.exit(1);
                    }
                    try {
                        controlPort = Integer.parseInt(args[argAt]);
                        argAt++;
                    } catch (NumberFormatException e) {
                        mylogger.severe("Wrong control port format: " + args[argAt]);
                        System.exit(1);
                    }
                } else if (argumento.equals("-udpport")) {
                    argAt++;
                    if (argAt >= args.length) {
                        mylogger.severe("Missing udp port value.");
                        System.exit(1);
                    }
                    try {
                        rudpserverPort = Integer.parseInt(args[argAt]);
                        argAt++;
                    } catch (NumberFormatException e) {
                        mylogger.severe("Wrong udp port format: " + args[argAt]);
                        System.exit(1);
                    }
                } else if (argumento.equals("-udpremoteport")) {
                    argAt++;
                    if (argAt >= args.length) {
                        mylogger.severe("Missing udp port value.");
                        System.exit(1);
                    }
                    try {
                        rudpremotePort = Integer.parseInt(args[argAt]);
                        argAt++;
                    } catch (NumberFormatException e) {
                        mylogger.severe("Wrong remote udp port format: " + args[argAt]);
                        System.exit(1);
                    }

                } else {
                    mylogger.warning("Unknown option: " + args[argAt]);
                    System.exit(1);
                }
            } else {
                if (isRoot) {
                    isRoot = false;
                    remoteHost = args[argAt];
                    argAt++;
                } else {
                    try {
                        remotePort = Integer.parseInt(args[argAt]);
                        argAt++;
                    } catch (NumberFormatException e) {
                        mylogger.severe("Wrong remote data port format: " + args[argAt]);
                        System.exit(1);
                    }
                    break;
                }
            }
        }

        if (isRoot) {
            mylogger.info("Launching root SeCo server at tcp port: " + serverPort +
                    "; udp port: " + rudpserverPort +
                    "; Control Port: " + controlPort);
        } else {
            mylogger.info("Launching SeCo server at tcp port: " + serverPort +
                    "; udp port: " + rudpserverPort +
                    "; Control Port: " + controlPort +
                    "; Remote host name: " + remoteHost +
                    "; Remote tcp port: " + remotePort +
                    "; Remote udp port: " + rudpremotePort);
        }

        // objeto que almacena el estado del nodo.
        final State state = new State();

        // Registers a shutdown hook to:
        // - close the connections with the DownDataLink sites.
        // - close the connection with the UpDataLink site.
        Runtime.getRuntime().addShutdownHook(new Thread() {

            public void run() {

                mylogger.info("SeCo ShutdownHook: closing connection to UpDataLink ...");
                PacketProcessor up = state.getClientPP(0);
                if (up != null) {
                    try {
                        up.close();
                    } catch (IOException ioe) {
                        mylogger.severe("IOException = " + ioe.getMessage());
                    }
                }
                mylogger.info("SeCo ShutdownHook: closed connection to UpDataLink.");

                mylogger.info("SeCo ShutdownHook: closing connections to DownDataLinks ...");
                Iterator<PacketProcessor> it = state.getAllLocalSeCoPPs();
                while (it.hasNext()) {
                    try {
                        it.next().close();
                    } catch (IOException ioe) {
                        mylogger.severe("IOException = " + ioe.getMessage());
                    }
                }
                mylogger.info("SeCo ShutdownHook: closed connections to DownDataLinks.");
            }
        });

        try {

            if (isRoot) {

                mylogger.finer("Creating the thread to manage the control connections.");
                Control ccc = new Control(controlPort);
                ccc.start();

                // Objeto que realiza la comprobacion de permisos en el nodo
                // raiz.
                Permissions permObj = new Permissions();

                // Generador de identificadores unicos para clientes y grupos
                IdGenerator idGenObj = new IdGenerator();


                new TCPAcceptor(mylogger, serverPort, state, permObj, idGenObj).start();
                new RUDPAcceptor(mylogger, rudpserverPort, state, permObj, idGenObj).start();

            } else { // no es el nodo raiz

                // Conectamos con el socket servidor de nuestro padre
                RUDPSocket rudpremote = null;

                Socket remote = null;

                int maxTries = 12;  //6
                for (int tries = 0;; tries++) {
                    if (tries > maxTries) {
                        mylogger.severe("I can't connect with parent SeCo node. Exiting now.");
                        System.exit(1);
                    }

                    try {
                        mylogger.info("Connecting to " + remoteHost + ":" + rudpremotePort + " using RUDP");
                        rudpremote = new RUDPSocket(InetAddress.getByName(remoteHost), rudpremotePort);
                        break;
                    } catch (Exception e) {
                        rudpremote = null;
                        mylogger.warning("Unable to connect with " + remoteHost + " using RUDP:" + e);
                    }

                    /*
		      try {
		      remote = new Socket(remoteHost, remotePort);
		      break;
		      } catch (Exception e) {
		      try {
		      mylogger.warning("Unable to connect with " + remoteHost + " using TCP:" + e);
		      Thread.sleep(1000);
		      } catch (InterruptedException ie) {
		      }
		      }
		    */
                }
                // Cola FIFO para DownDataLink pendientes de identificador
                Queue queue = new Queue();

                // Almacena DownDataLink pendientes de permisos
                HashMap list = new HashMap();

                // Arrancamos UpDataLink a nodo padre.
                mylogger.finer("Creating UpDataLink to host " + remoteHost + " and port " + remotePort);

                if (rudpremote != null) {
                    new UpDataLink(rudpremote, queue, state, list).start();
                } else if (remote != null) {
                    new UpDataLink(remote, queue, state, list).start();
                }

                mylogger.finer("Creating the thread to manage the control connections.");
                Control ccc = new Control(controlPort);
                ccc.start();

                // Generador de identificadores unicos para clientes y grupos
                IdGenerator idGenObj = new IdGenerator();

                new TCPAcceptor(mylogger, serverPort, state, queue, list, idGenObj).start();
                new RUDPAcceptor(mylogger, rudpserverPort, state, queue, list, idGenObj).start();
            }
        } catch (IOException e) {
            mylogger.severe("SeCo Fatal error: " + e.getMessage());
            e.printStackTrace(System.err);
            System.exit(1);
        }
    }
}

