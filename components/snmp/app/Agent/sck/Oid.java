// -----------------------Oid.java
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

/** Object Identifier (tag: 0x06).
 */ 
final public class Oid extends smi { 
  
  private int[] valeur; // les subId non codes.

  synchronized private void StringToTab(String val)throws InvalidObjectException{
    if ((val == null) || (val.equals("")))
      throw new InvalidObjectException("Can't initialize Oid with an empty or null String !");
    StringTokenizer tok = new StringTokenizer(val, ".");
    int nb = tok.countTokens();
    if (nb <2 )
      throw new InvalidObjectException("Need more than one subid !");
    valeur = new int[nb];
    for (int i =0; i< nb; i++)
      valeur[i] = java.lang.Integer.valueOf(tok.nextToken()).intValue();
  }

  /** Buils an Oid from a String.
   *  An Oid is made of sub-identifiers (integer) 
   *  s&eacute;par&eacute; par des '.'
   *  <P>Example: Oid sysDescr("1.3.6.1.2.1.1.0")
   *  @exception InvalidObjectException is thrown if an Oid l'Oid can't
   *  be build from the String.
   */
  public Oid(String v) throws InvalidObjectException {
    super(smi.OID_tag);
    this.StringToTab(v);
  }

  /** Returns true if this Oid belongs to the tree given in argument.
   *  <P>Example: oid = new Oid("1.3.6.1.2.1.1.1.0"); 
   *  <BR> oid.sameTree("1.3.6.1.2") returns true.
   */
  public boolean sameTree(String tree) {
    return this.toString().startsWith(tree);
  }

  /** Builds an Oid from a ByteArrayInputStream holding an oid Ber coding. 
   *  <BR>Bytes read are removed from the stream.
   *  <P><B>Note:</B> The ByteArrayInputStream must not contain the oid Tag.
   *  @exception IOException is thrown if a problem occurs while trying to decode the stream.
   */
  public Oid(ByteArrayInputStream inBer) throws IOException{
    super(smi.OID_tag);
    decodeBer(inBer);
  }

  /** Compares two Oid for equality.
   *  @return true if this oid is the same as the obj argument, false otherwise.
   */
  public boolean equals (Object obj) {
    if (this == obj) // egalite de pointeurs ?
      return true;
    if (!super.equals(obj)) // egalite de type ?
      return false;

    Oid oid2 = (Oid)obj;
    int lg = oid2.valeur.length;
    if (lg != this.valeur.length)
      return false;
    for (int i=--lg; i>=0 ; i--) // on commence la comparaison a l'envers
      if ( this.valeur[i] != oid2.valeur[i] )
        return false; 
    return true;
    }

  /** Returns the value of this Oid as a String.
   */  
  public String toString(){
    StringBuffer chaine = new StringBuffer();
    
    int lastOffset = this.valeur.length -1;
    for (int i=0; i < lastOffset; i++) 
      chaine.append(this.valeur[i]).append(".");
    chaine.append(this.valeur[lastOffset]);
    
    return (chaine.toString());
  }

	//------------------------------------------------------------------------
	// Modifications made to sck original distribution:
	//		getSubIdentifiers() --> Da acceso a los elementos del Oid.
	//		equalsThan (Oid) --> Comparacion entre Oids.
	//		greaterThan (Oid) --> Comparacion entre Oids.
	//		minorThan (Oid) --> Comparacion entre Oids.
	//------------------------------------------------------------------------
	/** Returns an array of int's containing the sub-identifiers of the Oid.
    */
  public int[] getSubIdentifiers() throws StringIndexOutOfBoundsException {
    return valeur;
  } 
 
	/** Javier: Returns true if this Oid equals the one passed as argument.
    */
  public boolean equalThan (Oid oid) {
    return this.equals(oid);
  }     

