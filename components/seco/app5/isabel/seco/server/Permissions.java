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
 * Permissions.java
 *
 */

package isabel.seco.server;

import java.io.*;
import java.util.*;
import java.util.logging.*;
import java.security.*;
import java.security.spec.*;

/**
 *  La clase Permissions es exclusiva del nodo SeCo raiz, y se encarga de la autenticacion
 *  de clientes mediante claves y fichero y la asignacion de identificadores unicos
 *  de grupo.
 *
 * @author Javier Calahorra
 * @author Santiago Pavon
 */
public class Permissions {
    
    /** 
     *  Logger where logs are written 
     */
    private Logger mylogger;
    
    /** 
     *  Constante que indica los permisos del cliente al que atiende: en proceso 
     */
    static final byte IN_PROCESS=0;
    
    /** 
     *  Constante que indica los permisos del cliente al que atiende: ninguno 
     */
    static final byte NONE=1;
    
    /** 
     *  Constante que indica los permisos del cliente al que atiende: solo lectura 
     */
    static final byte RECEIVE_ONLY=2;
    
    /** 
     *  Constante que indica los permisos del cliente al que atiende: lectura y escritura 
     */
    static final byte SEND_AND_RECEIVE=3;
    
    /** 
     *  Fichero con los clientes con permisos de solo lectura 
     */
    private static final String RO="read_only.properties";
    
    /** 
     *  Fichero con los clientes con permisos de lectura-escritura 
     */
    private static final String RW="read_and_write.properties";
    
    /**
     *  Contruye el objeto en cargado de authenticar a los clientes.
     */
    public Permissions(){

	mylogger=Logger.getLogger("isabel.seco.server.Permissions");
	mylogger.finer("Creating new Permissions");
    }
    
    /**
     *  Calcula los permisos 
     *  @param data identificador para acceder al fichero de claves
     *  @return tipo de permiso obtenido
     *  @throws IOException algo fue mal.
     */
    byte getPermissions(byte[] data) throws IOException {

	ByteArrayInputStream bais=new ByteArrayInputStream(data);
	String info=new String(data);
	mylogger.finest("Login data= "+info);
	mylogger.finest("Login data length= "+data.length);

	StringTokenizer st=new StringTokenizer(info,"@");
	String id=st.nextToken();
	mylogger.finest("Login id= "+id);
	byte[] aux=new byte[id.length()+1];
	mylogger.finest(aux.length+"");
	bais.read(aux,0,aux.length);
	byte[] text=new byte[bais.available()];
	mylogger.finest(text.length+"");
	bais.read(text,0,text.length);
	mylogger.finest("Login text= "+new String(text));
	mylogger.finest("Login text length= "+text.length);
	
	byte permData = analysePermissions(id,text);
	mylogger.finest("Final permissions= "+permData);
	return permData;
	
    }
    
