
//---------------------- VarList.java

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
	 
  /** Constructs an empty VarList
    */
  public VarList() {
      super(smi.SEQUENCE_tag);
       valor = new Vector();
   }  
   
  /** Constructs an empty VarList of the size included as parameter.
    * This size can be increased if needed.
    * @param initialSize 
    */
  public VarList(int initialSize) {
       super(smi.SEQUENCE_tag);
       valor = new Vector(initialSize);
   } 

  /** Constructs a VarList holding a single Var.
   *  @param Var to be holded Varlist
   */
   public VarList(Var v){
     super(smi.SEQUENCE_tag);
     valor = new Vector(1);
     valor.addElement(v);
   }

  /** Constructs a VarList holding Varlists.
   *  @param v Vector of Vars.
   */
   public VarList(Vector v){
     super(smi.SEQUENCE_tag);
     valor = (Vector) v.clone();
   } 

   /** Constructs a VarList holding Varlists.
   *  @param tab array of Vars.
   */
   public VarList(Var[] tab){
     super(smi.SEQUENCE_tag);
     int taille = tab.length;
     valor = new Vector(taille);
     for (int i=0; i<taille; i++) 
       valor.addElement(tab[i]);
   } 


  /** Builds a VarList from a ByteArrayInputStream holding a VarList Ber coding. 
   *  <BR>Bytes read are removed from the stream.
   *  <P><B>Note:</B> The ByteArrayInputStream must not contain the Var Tag.
   *  @exception IOException is thrown if a problem occurs while trying to decode the stream.
   */
  public VarList(ByteArrayInputStream inBer) throws IOException{
    super(smi.SEQUENCE_tag);
    valor = new Vector();
    decodeBer(inBer);
  }

  /** Adds a Var to the tail of the VarList
   * @param var to be added
   */
   public void addVar( Var v) {
       valor.addElement(v);
   }
   
  /** Adds a var to the varList in the specified
    * position
    */
   public void add(int index, Var v) {
       valor.add(index, v); 
   } 
   
   /** Concatenates a VarList
    *  @param varList to be concatenated
    */
   public boolean addVarList( VarList varList) throws IndexOutOfBoundsException {
      return this.valor.addAll(varList.valor);
   } 

  /** Returns a Vector containing the Oid's of the Var's in this VarList.
    */
  public Vector getNames() {
    Vector temp = new Vector();
    for (int i = 0; i < this.getSize(); i++) 
      temp.add(this.elementAt(i).getName());
    return temp;
  }  
  
  /** Returns a Vector containing the Oid's as Strings of the Var's in this VarList.
    */
  public Vector getNamesAsStrings() {
    Vector names = this.getNames();
    Vector temp = new Vector();
    for (int i = 0; i < this.getSize(); i++) 
      temp.add(((Oid) names.get(i)).toString());
    return temp;  
  }
  
  /** Returns the Var at the specified index.
   */
  public Var elementAt(int i) throws IndexOutOfBoundsException {
    return (Var)valor.elementAt(i); // Sans soucis: Var est immutable
  }

  /** Returns the first ocurrence of the Var specified as parameter, 
   *  and returns its index in the varList. Returns -1 if the Var is
   *  not found.
   *  @param var to be found
   */
   public int indexOf(Var v) {
     return valor.indexOf(v);   
   }

  /** Returns a sub-VarList delimited by the indexes specified, both included.
    * @param the delimitators of the sub-varList.
    */
  public VarList subVarList(int low, int high) throws IndexOutOfBoundsException {
     if (high == low)
       return new VarList(this.elementAt(high));
     if (high < low) {
       int finstro = high;
       high = low;
       low = finstro;    
     }
         
    VarList temp = new VarList();
    
    for(int i = low; i <= high; i++ )
      temp.addVar(this.elementAt(i));
      
    return temp;    
  }  
   
  /** Returns the Var with the specified Oid
    */
  public Var get(Oid oid) {
    Vector names = this.getNames();
    int p = names.indexOf(oid);
    return this.elementAt(p);
  }  
    
  /** Returns the number of Var
   */
  public int getSize(){
    return valor.size();
  }
  
  /** Returns a VarList object with the same Var objects than this, but
    * after sorting, using lexicographical ordering of its Oid's.
    */
  public VarList sort() {
    Vector names = this.getNames();
    names = Oid.sort(names);
    VarList temp = new VarList();
    for(int i=0; i<this.getSize(); i++)       
      temp.addVar(this.get((Oid) names.elementAt(i)));    
    return temp;  
  }    
  
  /** Used  smi.decodeBer().
   *  A VarList is in fact an array of Vars.
   */
  void decodeValor(ByteArrayInputStream bufferBer, int lgBervalor) throws IOException {
    Var v;
    int lg;
    try{
      while (lgBervalor >0){
        int tag = bufferBer.read();
        lgBervalor --;
        if ( tag != smi.SEQUENCE_tag )
          throw new IOException ("error decoding tag Var in VarList: byte " + 
                                 java.lang.Integer.toHexString(tag) +" read.");
        v =new Var();
        lg = v.decodeBer(bufferBer); 
        this.valor.addElement(v);
        lgBervalor -= lg; 
      }
    } catch (IOException e){
      throw new IOException ("error decoding Value : " + e);
    }
  }
}