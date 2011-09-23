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
 * AudioControl.java
 *
 * Created on 8 de julio de 2003, 11:19
 */

package isabel.sam;

import java.util.HashSet;
import java.util.StringTokenizer;

/**
 * Esta clase representa uno de los controles de la tarjeta de sonido.
 * Los cambios sobre uno de estos objetos no tienen efecto en el hardware.
 * Para hacer efectivos los cambios hay que utilizar el método setControl()
 * de la clase AlsaMixer.
 * @author  Fernando Escribano
 */
public class AudioControl {

    /**
     * Palabra que indica que hay control de volumen de reproduccion.
     */
    private static final String P_VOLUME = "pvolume";
    
    /**
     * Palabra que indica que hay control de volumen de captura.
     */
    private static final String C_VOLUME = "cvolume";
    
    /**
     * Palabra que indica que hay control de volumen.
     */
    private static final String VOLUME = "volume";
    
    /**
     * Palabra que indica que hay control de mute.
     */
    private static final String P_SWITCH = "pswitch";

    /**
     * Palabra que indica que hay control de captura
     */
    private static final String C_SWITCH = "cswitch";
    
    /**
     * Cadena con la que comienza la descripcion de los controles en la salida
     * de amixer. La siguiente palabra es el nombre del control.
     */
    private static final String NAME_LINE = "Simple mixer control";
    
    // Campos de la salida de amixer
    /**
     * Campo de capacidades del control
     */
    private static final String CAPABILITIES = "Capabilities";
    
    /**
     * Campo de canales de reproduccion del control
     */    
    private static final String P_CHANNELS = "Playback channels";
    
    /**
     * Campo de canales de captura del control
     */    
    private static final String C_CHANNELS = "Capture channels";
    
    /**
     * Campo de valores hardware posibles para el volumen del control.
     */    
    private static final String LIMITS = "Limits";
    
    /**
     * Campo de grupo de captura ¿?
     */    
    private static final String CAPTURE_GROUP = "Capture exclusive group";
    
    /**
     * Palabra que indica que los valores posteriores correspondes a reproduccion.
     */    
    private static final String PLAYBACK = "Playback";
        
    /**
     * Palabra que indica que los valores posteriores correspondes a captura
     */    
    private static final String CAPTURE = "Capture";
    
    /**
     * Palabra que indica activado.
     */
    private static final String ON = "on";
    
    /**
     * Palabra que indica desactivado.
     */
    private static final String OFF = "off";
    
    /**
     * Nombre del control.
     */
    private String name;
    
    /**
     * Volumen del control. Entero entre 0 y 100.
     */
    private int volume;
    
    /**
     * Boolean que indica si el mute esta activado o no.
     */
    private boolean mute;
    
    /**
     * Boolean que indica si la captura esta activada o no.
     */
    private boolean capture;
    
    /**
     * Conjunto de capacidaes del control.
     */
    private HashSet capabilities;
    
    /**
     * Contruye un objeto de la clase AudioControl.
     * @param data String que contiene los datos de un control tal y como los escribe amixer.
     *
     */    
    public AudioControl(String data) {
        capabilities = new HashSet();
        parseData(data);
    }
    
    /**
     * Establece el nombre del control.
     * @param name String con el nombre del control.
     */    
    public void setName(String name) {
        this.name = name;
    }
    
    /**
     * Devuelve el nombre del control.
     * @return String con el nombre del control.
     */    
    public String getName() {
        return name;
    }
    
    /**
     * Establece el volumen del control. Si no tiene volumen no hace nada.
     * @param volume int con el volumen del dispositivo.
     */    
    public void setVolume(int volume) {
        if (this.hasVolume())
            this.volume = volume;
    }
    
    /**
     * Devuelve el volumen del control.
     * @return int con el volumen. Si el control no tiene volumen devuelve 0.
     */
    public int getVolume() {
        return volume;
    }
    
    /**
     * Establece el mute del dispositivo. Si no tiene mute no hace nada.
     * @param mute True para avcttivar y false para desactivar.
     */
    public void setMute(boolean mute) {
        if (this.hasMute())
            this.mute = mute;
    }
    
    /**
     * Devuelve el estado del mute del control.
     * @return True si esta activado y false en caso contrario.
     */
    public boolean getMute() {
        return mute;
    }
    
    /**
     * Establece si se captura o no de este control.
     * @param capture True para capturar y false para no hacerlo.
     */
    public void setCapture(boolean capture) {
        this.capture = capture;
    }
    
    /**
     * Devuelve el estado de capture del control.
     * @return True si se captura y false en caso contrario.
     */
    public boolean getCapture() {
        return capture;
    }
    
    /**
     * Dice si este control tiene contro de volumen o no.
     * @return True si tiene volumen y false en caso contrario.
     */
    public boolean hasVolume() {
        return (capabilities.contains(P_VOLUME) || capabilities.contains(C_VOLUME) || capabilities.contains(VOLUME));
    }
    
    /**
     * Dice si este control tiene control de mute.
     * @return True si tiene mute y false en caso contrario.
     */
    public boolean hasMute() {
        return capabilities.contains(P_SWITCH);
    }
    