    /**
     *  Devuelve un permiso para cliente en funcion de su identificador (si estÃ¡
     *  presente en read_only.properties, read_and_write.properties o en ninguno)
     *  y de si se verifica la firma digital de la palabra clave o no.
     *  @param id identificador de cliente (no identificador Ãºnico numerico).
     *  @param text firma digital de la palabra clave ISABEL con la clave 
     *  privada de cliente.
     *  @return permisos que se van a asignar al cliente.
     */
    private byte analysePermissions(String id,byte[] text){

	// Ruta donde se encuentran los ficheros de usuarios y claves
	String path = "/tmp/";
	if (System.getProperty("ruta")!=null)
	    path=System.getProperty("ruta");
    
	try {
	    //si no tenemos ningÃºn fichero todo el mundo lectura-escritura
	    if (!new File(path+RO).exists() && !new File(path+RW).exists()){
		mylogger.finest("read_only.properties & read_and_write.properties don't exist");
		return SEND_AND_RECEIVE; //no hay que verificar firma
	    }
	    //si tenemos solo el de lectura-escritura los clientes que no esten
	    //se conectarÃ¡n como solo lectura
	    else if(!new File(path+RO).exists() && new File(path+RW).exists()){
		Properties rw=new Properties();
		rw.load(new FileInputStream(path+RW));
		if(rw.containsKey(id)){
		    mylogger.finest("Client exists in read_and_write.properties");
		    if(verifyText(rw.getProperty(id),text)){
			mylogger.finest("Text verified");
			return SEND_AND_RECEIVE;
		    }
		    else{
			mylogger.finest("Text not verified");
			return NONE;
		    }
		}
		else{
		    mylogger.finest("Client doesn't exist anywhere");
		    return RECEIVE_ONLY;
		}
	    }
	    //si tenemos Ãºnicamente el de solo lectura es un error, nadie
	    //tendrÃ¡ permisos
	    else if(new File(path+RO).exists() && !new File(path+RW).exists()){
		mylogger.severe("Error with .properties archives");
		return NONE;
	    }
	    //si tenemos los dos ficheros los clientes serÃ¡n de lectura-escritura,
	    //de solo lectura o no podrÃ¡n conectarse
	    else if (new File(path+RO).exists() && new File(path+RW).exists()) {
		Properties rw = new Properties();
		rw.load(new FileInputStream(path+RW));
		if(rw.containsKey(id)){
		    mylogger.finest("Client exists in read_and_write.properties");
		    if(verifyText(rw.getProperty(id),text)){
			mylogger.finest("Text verified");
			return SEND_AND_RECEIVE;
		    }
		    else{
			mylogger.finest("Text not verified");
			return NONE;
		    }
		}
		else{
		    Properties ro=new Properties();
		    ro.load(new FileInputStream(path+RO));
		    if(ro.containsKey(id)){
			mylogger.finest("Client exists in read_only.properties");
			if (verifyText(ro.getProperty(id), text)){
			    mylogger.finest("Text verified");
			    return RECEIVE_ONLY;
			}
			else{
			    mylogger.finest("Text not verified");
			    return NONE;
			}
		    }
		    else{
			mylogger.warning("Client doesn't exist anywhere");
			return NONE; //no hay que verificar firma
		    }
		}
	    }
	}catch(FileNotFoundException e){
	    e.printStackTrace();
	}catch(IOException e){
	    e.printStackTrace();
	}
	return NONE;
    }
    
    /**
     *  Metodo al que le pasamos como parÃ¡metros el fichero con la clave
     *  pÃºblica correspondiente a un cliente dado y la firma digital del
     *  texto ISABEL, y en funcion de su verificacion se autenticarÃ¡ al
     *  cliente o no en el servicio.
     *  @param keyFile ruta al fichero que contiene la clave pÃºblica 
     *  almacenada en su codificacion primaria.
     *  @param text firma digital hecha por el cliente al texto ISABEL.
     *  @return true si se verifica la firma digital, false en caso
     *  contrario.
     */
    private boolean verifyText(String keyFile,byte[] text){
	FileInputStream in=null;
	try{
	    
	    in=new FileInputStream(keyFile);
	    byte[] pubKeyEnc=new byte[in.available()];
	    in.read(pubKeyEnc);
	    
	    X509EncodedKeySpec pubKeySpec = new X509EncodedKeySpec(pubKeyEnc);
	    KeyFactory keyFactory = KeyFactory.getInstance("DSA");
	    PublicKey pubKey=keyFactory.generatePublic(pubKeySpec);
	    
	    Signature dsa=Signature.getInstance("SHA1withDSA");
	    dsa.initVerify(pubKey);
	    dsa.update("ISABEL".getBytes());
	    mylogger.finest("Signature initalized with ISABEL");
	    mylogger.finest("keyFile= "+keyFile);
	    mylogger.finest("text= "+new String(text));
	    mylogger.finest("text length= "+text.length);
	    return dsa.verify(text);
	    
	}catch(NoSuchAlgorithmException e){
	    e.printStackTrace();
	}catch(InvalidKeySpecException e){
	    e.printStackTrace();
	}catch(InvalidKeyException e){
	    e.printStackTrace();
	}catch(SignatureException e){
	    e.printStackTrace();
	}catch(FileNotFoundException e){
	    e.printStackTrace();
	}catch(IOException e){
	    e.printStackTrace();
	}finally{
	    try{
		if (in != null) in.close();
	    }catch(IOException e){
		e.printStackTrace();
	    }
	}
	return false;
    }
    
}
