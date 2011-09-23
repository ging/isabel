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

package isabel.lib.tasksock;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.Socket;
import java.util.ArrayList;
import java.util.logging.Logger;

/**
 * Esta clase se encarga de lanzar un programa ejecutable, que se controla 
 * mediante un socket.
 * Por el socket se envian comandos de control y se reciben sus respuestas.
 * Uno de estos comandos de control es tipo NOP, para permitir saber si 
 * el programa sigue vivo. Si el programa se muere o bloquea se avisa a los objetos
 * DeadTaskSockListener que esten registrados.
 * @author  Santiago Pavon, Fernando Escribano
 * @version 2.0
 */
public class TaskSock {
    
    /** El nombre asignado al programa para identificarlo. */
    private String name;
    
    /** Comando y parametros necesarios para lanzar el proceso desde la shell. */
    private String[] startShCmd;
    
    /** Primitiva QUIT para matar el programa. */
    private String quitCmd;
    
    /** El objeto que controla que el programa sigue vivo. */
    private TaskSockPing ping;
    
    /** El serializer para comunicarse con el programa que queremos controlar. */
    private TaskSockSerializer serializer;

    /** Puerto donde se escuchara para atender las solicitudes de conexiones 
     *  de control.
     *  Un valor 0 indicara que sera la tarea la que elegira el numero de puerto
     *  a utilizar, valor que escribira inicialmente en su salida estandar.
     */
    private int port;
 
    /** Logger where logs are written. */
    private Logger mylogger;

    /** ShutdownHook to destroy the program on exit */    
    private Thread shutdownHook;

    /** El proceso que ejecuta el demonio.*/
    Process process;

    /** Lista de DeadTaskSockListeners.
     *  Se les avisa cuando el demonio bajo control no responde.
     */
    private ArrayList<DeadTaskSockListener> listeners;


    /** 
     *  Construye un objeto TaskSock.
     *  Este constructor no toma como parametros ni el host, ni el puerto donde conectarse con la tarea a controlar.
     *  Usara los valores 127.0.0.1 y 0.
     *
     *  El valor de puerto 0 indica que sera la tarea la que elegira el puerto a usar.
     *  La tarea escribira en primer lugar por su salida estandar el valor de
     *  puerto elegido.
     *
     *  @param name Nombre asignado al proceso para su posterior identificación.
     *  @param startShCmd Array con el comando y los parametros del programa a ejecutar.
     *  @param nopCmd  Primitiva NOP usado por el hilo de ping.
     *  @param quitCmd Primitiva QUIT para matar el demonio.
     *  @throws <tt>Exception</tt> Si se produce algun error al lanzar el proceso.
     */
    public TaskSock(String name, String[] startShCmd,
		    String nopCmd, String quitCmd) throws Exception {
	
	this(name, startShCmd, 0, nopCmd, quitCmd);
    }



    /** 
     *  Construye un objeto TaskSock.
     *  Este constructor toma como parametro solo el puerto para conectarse con la tarea 
     *a controlar, y como host usa el valor 127.0.0.1.
     *  @param name Nombre asignado al proceso para su posterior identificación.
     *  @param startShCmd Array con el comando y los parametros del programa a ejecutar.
     *  @param port  Puerto donde escucha para conexiones de control.
     *  @param nopCmd  Primitiva NOP usado por el hilo de ping.
     *  @param quitCmd Primitiva QUIT para matar el demonio.
     *  @throws <tt>Exception</tt> Si se produce algun error al lanzar el proceso.
     */
    public TaskSock(String name, String[] startShCmd, int port,
		    String nopCmd, String quitCmd) throws Exception {
	this(name, startShCmd, InetAddress.getByName("127.0.0.1"), port, nopCmd, quitCmd);
    }
    

