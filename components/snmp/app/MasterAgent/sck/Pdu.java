
//--------------------------------- Pdu.java

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

/** Pdu : set, get, get-next & response are SNMP Protocol Data Units.
 */
public class Pdu extends construct implements PduInterface {
 
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
  static { TYPE.put(new Byte(PduInterface.GET),new String("GET"));
           TYPE.put(new Byte(PduInterface.GET_NEXT),new String("GET_NEXT"));
           TYPE.put(new Byte(PduInterface.RESPONSE),new String("RESPONSE"));
           TYPE.put(new Byte(PduInterface.SET),new String("SET"));
  };
  /** Returns tag (as a Byte) corresponding to the pdu specified.
   *  <BR>Keys (String) = "GET", "GET_NEXT", "RESPONSE", "SET".
   */
  static final public Hashtable TAG = new Hashtable(); 
  static { TAG.put(new String("GET"),new Byte(PduInterface.GET));
           TAG.put(new String("GET_NEXT"),new Byte(PduInterface.GET_NEXT));
           TAG.put(new String("RESPONSE"),new Byte(PduInterface.RESPONSE));
           TAG.put(new String("SET"),new Byte(PduInterface.SET));
  };
           
  /** Constructor, complex form
   *  @param tag Pdu type: (Pdu.GET, Pdu.SET, Pdu.GET_NEXT, Pdu.RESPONSE).
   *  @param RequestId serial number for the request.
   *  @param ErrorStatus kind of error.
   *  @param ErrorIndex index of a Var in VarList which raises this error.
   */                    
  public Pdu(byte tag, snmpInteger RequestId, snmpInteger ErrorStatus, snmpInteger ErrorIndex, VarList Vl){
    super(tag);
    this.valor = new Vector(4);
    this.valor.addElement(RequestId);
    this.valor.addElement(ErrorStatus);
    this.valor.addElement(ErrorIndex);
    this.valor.addElement(Vl);
  }

  /** Simple form constructor ( RequestId = ErrorStatus= ErrorIndex =0).
   */
  public Pdu(byte tag, VarList Vl){
    this(tag, new snmpInteger(0), new snmpInteger(0), new snmpInteger(0),Vl);
  }

  /** Builds a Pdu from a ByteArrayInputStream holding a Pdu Ber coding. 
   *  <BR>Bytes read are removed from the stream.
   *  <P><B>Note:</B> The ByteArrayInputStream must not contain the Pdu Tag.
   *  @exception IOException is thrown if a problem occurs while trying to decode the stream.
   */
  public Pdu(byte tag,ByteArrayInputStream inBer) throws IOException{
    super(tag);
    this.valor = new Vector(4);
    decodeBer(inBer);
  }

  /** Returns the VarList holded by this Pdu.
   */
  public VarList getVarList(){
    return (VarList) valor.elementAt(3);
  }

  /** Returns a Pdu in a StringBuffer.
   * @param tab indentation.
   */
  public void println(String tab, StringBuffer sb){
    String tab2 = "  " + tab;
    int errorStatus = java.lang.Integer.valueOf(((smi)valor.elementAt(1)).toString()).intValue();

    sb.append(tab).append(getShortClassName()).append(":")
      .append((String)Pdu.TYPE.get(new Byte(this._tag))).append(" {\n");
    sb.append(tab2).append("Request-ID: ").append(((smi)valor.elementAt(0)).toString()).append("\n");
    sb.append(tab2).append("Error-status: ").append(Pdu.SNMPERROR[errorStatus]).append("\n");
    sb.append(tab2).append("Error-index: ").append(((smi)valor.elementAt(2)).toString()).append("\n");
    ((smi) valor.elementAt(3)).println(tab2,sb);
    sb.append(tab).append("}\n"); 
  }

  /** Returns error Status for this pdu.
   */
  public int getErrorStatus(){
    return java.lang.Integer.valueOf(((smi)valor.elementAt(1)).toString()).intValue();
  }

  /** Returns error Index for this pdu.
   */
  public int getErrorIndex(){
    return java.lang.Integer.valueOf(((smi)valor.elementAt(2)).toString()).intValue();
  }

  /** Returns request Id for this pdu.
   */
  public int getRequestId(){
    return java.lang.Integer.valueOf(((smi)valor.elementAt(0)).toString()).intValue();
  }

  /** Returns Pdu tag.
   */
  public byte getTag(){
    return super._tag;
  }

  /** Used by smi.decodeBer().
   */
  void decodeValor(ByteArrayInputStream bufferBer, int lgBervalor) throws IOException {
    int tag;
    try{
      tag = bufferBer.read();
      if ( tag != smi.INTEGER_tag )
        throw new IOException ("error decoding tag Integer (request-id) : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      this.valor.addElement(new snmpInteger(bufferBer));

      tag = bufferBer.read();
      if ( tag != smi.INTEGER_tag )
        throw new IOException ("error decoding tag Integer (error-status) : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      this.valor.addElement(new snmpInteger(bufferBer));

      tag = bufferBer.read();
      if ( tag != smi.INTEGER_tag )
        throw new IOException ("error decoding tag Integer (error-index) : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      this.valor.addElement(new snmpInteger(bufferBer));
     
      tag = bufferBer.read();
      if ( tag != smi.SEQUENCE_tag )
        throw new IOException ("error decoding tag SEQUENCE (VarList) : byte "
                               +  java.lang.Integer.toHexString(tag) +" read.");
      this.valor.addElement(new VarList(bufferBer));

    } catch (IOException e){
      throw new IOException ("error decoding Pdu : " + e);
    }
  }

}