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
 * Task.java
 *
 */

package isabel.lib.tasks;

import isabel.lib.*;

import java.io.*;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.logging.*;
import java.util.Map;
import java.util.HashMap;


/**
 * Esta clase se encarga de lanzar programas externos.
 * Permite que en caso de muerte del programa este sea relanzado
 * automáticamente.
 * Ademas comunica la muerte del proceso mediante eventos a los objetos
 * registrados como Listeners.
 * Para crear una tarea mediante esta clase se crea el objeto, se le añaden los
 * listeners y se inicia mediante el método start().
 */
public class Task extends Thread {
    
    /** 
     *  El proceso asociado al programa que ejecutamos.
     */
    private Process process;
    

    /** 
     *  Nombre para diferenciar esta tarea de otras.
     */
    private String taskname;
    

    /** 
     *  Creador de los process a ejecutar.
     */
    private ProcessBuilder builder;
    

    /** 
     *  Indica si debemos reiniciar el programa cada vez que se muere.
     */
    private boolean keepAlive;
    

    /** 
     *  Listeners interesados en saber cuando se ha muerto el programa.
     */
    private ArrayList<TaskListener> listeners;


    /**
     *  Flag usado para indicar que se llamo al metodo kill para matar el programa,
     *  y por tanto no hay que avisar a los listeners.
     */
    private boolean killed;


    /** 
     *  Thread encargada de matar el programa cuando se muere la maquina virtual java.
     */
    private Thread shutdownHook;


    /** 
     *  Logger where logs are written. 
     */
    private Logger mylogger;

    
    /**
     *  PrintStream where the normal output of the process is sent.
     */
    private PrintStream pout;

    /**
     *  PrintStream where the error output of the process is sent.
     */
    private PrintStream perr;


    /**
     *  Counter used to know if the threads which read the standard and error output
     *  of the process hasvefinished.
     *  Before launching the process this values is set to 2, and the threads that read
     *  these outputs, decrement it value.
     *  When this value is 0, the run method can end.
     */
    private int cleanOutErrCont;


    /**
     * Timeout (in miliseconds) to wait for the process to close its output and error streams.
     * When this timeout expires, the streams are interrupted and closed.
     * Default value is 10000.
     * Timeout 0 means wait forever.
     */
    private int closingTimeout = 10000;

    /**
     *  Thread used to read the standard output of the executing process.
     *  The read lines are sent to pout.
     */
    private Thread cleanStdIn;

    /**
     *  Thread used to read the error output of the executing process.
     *  The read lines are sent to perr.
     */
    private Thread cleanStdErr;


    /**
     *  Indica si debe o no debe hacerse un process.destroy() cuando se invoca kill().
     */
    private boolean destroyOnKill = true;


    //----------------------

    /** 
     *  Crea un nuevo objeto Task.
     *  Al crear el objeto se arranca la ejecucion del programa, y en caso de que no se pueda
     *  se lanza una IOException.
     *  La salida normal y de error del programa se redirige a System.out y System.err.
     *  @param cmd  comando necesario para lanzar el programa desde la shell.
     *  @param keep boolean que indica si queremos relanzarlo en caso de muerte o no.
     *  @throws <tt>IOException</tt> Si se produce algun error al lanzar el proceso.
     *  @throws <tt>Exception</tt> Si el coamndo esta mal formado.
     */
    public Task(String cmd, boolean keep) throws Exception, IOException {

	this("noname",cmd,keep,System.out,System.err);
    }