    /** 
     *  Construye un objeto TaskSock.
     *  Este constructor toma como parametros el host y el puerto de conexion con la tarea a 
     *  controlar.
     *  @param name Nombre asignado al proceso para su posterior identificación.
     *  @param startShCmd Array con el comando y los parametros del programa a ejecutar.
     *  @param port  Puerto donde escucha para conexiones de control.
     *  @param nopCmd  Primitiva NOP usado por el hilo de ping.
     *  @param quitCmd Primitiva QUIT para matar el demonio.
     *  @throws <tt>Exception</tt> Si se produce algun error al lanzar el proceso.
     */
	public TaskSock(String name, String[] startShCmd, InetAddress addr, int port,
		      String nopCmd, String quitCmd) throws Exception {
	mylogger = Logger.getLogger("isabel.lib.tasksock.TaskSock."+name);
	mylogger.fine("Creando TaskSock para "+name);
        this.name = name;
	this.startShCmd = startShCmd;
	this.quitCmd    = quitCmd;
	this.port = port;

        listeners = new ArrayList<DeadTaskSockListener>();

	startTask();
	for (int tries = 0 ; ; tries ++) {
	    try {	
		mylogger.fine("Conectando el socket de control (port="+this.port+") del TaskSock de "+name);
		Socket sock = new Socket(addr, this.port);
		mylogger.fine("Creando el serializador de control del TaskSock de "+name);
		serializer = new TaskSockSerializer(name,sock);
		mylogger.fine("Creando el Ping del control del TaskSock de "+name);
		ping = new TaskSockPing(name,this,nopCmd);
		break;
	    }
	    catch (Exception e) {
		if (tries > 10) {
		    // Si hay problemas al conectarme al programa, mato el programa.
		    mylogger.warning("No puedo conectar al TaskSock "+name+". Anulando creacion.");
		    process.destroy();
		    throw e;
		} else {
		    Thread.sleep(1000);
		}
	    }
	}
	
	// Register a shutdown hook to destroy the process.
	final String dname =name;
	shutdownHook = new Thread() {
		public void run() {
		    mylogger.info("TaskSock "+dname+": Executing ShutdownHook ...");
		    //process.destroy();
		    try {
			quit();
		    } catch (Exception e) {
			mylogger.info("TaskSock "+dname+": Exception thrown while executing ShutdownHook.");
		    }
		    mylogger.info("TaskSock "+dname+": Executed ShutdownHook.");
		}
	    };
	Runtime.getRuntime().addShutdownHook(shutdownHook);
    }


    /** Lanza el programa.
     *  @throws <tt>Exception</tt> Si se produce algun error al lanzar el programa.
     */
    private void startTask() throws Exception {
        // Lanzamos el programa
	mylogger.info("Lanzando el TaskSock de "+name+": " + startShCmd[0]);
	Runtime rt = Runtime.getRuntime();
	process = rt.exec(startShCmd);

	if (port == 0) {
	    // Leemos el puerto al que conectarnos
	    String thePort = new BufferedReader(new InputStreamReader(process.getInputStream())).readLine();
	    mylogger.fine(name+" returns = " + thePort);
	    
	    if (thePort == null) {
		String emsg = "I can not execute "+name+" daemon: it exits before printing its control port.";
		mylogger.severe(emsg);
		throw new Exception(emsg);
	    }
	

	    try {
		port = Integer.parseInt(thePort);
		mylogger.fine(name+" daemon control port: " + port);
	    }
	    catch (NumberFormatException nfe) {
		String errMsg = name+" daemon returns a bad control port number: " + thePort;
		mylogger.warning(errMsg);
		process.destroy();
		throw new Exception(errMsg);
	    }
	}

	// Imprime los mensajes enviados a la salida estandar. 
	Thread cleanStdIn = new Thread() {
		
		public void run() {
		    BufferedReader br;
		    br = new BufferedReader(new InputStreamReader(process.getInputStream()));
		    try {
			String msg;
			while ((msg = br.readLine()) != null) {
			    System.out.println(name+": "+msg);
			}
		    }
		    catch (Exception e) {
		    }
		    try {
			br.close();
		    }
		    catch (Exception e) {
		    }
		}
		
	    };
	cleanStdIn.start();

	// Elimina cualquier mensaje enviado a la salida de error.
	Thread cleanStdErr = new Thread() {
		
		public void run() {
		    BufferedReader br;
		    br = new BufferedReader(new InputStreamReader(process.getErrorStream()));
		    try {
			String msg;
			while ((msg = br.readLine()) != null) {
			    System.err.println(name+": "+msg);
			}
		    }
		    catch (Exception e) {
		    }
		    try {
			br.close();
		    }
		    catch (Exception e) {
		    }
		}
		
	    };
	cleanStdErr.start();
    }

