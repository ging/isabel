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
 * Queue.java
 *
 * Created on 10 de julio de 2001
 */

package isabel.lib;

import java.util.LinkedList;

/**
 * Esta clase implementa una cola FIFO (fisrt in first out) con exclusion
 * mutua y contiene los métodos habituales.
 * Esta versión utiliza para la implementación de la cola una LinkedList.
 * @author  Fernando Escribano
 * @version 1.0
 */
public class Queue {
    private LinkedList<Object> list;

    public Queue() {
        list = new LinkedList<Object>();
    }

   /** Con este método se introduce un objeto en la cola
    * @param v El objeto que deseamos introducir en la cola.
    */
    public synchronized void put(Object v) {
        list.addFirst(v);
    }
    
    /** Con este método se saca de la cola un objeto segun lña política FIFO.
     * Tiene como valor de retorno dicho objeto.
     */
    public synchronized Object get() { 
        return list.removeLast(); 
    }
    
    /** Método para comprobar si la cola está vacía.
     */
    public synchronized boolean isEmpty() { 
        return list.isEmpty(); 
    }
    
    // Para probar el funcionamiento.
    public static void main(String[] args) {
        Queue queue = new Queue();
        for(int i = 0; i < 10; i++)
            queue.put(Integer.toString(i));
        while(!queue.isEmpty())
            System.out.println(queue.get());
    }
}
