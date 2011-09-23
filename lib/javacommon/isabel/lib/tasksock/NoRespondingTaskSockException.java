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
 * NoRespondingTaskSockException.java
 *
 * Created on November 5, 2003.
 */

package isabel.lib.tasksock;


/**
 *  This class define a new exception type to use when a TaskSock
 *  doesn't respond to a RDO, RPC or RCB primitive.
 *  A TaskSock can not respond because, the task is dead, or because
 *  the quit() method was invoked.
 *  @author Santiago Pavon
 *  @version 1.0
 */
public class NoRespondingTaskSockException extends Exception {
   
	private static final long serialVersionUID = -7760984704052244422L;

	/** Construct the exception object.
     *  @param msg Information about the exception.
     */ 
    public NoRespondingTaskSockException(String msg) {
	super(msg);
    }
}
