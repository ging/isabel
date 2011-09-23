// -----------------------uint32.java
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

/** Implements methods for coding and decoding unsigned integer (32 bits)
 * <P> Can't be instantiated directly (constructors protected)
 */

public class uint32 extends smi{

  protected long valeur  ; 

  /** Builds an uint32 from a long.
   */
  protected uint32(byte tag, long valeur){
    super(tag);
    this.valeur = valeur;
  }

  /** Builds an uint32 from a ByteArrayInputStream holding a generic uint32 Ber coding. 
   *  <BR>Bytes read are removed from the stream.
   *  <P><B>Note:</B> The ByteArrayInputStream must not contain the Smi object Tag.
   *  @exception IOException is thrown if a problem occurs while trying to decode the stream.
   */
  protected uint32(byte tag, ByteArrayInputStream inBer) throws IOException{
    super(tag);
    decodeBer(inBer);
  }

  /** Builds a uint32 from a String. 
   *  <BR>If v == null then uint32(0).
   *  @exception InvalidObjectException is thrown if a generic uint32
   *  can't be build from the String.
   *  @see java.lang.Long#valueOf(java.lang.String)
   */
  protected uint32(byte tag, String v) throws InvalidObjectException{
    super(tag);
    try{
      if ( v == null)
        valeur = 0;
      else
        valeur = java.lang.Long.valueOf(v).longValue();
    } catch (Exception e) {
      throw new InvalidObjectException("Long.valueOf( " + v + ") : illegal !");
    }
  }

  /** Compares two uint32 for equality.
   *  @return true if this uint32 is the same as the obj argument, false otherwise.
   */
  public boolean equals (Object obj) {
    if (this == obj)
      return true;
    if (!super.equals(obj))
      return false;
    if (this.valeur == ((uint32)obj).valeur)
      return true;
    return false;
    }
	
  /** Returns the value of this Integer as a String.
   */    
  public String toString(){
    return (String.valueOf(this.valeur));
  }

  /** Returns Ber coding of this uint32.
   *  Used by smi.CodeBer().
   */
  byte[] codeValeur(){
    long entier = this.valeur;
    int taille = 1; 

    //calcul taille de l'entier (sans les 0x00 inutiles)
    while (( entier >>>=8) > 0){
	taille++;
    }
    
    entier = this.valeur;
    byte b[] = new byte[taille];
    int offset = --taille;
    for (int i=offset; i>=0; i--){
      b[i] = (byte) entier ;
      entier >>>=8; // decalage non signe
    }
  return (b);
  }
  
  /** Used  smi.decodeBer().
   */
  void decodeValeur(ByteArrayInputStream bufferBer, int lgBerValeur) throws IOException{
    if (lgBerValeur > 4) 
      throw new IOException(" length >4 ! ");
    valeur = 0x00;
    do {
      valeur = (valeur << 8) | bufferBer.read();
    }while (--lgBerValeur > 0);
  }
}