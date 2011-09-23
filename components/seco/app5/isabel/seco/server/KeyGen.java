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
 * KeyGen.java
 */

package isabel.seco.server;

import java.security.*;
import java.io.*;
import java.util.*;

/**
 * Clase a la que pasamos un identificador de cliente (no un identificador
 * numerico de los que asigna SeCo) y un tipo de permiso, rw para lectura-
 * escritura y ro para solo lectura y nos genera dos archivos, uno con la
 * clave publica y otro con la clave privada. Ademas inserta ese identificador
 * en el archivo de los clientes de solo lectura o de lectura-escritura segun
 * corresponda. El cliente tendra que disponer de la clave privada para
 * poder realizar la firma digital de cierta informacion de control, y SeCo
 * de la clave publica, para poder autenticar a sus clientes en base a esa
 * firma.
 * @author Javier Calahorra
 */
public class KeyGen {
    
    /** ruta donde se almacenaran los ficheros de usuarios y de claves */	
    public static String path;
    
    /** ruta del archivo que almacena clientes de solo lectura */
    public static final String RO="read_only.properties";
    
    /** ruta del archivo que almacena clientes de lectura-escritura */
    public static final String RW="read_and_write.properties";
    
    public static void main(String[] args){
	
	FileOutputStream pubKeyEncFile = null;
	FileOutputStream privKeyEncFile = null;
	
	if (System.getProperty("ruta")!=null)
	    path=System.getProperty("ruta");
	else
	    path="/tmp/";
	
	try{
	    
	    if (args.length!=2) {
		System.out.println("java -Druta=/tmp/ isabel.seco.server.KeyGen id ro/rw");
		return;
	    }
	    String id = args[0];
	    String algorithm = "DSA";
	    String permissions = args[1]; //ro o rw
	    
	    //generamos el par de claves para algortimo DSA
	    KeyPairGenerator keyGen = KeyPairGenerator.getInstance(algorithm);
	    SecureRandom random=SecureRandom.getInstance("SHA1PRNG","SUN");
	    keyGen.initialize(1024,random);
	    KeyPair pair=keyGen.generateKeyPair();
	    
	    //almacenamos la clave publica en su fichero
	    PublicKey pubKey=pair.getPublic();
	    byte[] pubKeyEnc=pubKey.getEncoded();
	    pubKeyEncFile=new FileOutputStream(path+id+"_"+algorithm+".pub");
	    pubKeyEncFile.write(pubKeyEnc);
	    
	    //almacenamos la clave privada en su fichero
	    PrivateKey privKey=pair.getPrivate();
	    byte[] privKeyEnc=privKey.getEncoded();
	    privKeyEncFile=new FileOutputStream(path+id+"_"+algorithm+".prv");
	    privKeyEncFile.write(privKeyEnc);
	    
	    //actualizamos el archivo de solo lectura cuando corresponda
	    if (permissions.equalsIgnoreCase("ro")) {
		if(!new File(path+RO).exists())
		    new Properties().store(new FileOutputStream(path+RO), "RECEIVE_ONLY");
		Properties ro=new Properties();
		ro.load(new FileInputStream(path+RO));
		ro.setProperty(id,path+id+"_"+algorithm+".pub");
		ro.store(new FileOutputStream(path+RO),"RECEIVE_ONLY");
	    }
	    //o actualizamos el de lectura-escritura
	    else if (permissions.equalsIgnoreCase("rw")) {
		if(!new File(path+RW).exists())
		    new Properties().store(new FileOutputStream(path+RW),"SEND_AND_RECEIVE");
		Properties rw=new Properties();
		rw.load(new FileInputStream(path+RW));
		rw.setProperty(id,path+id+"_"+algorithm+".pub");
		rw.store(new FileOutputStream(path+RW),"SEND_AND_RECEIVE");
	    }
	    
	} catch(NoSuchAlgorithmException e) {
	    e.printStackTrace();
	} catch(NoSuchProviderException e) {
	    e.printStackTrace();
	} catch(FileNotFoundException e) {
	    e.printStackTrace();
	} catch(IOException e) {
	    e.printStackTrace();
	} finally {
	    try {
		if (pubKeyEncFile != null) pubKeyEncFile.close();
		if (pubKeyEncFile != null) privKeyEncFile.close();
	    } catch(IOException e) {
		e.printStackTrace();
	    }
	}
    }
}
