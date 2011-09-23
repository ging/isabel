// -----------------------construct
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

/** Abstract class implementing recursive methods shared by Var, VarList, Pdu ....
 */
public abstract class construct extends smi { 
 
  protected Vector valeur; // SUBCLASSES MUST INITIALIZE THIS VECTOR
   
  /** Same as contruct(smi.SEQUENCE_tag).
   *  Var, to be deserializable, needs to have parents with null constructors.
   */
  protected construct() {
    super(smi.SEQUENCE_tag);
  }
                        
  protected construct(byte tag){
    super(tag);
  }

  /** Compares two object for equality.
   *  @returns true if this object is the same as the obj argument, false otherwise.
   */
  public boolean equals (Object obj) {
    if (this == obj) // pointers equality ?
      return true;
    if (!super.equals(obj)) // type equality ?
      return false;

    construct seq2 = (construct)obj;
    int nbObj = seq2.valeur.size();
    if (this.valeur.size() != nbObj)
      return false;
    for (int i=0; i< nbObj ; i++) // recursive
      if (! this.valeur.elementAt(i).equals(seq2.valeur.elementAt(i)) )
        return false;
  
    return true;
    }

  /** Returns the value of this object as a String.
   *  Only Var overloads this method. Other subclasses are too complex to be returned in a String 
   * (this method returns an empty String).
   *  Use println(String tab, StringBuffer sb) to display them.
   */  
  public String toString(){
    return new String(" CONTRUCT : DEADBEEF");
  }

  /** Returns an object in a StringBuffer.
   * @param tab indentation.
   */
  public void println(String tab, StringBuffer sb){
    String tab2 = "  " + tab;
  
    sb.append(tab).append(getShortClassName()).append(" {\n");
    int nbObj = valeur.size();
    for (int i=0; i< nbObj ; i++)
      ((smi) valeur.elementAt(i)).println(tab2,sb);
    sb.append(tab).append("}\n"); 
  }


  /** Returns Ber Coding of this object's value.
   *  Used by smi.codeBer().
   */
  byte[] codeValeur() {
    byte[][] tableau; // holds Ber coding of smi objects.
    int lgTot = 0; // length of sequence Ber coding (sum of objects lengths)
    int i;

    int nbObj = valeur.size();
    tableau = new byte[nbObj][];

    for (i=0; i< nbObj ; i++){
      tableau[i] = ( (smi) valeur.elementAt(i)).codeBer();
      lgTot += tableau[i].length;
    }

    byte[] buf = new byte[lgTot];
    int offset = 0;
    for (i=0; i< nbObj ; i++){
      System.arraycopy(tableau[i],0,buf,offset,tableau[i].length);
      offset += tableau[i].length;
    }
    return buf;
  }
}
