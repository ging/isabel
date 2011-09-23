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
 * TaskListener.java
 *
 * Created on 7 de diciembre de 2001, 16:41
 */

package isabel.lib.tasks;

/**
 * Interfaz que deben implementar las clases que quieran recibir eventos relativos
 * a la muerte de los procesos externos.
 * @author  Fernando Escribano
 * @version 1.0
 */
public interface TaskListener {
    
    /** Este método es invocado cuando se produce la muerte de un proceso.
     */
    public void deadTask();
}
