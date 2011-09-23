
//---------------- SnmpError.java

// This program is free software; you can redistribute it and/or
// modify it under the terms of the Affero GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details (cf. file COPYING).


package sck;

/** SnmpError : holds the error status .
 */
public class SnmpError extends Exception{ 
  private int error =0;
  
  /** errorStatus is an integer corresponding to the error (cf Pdu.SNMPERROR)
   */                    
  public SnmpError(int errorStatus){
    this.error = errorStatus;
  }

  public String toString() {
    return ("Snmp error: " + Pdu.SNMPERROR[error]);
  }
}