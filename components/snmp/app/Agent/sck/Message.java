// -----------------------Message.java
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

/** Message::= SEQUENCE {
 *               version Integer -- 0: SNMPV1
 *               community OctetString 
 *               pdu Pdu
 *             }
 */
public class Message extends construct { 
 
  static final public sck.Integer SNMPV1 = new sck.Integer(0);
           
  /** Constructor, complex form.
   *  @param community name of community
   *  @param pdu Pdu (PduCmd or Trap) holded by Message.
   */                    
  public Message(OctetString community, Pdu pdu){
    super(smi.SEQUENCE_tag);
    this.valeur = new Vector(3);
    this.valeur.addElement(Message.SNMPV1);
    this.valeur.addElement(community);
    this.valeur.addElement(pdu);
  }

  /** Builds a Message from a ByteArrayInputStream holding a Message Ber coding. 
   *  <BR>Bytes read are removed from the stream.
   *  <P><B>Note:</B> The ByteArrayInputStream must not contain the Message Tag (0x30 = Sequence).
   *  @exception IOException is thrown if a problem occurs while trying to decode the stream or snmp
   *  version is not equal to SNMPV1.
   */
  public Message(ByteArrayInputStream inBer) throws IOException{
    super(smi.SEQUENCE_tag);
    this.valeur = new Vector(3);
    decodeBer(inBer);
  }

  /** Returns Pdu holded by this message.
   */
  public Pdu getPdu(){
    return (Pdu) this.valeur.elementAt(2);
  }

  /** Returns community as a String.
   */
  public String getCommunity(){
    return  ((smi)valeur.elementAt(1)).toString() ;
  }

  /** Returns a Message in a StringBuffer.
   * @param tab indentation.
   */
  public void println(String tab, StringBuffer sb){
    String tab2 = "  " + tab;

    sb.append(tab).append("Message: {\n");
    sb.append(tab2).append("Version: SNMPV1\n");
    sb.append(tab2).append("Community: ").append(((smi)valeur.elementAt(1)).toString()).append("\n");
    ((smi) valeur.elementAt(2)).println(tab2,sb);
    sb.append(tab).append("}\n"); 
  }

  /** Used by smi.decodeBer().
   */
  void decodeValeur(ByteArrayInputStream bufferBer, int lgBerValeur) throws IOException {
    int tag;
    try{
      tag = bufferBer.read();
      if ( tag != smi.INTEGER_tag )
        throw new IOException ("error decoding tag Integer (version) : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      sck.Integer version = new sck.Integer(bufferBer);
      if (! version.equals(Message.SNMPV1))
         throw new IOException ("Version is not SNMPV1 !");
      this.valeur.addElement(version);

      tag = bufferBer.read();
      if ( tag != smi.OCTETSTRING_tag )
        throw new IOException ("error decoding tag OCTETSTRING (community) : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      this.valeur.addElement(new sck.OctetString(bufferBer));

      tag = bufferBer.read();
      int tagLim = 0xff & ( CONTEXTSPECIFIC | CONSTRUCT ); // start of Pdu tags.
      if (( tag < tagLim) || (tag > tagLim + 4))
        throw new IOException ("error decoding tag Pdu : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      if ( tag != ( 0xff & Pdu.TRAP)) // convert byte (signed) to int.
        this.valeur.addElement(new PduCmd((byte) tag,bufferBer));
      else
        this.valeur.addElement(new Trap(bufferBer));
   
    } catch (IOException e){
      throw new IOException ("error decoding Message : " + e);
    }
  }

}