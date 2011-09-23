
//---------------------- Var.java

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

/** ASN.1 grammar for Var:
 * Var ::=
 * SEQUENCE
 *        { name Oid
 *          value CHOICE {Null, Integer, Counter, gauge, Timeticks, IpAddress, OctetString}
          }
 */
final public class Var extends construct implements Serializable { 

  public Var(Oid o, smi s){
    this();
    valor.addElement(o);
    valor.addElement(s);
  }

  /** Same as Var(new Oid(oid), new Null()).
   */
  public Var(String oid) throws IOException{
    this(new Oid(oid), new Null());
  }


  /** Builds a Var from a ByteArrayInputStream holding a Var Ber coding. 
   *  <BR>Bytes read are removed from the stream.
   *  <P><B>Note:</B> The ByteArrayInputStream must not contain the Var Tag.
   *  @exception IOException is thrown if a problem occurs while trying to decode the stream.
   */
  public Var(ByteArrayInputStream inBer) throws IOException{
    this();
    decodeBer(inBer);
  }

  /** Used only by VarList.
   */
  Var(){
    super(smi.SEQUENCE_tag);
    valor = new Vector(2);
  }

  /** Returns the name of this Var.
   */
  public Oid getName(){
    return (Oid) valor.elementAt(0);
  }

  /** Returns the value of this Var.
   */
  public smi getValue(){
    return (smi) valor.elementAt(1);
  }

  /** Returns the value of this Var as a String.
   */  
  public String toString() {
    try{
    return ((smi)valor.elementAt(0)).toString() + " = " + ((smi)valor.elementAt(1)).toString();
    }catch (IndexOutOfBoundsException e) { // ne doit pas se produire.
      System.out.println("Error de codigo internoen el tipo Var.");
      System.exit(1);
    }
    return null;
  }

  /** Used smi.decodeBer().
   */
  void decodeValor(ByteArrayInputStream bufferBer, int lgBervalor) throws IOException {
    int tag = bufferBer.read();
    if ( tag != smi.OID_tag )
      throw new IOException ("Error de decodificacion en la etiqueta Oid " +     java.lang.Integer.toHexString(tag) +"leida");
    Oid name = new Oid(bufferBer);
    this.valor.addElement(name);
    // el objeto siguiente
    tag = bufferBer.read();
    try{
    smi _valor = smiFactory.create(tag,bufferBer);
    this.valor.addElement(_valor);
    } catch (IOException e){
      throw new IOException ("Error al decodificar el campo Valor : " + e);
    }
  }

  /** Custom serialization: Ber coding is written in ObjectOutputStream.
   */
  private void writeObject(ObjectOutputStream out) throws IOException{
    byte b[] = this.codeBer();
    out.writeInt(b.length);
    out.write(b);
    out.flush();
  }

  private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
    this._tag = smi.SEQUENCE_tag;
    this.valor = new Vector();

    int len = in.readInt();
    byte b[] = new byte[len];
    in.readFully(b);
    this.decodeBer(new ByteArrayInputStream(b,1,b.length-1)); 
  }
}