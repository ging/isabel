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
 * Clase para establecer los colores de un interfaz en java a partir de un fichero
 * de propiedades.
 */
package isabel.lib;

import java.awt.Color;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.UIDefaults;
import javax.swing.UIManager;

/**
 * 
 * @author Fernando Escribano
 */
public class ColorLoader {

    /**
     * Carga los colores de un fichero de propiedades como valores por defecto
     * para los interfaces.
     * @param filename Fichero de propiedades con los colores.
     */
    public static void setUIColors(InputStream file) {
        try {            
            UIDefaults defs = UIManager.getDefaults();

            Properties props = new Properties();
            props.load(file);

            for (String s : props.stringPropertyNames()) {
                defs.put(s, new Color(Integer.parseInt(props.getProperty(s), 16)));
            }
        } catch (IOException ex) {
            Logger.getLogger(ColorLoader.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
