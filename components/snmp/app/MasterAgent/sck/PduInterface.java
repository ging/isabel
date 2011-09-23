
//------------------------ PduInterface.java

/** Pdu is a PduCmd ( set, get, get-next and response) or Trap.
 *  @see PduCmd
 *  @see Trap
 */



 // This program is free software; you can redistribute it and/or
// modify it under the terms of the Affero GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details (cf. file COPYING).


package sck;

public interface PduInterface {
  static public final byte GET           =(byte) ( smi.CONTEXTSPECIFIC | smi.CONSTRUCT | 0x00 );
  static public final byte GET_NEXT      =(byte) ( smi.CONTEXTSPECIFIC | smi.CONSTRUCT | 0x01 );
  static public final byte RESPONSE      =(byte) ( smi.CONTEXTSPECIFIC | smi.CONSTRUCT | 0x02 );
  static public final byte SET           =(byte) ( smi.CONTEXTSPECIFIC | smi.CONSTRUCT | 0x03 );
  


  /** Returns the VarList holded by this Pdu.
   */
  public VarList getVarList();

   /** Returns error Status for this pdu.
   */
  public int getErrorStatus();

  /** Returns error Index for this pdu.
   */
  public int getErrorIndex();

  /** Returns request Id for this pdu.
   */
  public int getRequestId();

   /** Returns Pdu tag.
   */
  public byte getTag();
  
} 
