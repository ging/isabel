// -----------------------Timeticks.java
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

/** Tag (0x43)
 *  <BR>Timeticks is a period ( 1/100 of seconds).
 *  <BR>Timeticks is an unsigned integer (32 bits).
 */
final public class Timeticks extends uint32{

  /** Builds a Timeticks from a long.
   */
  public Timeticks(long valeur){
    super(smi.TIMETICKS_tag,valeur);
  }

  /** Builds a Timeticks from a ByteArrayInputStream holding a Timeticks Ber coding. 
   *  <BR>Bytes read are removed from the stream.
   *  <P><B>Note:</B> The ByteArrayInputStream must not contain the Timeticks Tag.
   *  @exception IOException is thrown if a problem occurs while trying to decode the stream.
   */
  public Timeticks(ByteArrayInputStream inBer) throws IOException{
    super(smi.TIMETICKS_tag,inBer);
  }

  /** Builds a Timeticks from a String. 
   *  String has the following format: 
   *  <i>dd</i> d <i>hh</i> h <i>mm</i> m <i>ss</i> s <i>cc</i>
   *  where <i>dd</i> is the number of days, <i>hh</i> the number of hours,
   * <i>mm</i> the number of minutes, <i>ss</i> the number of seconds and cc the number of 1/100 of seconds.
   *  <BR>If v == null then Timeticks(0).
   *  @exception InvalidObjectException is thrown if a Timeticks
   *  can't be build from the String.
   *  @see java.lang.Long#valueOf(java.lang.String)
   */
  public Timeticks(String v) throws InvalidObjectException{   
    super(smi.TIMETICKS_tag,0);
    this.valeur = 0;
    long l =0;
    try{
      if (v != null) {
        StreamTokenizer st = new StreamTokenizer( new BufferedReader(new StringReader(v)));
        st.ordinaryChar('d');
        st.ordinaryChar('h');
        st.ordinaryChar('m');
        st.ordinaryChar('s');
        while (st.nextToken() != StreamTokenizer.TT_EOF ) {
          l = (long) st.nval;
          switch (st.nextToken()) {
            case 'd' :
              this.valeur = l * 24 * 360000;
              break; 
            case 'h' :
              this.valeur += l * 360000;
              break; 
            case 'm' :
              this.valeur += l * 6000;
              break; 
            case 's' :
              this.valeur += l * 100;
              break; 
            case StreamTokenizer.TT_EOF :
              this.valeur += l;
              break;
            default:
              throw new InvalidObjectException("Cannot construct Timeticks from string " +v);
          }
        } 
      }
    } catch (IOException e){
      throw new InvalidObjectException("Cannot construct Timeticks from string " +v + " ."+e);
    }
  }

  /** Returns the value of this Timeticks as a String .
   *  @ see Timeticks(java.lang.String)
   */    
  public String toString(){  
    long d,h,m,s,c;
    StringBuffer sb = new StringBuffer();
    c = this.valeur % 100;
    s = this.valeur / 100;
    m = s / 60; s = s % 60;
    h = m / 60; m = m % 60;
    d = h / 24; h = h % 24;
    if (d > 0)
      sb.append(d).append("d ");
    if ( (h > 0) || (d >0) )
      sb.append(h).append("h ");
    if ( (m > 0) || (h > 0) || (d>0))
      sb.append(m).append("m ");
    if ( (s > 0) || (m > 0) || (h > 0) || (d > 0) )
      sb.append(s).append("s ");
    sb.append(c);
    return sb.toString();
  }

}