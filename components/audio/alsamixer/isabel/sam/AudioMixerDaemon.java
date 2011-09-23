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
 * AudioMixerDaemon.java
 *
 * Created on 9 de julio de 2003, 15:03
 */

package isabel.sam;

import java.io.*;
import java.net.*;

import java.util.Map;
import java.util.HashMap;
import java.util.StringTokenizer;
import java.util.Iterator;

/**
 * Esta clase implementa el demonio de control del mezclador local de audio.
 * Abre un socket por el que atiende una sola conexión a la vez. Por este socket
 * atiende diversos comandos y escribe la respuesta.
 * @author  Fernando Escribano
 */
public class AudioMixerDaemon extends Thread {
    
    /**
     * Puerto por el que se atienden las conexiones por las que se reciben los
     * comandos.
     */
    public static final int PORT = 10100;
    
    /**
     * Path relativo al directorio de configuarion donde se encuentra el archivo de
     * configuración del audio.
     */
    public static final String CONFIG_FILE = "/sam.cfg";
    
    /**
     * Directorio que guarda la configuracion del usuario
     * de la aplicacion isabel. Es la clave de una propiedad de System.
     */
    private static final String ISABEL_CONFIG_DIR = "isabel.config.dir";
    
    // COMANDOS QUE SE ESPERA RECIBIR POR EL SOCKET
    
    /**
     * Comandos para establecer el volumen de un control.
     * Toma como parametros el nombre del control y el volumen a establecer.
     * El volumen es un numero entero entre 0 y 100.
     * Devuelve OK en caso de exito.
     */
    private static final String SET_VOLUME_CMD      = "sam_set_volume";
    
    /**
     * Comando para activar el mute de un control.
     * Toma como parametro el nombre del control.
     * Devuelve OK en caso de exito.
     */
    private static final String SET_MUTE_CMD        = "sam_set_mute";
    
    /**
     * Parametro para desactivar el mute de un control.
     * Toma como parametro el nombre del control.
     * Devuelve OK en caso de exito.
     */
    private static final String SET_UNMUTE_CMD      = "sam_set_unmute";
    
    /**
     * Activa la captura desde un control. Puede hacer que se deje de
     * capturar desde otro control.
     * Toma como parametro el nombre del control.
     * Devuelve OK en caso de exito.
     */
    private static final String SET_CAPTURE_CMD     = "sam_set_capture";
    
    /**
     * Comando para desactivar la captura desde un control. Puede no tener
     * efecto.
     * Toma como parametro el nombre del control.
     * Devuelve OK en caso de exito.
     */
    private static final String SET_NOCAPTURE_CMD   = "sam_set_nocapture";
    
    /**
     * Comando para obtener el estado de un control.
     * Toma como parametro el nombre del control.
     * Devuelve una lista entre llaves con: {volumen true/false true/false}
     * Correspondiendo el primer boolean al estado del mute y el segundo
     * al estado del capture.
     */
    private static final String GET_STATUS_CMD      = "sam_get_status";

    /**
     * Comando para recargar el fichero de configuracion
     * Devuelve OK en caso de exito
     */
    private static final String RELOAD_FILE_CMD      = "sam_reload";

    /**
     * Comando para recargar el fichero de configuracion
     * Devuelve OK en caso de exito
     */
    private static final String LOAD_FILE_CMD      = "sam_load";


    
    /**
     * Comando para saber si un control tiene control de volumen.
     * Toma como parametro el nombre del control.
     * Devuelve TRUE o FALSE
     */
    private static final String HAS_VOLUME_CMD      = "sam_query_volume";
    
    /**
     * Comando para saber si un control tiene control de mute
     * Toma como parametro el nombre del control.
     * Devuelve TRUE o FALSE
     */
    private static final String HAS_MUTE_CMD        = "sam_query_mute";
    
    /**
     * Comando para saber si un control tiene control de captura
     * Toma como parametro el nombre del control.
     * Devuelve TRUE o FALSE
     */    
    private static final String HAS_CAPTURE_CMD     = "sam_query_capture";
    
    /**
     * Comando para obtener una lista de los controles disponibles.
     * Devuelve una lista entre llaves con los nombres: {c1 c2 c3}
     */
    private static final String GET_CONTROLS_CMD    = "sam_query_controls";
    
    /**
     * Comando para ordenar al demonio que termine.
     */
    private static final String QUIT_CMD            = "sam_quit";
    
    /**
     * Comando para despedirse del demonio y cerrar la conexion.
     */
    private static final String BYE_CMD             = "sam_bye";
    
    /**
     * Comando que no realiza ninguna operacion.
     * Devuelve OK.
     */
    private static final String NOP_CMD             = "sam_nop";
    
    /**
     * Tabla con los datos del fichero de configuración.
     */
    private AudioConfigFile configFile;
    
