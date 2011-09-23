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
 * TaskSockPing.java
 *
 */


package isabel.lib.tasksock;

import java.util.logging.Logger;

/**
 * Esta clase representa una tarea que envía mensajes periódicos al proceso
 * que se comunica mediante un TaskSockSerializer, con el fin de detectar si ha muerto.
 * Ademas permite que haya objetos escuchando para recibir avisos cuando muere
 * el proceso.
 * @author  Santiago Pavon
 * @author  Fernando Escribano
 * @version 1.0
 */
public class TaskSockPing extends Thread implements TaskSockRCBManager {

    /** El nombre asignado al programa para identificarlo. */
    private String name;

    /** Tiempo (en milisegundos) que espera hasta considerar que el programa que
     *  supervisa ha muerto.
     */
    private static final int TIMEOUT = 10000;

    /** Tiempo (en milisegundos) entre pings. 
     *  Tiene que ser mayor que timeout. 
     */
    private static final int PINGPERIOD = 15000;

    /** Mensaje que tiene que enviar (por ejemplo video_nop()). */
    private String pingMsg;


    /** Dice si el programa que se supervisa ha respondido a tiempo. */
    private boolean alive;
    
   /** Indica que el ping se ha detenido.
    *  Si se ha detenido (un quit intencionado) no avisara a los listeners.
    */
    private boolean stopped;

    /** TaskSock que nos creo.. */
    private TaskSock tasksock;

    /** Logger where logs are written. */
    private Logger mylogger;
    

    /** Crea un nuevo objeto TaskSockPing.
     * @param name Nombre asignado al proceso para su posterior identificación.
     * @param ser El objeto serializador que accede al programa.
     * @param msg El mensaje que hay que enviar como ping (video_nop(), audio_nop()).
     */
    public TaskSockPing (String name, TaskSock tasksock, String msg) {

	mylogger = Logger.getLogger("isabel.lib.tasksock.TaskSockPing."+name);
	this.name = name;
        this.tasksock = tasksock;
        alive = true;
        stopped = false;
        pingMsg = msg;
        setDaemon(true);
        start();
    }


    /** Este metodo se encarga de poner a true la variable alive
     * cuando el programa contesta a un ping.
     * @param answer La respuesta del programa
     */    
    public synchronized void done (String answer) {
	if (answer != null) {
	    alive = true;
	    mylogger.finest("PING: Pong!");
	} else { 
	    if (stopped) {
		mylogger.finest("PING: el thread fue parado (" + pingMsg + ").");
	    } else {
		mylogger.finest("PING: Error en el ping " + pingMsg);
	    }
	}
    }

    

    /** Método para detener el ping continuo y poder matar
     * el programa sin generar aventos de aviso.
     */
    public synchronized void quit() {
        stopped  = true;
    }


    /** Tarea encargada de lanzar los mensajes de ping al
     * proceso que queremos controlar para detectar si está vivo.
     */
    public void run() {

	while (true) {
	    mylogger.finest("PING: nueva vuelta del run.");
	    try {
		synchronized (this) {
		    alive = false;
		}
		
		// Enviamos ping al demonio. La respuesta pondra alive a true.
		mylogger.finest("PING: enviando "+pingMsg);
		tasksock.RCB(pingMsg, this); // ping!
		
		Thread.sleep(TIMEOUT);  // Tiempo de espera
		
		synchronized (this) {
		    if (stopped) {
			mylogger.finest("Ha sido parado el ping de "+name);
			break;
		    }
		    if (!alive) {
			mylogger.finest("No responde el demonio de "+name);
			break;
		    }
		}
		
		Thread.sleep(PINGPERIOD-TIMEOUT); // dormir hasta siguiente ping.
		
	    } 
	    catch (InterruptedException e) {
		mylogger.finest("PING: ping thread esperando por "+pingMsg+" interrumpida. "+e.getMessage());
		break;
	    }
	    catch (NoRespondingTaskSockException ddse) {
		mylogger.finest("PING: program is dead."+ddse.getMessage());
		break;
	    }
	}
	mylogger.finest("PING: fuera del run.");
	if (!stopped) {
	    tasksock.warnListeners();
	}
	tasksock = null;
    }
}
