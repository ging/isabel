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
 * TclMarshaller.java
 */

package isabel.seco.network.tclser;

import java.io.IOException;

import java.util.regex.*;

import isabel.seco.network.Message;
import isabel.seco.network.Marshaller;


/**
 *  Serializa objetos de tipo TclMessage.
 *
 *  @author  Santiago Pavon
 *  @version 1.0
 */

public class TclMarshaller implements Marshaller {
    
    
    /**
     *  Construye un objeto TclMessage a partir de un array de bytes.
     */
    public TclMessage unmarshall(byte[] msg) throws IOException {
	
	String aux = new String(msg,"ISO-8859-1");

	Pattern p = Pattern.compile("rdo\\s*(.*)\\z",
				    Pattern.DOTALL);
	Matcher m = p.matcher(aux);
	if (m.matches()) {
	    
	    String cmd    = m.group(1);
	    
	    return new TclMessage(cmd);
	} else {
	    throw new IOException("Invalid msg format: \""+aux+"\".");
	}
    }
    
    
    
    /**
     *  Serializa un mensaje TclMessage en un array de bytes.
     */
    public byte[] marshall(Message msg) throws IOException {

	TclMessage tm = (TclMessage) msg;

	return ("rdo " + tm.cmd).getBytes("ISO-8859-1");
    }
    
}

