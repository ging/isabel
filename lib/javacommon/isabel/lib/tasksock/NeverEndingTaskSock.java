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


/**
 * Esta clase crea un TaskSock, y se queda vigilante para relanzarlo cuando se muere.
 * @author  Santiago Pavon
 * @version 1.0
 */
public class NeverEndingTaskSock extends Thread implements DeadTaskSockListener {
    
    /** El nombre asignado a la tarea para identificarla. */
    private String name;
    
    /** Comando necesario para lanzar el proceso desde la shell. */
    private String[] startShCmd;
    
    /** Control port. */
    private int port;

    /** Primitiva QUIT para matar la tarea. */
    private String quitCmd;

    /** Primitiva NOP para sondear la tarea. */
    private String nopCmd;

    /** The current TaskSock. */
    private TaskSock ds;


    /** 
     *  Construye un objeto NeverEndingTaskSock.
     *  Este constructor no tiene como parametro el puerto para escuchar las
     *  conexiones de control, luego usara el valor 0 para indicar que sea la tarea
     *  la que elija el puerto a utilizar.
     *  @param name Nombre asignado al proceso para su posterior identificación.
     *  @param startShCmd Comando y argumentod de shell para lanzar el programa.
     *  @param nopCmd  Primitiva NOP usado por el hilo de ping.
     *  @param quitCmd Primitiva QUIT para matar el demonio.
     */
    public NeverEndingTaskSock(String name, String[] startShCmd,
			       String nopCmd, String quitCmd) {
	this(name, startShCmd, 0, nopCmd, quitCmd);
    }
    
    
    /** 
     *  Construye un objeto NeverEndingTaskSock.
     *  Este constructor toma un parametro que es el puerto donde se aceptaran
     *  conexiones de control.
     *  @param name Nombre asignado al proceso para su posterior identificación.
     *  @param startShCmd Comando y argumentod de shell para lanzar el programa.
     *  @param port  Puerto donde escucha para conexiones de control.
     *  @param nopCmd  Primitiva NOP usado por el hilo de ping.
     *  @param quitCmd Primitiva QUIT para matar el demonio.
     */
    public NeverEndingTaskSock(String name, String[] startShCmd, int port,
		      String nopCmd, String quitCmd) {

	this.name = name;
	this.startShCmd = startShCmd; 
	this.port = port;
	this.nopCmd = nopCmd;
	this.quitCmd = quitCmd;

	start();
    }
    
    public void run() {
	synchronized (this) {
	    while (true) {
		try {	    
		    try {	    
			ds = new TaskSock(name, startShCmd, port, nopCmd, quitCmd);
			ds.addDeadTaskSockListener(this);
			wait();
		    }
		    catch (Exception e) {
			sleep(3000); // Si no lo puedo lanzar, lo reintento dentro de 3 segundos.
		    }	    
		}
		catch (InterruptedException e) {
		    break;
		}
	    }
	}
    }
    
    public void dead() {
	synchronized (this) {
	    notify();
	}	
    }			

}







