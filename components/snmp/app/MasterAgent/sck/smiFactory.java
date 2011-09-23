
//-------------------- smiFactory.java

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

/** Used by Var.
 */
class smiFactory{
  static smi create(int smiTag, ByteArrayInputStream BufferBer) throws IOException{
    smi newObject;

    switch (smiTag){

      case (smi.INTEGER_tag) :
        newObject = new snmpInteger(BufferBer);
        break;
	
	case (smi.COUNTER_tag) :
	  newObject = new Counter(BufferBer);
        break;

      case (smi.GAUGE_tag) :
        newObject = new Gauge(BufferBer);
        break;

      case (smi.OCTETSTRING_tag) :
        newObject = new snmpOctetString(BufferBer);
        break;

      case (smi.TIMETICKS_tag) :
        newObject = new Timeticks(BufferBer);
        break;
 
      case (smi.OID_tag) :
        newObject = new Oid(BufferBer);
        break;

	case (smi.IPADDRESS_tag) :
        newObject = new IpAddress(BufferBer);
        break;

      case (smi.NULL_tag) :
        newObject = new Null(BufferBer);
        break;

      default :
        throw new IOException(" Tag unknown: "+smiTag);
        
    }
  return newObject;
  }
}
     