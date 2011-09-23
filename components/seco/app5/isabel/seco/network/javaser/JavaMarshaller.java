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
 * JavaMarshaller.java
 */

package isabel.seco.network.javaser;

import java.io.*; 

import isabel.seco.network.Message;
import isabel.seco.network.Marshaller;


/**
 *  Serializa objetos de tipo JavaMessage.
 *
 *  @author  Santiago Pavon
 *  @version 1.0
 */

public class JavaMarshaller implements Marshaller {
    
    
    /**
     *  Construye un objeto JavaMessage a partir de un array de bytes.
     */
    public JavaMessage unmarshall(byte[] msg) throws IOException {
	
	try {
	    return (JavaMessage)new ObjectInputStream(new ByteArrayInputStream(msg)).readObject();
	}
	catch (ClassNotFoundException cnfe) {
	    throw new IOException(cnfe.getMessage() + "::(ClassNotFoundException)::");
	}
}
    
    
    
    /**
     *  Serializa un mensaje JavaMessage en un array de bytes.
     */
    public byte[] marshall(Message msg) throws IOException {

        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        ObjectOutputStream oos = new ObjectOutputStream(baos);
        oos.writeObject((JavaMessage)msg);
        return baos.toByteArray();
    }
    
}