    /**
     * ServerSocket por el que se reciben conexiones.
     */
    private ServerSocket server;
    
    /**
     * Objeto que contiene los metodos que ejecutan los comandos.
     */
    private AudioDaemonCommandHandler handler;
    
    /**
     * Boolean para controlar cuando el programa debe seguir aceptando conexiones
     * cuando debe salir.
     */
    private boolean run;
    
    /**
     * Boolean que dice cuando hay una conexión abierta.
     */
    private boolean open;

    /**
     * Nombre del fichero de configuracion
     */
    private String configFileName;

    /**
     * Contruye un ojbeto AudioMixerDaemon.
     * @param port El puerto de control. Si es cero se utiliza cuialquier
     * puerto libre.
     * @throws IOException Si surge algun problema al crear el socket o al abrir el fichero
     * de configuración.
     */
    public AudioMixerDaemon(int port) throws IOException {
        server = new ServerSocket(port);
        String root;
        if (System.getProperty(ISABEL_CONFIG_DIR) != null)
            root = System.getProperty(ISABEL_CONFIG_DIR);
        else
            root = System.getProperty("user.home") + "/.isabel/config";
        configFileName = root + CONFIG_FILE;
        configFile = new AudioConfigFile(configFileName);
        handler = new AudioDaemonCommandHandler("0");
        open = false;
        run = true;
        this.start();
    }
    
    /**
     * Imprime el puerto por el que se atienden las conexiones de control.
     */
    public void printPort() {
        System.out.println(server.getLocalPort());
    }
    
    /**
     * Método principal de la hebra. Espera una conexion y luego va leyendo
     * comandos y respondiendo a ellos.
     */
    public void run() {
//        while (run) {
            try {
                Socket sock = server.accept();
                open = true;
                BufferedReader in = new BufferedReader(
                                        new InputStreamReader(sock.getInputStream()));
		             PrintWriter out = new PrintWriter(new BufferedWriter(
                                    new OutputStreamWriter(sock.getOutputStream())), true);
			     //PrintWriter out = new PrintWriter(
                             //         new OutputStreamWriter(sock.getOutputStream()), true);
                
                String line = "";
                while (open && ((line = in.readLine()) != null)) {
                    line = line.trim();
                    // Ignoro las lineas vacias
                    if (line.equals(""))
                        continue;
                    // Separo la linea en comando y argumentos
                    String[] cmd = parseCommand(line);
                    
                    // Ejecuto el comando pedido
                    String response = execCommand(cmd);

                    // escribo la respuesta por el socket
                    out.println(response);
                }
                in.close();
                out.close();
                sock.close();
                open = false;
            }catch (IOException e) {
                System.err.println(e);
                open = false;
            }
//        }
        try {
            server.close();
        }catch (IOException e) {
            System.err.println(e);
        }
    }
    
    /**
     * Separa una linea en comando y argumentos.
     * @param line La linea que se quiere parsear.
     * @return String []. El primer elemento es el comando y el resto son los
     * argumentos por el orden en que han sido escritos.
     */
    private String[] parseCommand(String line) {
        StringTokenizer st = new StringTokenizer(line, "(),");
        String[] result = new String[st.countTokens()];
        for (int i = 0; i < result.length; i++)
            result[i] = st.nextToken().trim();
        return result;
    }
    
