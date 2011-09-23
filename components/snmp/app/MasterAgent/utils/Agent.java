
//-------------------- Agent.java

// This program is free software; you can redistribute it and/or
// modify it under the terms of the Affero GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details (cf. file COPYING).


import java.io.*;
import java.net.*;

import sck.*;


/** Implements a false agent used for testing purposes.
 *
 *  @author Oscar J. Martin Vega
 *
 */

class Agent {
  private static int port = 161;
  private static snmpInteger errorStatus = new snmpInteger(0);
  private static int errorIndex_as_int;
  private static snmpInteger errorIndex = new snmpInteger(0);  

  public static void main(String[] args) {

    if (args.length != 0)  {
    if (args[0].equals("-h")|args[0].equals("-help")) {
     System.err.println("\n\nUsage: java Agent [portNumber] [-e errorCode errorIndex]\n");
     System.err.println("portNumber : the agent will listen to that port ");
     System.err.println("-e errorCode errorIndex : an error will be returned; errorCode specifies what errorCode and errorIndex what variable MOD varListLength");
     System.exit(0);
    }      
    else if (args.length==1) port = Integer.valueOf(args[0]).intValue(); 
     
    else    
    if (args[0].equals("-e")|args[1].equals("-e")) {
    try {
      int s=0;
      if (args[1].equals("-e")) {
        s=1;
        port = Integer.valueOf(args[0]).intValue();
      }
      errorStatus = new snmpInteger(args[s+1]);
      errorIndex_as_int = Integer.valueOf(args[s+2]).intValue();

          } catch (InvalidObjectException e) {
        System.err.println(e.toString());
        System.exit(0);
    }  
    }
    }
    run(errorStatus, errorIndex);
  }
  
  private static void run(snmpInteger errorStatus, snmpInteger errorIndex) {
    try {      
      DatagramSocket sock = new DatagramSocket(port);
      

      while (true){
        byte[] b = new byte[1024];
        DatagramPacket p = new DatagramPacket(b,b.length);

        sock.receive(p); // block 
        
        InetAddress master = p.getAddress();
        int clientPort = p.getPort();

        ByteArrayInputStream ber = new ByteArrayInputStream(b,1,p.getLength()-1); // without tag, like all smi object !
        Message m = new Message(ber);

        System.out.println("Message received at: "+ new java.util.Date());
        StringBuffer buf = new StringBuffer();
        m.println("",buf);
        System.out.println(buf);

        
        snmpOctetString community = new snmpOctetString( m.getCommunity() );
                
        snmpInteger requestId = new snmpInteger(m.getPdu().getRequestId() );              
        VarList varList = m.getPdu().getVarList();
        
        int errorIndex_as_int_temp = errorIndex_as_int%(varList.getSize()+1)==0 ? (errorIndex_as_int+1)%(varList.getSize()+1) : errorIndex_as_int%(varList.getSize()+1);

        //if(errorIndex_as_int_temp%responseVarList.getSize()==0) errorIndex_as_int++;

        
        VarList responseVarList = new VarList();
        
        if (!errorStatus.equals(new snmpInteger(0))) {
          responseVarList = varList;
        }
        else {
           
          final snmpOctetString valorConstante = new snmpOctetString("Hasta luego Lucas!");
          Var dummy, responseVar;
        
          switch (m.getPdu().getTag()) {
        
          case Pdu.GET :
            for (int i=0; i< varList.getSize(); i++) {
              dummy = varList.elementAt(i);          
              responseVarList.addVar(new Var(dummy.getName(), valorConstante));
            }
            break;
        
          case Pdu.GET_NEXT :
            for (int i=0; i< varList.getSize(); i++) {
              dummy = varList.elementAt(i);
              String oid = dummy.getName().toString();
              int ultimoPunto = oid.lastIndexOf('.');
              ultimoPunto++;
              int n = Integer.valueOf(oid.substring(ultimoPunto)).intValue();
              n++;
              oid = oid.substring(0, ultimoPunto) + new Integer(n).toString();
            
              Oid OID = new Oid(oid);
              responseVarList.addVar(new Var(OID, valorConstante));
            }
            break;
          
          case Pdu.SET :
            responseVarList = varList;
            break;          
                                        
                                
          }
        
        } 
        errorIndex = new snmpInteger( errorIndex_as_int_temp);
        
        Pdu pdu = new Pdu(Pdu.RESPONSE, requestId, errorStatus, errorIndex, responseVarList); 
        m = new Message(community, pdu);
        
        /*System.out.println("Message responded:");
        buf = new StringBuffer();
        m.println("",buf);
        System.out.println(buf);
        */ 
        
        b = m.codeBer(); 
        
        DatagramPacket packet = new DatagramPacket(b, b.length, master, clientPort);    
               
        sock.send(packet);        

      }
    } 
    catch (Exception e){
        System.err.println(e);
        System.exit(1);
    } 

  } //run  
  
} //class Agent