    /** 
     *  Crea un nuevo objeto Task.
     *  Al crear el objeto se arranca la ejecucion del programa, y en caso de que no se pueda
     *  se lanza una IOException.
     *  La salida normal y de error del programa se redirige a out y a err.
     *  @param cmd  comando necesario para lanzar el programa desde la shell.
     *  @param keep boolean que indica si queremos relanzarlo en caso de muerte o no.
     *  @param out PrintStream al que se envia la salida normal del programa. Si este
     *             parametro es null, se tira a la basura la salida.
     *  @param err PrintStream al que se envia la salida normal del programa. Si este
     *             parametro es null, se tira a la basura la salida.
     *  @throws <tt>IOException</tt> Si se produce algun error al lanzar el proceso.
     *  @throws <tt>Exception</tt> Si el coamndo esta mal formado.
     */
    public Task(String cmd, boolean keep, PrintStream out, PrintStream err) 
	throws Exception, IOException {

	this("noname",cmd,keep,out,err);
    }

    //----------------------

    /** 
     *  Crea un nuevo objeto Task.
     *  Al crear el objeto se arranca la ejecucion del programa, y en caso de que no se pueda
     *  se lanza una IOException.
     *  La salida normal y de error del programa se redirige a System.out y System.err.
     *  @param cmd  comando necesario para lanzar el programa desde la shell.
     *  @param keep boolean que indica si queremos relanzarlo en caso de muerte o no.
     *  @throws <tt>IOException</tt> Si se produce algun error al lanzar el proceso.
     */
    public Task(String[] cmd, boolean keep) throws IOException {

	this("noname",cmd,keep,System.out,System.err);
    }


    /** 
     *  Crea un nuevo objeto Task.
     *  Al crear el objeto se arranca la ejecucion del programa, y en caso de que no se pueda
     *  se lanza una IOException.
     *  La salida normal y de error del programa se redirige a out y a err.
     *  @param cmd  comando necesario para lanzar el programa desde la shell.
     *  @param keep boolean que indica si queremos relanzarlo en caso de muerte o no.
     *  @param out PrintStream al que se envia la salida normal del programa. Si este
     *             parametro es null, se tira a la basura la salida.
     *  @param err PrintStream al que se envia la salida normal del programa. Si este
     *             parametro es null, se tira a la basura la salida.
     *  @throws <tt>IOException</tt> Si se produce algun error al lanzar el proceso.
     */
    public Task(String[] cmd, boolean keep, PrintStream out, PrintStream err) 
	throws IOException {

	this("noname",cmd,keep,out,err);
    }

    //----------------------

    /** 
     *  Crea un nuevo objeto Task.
     *  Al crear el objeto se arranca la ejecucion del programa, y en caso de que no se pueda
     *  se lanza una IOException.
     *  La salida normal y de error del programa se redirige a System.out y System.err.
     *  @param name Identificador para el task creado.
     *  @param cmd  comando necesario para lanzar el programa desde la shell.
     *  @param keep boolean que indica si queremos relanzarlo en caso de muerte o no.
     *  @throws <tt>IOException</tt> Si se produce algun error al lanzar el proceso.
     *  @throws <tt>Exception</tt> Si el coamndo esta mal formado.
     */
    public Task(String name, String cmd, boolean keep) throws Exception, IOException {

	this(name,cmd,keep,System.out,System.err);
    }
    

    /** 
     *  Crea un nuevo objeto Task.
     *  Al crear el objeto se arranca la ejecucion del programa, y en caso de que no se pueda
     *  se lanza una IOException.
     *  La salida normal y de error del programa se redirige a out y a err.
     *  @param name Identificador para el task creado.
     *  @param cmd  comando necesario para lanzar el programa desde la shell.
     *  @param keep boolean que indica si queremos relanzarlo en caso de muerte o no.
     *  @param out PrintStream al que se envia la salida normal del programa. Si este
     *             parametro es null, se tira a la basura la salida.
     *  @param err PrintStream al que se envia la salida normal del programa. Si este
     *             parametro es null, se tira a la basura la salida.
     *  @throws <tt>IOException</tt> Si se produce algun error al lanzar el proceso.
     *  @throws <tt>Exception</tt> Si el coamndo esta mal formado.
     */
    public Task(String name, String cmd, boolean keep, PrintStream out, PrintStream err) 
	throws Exception, IOException {

	this(name, StringParser.toArray(cmd), keep, out, err);
    } 

    //----------------------

