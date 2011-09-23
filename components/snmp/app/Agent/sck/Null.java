// -----------------------null
// Copyright (C) 1998  Yves Soun.

// This program is free software; you can redistribute it and/or
// modify it under the terms of the Affero GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details (cf. file COPYING).

package sck;

import java.io.*;

/** Null. <P>
 *  Ber coding: 0x05 0x00
 */
public final class Null extends smi { 

  static private String String_Null = new String("NULL");

  public Null(){
    super(smi.NULL_tag);
  }

  /** Builds a Null from a ByteArrayInputStream holding a Null Ber coding. 
   *  <BR>Bytes read are removed from the stream.
   *  <P><B>Note:</B> The ByteArrayInputStream must not contain the Integer Tag.
   *  @exception IOException is thrown if a problem occurs while trying to decode the stream.
   */
  public Null(ByteArrayInputStream inBer) throws IOException{
    super(smi.NULL_tag);
    decodeBer(inBer);
  }

  /** Builds a Null from a String. 
   *  @param v can be anything.
   */
  public Null(String v){
    super(smi.NULL_tag);
  }

  /** Returns "NULL".
   */  
  public  String toString(){
    return String_Null;
  }

  /** Null (coding 0x05 0x00) is the only Smi Object overloading this method.
   */
  public byte[] codeBer() {
    byte[] _code = new byte[2];
    _code[0] = smi.NULL_tag;
    _code[1] = 0x0;
    return _code;
  }

  /** Returns null.
   *  Used par smi.CodeBer().
   */
  byte[] codeValeur(){
    return null ;
  }
	
  /** Used by smi.decodeBer().
   * NOP
   */
  void decodeValeur(ByteArrayInputStream bufferBer, int lgBerValeur) throws IOException{
    if ( lgBerValeur != 0) 
      throw new IOException("read " + java.lang.Integer.toHexString(lgBerValeur) + " instead of 0x00");
  }

}