    public void finalize() {
	mylogger.fine("Finalizando el objeto TaskSock del "+name);
	quit();
	// Deregister the shutdown hook to destroy the process.
	try {
	    Runtime.getRuntime().removeShutdownHook(shutdownHook);
	}
	catch (Exception e) {
	}
    }


    /** Método para mater el programa, enviandole la primitiva quitCmd por el 
     *  socket de control.
     *  Ademas se detiene el thread PING que periodicamente comprueba si el 
     *  programa sigue activo.
     *  Tras invocar este metodo, los metodos RDO, RPC y RCB solo retornaran valores null.
     */
    public void quit() {
	mylogger.fine("Realizando quit() en el TaskSock del "+name);

	if (ping != null) {
	    ping.quit();
	    ping = null;
	}
	
	try {
	    RDO(quitCmd);
	}
	catch (NoRespondingTaskSockException nrdse) {
	}
	try {
	    process.exitValue();
	}
	catch (IllegalThreadStateException itse) {
	    Thread killer = new Thread() {
		    public void run() {
			try {
			    Thread.sleep(3000);
			}
			catch (InterruptedException ie) {
			}
			process.destroy();    
		    }
		};
	    killer.start();
	    try {
		process.waitFor();
	    }
	    catch (InterruptedException ie) {
	    }
	}
    }



    /** Envia un mensaje al programa e ignora la respuesta.
     *  @param msg El mensaje que se envíará.
     *  @throws NoRespondingTaskSockException When the program doesn't answer.
     */
    public void RDO(String msg) throws NoRespondingTaskSockException {
	serializer.RDO(msg);
    }

    
    /** Envia un mensaje al programa y devuelve su respuesta.
     *  @param msg El mensaje que se envíará por el socket.
     *  @return La respuesta al mensaje. 
     *  @throws NoRespondingTaskSockException When the program doesn't answer.
     */
    public String RPC(String msg) throws NoRespondingTaskSockException {
	return serializer.RPC(msg);
    }
    
    
    /** Envia un mensaje al programa, e indica que objeto TaskSockRCBManager procesara 
     *  la respuesta.
     *  @param msg El mensaje que se envíará por el socket.
     *  @param manager El TaskSockRCBManager que procesara la respuesta.
     *  @throws NoRespondingTaskSockException When the program doesn't answer.
     */
    public void RCB(String msg, TaskSockRCBManager manager) throws NoRespondingTaskSockException {
	serializer.RCB(msg,manager);
    }
   

    /** Apunta el listener especificado como parámetro para recibir un aviso cuando
     *  el programa muere abruptamente.
     *  @param lis El listener que queremos que reciba los eventos.
     */
    public synchronized void addDeadTaskSockListener(DeadTaskSockListener lis) {
        if (lis == null)
            return;
        listeners.add(lis);
    }

    /** Elimina el listener especificado como parámetro para que deje de recibir eventos.
     * @param lis El listener que queremos que deje de recibir los eventos.
     */
    public synchronized void removeDeadTaskSockListener(DeadTaskSockListener lis) {
       if (lis == null)
           return;
       listeners.remove(lis);
    }


    /** Método para avisar a todos los listeners de la muerte del proceso externo.
     */
    void warnListeners() {

	quit();

        ArrayList<DeadTaskSockListener> list;
        synchronized(this) {
            list = (ArrayList<DeadTaskSockListener>)listeners.clone();
        }
        
        for (DeadTaskSockListener l : list) {
            l.dead();
        }
    }
 
}
