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
package xedl.lib.xedl;

import java.io.PrintWriter;

import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;

import services.isabel.lib.Constants;

/**
 * class to create a singleton of JAXB instances for marshaller and unmarshaller, so you just create just
 * one marshaller and one unmarshaller for every run of the service (connect to, start session server ...)
 * @author enrique
 *
 */
public class JAXBSingleton {
	private static JAXBContext jc = null;
	private static Unmarshaller unmarshaller = null;
	private static Marshaller marshaller = null;
	private static PrintWriter outTraceFile= null;
	
	public static Marshaller getMarshaller(){
		outTraceFile = Constants.inicializa_trazas_jaxb();
		if(marshaller==null){
			outTraceFile.println("JAXBSingleton: marshaller era null");
			try {
				if(jc==null)
				{
					outTraceFile.println("JAXBSingleton: jc era null");
					jc = JAXBContext.newInstance(new Class[] {xedl.lib.jaxb.Edl.class});
					outTraceFile.println("JAXBSingleton: jc creado");
				}
				marshaller = jc.createMarshaller();
				outTraceFile.println("JAXBSingleton: marshaller creado");
			} catch (JAXBException e) {
				e.printStackTrace();
			}
		}
		return marshaller;
	}
	
	
	public static Unmarshaller getUnmarshaller(){
		outTraceFile = Constants.inicializa_trazas_jaxb();
		if(unmarshaller==null){
			outTraceFile.println("JAXBSingleton: unmarshaller era null");
			try {
				if(jc==null)
				{
					outTraceFile.println("JAXBSingleton: jc era null");
					jc = JAXBContext.newInstance(new Class[] {xedl.lib.jaxb.Edl.class});
					outTraceFile.println("JAXBSingleton: jc creado");
				}
				unmarshaller = jc.createUnmarshaller();
				outTraceFile.println("JAXBSingleton: unmarshaller creado");
			} catch (JAXBException e) {
				e.printStackTrace();
			}
		}
		return unmarshaller;
	}
	
}