    /**
     * Ejecuta un comando.
     * @param cmd String []. El primer elemento es el comando y el resto son los
     * argumentos.
     * @return String con la respuesta del comando. Puede ser tambien un error.
     */
    private String execCommand(String[] cmd) {
        try {
            // Dependiendo del nombre del comando ejecuto un método diferente
            if (cmd[0].equals(SET_VOLUME_CMD)) {
                String name = (String)configFile.get(cmd[1]);
                int volume = Integer.parseInt(cmd[2]);
                
                return handler.setControlVolume(name, volume);
            }
            
            if (cmd[0].equals(SET_MUTE_CMD)) {
                String name = (String)configFile.get(cmd[1]);
                return handler.setControlMute(name, true);
            }
            
            if (cmd[0].equals(SET_UNMUTE_CMD)) {
                String name = (String)configFile.get(cmd[1]);
                return handler.setControlMute(name, false);
            }
            
            if (cmd[0].equals(SET_CAPTURE_CMD)) {
                String name = (String)configFile.get(cmd[1]);
                return handler.setControlCapture(name, true);                
            }
            
            if (cmd[0].equals(SET_NOCAPTURE_CMD)) {
                String name = getName(cmd[1]);
                return handler.setControlCapture(name, false);
            }
            
            if (cmd[0].equals(HAS_VOLUME_CMD)) {
                String name = getName(cmd[1]);
                return handler.hasVolumeControl(name);                
            }
            
            if (cmd[0].equals(HAS_MUTE_CMD)) {
                String name = getName(cmd[1]);
                return handler.hasMuteControl(name);
            }            
            
            if (cmd[0].equals(HAS_CAPTURE_CMD)) {
                String name = getName(cmd[1]);
                return handler.hasCaptureControl(name);                
            }
            
            if (cmd[0].equals(GET_STATUS_CMD)) {
                String name = getName(cmd[1]);
                return handler.getControlStatus(name);
            }
            
            if (cmd[0].equals(GET_CONTROLS_CMD)) {

		String res = "";
		Iterator it = configFile.keySet().iterator();
		while (it.hasNext()) {
		    res += "{" + (String)it.next() + "} ";
		}
		return res; //configFile.getKeys();
            }

            if (cmd[0].equals(RELOAD_FILE_CMD)) {
                configFile = new AudioConfigFile(configFileName);
		return "OK";
            }

             if (cmd[0].equals(LOAD_FILE_CMD)) {
                 String root;
                 if (System.getProperty(ISABEL_CONFIG_DIR) != null)
                     root = System.getProperty(ISABEL_CONFIG_DIR);
                 else
                     root = System.getProperty("user.home") + "/.isabel/config";

                 configFileName = root + "/" + cmd[1];
                 configFile = new AudioConfigFile(configFileName);
                 handler = new AudioDaemonCommandHandler(cmd[2]);
                 return "OK";
            }

            
            if (cmd[0].equals(BYE_CMD)) {
                open = false;
                return "BYE";
            }
            
            if (cmd[0].equals(QUIT_CMD)) {
                open = false;
                run = false;
                return "BYE";
            }
            
            if (cmd[0].equals(NOP_CMD)) {
                return "OK";
            }
            
        }
	catch (IOException e) {
            return "ERROR: Problem while executing amixer: "+e.getMessage();
        }
	catch (NumberFormatException e) {
            return "ERROR: Bad parameters: "+e.getMessage();
        }
	catch (BadNameException e) {
            return "ERROR: Bad control name: "+e.getMessage();
        }
	catch (IndexOutOfBoundsException e) {
            return "ERROR: Bad parameters: "+e.getMessage();
        }
        return "ERROR: Unknown command: \""+cmd[0]+"\"";
    }
    
    /**
     * Devuelve el nombre real del control contenido en el fichero
     * de configuracion.
     * @param name El nombre virtual del control.
     * @return El nombre real del control.
     * @throws BadNameException Si el nombre no esta definido el fichero de
     * configuracion.
     */
    private String getName(String name) throws BadNameException {
        String result = (String)configFile.get(name);
        if (result == null)
            throw new BadNameException("Bad control name: " + name);
        return result;
    }
    
    /**
     * Excepcion lanzada cuando no se encuentra el nombre en la tabla.
     */
    private class BadNameException extends Exception {
        /**
         * Contruye un objeto BadNameException.
         * @param s Mensaje de error.
         */
        BadNameException(String s) {
            super(s);
        }
    }
    
    /**
     * Esta clase contiene los métodos que son llamados por el demonio del mezclador
     * de audio para llevar a cabo su función.
     */
    private class AudioDaemonCommandHandler {

        private AlsaMixer amixer;
        
        private Map controls;
        
        /**
         * Contruye un objeto de la clase AudioDaemonCommandHandler.
         * @param card Numero de tarjeta que se maneja. 
         * @throws IOException Si hay algun problema con la ejecucion de amixer.
         */
        public AudioDaemonCommandHandler(String card) throws IOException {
            amixer = new AlsaMixer(card);
            controls = getControls();
        }
        
        /**
         * Establece el volumen de un control.
         * @param name Nombre del control.
         * @param volume Volumen a establecer. Entero entre 1 y 100.
         * @return OK en caso de exito y ERROR en caso contrario.
         * @throws IOException Si hay algun problema con la ejecucion de amixer.
         */
        public String setControlVolume(String name, int volume) throws IOException {
            AudioControl ac = (AudioControl)controls.get(name);
            if (ac == null)
                return "ERROR: Control does not exist";
            ac.setVolume(volume);
            ac = amixer.setControlVolume(ac);
            /*
            if (ac.getVolume() == volume)
                return "OK";
            else
                return "ERROR: Unable to set control volume";
             */
            return "OK";
        }
        
        /**
         * Establece el mute de un control.
         * @param name Nombre del control.
         * @param mute True para activar y false para desactivar.
         * @return OK en caso de exito y ERROR en caso contrario.
         * @throws IOException Si hay algun problema con la ejecucion de amixer.
         */
        public String setControlMute(String name, boolean mute) throws IOException {
            AudioControl ac = (AudioControl)controls.get(name);
            if (ac == null)
                return "ERROR: Control does not exist";
            ac.setMute(mute);
            ac = amixer.setControlMute(ac);
            
            if (ac.getMute() == mute)
                return "OK";
            else
                return "ERROR: Unable to set control mute";
        }
        