    /** 
     *  Crea un nuevo objeto Task.
     *  Al crear el objeto se arranca la ejecucion del programa, y en caso de que no se pueda
     *  se lanza una IOException.
     *  La salida normal y de error del programa se redirige a System.out y System.err.
     *  @param name Identificador para el task creado.
     *  @param cmd  comando necesario para lanzar el programa desde la shell.
     *  @param keep boolean que indica si queremos relanzarlo en caso de muerte o no.
     *  @throws <tt>IOException</tt> Si se produce algun error al lanzar el proceso.
     */
    public Task(String name, String[] cmd, boolean keep) throws IOException {

	this(name,cmd,keep,System.out,System.err,null);
    }

    /** 
     *  Crea un nuevo objeto Task.
     *  Al crear el objeto se arranca la ejecucion del programa, y en caso de que no se pueda
     *  se lanza una IOException.
     *  La salida normal y de error del programa se redirige a System.out y System.err.
     *  @param name Identificador para el task creado.
     *  @param cmd  comando necesario para lanzar el programa desde la shell.
     *  @param keep boolean que indica si queremos relanzarlo en caso de muerte o no.
     *  @throws <tt>IOException</tt> Si se produce algun error al lanzar el proceso.
     */
    public Task(String name, String[] cmd, boolean keep, Map<String,String> environment) throws IOException {

	this(name,cmd,keep,System.out,System.err,environment);
    }


    /** 
     *  Crea un nuevo objeto Task.
     *  Al crear el objeto se arranca la ejecucion del programa, y en caso de que no se pueda
     *  se lanza una IOException.
     *  La salida normal y de error del programa se redirige a out y a err.
     *  @param name Identificador para el task creado.
     *  @param cmd  comando necesario para lanzar el programa desde la shell.
     *  @param keep boolean que indica si queremos relanzarlo en caso de muerte o no.
     *  @param out PrintStream al que se envia la salida normal del programa. Si este
     *             parametro es null, se tira a la basura la salida.
     *  @param err PrintStream al que se envia la salida normal del programa. Si este
     *             parametro es null, se tira a la basura la salida.
     *  @throws <tt>IOException</tt> Si se produce algun error al lanzar el proceso.
     */
    public Task(String name, String[] cmd, boolean keep, PrintStream out, PrintStream err) 
	throws IOException {

	this(name, cmd, keep, out, err, null); 
    }

    /** 
     *  Crea un nuevo objeto Task.
     *  Al crear el objeto se arranca la ejecucion del programa, y en caso de que no se pueda
     *  se lanza una IOException.
     *  La salida normal y de error del programa se redirige a out y a err.
     *  Tambien toma un Map<variable,valor> para crear nuevas variables de entorno.
     *  @param name Identificador para el task creado.
     *  @param cmd  comando necesario para lanzar el programa desde la shell.
     *  @param keep boolean que indica si queremos relanzarlo en caso de muerte o no.
     *  @param out PrintStream al que se envia la salida normal del programa. Si este
     *             parametro es null, se tira a la basura la salida.
     *  @param err PrintStream al que se envia la salida normal del programa. Si este
     *             parametro es null, se tira a la basura la salida.
     *  @throws <tt>IOException</tt> Si se produce algun error al lanzar el proceso.
     */
    public Task(String name, String[] cmd, boolean keep, PrintStream out, PrintStream err, Map<String,String> environment) 
	throws IOException {

	mylogger = Logger.getLogger("isabel.lib.tasks.Task."+name);
	mylogger.fine("Creando Task ("+name+") para: \""+cmd+"\".");

	builder = new ProcessBuilder(cmd);

	if (environment != null) {
	    Map<String,String> pbenv = builder.environment();
	    pbenv.putAll(environment);
	}
	
	taskname  = name;

	setName("Task "+name);

        keepAlive = keep;
	killed    = false;

	pout = out;
	perr = err;

        startProgram();
        listeners = new ArrayList<TaskListener>();
	
	// Register a shutdown hook to destroy the process when JVM exits.
	shutdownHook = new Thread() {
		public void run() {
		    kill();
		}
	    };
	Runtime.getRuntime().addShutdownHook(shutdownHook);
    }
    
