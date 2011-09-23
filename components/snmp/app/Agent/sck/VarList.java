// -----------------------VarList.java
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
import java.util.Vector;

/** ASN.1 grammar for VarList:
 * VarList ::=
 * SEQUENCE OF Var
 */
final public class VarList extends construct { 
	
  /** Constructs a VarList holding a single Var.
   *  @param Var to be holded Varlist
   */
   public VarList(Var v){
     super(smi.SEQUENCE_tag);
     valeur = new Vector(1);
     valeur.addElement(v);
   }

  /** Constructs a VarList holding Varlists.
   *  @param v Vector of Vars.
   */
   public VarList(Vector v){
     super(smi.SEQUENCE_tag);
     valeur = (Vector) v.clone();
   } 

   /** Constructs a VarList holding Varlists.
   *  @param tab array of Vars.
   */
   public VarList(Var[] tab){
     super(smi.SEQUENCE_tag);
     int taille = tab.length;
     valeur = new Vector(taille);
     for (int i=0; i<taille; i++) 
       valeur.addElement(tab[i]);
   } 


  /** Builds a VarList from a ByteArrayInputStream holding a VarList Ber coding. 
   *  <BR>Bytes read are removed from the stream.
   *  <P><B>Note:</B> The ByteArrayInputStream must not contain the Var Tag.
   *  @exception IOException is thrown if a problem occurs while trying to decode the stream.
   */
  public VarList(ByteArrayInputStream inBer) throws IOException{
    super(smi.SEQUENCE_tag);
    valeur = new Vector();
    decodeBer(inBer);
  }

  /** Returns the Var at the specified index.
   */
  public Var elementAt(int i) throws IndexOutOfBoundsException {
    return (Var)valeur.elementAt(i); // Sans soucis: Var est immutable
  }

  /** Returns the number of Var
   */
  public int getSize(){
    return valeur.size();
  }

  /** Used  smi.decodeBer().
   *  A VarList is in fact an array of Vars.
   */
  void decodeValeur(ByteArrayInputStream bufferBer, int lgBerValeur) throws IOException {
    Var v;
    int lg;
    try{
      while (lgBerValeur >0){
        int tag = bufferBer.read();
        lgBerValeur --;
        if ( tag != smi.SEQUENCE_tag )
          throw new IOException ("error decoding tag Var in VarList: byte " + 
                                 java.lang.Integer.toHexString(tag) +" read.");
        v =new Var();
        lg = v.decodeBer(bufferBer); 
        this.valeur.addElement(v);
        lgBerValeur -= lg; 
      }
    } catch (IOException e){
      throw new IOException ("error decoding Value : " + e);
    }
  }
}
