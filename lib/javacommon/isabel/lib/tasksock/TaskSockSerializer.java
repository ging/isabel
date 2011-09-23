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
 * TaskSockSerializer.java
 *
 * Created on November 5, 2003.
 */

package isabel.lib.tasksock;

import isabel.lib.Queue;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.logging.Logger;

/**
 * Esta clase se encarga de gestionar la comunicación con el programa 
 * que escucha en el socket que se pasa como parámetro al constructor.
 * Utiliza para ello una cola en la que se guardan los mensajes a ser enviados, que
 * se van mandando según contesta el programa que escucha en el socket.
 * @author  Santiago Pavon
 * @author  Fernando Escribano
 * @version 2.0
 */
public class TaskSockSerializer extends Thread {

    /** the program name. */
    private String name;

    /** Flag que indica que el serializador ha muerto. */
    private boolean isDead;
    
    /** Cola donde se guardan los mensajes para ser enviados */
    private Queue queue;
    
    /** Socket por donde se envian los mensajes */
    private Socket socket;
    
    /** Writer por el que se envian los mensajes */
    private PrintWriter out;
    
    /** Reader por el que se reciben las respuestas */
    private BufferedReader in;

    /** Logger where logs are written. */
    private Logger mylogger;
    
    /** Estructura de datos que se encola. 
     *  Contiene la primitiva a enviar, la respuesta, un flag que indica que ya esta la
     *  respuesta, y el TaskSockRCBManager (para llamadas RCB).
     */
    private class Operation {
        String msg;   // el comando a enviar
        String res;   // la respuesta
        boolean done; // true cuando ya se tiene la respuesta
        TaskSockRCBManager manager;  // el procesador de las respuestas (RCB)
	
        /** Construye un objeto tipo Operation.
         * @param msg Comando que se ha de enviar.
         */
        Operation (String msg) {
            this(msg,null);
        }

        /** Construye un objeto tipo Operation.
         * @param msg Comando que se ha de enviar.
         * @param manager TaskSockRCBManager para la respuesta (null si no se necesita).
         */
        Operation (String msg, TaskSockRCBManager manager) {
            this.msg = msg;
            this.manager = manager;
        }
    }
    
    /** Crea un nuevo objeto tipo TaskSockSerializer.
     *  @param name Program name.
     *  @param s El socket por el que comunicarnos con el proceso.
     *  @throws IOException  En caso de que se produzca un error
     *  en la creación de los Readers/Writers de Entrada/Salida.
     */
    public TaskSockSerializer(String name, Socket s) throws IOException {

	mylogger = Logger.getLogger("isabel.lib.tasksock.TaskSockSerializer."+name);
        socket = s;
	this.name = name;
        queue = new Queue();

        mylogger.finest("creando OUT.");
        out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(socket.getOutputStream())), true);

        mylogger.finest("creando IN.");
        in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

        start();
    }

    
    /** Encola un mensaje para su posterior envío.
     * @param msg El mensaje que se envíará.
     */
    public void RDO(String msg) throws NoRespondingTaskSockException {
        synchronized(this) {
	    if (isDead) {
		throw new NoRespondingTaskSockException(name+" is dead.");
	    }
	    Operation operation = new Operation(msg);
	    queue.put(operation);
	    notify();
	}
    }

    /** Encola un mensaje para su posterior envío, espera a la respuesta.
     * @param msg El mensaje que se envíará por el socket.
     * @return La respuesta al mensaje.
     */
    public String RPC(String msg) throws NoRespondingTaskSockException {
	Operation operation;
        synchronized(this) {
	    if (isDead) {
		throw new NoRespondingTaskSockException(name+" is dead.");
	    }
	    operation = new Operation (msg);
	    queue.put(operation);
	    notify();
	}

	synchronized(operation) {
	    while (!operation.done) {
		try {
		    operation.wait(1000);
		    if (isDead) {
			throw new NoRespondingTaskSockException(name+" is dead.");
		    }
		}
		catch (InterruptedException e){
		    mylogger.finest("RPC: interrumpido.");
		    throw new NoRespondingTaskSockException(name+" was interrupted.");
		}
	    }
	}

	return operation.res;
    }

    /** Encola un mensaje para su posterior envío, y prepara una callback para procesar
     *  la respuesta.
     * @param msg El mensaje que se envíará por el socket.
     * @param manager El TaskSockRCBManager que procesara la respuesta.
     */
    public synchronized void RCB(String msg, TaskSockRCBManager manager) throws NoRespondingTaskSockException {
	if (isDead) {
	    manager.done(null);
	    throw new NoRespondingTaskSockException(name+" is dead.");
	}
        Operation operation = new Operation (msg, manager);
        queue.put(operation);
	notify();
    }
    
    
    /** Tarea encargada de ir vaciando la cola y tratar las respuestas adecuadamente
     */
    public void run() {
	while (true) {
	    // Si la cola esta vacia, esperamos.
	    synchronized(this) {
		while (queue.isEmpty()) {
		    try {
			wait();
		    }
		    catch (InterruptedException e){
			mylogger.finest("SERIALIZER: interrumpido.");
			isDead = true;
			cleanQueue();
			return;
		    }
		}
	    }
	    
	    try {
		// Enviamos por el socket la siguiente primitiva de la cola.
		Operation op = (Operation)queue.get();
		mylogger.finest("SERIALIZER: sending: " + op.msg);
		out.println(op.msg);
		op.res = in.readLine();
		mylogger.finest("SERIALIZER: received: " + op.res);
		op.done = true;
		synchronized(op) {
		    op.notify();
		}
		// Si el método es RCB lanzamos una hebra que llame al RCBManager.
		if (op.manager != null) {
		    new Done(op.manager, op.res);
		}
		if (op.res == null) {
		    throw new IOException();
		}
	    }
	    catch (IOException e) {
		mylogger.finest("SERIALIZER: IOE");
		synchronized(this) {
		    isDead = true;
		    cleanQueue();
		}
		return;
	    }
	}
    }

    /** Limpia de la cola todas las operaciones pendientes.
     */ 
    private synchronized void cleanQueue() {
	mylogger.finest("SERIALIZER: Limpiando cola de mensajes.");
	while (!queue.isEmpty()) {
	    Operation op = (Operation)queue.get();
	    mylogger.finest("SERIALIZER: Eliminado operacion "+op.msg);
	    synchronized(op) {
		op.notify();
	    }
	    if (op.manager != null) {
		new Done(op.manager, null);
	    }
	}	
    }

    /** Esta clase se encarga de llamar a los RCBManager de forma que la
     * hebra que controla la cola pueda seguir a lo suyo.
     */
    private class Done extends Thread {
        
        private TaskSockRCBManager man;
        private String ans;
	
        /** Crea una hebra de proceso de un RCB.
         * @param man TaskSockRCBManager que procesara la respuesta.
         * @param ans String con la respuesta.
         */
        Done(TaskSockRCBManager man, String ans) {
            this.man = man;
            this.ans = ans;
            start();
        }
        /** Simplemente llama al metodo done del TaskSockRCBManager */
        public void run() {
            man.done(ans);
        }
    }
}


