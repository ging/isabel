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
 * AlsaMixer.java
 *
 * Created on 9 de julio de 2003, 10:54
 */

package isabel.sam;

import java.io.*;
import java.util.StringTokenizer;

/**
 * Esta clase se encarga del manejo del programa externo amixer. Con este
 * programa se pueden modificar los valores de volumen mute y capture de los
 * distintos controles de la tarjeta de sonido.
 * @author  Fernando Escribano
 */
public class AlsaMixer {
    
    /**
     * Programa externo para controlar el mezclador de audio.
     */
    private static final String AMIXER_PROGRAM = "amixer";
    
    /**
     * Comando de amixer para obtener los datos de un control.
     */
    private static final String GET_COMMAND = "sget";
    
    /**
     * Comando de amixer para establecer los datos de un control.
     */
    private static final String SET_COMMAND = "sset";
    
    /**
     * Palabra utilizada junbto con el comando sset para poner el mute de un control.
     */
    private static final String MUTE = "mute";
    
    /**
     * Palabra utilizada junbto con el comando sset para quitar el mute de un control.
     */
    private static final String UNMUTE = "unmute";
    
    /**
     * Palabra utilizada junbto con el comando sset para poner el capture de un control.
     */
    private static final String CAPTURE = "captured";
    
    /**
     * Palabra utilizada junbto con el comando sset para quitar el capture de un control.
     */
    private static final String NOCAPTURE = "nocapture";
    
    /**
     * Cadena con la que comienza la descripcion de los controles en la salida
     * de amixer.
     */
    private static final String NEW_CONTROL = "Simple mixer control";

    /**
     * Opcion para seleccionar tarejeta.
     */
    private static final String CARD_OPTION = "-c";
  

    /**
     * Numero de tarjeta que se maneja a traves de este objeto.
     */
    private String cardNumber = "0";

    /**
     * Construye un objeto AlsaMixer.
     * @param card Numero de tarjeta que se maneja traves de este objeto.
     */
    public AlsaMixer(String card) {
        cardNumber = card;
    }
 
    /**
     * Este método pregunta medianate el comando amixer al driver de la tarjeta
     * de sonido por el estado de un control.
     * @param name Nombre del control por el que se pregunta.
     * @return Un objeto AudioControl con el estado del control por el que se pregunta.
     * @throws IOException si ocurre algun problema durante la ejecución de amixer.
     */
    public AudioControl getControl(String name) throws IOException {
        String [] cmd = {AMIXER_PROGRAM, CARD_OPTION, cardNumber, GET_COMMAND, name};
        return new AudioControl(exec(cmd));
    }
    
    /**
     * Este método devuelve un array de AudioControl con todos los controles
     * de la tarjeta de sonido.
     * @return AudioConmtrol [] donde cada elemento representa un control de la
     * tarjeta de sonido.
     * @throws IOException si ocurre algun problema durante la ejecución de amixer.
     */
    public AudioControl[] getAllControls() throws IOException {
        String [] cmd = {AMIXER_PROGRAM, CARD_OPTION, cardNumber};
        // Hay que partir la salida en los controles
        String output = exec(cmd);
        String [] data = output.split(NEW_CONTROL);
        
        // Como la salida empieza por NEW_CONTROL el primer elemento es vacio
        // y no vale
        AudioControl [] controls = new AudioControl[data.length - 1];
        for (int i = 0; i < controls.length; i++)
            controls[i] = new AudioControl(NEW_CONTROL + data[i + 1]);
        return controls;
    }
    
    /**
     * Este método ejecuta el comando amixer para que establezca en el control
     * representado por el objeto control los valores en el contenidos.
     * @param control el objeto que representa el control que se quiere establecer.
     * @return Un objeto AudioControl con el estado del control que se establece.
     * @throws IOException si ocurre algun problema durante la ejecución de amixer.
     */
    public AudioControl setControl(AudioControl control) throws IOException {
        // Obtengo los valores que se deben establecer
        String name = control.getName();
        String volume = "" + control.getVolume() + "%";
        String mute = control.getMute()? MUTE : UNMUTE;
        String capture = control.getCapture()? CAPTURE : NOCAPTURE;
        String [] cmd = {AMIXER_PROGRAM, CARD_OPTION, cardNumber, SET_COMMAND, name, volume, mute, capture};
        
        return new AudioControl(exec(cmd));
    }
    