    //----------------------

    /**
     * Set the timeout (in miliseconds) to wait for the process to close its output and error streams.
     * When this timeout expires, the streams are interrupted and closed.
     * Default value is 10000.
     * Timeout 0 means wait forever.
     */
    public void setStreamsClosingTimeout(int milisecs) {
	closingTimeout = milisecs;
    }
    
    //----------------------

    /** 
     *  Arranca el programa.
     *  @throws <tt>IOException</tt> Si se produce algun error y no se puede
     *                               arrancar el programa.
     */
    private synchronized void startProgram() throws IOException {

	mylogger.fine("Starting task \""+taskname+"\".");

	cleanOutErrCont = 2;
	


        process = builder.start();
        final Process p = process;

	try {
	    p.getOutputStream().close();
	} catch (Exception e) {
	}

	// Redirige la salida estandar del programa a pout.
	cleanStdIn = new Thread() {
		
		public void run() {
		    BufferedReader br;
		    br = new BufferedReader(new InputStreamReader(p.getInputStream()));
		    try {
			String msg;
			while ((msg = br.readLine()) != null) {
			    if (pout != null) {
				pout.println(msg);
			    }
			}
		    } catch (Exception e) {
		    } 
		    try {
			br.close();
		    } catch (Exception e) {
		    }
		    cleanStdIn = null;
		}
		
	    };
	cleanStdIn.setDaemon(true);
	cleanStdIn.start();

	// Redirige la salida de error del programa a perr.
	cleanStdErr = new Thread() {

		public void run() {
		    BufferedReader br;
		    br = new BufferedReader(new InputStreamReader(p.getErrorStream()));
		    try {
			String msg;
			while ((msg = br.readLine()) != null) {
			    if (perr != null) {
				perr.println(msg);
			    }
			}
		    } catch (Exception e) {
		    } 
		    try {
			br.close();
		    } catch (Exception e) {
		    }

		    // Wait for the termination of the CleanStdIn thread.
		    if (cleanStdIn != null) {
			try {
			    cleanStdIn.join(closingTimeout);
			} catch (Exception e) {
			}
		    }

		    cleanStdErr = null;
		}
		
	    };
	cleanStdErr.setDaemon(true);
	cleanStdErr.start();
    }


    /**
     *  Indica si debe o no debe hacerse un process.destroy() cuando se invoca kill().
     *  @param destroy Usar true si debe ejecutarse process.destroy() al llamar al metodo kill(),
     *                 o false en caso contrario. Por defecto, se invoca llama a destroy().
     */
    public void setDestroyOnKill(boolean destroy) {
	destroyOnKill = destroy;
    }


    /** 
     *  Mata el programa que estamos ejecutando, y ademas no se relanza,
     *  ni se avisa a los escuchadores.
     */
    public synchronized void kill() {

        keepAlive = false;
	killed = true;

	if (cleanStdIn != null) {
	    cleanStdIn.interrupt();
	}
	if (cleanStdErr != null) {
	    cleanStdErr.interrupt();
	}

	if (process != null) {
	    try {process.getOutputStream().close();} catch (Exception e) {}
	    try {process.getInputStream().close();} catch (Exception e) {}
	    try {process.getErrorStream().close();} catch (Exception e) {}
	    
	    if (destroyOnKill) {
		process.destroy();
	    }
	}
	process = null;
    }



