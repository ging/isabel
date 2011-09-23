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
package services.isabel.lib;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.security.spec.AlgorithmParameterSpec;
import java.security.spec.KeySpec;

import javax.crypto.*;
import javax.crypto.spec.PBEKeySpec;
import javax.crypto.spec.PBEParameterSpec;


public class DesEncrypter {
	
	private String passPhrase = null;

	
    public DesEncrypter(String passPhrase) {
    	this.passPhrase = passPhrase;
    }

    public String encrypt(String str) {
    	String command = "openssl enc -des -e -a -A -pass pass:" + passPhrase ;
    	Process child; 
    	try{
    		child = Runtime.getRuntime().exec(command);
    		OutputStream out = child.getOutputStream();
           	out.write(str.getBytes());
        	out.close();
        	//Get the input stream and read from it
        	InputStream in = child.getInputStream();
        	int c;
        	String passwd = "";
        	while ((c = in.read()) != -1) {
        		passwd += (char)c;
        	}
        	in.close();
        	return passwd;
    	}
        catch(IOException e)
        {
        	e.printStackTrace();
        	return null;
        }
    }

    public String decrypt(String str) {
    	String command = "openssl enc -des -d -a -A -pass pass:" + passPhrase ;
    	Process child; 
    	try{
    		child = Runtime.getRuntime().exec(command);
    		OutputStream out = child.getOutputStream();
           	out.write(str.getBytes());
        	out.close();
        	//Get the input stream and read from it
        	InputStream in = child.getInputStream();
        	int c;
        	String passwd = "";
        	while ((c = in.read()) != -1) {
        		passwd += (char)c;
        	}
        	in.close();
        	return passwd;
    	}
        catch(IOException e)
        {
        	e.printStackTrace();
        	return null;
        }
    }
    
    public static void main (String args[])
    {

//    	 Here is an example that uses the class
    	try {
    	    // Create encrypter/decrypter class
    	    DesEncrypter encrypter = new DesEncrypter("MyPassPhrase!");

    	    // Encrypt
    	    String encrypted = encrypter.encrypt("Don't tell anybody!");
    	    System.out.println("encriptado: " + encrypted);
    	    // Decrypt
    	    String decrypted = encrypter.decrypt(encrypted);
    	    System.out.println("desencriptado: " + decrypted);
    	} catch (Exception e) {
    	}
    }
}