   /** Javier: Returns true if this Oid is lexicographycally minor than the one
    * passed as argument.
    */
  public boolean minorThan(Oid oid) {
    if (this.equalThan(oid))
      return false;
    else {
      int[] thisSubids = this.getSubIdentifiers();
      int[] oidSubids = oid.getSubIdentifiers();

      for(int i=0; (i<thisSubids.length)&(i<oidSubids.length); i++) {
        if(thisSubids[i]<oidSubids[i])
          return true;
        if(thisSubids[i]>oidSubids[i])
          return false;
      }

      return thisSubids.length < oidSubids.length ? true : false;
    }
  }     

/** Javier: Returns true if this Oid is lexicographycally greater than the one
  * passed as argument.
  */
  public boolean greaterThan(Oid oid) {
  if (this.equalThan(oid))
      return false;
    else {
      int[] thisSubids = this.getSubIdentifiers();
      int[] oidSubids = oid.getSubIdentifiers();

      for(int i=0; (i<thisSubids.length)&(i<oidSubids.length); i++) {
        if(thisSubids[i]>oidSubids[i])
          return true;
        if(thisSubids[i]<oidSubids[i])
          return false;
      }

      return thisSubids.length < oidSubids.length ? false : true;
    }
  }
   //-----------------------------------------------------------------------
	// Fin de las modificaciones a sck
	//-----------------------------------------------------------------------
 
	/** inner class (used by codeValeur)
   */
  private class reverseBuf { 
    private int offset;
    private byte[] buf;

    reverseBuf(){
      offset = 31; // pointer on the first empty cell
      buf = new byte[32];
    }
      
    // returns a bytes array which fit exactly to size.
    public byte[] getBytes(){
      int newLg = buf.length - offset -1 ;
      byte[] newBuf = new byte[ newLg ];
      System.arraycopy(buf, offset +1 , newBuf, 0, newLg);
      return newBuf;
    }

    public void add(byte b){
      if (offset == -1){ // increases buffer space
        byte[] newBuf = new byte[ buf.length + 32];
        System.arraycopy(buf, 0, newBuf, 32, buf.length);
        buf = newBuf;
        offset = 31;
      }
      buf[offset--] = b;
    }

    public void rewind() {
      offset++;
    }

  } // end of reverseBuf

  /** Used by codeValeur().
   */
  private void codeSubId(int subid, reverseBuf tampon){
    byte b = (byte) (subid & 0x7f) ;
    tampon.add(b);
    while ((subid >>>=7) != 0){
      b = (byte) ( (subid & 0x7f) | 0x80); 
	tampon.add(b);
    }
  }

  /** Returns Ber coding of this Oid.
   *  Used by smi.CodeBer().
   *
   *  ASN.1 grammar for an Oid coding is:
   *  objid ::= 0x06 asnlength sub-identifier {sub-identifier}*
   *  sub-identifier ::= {leadingbyte}* lastbyte
   *  leadingbyte ::= 1 7bitvalue
   *  lastbyte ::= 0 7bitvalue
   *  Each subId is coded by packet of 7 bits (high bit is set to 1 to indicate that this byte
   *  is not the end of the subId). The last Byte of a subId coding has the last bit set to 0.
   *  Be carefull, a pre-processing must be done on the one and second subId:
   *  new subId1 = ( subId1 * 40) + subId2.
   */
  synchronized byte[] codeValeur(){      
    reverseBuf t = new reverseBuf();

    // starts from the end
    for (int i=this.valeur.length -1 ; i>=2; i--){ // dont process subid 1 & 2
      codeSubId(this.valeur[i],t);
    }
    codeSubId( (valeur[0] * 40) + valeur[1], t);
    return t.getBytes();
  }

  /** Used smi.decodeBer().
   */
  synchronized void decodeValeur(ByteArrayInputStream bufferBer, int lgBerValeur) throws IOException{
    Vector list = new Vector();
    int subId, val,lastBit;

    while (lgBerValeur > 0){
      subId =0;
      do {
        val = bufferBer.read() ; // lis byte (0-255)
        if ( val == -1) 
          throw new IOException(" error decoding: end of stream reached !");
        lastBit = val & 0x80;
        subId = ( subId << 7) |  ( val & 0x7f); 
        lgBerValeur --;
      } while ( lastBit !=0 ); // on s'arrete si 8eme bit = 0: fin du decodage du subId
      list.addElement( new java.lang.Integer(subId) );
    }
    
    int nbSubId = list.size() + 1; // cf cas particulier
    this.valeur = new int[nbSubId];

    // on traite le cas particulier du 1er subId ...
    subId = ((java.lang.Integer)list.elementAt(0)).intValue();
    valeur[1] = subId % 40;
    valeur[0] = (subId - valeur[1]) / 40;

    // ... ensuite c'est tout simple !
    int fin = nbSubId - 1;
    for (int i=2; i <= fin; i++)
      valeur[i] = ((java.lang.Integer)list.elementAt(i-1)).intValue();
  }

}
