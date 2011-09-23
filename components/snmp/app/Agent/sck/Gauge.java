// -----------------------Gauge.java
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

/** Tag (0x42)
 *  <BR>Gauge increases and decreases but can't overrun a threshold
 *  <BR>Gauge is an unsigned integer (32 bits)
 */
final public class Gauge extends uint32{

  /** Builds a Gauge from a long.
   */
  public Gauge(long valeur){
    super(smi.GAUGE_tag,valeur);
  }

  /** Builds a Gauge from a ByteArrayInputStream holding a Gauge Ber coding. 
   *  <BR>Bytes read are removed from the stream.
   *  <P><B>Note:</B> The ByteArrayInputStream must not contain the Gauge Tag.
   *  @exception IOException is thrown if a problem occurs while trying to decode the stream.
   */
  public Gauge(ByteArrayInputStream inBer) throws IOException{
    super(smi.GAUGE_tag,inBer);
  }

  /** Builds a Gauge from a String. 
   *  <BR>If v == null then Counter(0).
   *  @exception InvalidObjectException is thrown if a Gauge
   *  can't be build from the String.
   *  @see java.lang.Long#valueOf(java.lang.String)
   */
  public Gauge(String v) throws InvalidObjectException{
    super(smi.GAUGE_tag,v);
  }
}