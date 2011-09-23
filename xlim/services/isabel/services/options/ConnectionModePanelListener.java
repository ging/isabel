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
 * ConnectionModePanelListener.java
 *
 * Created on 15 de septiembre de 2003, 1:56
 */

package services.isabel.services.options;

/**
 *
 * @author  Fernando
 */
public interface ConnectionModePanelListener {
    
    /**
     * Este metodo se ejecuta cuando se selecciona el modo de conexion.
     * @param multi True si es multicast y false en otro caso.
     */
    public void multicastSelected(boolean multi);
    
    /**
     * Este m�todo se ejecuta cuando es pulsado el boton de configuracion
     * de los grupos multicast.
     */
    public void configButtonPressed();
    
}
