
// -----------------------Oid.java

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
public class Oid extends smi { 
  
  private int[] valor; // les subId non codes.

  synchronized void StringToTab(String val)throws InvalidObjectException{
    if ((val == null) || (val.equals("")))
      throw new InvalidObjectException("Can't initialize Oid with an empty or null String !");
    StringTokenizer tok = new StringTokenizer(val, ".");
    int nb = tok.countTokens();
    if (nb <2 )
      throw new InvalidObjectException("Need more than one subid !");
    valor = new int[nb];
    for (int i =0; i< nb; i++)
      valor[i] = java.lang.Integer.valueOf(tok.nextToken()).intValue();      
  }

  /** Basic constructor. Used only by Oid's subclass, OidPrefix.
    */
  public Oid() {
    super(smi.OID_tag);
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

  /** Returns an array of Strings containing the sub-identifiers of the Oid.
    */
  public String[] getStringSubIdentifiers() throws StringIndexOutOfBoundsException {   
    ArrayList valoresString = new ArrayList();
    for (int i=0; i<valor.length; i++)
      valoresString.add(Integer.toString(valor[i]));
    return (String[])valoresString.toArray(new String[0]);  
  }
  
  /** Returns an array of int's containing the sub-identifiers of the Oid.
    */
  public int[] getSubIdentifiers() throws StringIndexOutOfBoundsException {        
    return valor;  
  }

  /** Returns true if this Oid equals the one passed as argument.
    */
  public boolean equalThan (Oid oid) {
    return this.equals(oid);  
  }  
  
  /** Returns true if this Oid is lexicographycally minor than the one 
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

/** Returns true if this Oid is lexicographycally greater than the one 
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

  /** Returns true if this Oid is lexicographycally minor or equal than the  
    * one passed as argument.
    */
  public boolean minorOrEqualThan(Oid oid) {
   return ! this.greaterThan(oid);   
  }
  
  /** Returns true if this Oid is lexicographycally greater or equal than the  
    * one passed as argument.
    */
  public boolean greaterOrEqualThan(Oid oid) {
   return ! this.minorThan(oid);     
  }
  
  /** Returns true if this Oid belongs to the tree given in argument.
   *  <P>Example: oid = new Oid("1.3.6.1.2.1.1.1.0"); 
   *  <BR> oid.sameTree("1.3.6.1.2") returns true.
   */
  public boolean sameTree(String tree) {
    return this.toString().startsWith(tree);
  }

  /** Sorts a vector of Oid's using the bubbleSort algorithm. 
    */
  public static Vector sort(Vector items) {  
    try {
    
      for (int i=0; i<items.size(); i++) {
        for (int j=0; j<items.size()-1; j++) {
          Oid currentItem = (Oid) items.get(j);
          Oid nextItem = (Oid) items.get(j+1);
          
          if (nextItem.minorThan(currentItem)) {
            // Interchange currentItem and nextItem
            items.setElementAt(nextItem, j);
            items.setElementAt(currentItem, j+1);
          }

        }
      }
      
      return items;
    
    } catch(Exception e) {
      System.out.println(e.toString());
      System.exit(0);
      return null;
    }
  }   
  
  /** Sorts an enumeration of Strings representing Oid's
    * using the bubbleSort algorithm. 
    */
  public static Vector sort(Enumeration items) throws java.io.InvalidObjectException {    
    Vector vector = new Vector();
    
    while (items.hasMoreElements()) {   
      String name = new String((String) items.nextElement());    
      vector.add(new Oid(name));     
    } 
    return Oid.sort(vector);    
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
    if (this == obj) // ¿igualdad de punteros?
      return true;
    if (!super.equals(obj)) // ¿igualdad de tipos?
      return false;

    Oid oid2 = (Oid)obj;
    int lg = oid2.valor.length;
    if (lg != this.valor.length)
      return false;
    for (int i=--lg; i>=0 ; i--) // comienza la comparacion
      if ( this.valor[i] != oid2.valor[i] )
        return false; 
    return true;
    }

  /** Returns the value of this Oid as a String.
   */  
  public String toString(){
    StringBuffer chaine = new StringBuffer();
    
    int lastOffset = this.valor.length -1;
    for (int i=0; i < lastOffset; i++) 
      chaine.append(this.valor[i]).append(".");
    chaine.append(this.valor[lastOffset]);
    
    return (chaine.toString());
  }

  /** inner class (used by codeValor)
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

  /** Used by codeValor().
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
  synchronized byte[] codeValor(){      
    reverseBuf t = new reverseBuf();

    // starts from the end
    for (int i=this.valor.length -1 ; i>=2; i--){ // dont process subid 1 & 2
      codeSubId(this.valor[i],t);
    }
    codeSubId( (valor[0] * 40) + valor[1], t);
    return t.getBytes();
  }

  /** Used smi.decodeBer().
   */
  synchronized void decodeValor(ByteArrayInputStream bufferBer, int lgBerValor) throws IOException{
    Vector list = new Vector();
    int subId, val,lastBit;

    while (lgBerValor > 0){
      subId =0;
      do {
        val = bufferBer.read() ; // lee bytes (0-255)
        if ( val == -1) 
          throw new IOException(" error decoding: end of stream reached !");
        lastBit = val & 0x80;
        subId = ( subId << 7) |  ( val & 0x7f); 
        lgBerValor --;
      } while ( lastBit !=0 ); // si octavo bit = 0: fin de decodificacion de subId
      list.addElement( new java.lang.Integer(subId) );
    }
    
    int nbSubId = list.size() + 1; // caso particular
    this.valor = new int[nbSubId];

    // trata el caso particular  ...
    subId = ((java.lang.Integer)list.elementAt(0)).intValue();
    valor[1] = subId % 40;
    valor[0] = (subId - valor[1]) / 40;

   
    int fin = nbSubId - 1;
    for (int i=2; i <= fin; i++)
      valor[i] = ((java.lang.Integer)list.elementAt(i-1)).intValue();
  }

}
