// -----------------------Trap.java
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
import java.util.Hashtable;

/** Trap ::= SEQUENCE { -- type 0xA4
 *             enterprise Oid -- id of trap source.
 *             agent-addr IpAddress -- Agent address.
 *             generic-trap Integer -- kind of trap.
 *             specific-trap Integer -- id if specific trap.
 *             time-stamps TimeTicks -- epoch for the source event.
 *             variable-bindings VarList
 *           }
 */
public class Trap extends construct implements Pdu { 
 
  static final public String TRAPTEXT[]={ "coldStart", "warmStart", "linkDown", "linkUp", 
                             "authenticationFailure", "egpNeighborLoss","enterpriseSpecific"};
  static final public Hashtable TRAPNUMBER = new Hashtable(); // return Trap number according to trap Name 
  static { TRAPNUMBER.put(new java.lang.Integer(0),new String("coldStart"));
           TRAPNUMBER.put(new java.lang.Integer(1),new String("warmStart"));
           TRAPNUMBER.put(new java.lang.Integer(2),new String("linkDown"));
           TRAPNUMBER.put(new java.lang.Integer(3),new String("linkUp"));
           TRAPNUMBER.put(new java.lang.Integer(4),new String("authenticationFailure"));
           TRAPNUMBER.put(new java.lang.Integer(5),new String("egpNeighborLoss"));
           TRAPNUMBER.put(new java.lang.Integer(6),new String("enterpriseSpecific"));
  };
           
  /** Complex constructor.
   *  @param enterprise id of trap source (Oid).
   *  @param addr Agent address (IpAddress).
   *  @param generic kind of trap (sck.Integer).
   *  @param specific id if specific trap (sck.Integer).
   *  @param timestamps epoch for the source event (Timeticks).
   *  @param vl VarList.
   */                    
  public Trap(Oid enterprise, IpAddress addr, sck.Integer generic, sck.Integer specific, Timeticks timestamps, VarList Vl){
    super(TRAP);
    this.valeur = new Vector(6);
    this.valeur.addElement(enterprise);
    this.valeur.addElement(addr);
    this.valeur.addElement(generic);
    this.valeur.addElement(specific);
    this.valeur.addElement(timestamps);
    this.valeur.addElement(Vl);
  }

  /** Builds a Trap from a ByteArrayInputStream holding a Trap Ber coding. 
   *  <BR>Bytes read are removed from the stream.
   *  <P><B>Note:</B> The ByteArrayInputStream must not contain the Trap Tag.
   *  @exception IOException is thrown if a problem occurs while trying to decode the stream.
   */
  public Trap(ByteArrayInputStream inBer) throws IOException{
    super(TRAP);
    this.valeur = new Vector(6);
    decodeBer(inBer);
  }

  /** Returns the VarList holded by this Trap.
   */
  public VarList getVarList(){
    return (VarList) valeur.elementAt(5);
  }

  /** Returns a Trap in a StringBuffer.
   * @param tab indentation.
   */
  public void println(String tab, StringBuffer sb){
    String tab2 = "  " + tab;
    int idTrap = java.lang.Integer.valueOf(((smi)valeur.elementAt(2)).toString()).intValue();
 
    sb.append(tab).append("TRAP: {\n");
    sb.append(tab2).append("Enterprise: ").append(((smi)valeur.elementAt(0)).toString()).append("\n");
    sb.append(tab2).append("Agent Address: ").append(((smi)valeur.elementAt(1)).toString()).append("\n");
    sb.append(tab2).append("Generic-Trap: ").append(Trap.TRAPTEXT[idTrap]).append("\n");
    sb.append(tab2).append("Specific-Trap: ").append(((smi)valeur.elementAt(3)).toString()).append("\n");
    sb.append(tab2).append("Time-stamps: ").append(((smi)valeur.elementAt(4)).toString()).append("\n");
    ((smi) valeur.elementAt(5)).println(tab2,sb);
    sb.append(tab).append("}\n"); 
  }

  /** Used by smi.decodeBer().
   */
  void decodeValeur(ByteArrayInputStream bufferBer, int lgBerValeur) throws IOException {
    int tag;
    try{
      tag = bufferBer.read();
      if ( tag != smi.OID_tag )
        throw new IOException ("error decoding tag Oid (enterprise) : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      this.valeur.addElement(new sck.Oid(bufferBer));

      tag = bufferBer.read();
      if ( tag != smi.IPADDRESS_tag )
        throw new IOException ("error decoding tag IpAddress (agent address) : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      this.valeur.addElement(new sck.IpAddress(bufferBer));


      tag = bufferBer.read();
      if ( tag != smi.INTEGER_tag )
        throw new IOException ("error decoding tag Integer (generic-trap) : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      this.valeur.addElement(new sck.Integer(bufferBer));

      tag = bufferBer.read();
      if ( tag != smi.INTEGER_tag )
        throw new IOException ("error decoding tag Integer (specific-trap) : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      this.valeur.addElement(new sck.Integer(bufferBer));

      tag = bufferBer.read();
      if ( tag != smi.TIMETICKS_tag )
        throw new IOException ("error decoding tag Timeticks (time-stamps) : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      this.valeur.addElement(new sck.Timeticks(bufferBer));
     
      tag = bufferBer.read();
      if ( tag != smi.SEQUENCE_tag )
        throw new IOException ("error decoding tag SEQUENCE (VarList) : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      this.valeur.addElement(new VarList(bufferBer));

    } catch (IOException e){
      throw new IOException ("error decoding Trap : " + e);
    }
  }

}