    /**
     * Este método ejecuta el comando amixer para que establezca en el control
     * representado por el objeto control el valor del mute.
     * @param control el objeto que representa el control que se quiere establecer.
     * @return Un objeto AudioControl con el estado del control que se establece.
     * @throws IOException si ocurre algun problema durante la ejecución de amixer.
     */
    public AudioControl setControlMute(AudioControl control) throws IOException {
        String name = control.getName();
        String mute = control.getMute()? MUTE : UNMUTE;
        String [] cmd = {AMIXER_PROGRAM, CARD_OPTION, cardNumber, SET_COMMAND, name, mute};
        
        return new AudioControl(exec(cmd));
    }
    
    /**
     * Este método ejecuta el comando amixer para que establezca en el control
     * representado por el objeto control el valor del volumen.
     * @param control el objeto que representa el control que se quiere establecer.
     * @return Un objeto AudioControl con el estado del control que se establece.
     * @throws IOException si ocurre algun problema durante la ejecución de amixer.
     */
    public AudioControl setControlVolume(AudioControl control) throws IOException {
        String name = control.getName();
        String volume = "" + control.getVolume() + "%";
        String [] cmd = {AMIXER_PROGRAM, CARD_OPTION, cardNumber, SET_COMMAND, name, volume};
        
        return new AudioControl(exec(cmd));
    }    
    
    /**
     * Este método ejecuta el comando amixer para que establezca en el control
     * representado por el objeto control el valor del volumen.
     * @param control el objeto que representa el control que se quiere establecer.
     * @return Un objeto AudioControl con el estado del control que se establece.
     * @throws IOException si ocurre algun problema durante la ejecución de amixer.
     */
    public AudioControl setControlCapture(AudioControl control) throws IOException {
        String name = control.getName();
        String capture = control.getCapture()? CAPTURE : NOCAPTURE;
        String [] cmd = {AMIXER_PROGRAM, CARD_OPTION, cardNumber, SET_COMMAND, name, capture};
        
        return new AudioControl(exec(cmd));
    }

    /**
     * Ejecuta un comando y devuelve un String con su salida.
     * @param cmd String[] con el comando y opciones a ejecutar.
     * @return String con la salida del programa.
     * @throws IOException si ocurre algun problema durante la ejecución.
     */
    private String exec(String[] cmd) throws IOException {
        // Ejecuto el programa
        Process p = Runtime.getRuntime().exec(cmd);
        
        // Guardo su salida en un byte[]
        ByteArrayOutputStream buffer = new ByteArrayOutputStream();
        BufferedReader in = new BufferedReader(new InputStreamReader(p.getInputStream()));
        PrintWriter out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(buffer)));
        String line;
        while ((line = in.readLine()) != null)
            out.println(line);
        in.close();
        out.close();
        // devuelvo la salida como un String
        String result = new String(buffer.toByteArray());
        //System.out.println(result);
	p.getOutputStream().close();
	p.getErrorStream().close();
        return result;
    }
    
    /**
     * Metodo de prueba de la clase.
     * @param args Argumentos de la linea de comando. No se utilizan.
     * @throws IOException si ocurre algun problema durante la ejecución.
     */
    public static void main(String[] args) throws IOException {
        AlsaMixer am = new AlsaMixer("0");
        
        System.out.println("Getting Line...");
        AudioControl line = am.getControl("Line");
        System.out.println(line);
        System.out.println();
        System.out.println("Setting Line volume to 40");
        line.setVolume(40);
        System.out.println(line);
        System.out.println("Changing Line mute");
        line.setMute(!line.getMute());
        line = am.setControl(line);
        System.out.println(line);
        System.out.println();
        System.out.println("Changing volume(60) and mute");
        line.setVolume(60);
        line.setMute(!line.getMute());
        line = am.setControl(line);
        System.out.println(line);
        System.out.println();
        
        AudioControl [] controls = am.getAllControls();
        for (int i = 0; i < controls.length; i++)
            System.out.println(controls[i]);
    }
}
