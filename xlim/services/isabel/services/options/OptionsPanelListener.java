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
 * OptionsPanelListener.java
 *
 * Created on 16 de septiembre de 2003, 11:12
 */

package services.isabel.services.options;

/**
 * Interfaz que deben implementar las clases que quieran responder
 * a los botones de cancel y done del panel de opciones.
 * @author  Fernando Escribano
 */
public interface OptionsPanelListener {
    
    /**
     * M�todo que se ejecuta cuando se pulsa el boton de cancel en
     * el panel de opciones.
     */
    public void cancelButtonPressed();
    
    /**
     * M�todo que se ejecuta cuando se pulsa el boton de done en
     * el panel de opciones.
     */
    public void doneButtonPressed();
    
    /**
     * M�todo que se ejecuta cuando se pulsa el boton de reset en
     * el panel de opciones.
     */
    public void resetButtonPressed();
    
}
