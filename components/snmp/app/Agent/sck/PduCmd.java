// -----------------------PduCmd.java
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

/** PduCmd : set, get, get-next & response are SNMP Protocol Data Units.
 */
public class PduCmd extends construct implements Pdu { 
 
  /** Array of error code (from 0 to 5) for snmpV1.
   */
  static final public String SNMPERROR[]={ "noError", "tooBig", "noSuchName", 
                              "badValue", "readOnly", "genErr"};
  /** Returns code (as a java.lang.Integer) corresponding to the specified error .
   *  <BR>Keys(String) = "noError", "tooBig", "noSuchName","badValue", "readOnly", "genErr".
   */
  static final public Hashtable ERRORCODE = new Hashtable();
  static { ERRORCODE.put(new String("noError"),new java.lang.Integer(0));
           ERRORCODE.put(new String("tooBig"),new java.lang.Integer(1));
           ERRORCODE.put(new String("noSuchName"),new java.lang.Integer(2));
           ERRORCODE.put(new String("badValue"),new java.lang.Integer(3));
           ERRORCODE.put(new String("readOnly"),new java.lang.Integer(4));
           ERRORCODE.put(new String("genErr"),new java.lang.Integer(5));
  };

  /** Giving a tag (Byte), returns type of Pdu as a String.
   *  <BR>Keys (Byte) = Pdu.GET, Pdu.GET_NEXT, Pdu.RESPONSE, PDU.SET.
   */
  static final public Hashtable TYPE = new Hashtable(); 
  static { TYPE.put(new Byte(Pdu.GET),new String("GET"));
           TYPE.put(new Byte(Pdu.GET_NEXT),new String("GET_NEXT"));
           TYPE.put(new Byte(Pdu.RESPONSE),new String("RESPONSE"));
           TYPE.put(new Byte(Pdu.SET),new String("SET"));
  };
  /** Returns tag (as a Byte) corresponding to the pdu specified.
   *  <BR>Keys (String) = "GET", "GET_NEXT", "RESPONSE", "SET".
   */
  static final public Hashtable TAG = new Hashtable(); 
  static { TAG.put(new String("GET"),new Byte(Pdu.GET));
           TAG.put(new String("GET_NEXT"),new Byte(Pdu.GET_NEXT));
           TAG.put(new String("RESPONSE"),new Byte(Pdu.RESPONSE));
           TAG.put(new String("SET"),new Byte(Pdu.SET));
  };
           
  /** Constructor, complex form
   *  @param tag PduCmd type: (Pdu.GET, Pdu.SET, Pdu.GET_NEXT, Pdu.RESPONSE).
   *  @param RequestId serial number for the request.
   *  @param ErrorStatus kind of error.
   *  @param ErrorIndex index of a Var in VarList which raises this error.
   */                    
  public PduCmd(byte tag, sck.Integer RequestId, sck.Integer ErrorStatus, sck.Integer ErrorIndex, VarList Vl){
    super(tag);
    this.valeur = new Vector(4);
    this.valeur.addElement(RequestId);
    this.valeur.addElement(ErrorStatus);
    this.valeur.addElement(ErrorIndex);
    this.valeur.addElement(Vl);
  }

  /** Simple form constructor ( RequestId = ErrorStatus= ErrorIndex =0).
   */
  public PduCmd(byte tag, VarList Vl){
    this(tag, new sck.Integer(0), new sck.Integer(0), new sck.Integer(0),Vl);
  }

  /** Builds a PduCmd from a ByteArrayInputStream holding a pduCmd Ber coding. 
   *  <BR>Bytes read are removed from the stream.
   *  <P><B>Note:</B> The ByteArrayInputStream must not contain the PduCmd Tag.
   *  @exception IOException is thrown if a problem occurs while trying to decode the stream.
   */
  public PduCmd(byte tag,ByteArrayInputStream inBer) throws IOException{
    super(tag);
    this.valeur = new Vector(4);
    decodeBer(inBer);
  }

  /** Returns the VarList holded by this PduCmd.
   */
  public VarList getVarList(){
    return (VarList) valeur.elementAt(3);
  }

  /** Returns a PduCmd in a StringBuffer.
   * @param tab indentation.
   */
  public void println(String tab, StringBuffer sb){
    String tab2 = "  " + tab;
    int errorStatus = java.lang.Integer.valueOf(((smi)valeur.elementAt(1)).toString()).intValue();

    sb.append(tab).append(getShortClassName()).append(":")
      .append((String)PduCmd.TYPE.get(new Byte(this._tag))).append(" {\n"); 
    sb.append(tab2).append("Request-ID: ").append(((smi)valeur.elementAt(0)).toString()).append("\n");
    sb.append(tab2).append("Error-status: ").append(PduCmd.SNMPERROR[errorStatus]).append("\n");
    sb.append(tab2).append("Error-index: ").append(((smi)valeur.elementAt(2)).toString()).append("\n");
    ((smi) valeur.elementAt(3)).println(tab2,sb);
    sb.append(tab).append("}\n"); 
  }

  /** Returns error Status for this pdu.
   */
  public int getErrorStatus(){
    return java.lang.Integer.valueOf(((smi)valeur.elementAt(1)).toString()).intValue();
  }

  /** Returns error Index for this pdu.
   */
  public int getErrorIndex(){
    return java.lang.Integer.valueOf(((smi)valeur.elementAt(2)).toString()).intValue();
  }

  /** Returns request Id for this pdu.
   */
  public int getRequestId(){
    return java.lang.Integer.valueOf(((smi)valeur.elementAt(0)).toString()).intValue();
  }

  /** Returns Pdu tag.
   */
  public byte getTag(){
    return super._tag;
  }

  /** Used by smi.decodeBer().
   */
  void decodeValeur(ByteArrayInputStream bufferBer, int lgBerValeur) throws IOException {
    int tag;
    try{
      tag = bufferBer.read();
      if ( tag != smi.INTEGER_tag )
        throw new IOException ("error decoding tag Integer (request-id) : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      this.valeur.addElement(new sck.Integer(bufferBer));

      tag = bufferBer.read();
      if ( tag != smi.INTEGER_tag )
        throw new IOException ("error decoding tag Integer (error-status) : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      this.valeur.addElement(new sck.Integer(bufferBer));

      tag = bufferBer.read();
      if ( tag != smi.INTEGER_tag )
        throw new IOException ("error decoding tag Integer (error-index) : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      this.valeur.addElement(new sck.Integer(bufferBer));
     
      tag = bufferBer.read();
      if ( tag != smi.SEQUENCE_tag )
        throw new IOException ("error decoding tag SEQUENCE (VarList) : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      this.valeur.addElement(new VarList(bufferBer));

    } catch (IOException e){
      throw new IOException ("error decoding PduCmd : " + e);
    }
  }

}