    /**
     * Dice si este control tiene control de captura o si no.
     * @return True en si tiene control de captura y false en caso contrario.
     */
    public boolean hasCapture() {
        return capabilities.contains(C_SWITCH);
    }
    
    /**
     * Este metodo se encarga de hacer el parseo de los datos que devuelve el programa
     * amixer para rellenar los distintos campos de este objeto. Es llamado desde el
     * constructor del objeto.
     * @param data String que contiene los datos de un control tal y como los escribe amixer.
     */
    private void parseData(String data) {
        // Parto el string en las lineas que contiene.
        String [] lines = data.split("\n");
        
        // La primera linea contiene el nombre del dispositivo
        name = lines[0].substring(NAME_LINE.length()).trim();
        
        // El resto de las lineas tienen como formato
        //          variable1: valor
        //          variable2: valor
        //          ................
        // Obtengo las variables y sus valores los proceso según el nombre.
        for (int i = 1; i < lines.length; i++) {
            int twopoints = lines[i].indexOf(":");
            String header = lines[i].substring(0, twopoints).trim();
            String body   = lines[i].substring(twopoints + 1).trim();
            
            // La linea de capacidades es: "Capabilities: cap1 cap2 cap3 ..."
            if (header.equals(CAPABILITIES)) {
                StringTokenizer st = new StringTokenizer(body);
                while (st.hasMoreTokens())
                    capabilities.add(st.nextToken());
                continue;
            }
            
            // Las lineas de channels, la de limits y la de grupo de captura no interesan
            if (header.equals(P_CHANNELS) || header.equals(C_CHANNELS))
                continue;
            if (header.equals(LIMITS))
                continue;
            if (header.equals(CAPTURE_GROUP))
                continue;
            
            // El resto de las lineas son de configuracion de canales. El formato es:
            //     Playback PVolHW [PVol%] [Mute] Capture CVolHW [CVol%] [Capt]
            // dependiendo de las capacidades del control necesitare encontrar algunas o no
            StringTokenizer st = new StringTokenizer(body, "[] ");
            boolean playback = false;
            while(st.hasMoreTokens()) {
                String token = st.nextToken().trim();
                if (token.equals(PLAYBACK))
                    playback = true;
                if (token.equals(CAPTURE))
                    playback = false;
                if (token.endsWith("%"))
                    setVolume(new Integer(token.substring(0, token.indexOf("%"))).intValue());
                if (token.equals(ON)) {
                    if (playback)
                        setMute(false);
                    else
                        setCapture(true);
                }              
                if (token.equals(OFF)) {
                    if (playback)
                        setMute(true);
                    else
                        setCapture(false);
                }
            }
        }
    }
    
    /**
     * Devuelve una represenatcion textual del objeto.
     * Sirve para imprimir por pantalla los objetos AudioControl para hacer
     * pruebas.
     * @return Un String que representa el objeto.
     */
    public String toString() {
        String result;
        result = "Name: " + this.getName() + "\n" + 
                 "Tiene volumen: " + this.hasVolume() + "\n" +
                 "Volumen: " + this.getVolume() + "\n" +
                 "Tiene mute: " + this.hasMute() + "\n" +
                 "Mute: " + this.getMute() + "\n" +
                 "Tiene capture: " + this.hasCapture() + "\n" +
                 "Capture: " +  this.getCapture();
        return result;
    }
        
    /**
     * Metodo de prueba de la clase.
     * @param args Argumentos de la linea de comando. No se utilizan.
     */
    public static void main(String [] args) {
        
        String s = "Simple mixer control 'Master',0" + "\n" + 
                    "    Capabilities: pvolume pswitch pswitch-joined" + "\n" + 
                    "    Playback channels:Front Left - Front Right" + "\n" + 
                    "    Limits: Playback 0 - 31" + "\n" + 
                    "    Front Left:     Playback 21 [68%] [on]" + "\n" + 
                    "    Front Right: Playback 21 [68%] [on]";
        System.out.println(s);
        System.out.println(new AudioControl(s));
        
        s = "Simple mixer control 'Line',0" + "\n" +
            "Capabilities: pvolume pswitch pswitch-joined cswitch cswitch-joined cswitch-exclusive" + "\n" +
            "Capture exclusive group: 0" + "\n" +
            "Playback channels: Front Left - Front Right" + "\n" +
            "Capture channels: Front Left - Front Right " + "\n" +
            "Limits: Playback 0 - 31 " + "\n" +
            "Front Left: Playback 24 [77%] [on] Capture [off] " + "\n" +
            "Front Right: Playback 24 [77%] [on] Capture [off] ";
        System.out.println(s);
        System.out.println(new AudioControl(s));
        
        s = "Simple mixer control 'Mic',0" + "\n" +
            "Capabilities: pvolume pvolume-joined pswitch pswitch-joined cswitch" + "\n" +
            "Capture exclusive group: 0" + "\n" +
            "Playback channels: Mono" + "\n" +
            "Capture channels: Front Left - Front Right " + "\n" +
            "Limits: Playback 0 - 31 " + "\n" +
            "Mono: Playback 28 [90%] [off] " + "\n" +
            "Front Left: Capture [on] " + "\n" +
            "Front Right: Capture [on] ";
        System.out.println(s);
        System.out.println(new AudioControl(s));
    }
}
