// -----------------------IpAddress.java
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
import java.util.*;


/** IpAddress(tag: 0x40)
 *  <P>The Ber coding of an IpAddress is the same than an OctetString.
 */
public class IpAddress extends smi { 
  
  private byte[] valeur ; // internal coding: 4 bytes.

  synchronized private void StringToTab(String val)throws InvalidObjectException{
    if ((val == null) || (val.equals("")))
      throw new InvalidObjectException("Cant initialize IpAddress with an empty or null String !");
    StringTokenizer tok = new StringTokenizer(val, ".");
    int nb = tok.countTokens();
    if (nb !=4 )
      throw new InvalidObjectException("String format is not of the form xx.xx.xx.xx!");
    valeur = new byte[4];
    for (int i =0; i< 4; i++) // manque teste byte <= 255
      valeur[i] = (byte)java.lang.Integer.valueOf(tok.nextToken()).intValue(); // byte <128: we can't read directly
  }

  /** Builds an IpAddress from a String (xx.xx.xx.xx).
   */
  public IpAddress(String s) throws InvalidObjectException{
    super(smi.IPADDRESS_tag);
    this.StringToTab(s);
  }

  /** Builds an IpAddress from a ByteArrayInputStream holding an IpAddress Ber coding. 
   *  <BR>Bytes read are removed from the stream.
   *  <P><B>Note:</B> The ByteArrayInputStream must not contain the IpAddress Tag.
   *  @exception IOException is thrown if a problem occurs while trying to decode the stream.
   */
  public IpAddress(ByteArrayInputStream inBer) throws IOException{
    super(smi.IPADDRESS_tag);
    decodeBer(inBer);
  }

  /** Compares two IpAddresses for equality.
   *  @return true if this Address is the same as the obj argument, false otherwise.
   */
  public boolean equals (Object obj) {
    if (this == obj)
      return true;
    if (!super.equals(obj))
      return false;
    IpAddress ip2 = (IpAddress)obj;
    for (int i=3; i>=0 ; i--) // on commence la comparaison a l'envers
      if ( this.valeur[i] != ip2.valeur[i] )
        return false; 

    return false;
    }

  /** Returns the value of this IpAddress as String.
   */  
  public String toString(){
    StringBuffer chaine = new StringBuffer();
    
    for (int i=0; i < 3; i++) 
      chaine.append(0xff & this.valeur[i]).append("."); // byte est signe => cast en int sans signe
    chaine.append(0xff & this.valeur[3]);
    
    return (chaine.toString());
  }

  /** Returns Ber coding of this Ipaddress.
   *  Used by smi.CodeBer().
   */
  byte[] codeValeur(){
    byte b[] = new byte[4];
    System.arraycopy(valeur,0,b,0,4); // byte [] n'est pas immutable: on clone !
    return b;
  }
  
  /** Used by smi.decodeBer().
   */
  void decodeValeur(ByteArrayInputStream bufferBer, int lgBerValeur) throws IOException{
    int oct;
    if (lgBerValeur != 4) 
      throw new IOException("Error decoding IpAddress: length != 4");
    valeur = new byte[4];
    for (int i=0; i<4; i++){
      oct = bufferBer.read(); 
      if ( oct == -1) 
          throw new IOException("Error decoding  IpAddress: end of stream !");
      valeur[i] = (byte) oct; 
    }
  }
}
