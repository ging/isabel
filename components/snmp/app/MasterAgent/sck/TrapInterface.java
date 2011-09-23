
//------------------TrapInterface.java

// This program is free software; you can redistribute it and/or
// modify it under the terms of the Affero GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details (cf. file COPYING).


package sck;

public interface TrapInterface {
 static public final byte TRAP =(byte) ( smi.CONTEXTSPECIFIC | smi.CONSTRUCT | 0x04 );
  static final public String TRAPTEXT[]={ "coldStart", "warmStart", "linkDown", "linkUp",
                             "authenticationFailure", "egpNeighborLoss","enterpriseSpecific"};
          

   /** Returns the VarList holded by this Trap.
   */
  public VarList getVarList();
} 