// -----------------------Counter.java
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

/** Tag (0x41)
 *  <BR>Counter increases up to a threshold then restarts from zero.
 *  <BR>This is an unsigned integer (32 bits)
 */
final public class Counter extends uint32{

  /** Build a Counter from a long.
   */
  public Counter(long valeur){
    super(smi.COUNTER_tag,valeur);
  }

  /** Builds a Counter from a ByteArrayInputStream holding a Counter Ber coding. 
   *  <BR>Bytes read are removed from the stream.
   *  <P><B>Note:</B> The ByteArrayInputStream must not contain the Counter Tag.
   *  @exception IOException is thrown if a problem occurs while trying to decode the stream.
   */
  public Counter(ByteArrayInputStream inBer) throws IOException{
    super(smi.COUNTER_tag,inBer);
  }

  /** Builds a Counter from a String. 
   *  <BR>If v == null then Counter(0).
   *  @exception InvalidObjectException is thrown if a Counter
   *  can't be build from the String.
   *  @see java.lang.Long#valueOf(java.lang.String)
   */
  public Counter(String v) throws InvalidObjectException{
    super(smi.COUNTER_tag,v);
  }
}