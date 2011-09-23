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
 * Marshaller.java
 */


package isabel.seco.network;

import java.io.IOException;

/**
 *  Los objetos que implementen este interface son los encargados de 
 *  convertir en array de bytes, e inversa,  los mensajes usados por
 *  las aplicaciones.
 *
 *  Para cada tipo de mensaje sera necesario un Marshaller especifico.
 */

public interface Marshaller {
    
    
    /**
     *  Construye un objeto Message a partir de un array de bytes.
     */
    public Message unmarshall(byte[] msg) throws IOException;
    
    
    
    /**
     *  Serializa un mensaje Message en un array de bytes.
     */
    public byte[] marshall(Message msg) throws IOException;
    
}