    /** 
     *  Nucleo del thread que se encarga del relanzamiento del programa, y de avisar 
     *  los escuchadores cada vez que se muere.
     */
    public void run() {

	while (true) {
	    try {
		// Esperar a que termine el programa.
		if (process != null) {
		    process.waitFor();
		    mylogger.info("Task \""+taskname+"\" has finished.");
		    process = null;
		}

		// Si me hicieron kill() termino
		if (killed) break;

		// Wait for the termination of the cleanStdErr thread, which (maybe) is
		// waiting for the termination of the CleanStdIn thread.
		if (cleanStdErr != null) {
		    try {
			cleanStdErr.join(closingTimeout);
		    } catch (Exception e) {
		    }
		}

		if (cleanStdIn != null) {
		    mylogger.warning("Task \""+taskname+"\" finished without closing its standart output.");
		    cleanStdIn.interrupt();
		}
		if (cleanStdErr != null) {
		    mylogger.warning("Task \""+taskname+"\" finished without closing its error output.");
		    cleanStdErr.interrupt();
		}
		try {process.getOutputStream().close();} catch (Exception e) {}
		try {process.getInputStream().close();} catch (Exception e) {}
		try {process.getErrorStream().close();} catch (Exception e) {}
		
                // Cuando el programa termina avisamos a los listeners
                warnListeners();

                // Si no hay que relanzarlo, termino.
                if (!keepAlive) break;

		// Espera un poco por si la aplicacion murio abruptamente,
		// para que los ganchos de salida puedan hacer kill antes de intentar
		// relanzar el programa.
		try {
		    Thread.sleep(1000);
		} catch (InterruptedException ie) {
		}
		
		// Si me hicieron un kill miestras estaba en el sleep anterior, entonces no rearranco.
		synchronized(this) {
		    if (killed) break;
		    // Rearrancar el programa.
		    startProgram();
		}
            } catch (InterruptedException e) {
		// waitfor fue interrunpido. Me pongo otra vez a esperar.
		mylogger.severe(e.getMessage());
	    } catch (IOException e) {
		// No puedo rearrancar el programa. Es como si hubiera terminado otra vez.
		mylogger.severe(e.getMessage());
	    }
	    // Thread.yield(); // cedo control a otro thread.
        }
	mylogger.fine("Method \"run\" of thread \""+taskname+"\" has finished.");
    }


    /**
     *  Finalizo el objeto matando el programa si fuera necesario, y
     *  eliminando el gancho.
     */
    public void finalize() {
	kill();

	// Deregister the shutdown hook to destroy the process.
	if (shutdownHook != null) {
	    try {
		Runtime.getRuntime().removeShutdownHook(shutdownHook);
	    }
	    catch (Exception e) {
	    }
	    shutdownHook = null;
	}
    }


    /** 
     *  Apunta el listener especificado como parámetro para recibir un aviso cuando
     *  el proceso muere.
     *  Si lis es null no se lanza ninguna excepción ni se realiza ninguna acción.
     *  @param lis El listener que queremos que reciba los eventos.
     */
    public synchronized void addTaskListener(TaskListener lis) {
        if (lis == null)
            return;
        listeners.add(lis);
    }
    

    /** 
     *  Elimina el listener especificado como parámetro para que deje de recibir eventos.
     *  Si el listener que se pasa como parámetro no había sido añadido o es null no
     *  se realiza ninguna acción.
     *  @param lis El listener que queremos que deje de recibir los eventos.
     */
    public synchronized void removeTaskListener(TaskListener lis) {
       if (lis == null)
           return;
       listeners.remove(lis);
    }

    
    /** 
     *  Método para avisar a todos los listeners de la muerte del proceso externo.
     */
    private void warnListeners() {

	mylogger.fine("Warning listener of task \""+taskname+"\".");

        ArrayList<TaskListener> list;
        synchronized(this) {
            list = (ArrayList<TaskListener>)listeners.clone();
        }

        Iterator<TaskListener> it = list.iterator();

        while(it.hasNext()) {
            TaskListener listener = it.next();
            listener.deadTask();
        }
    }
    

    /** 
     *  Método para probar la clase.
     *  @param args the command line arguments
     *  @throws IOException Si surge algun porblema
     */
    public static void main (String args[]) throws Exception, IOException {
        new Task(args[0], false).start();
    }
}

