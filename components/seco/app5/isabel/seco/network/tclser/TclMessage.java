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
 * TclMessage.java
 */

package isabel.seco.network.tclser;


import isabel.seco.network.Message;

/**
 * Clase que representan los mensajes que la version TCL de isabel envia usando SeCo.
 *
 * Los mensajes enviados son Strings que siguen el siguiente formato:
 *    rdo $cmd
 *
 * Nota: no se soportan los mensajes de tipo RPC ni RCB.
 *
 * @author  Santiago Pavon
 * @version 1.0
 */

public class TclMessage implements Message {

    /**
     *  El mensaje enviado.
     */
    public String cmd;


    /**
     *  Constructor
     */
    public TclMessage(String cmd) {

	this.cmd    = cmd;
    }


    /**
     * Representacion com un String
     */
    public String toString() {

	return "<rdo " + cmd + ">";
    }
    
    /**
     * Equals
     */
    @Override
    public boolean equals(Object msg) {
    	System.out.println("=======llamando a tclMsg.equals");
    	return cmd.equals(((TclMessage)msg).cmd);
    }
    

}
