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
 * ValidablePanel.java
 *
 * Created on 15 de septiembre de 2003, 10:35
 */

package services.isabel.services.options;

/**
 * Interfaz que implementan los paneles para introducir datos.
 * Contiene un unico m�todo para indicar si los datos son correctos.
 * El comportamiento t�pico cuando los datos no son correctos sera mostrar un
 * aviso mediante una ventana emergente a al usuario.
 * @author  Fernando Escribano
 */
public interface ValidablePanel {
    
    /**
     * Este m�todo comprueba que los datso contenidos en el panel son
     * correctos. Es decir son numeros, letras, tienen la longitud adecuada
     * o cualquier regla que se pueda definir para ellos.
     * @return True si los datos son correctos y false si no lo son.
     */
    public boolean validateData();
    
}
