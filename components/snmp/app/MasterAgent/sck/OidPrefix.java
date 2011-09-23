// -----------------------OidPrefix.java

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
import java.util.*;

final public class OidPrefix extends Oid {

  private boolean endsWithDot;
  
  synchronized void StringToTab(String val) throws InvalidObjectException {        
    super.StringToTab(val);
  }

  /** Buils an OidPrefix from a String.
   *  An OidPrefix is made of sub-identifiers (integer) 
   *  s&eacute;par&eacute; par des '.', and can finish with a dor.
   *  <P>Example: OidPrefix ("1.3.6.1.2.1.1.")
   *  @exception InvalidObjectException is thrown if an OidPrefix can't
   *  be build from the String.
   */
  public OidPrefix(String v) throws InvalidObjectException {
    super();
    this.endsWithDot = v.endsWith(new String("."));     
    this.StringToTab(v);
  }
  
  /** @ return true if ends with a dot
   */
  public boolean endsWithDot() {
    return endsWithDot;   
  } 
   
  /** Returns true if this OidPrefix is lexicographycally minor than the one 
    * passed as argument.
    */
  public boolean minorThan(OidPrefix oidPrefix) {
    if((this.endsWithDot() & oidPrefix.endsWithDot())|((!this.endsWithDot()) & (!oidPrefix.endsWithDot()))) {
      return super.minorThan(oidPrefix);
    }
    else {      
        int[] thisSubids = this.getSubIdentifiers(); 
        int[] oidSubids = oidPrefix.getSubIdentifiers();            
      
        for(int i=0; (i<thisSubids.length)&(i<oidSubids.length); i++) {
          if(thisSubids[i]<oidSubids[i]) 
            return true;
          if(thisSubids[i]>oidSubids[i]) 
            return false;                  
        }
        
        return this.endsWithDot() ? false : true;                              
    }
  }
  
  /** Returns true if this OidPrefix is lexicographycally minor than the String 
    * passed as argument.
    */
  public boolean minorThan(String oidPrefix) {
    try {
      OidPrefix oidp = new OidPrefix(oidPrefix);
      return this.minorThan(oidp);
    } catch (java.io.InvalidObjectException e) {
        System.out.println("Invalid oidPrefix passed as String!");
        System.out.println(e.toString());
        System.exit(0);
        return false;  //Por poner algo
    }  
  }
  
  /** Returns true if this Oid is lexicographycally greater than the one 
    * passed as argument.
    */
  public boolean greaterThan(OidPrefix oidPrefix) {
    if((this.endsWithDot() & oidPrefix.endsWithDot())|((!this.endsWithDot()) & (!oidPrefix.endsWithDot()))) {
      return super.minorThan(oidPrefix);
    }
    else {      
        int[] thisSubids = this.getSubIdentifiers(); 
        int[] oidSubids = oidPrefix.getSubIdentifiers();            
      
        for(int i=0; (i<thisSubids.length)&(i<oidSubids.length); i++) {
          if(thisSubids[i]>oidSubids[i]) 
            return true;
          if(thisSubids[i]<oidSubids[i]) 
            return false;                  
        }
        
        return this.endsWithDot() ? true : false;                              
    }
  }
  
  /** Returns true if this Oid is lexicographycally greater than the String 
    * passed as argument.
    */
  public boolean greaterThan(String oidPrefix) {
    try {
      OidPrefix oidp = new OidPrefix(oidPrefix);
      return this.greaterThan(oidp);
    } catch (java.io.InvalidObjectException e) {
        System.out.println("Invalid oidPrefix passed as String!");
        System.out.println(e.toString());
        System.exit(0);
        return false; // por poner algo
    }    
  }
  
  /** Sorts a vector of Oid's using the bubbleSort algorithm. 
    */
  public static Vector sort(Vector items) {  
    try {
    
      for (int i=0; i<items.size(); i++) {
        for (int j=0; j<items.size()-1; j++) {
          Oid currentItem = (OidPrefix) items.get(j);
          Oid nextItem = (OidPrefix) items.get(j+1);
          
          if (nextItem.minorThan(currentItem)) {
            // Interchange currentItem and nextItem
            items.setElementAt(nextItem, j);
            items.setElementAt(currentItem, j+1);
          }

        }
      }
      
      return items;
    
    } catch(Exception e) {
      System.out.println(e.toString());
      System.exit(0);
      return null;
    }
  }   
  
 /** Sorts an enumeration of Strings representing OidPrefix's
   * using the bubbleSort algorithm. Returns an array of OidPrefix's.
   */
  public static Vector sort(Enumeration items) throws java.io.InvalidObjectException {    
    Vector vector = new Vector();
    
    boolean containsDefault = false;
    
    while (items.hasMoreElements()) {      
      String name = new String((String) items.nextElement() );
       
      if (name.equals("default")) 
        containsDefault = true;     
      else
        vector.add(new OidPrefix(name));
    } 
    
    vector = OidPrefix.sort(vector);            
   
    
    if (containsDefault)
      vector.add(new String("default"));
      
    return vector;  
  }

 
  /** Compares two OidPrefixes for equality.
   *  @return true if this oid is the same as the obj argument, false otherwise.
   */
  public boolean equals (Object obj) {
    String este = this.toString();
    return este.equals(obj.toString());
  }
  
  /** Returns the value of this OidPrefix as a String.
   */  
  public String toString() {
    StringBuffer chaine = new StringBuffer();
    
    int [] subids = this.getSubIdentifiers();
    
    for (int i=0; i<subids.length; i++)
      chaine.append(Integer.toString(subids[i])+new String("."));
    
    String cadena = chaine.toString(); 
      
    if(!this.endsWithDot())  
      cadena = cadena.substring(0, cadena.length()-1);

    return cadena;
  }  
} //OidPrefix class