        /**
         * Establece el capture de un control.
         * @param name Nombre del control.
         * @param capture True para activar y false para desactivar.
         * @return OK en caso de exito y ERROR en caso contrario.
         * @throws IOException Si hay algun problema con la ejecucion de amixer.
         */
        public String setControlCapture(String name, boolean capture) throws IOException {
            AudioControl ac = (AudioControl)controls.get(name);
            if (ac == null)
                return "ERROR: Control does not exist";
            ac.setCapture(capture);
            ac = amixer.setControlCapture(ac);
            
            if (ac.getCapture() == capture)
                return "OK";
            else
                return "ERROR: Unable to set control capture";
        }

        /**
         * Devuelve el estado de un control.
         * @param name Nombre del control.
         * @return Lista con el siguiente formato {volumen mute capture} y ERROR en caso de fallo.
         * @throws IOException Si hay algun problema con la ejecucion de amixer.
         */
        public String getControlStatus(String name) throws IOException {
            AudioControl ac = amixer.getControl(name);
            if (ac == null)
                return "ERROR: Control does not exist";
            String result = (ac.getVolume()  + " " + 
			     ac.getMute()    + " " +
			     ac.getCapture()).toUpperCase();
            return result;
        }

        /**
         * Dice si el control tiene control de volumen.
         * @param name Nombre del control.
         * @return TRUE en caso de exito y FALSE en caso contrario.
         */
        public String hasVolumeControl(String name) {
            AudioControl ac = (AudioControl)controls.get(name);
            if (ac == null)
                return "ERROR: Control does not exist";
            String result = "" + ac.hasVolume();
            return result.toUpperCase();
        }

        /**
         * Dice si el control tiene control de mute.
         * @param name Nombre del control.
         * @return TRUE en caso de exito y FALSE en caso contrario.
         */
        public String hasMuteControl(String name) {
            AudioControl ac = (AudioControl)controls.get(name);
            if (ac == null)
                return "ERROR: Control does not exist";
            String result = "" + ac.hasMute();
            return result.toUpperCase();
        }

        /**
         * Dice si el control tiene control de captura
         * @param name Nombre del control.
         * @return TRUE en caso de exito y FALSE en caso contrario.
         */
        public String hasCaptureControl(String name) {
            AudioControl ac = (AudioControl)controls.get(name);
            if (ac == null)
                return "ERROR: Control does not exist";
            String result = "" + ac.hasCapture();
            return result.toUpperCase();
        }

        /**
         * Este método obtiene la configuración actual de todos los controles de
         * la tarjeta y lo guarda en una tabla.
         * @return Una tabla con todos los controles de la tarjeta.
         * @throws IOException Si hay algun problema con la ejecucion de amixer.
         */
        private Map getControls() throws IOException {
            HashMap result = new HashMap();
            AudioControl[] ac = amixer.getAllControls();
            for (int i = 0; i < ac.length; i++)
                result.put(ac[i].getName(), ac[i]);
            
            return result;
        }
    }

    /**
     *  Prints usage message.
     */
    private static void usage() {

	System.out.println("AudioMixerDaemon usage:");
	System.out.println("  java -cp <ISABEL_DIR>/libexec/isabel_alsamixer.jar \\");
	System.out.println("       -Disabel.config.dir=<ISABEL_CONFIG_DIR> \\");
	System.out.println("       isabel.mixer.AudioMixerDaemon \\");
	System.out.println("       [-cport <control_port>] \\");
	System.out.println("       [-uniqid <uniqId>]");
    }

    
    /**
     * Método principal de la clase.
     * @param args Parametros de la linea de comandos.
     */
    public static void main(String [] args) {
        int port = 0;

	for (int i=0 ; i<args.length ; ) {

	    if (args[i].equals("-h")) {
		usage();
		i++;
	    } else if (args[i].equals("-uniqid")) {
		if (i+2 > args.length) {
		    System.err.println("Incomplete list of arguments.");
		    break;
		}
		i += 2;

	    } else if (args[i].equals("-cport")) {
		if (i+2 > args.length) {
		    System.err.println("Incomplete list of arguments.");
		    break;
		}
		try {
		    port = Integer.parseInt(args[i+1]);
		}
		catch (NumberFormatException e) {
		    System.err.println("Invalid port number: \""+args[i+1]+"\". Using port 0.");
		}
		i += 2;

	    } else {
		System.err.println("Unknown option \""+args[i]+"\".");
		i++;
	    }
	}

        try {
            new AudioMixerDaemon(port).printPort();
        }catch (IOException e) {
            System.err.println(e);
        }
    }
}
