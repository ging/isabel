
//--------------------- VManager.java

// This program is free software; you can redistribute it and/or
// modify it under the terms of the Affero GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details (cf. file COPYING).


import java.net.*;
import java.io.*;
import java.util.*;

import sck.*;

/** Devoted to testing purposes, it implements a virtual manager.
 *  @author Oscar J. Martin Vega
 */

public class VManager {

  public static void usage(){
    System.err.println("\n\nUsage: java VManager [[next] [set][get]] [-h]  [-p port]  [-t timeOut] [-c community] [-oid objectIdentifier] -dest targetHost\n");
    System.err.println("set or get-next : kind of request, default get");
    System.err.println("-h help");
    System.err.println("-p port : destination port, default = 161");
    System.err.println("-t timeOut: socket's timeOut in milliseconds");
    System.err.println("-c community : read community, default = public");
    System.err.println("-oid objectIdentifier : get objectIdentifier, default = 1.3.6.1.2.1.1.1.0 (sysDescr). You can add several Oid's here");
    System.err.println("-dest target host : target agent host");
    System.err.println("\n   If invoked with no params, sends a get-request for the sysDescr of the local host");
    System.exit(0);
  }

  public static void main(String argv[]){

  int snmpPort = 161;
  int snmpTimeout  = 3000; // 3000 ms
  byte tag = Pdu.GET;
  String community = new String("public");
  Message request;
  String host = new String("");
  VarList varList = new VarList() ;

  boolean oidParamInvoked = false;

  try{

  int i = -1;
    while (++i < argv.length) {
      if (argv[i].equals("next")) {   
        tag = Pdu.GET_NEXT;    
      }
      if (argv[i].equals("set")) {
        tag = Pdu.SET;       
      }
      if (argv[i].equals("-h")||(argv[i].equals("-help"))){
        usage();
      }
      if (argv[i].equals("-p")){
          snmpPort  = java.lang.Integer.valueOf(argv[++i]).intValue();
          continue;
      }
      if (argv[i].equals("-t")){
          snmpTimeout  = java.lang.Integer.valueOf(argv[++i]).intValue();
          continue;
      }
      if (argv[i].equals("-c")){
          community  = argv[++i];
          continue;
      }
      if (argv[i].equals("-oid")){
         oidParamInvoked = true;
         int j = i+1;
         while (( j < argv.length)&(argv[j].charAt(0) != '-')) {
              String oid =  argv[++i];
              if (tag==Pdu.SET) {
                varList.addVar( new Var(new Oid(oid), new snmpOctetString("Que pacha Pepe!")));
              }
              else {
                varList.addVar( new Var(oid) );
              }  
              j++;
              }
         if ( i == argv.length ) break;         
         continue;
      }
     if (argv[i].equals("-dest")) {
         host = argv[++i];
     }
    }

  } catch (Exception e){
    System.out.println(e);
    usage();
    }

  if (host.equals("") ) {
    try {
          host = InetAddress.getLocalHost().getHostName();
              
    } catch (UnknownHostException e) {
             System.err.println("Error: you do not have any IP address");
      }
  }


  try { 
    InetAddress snmpHost = InetAddress.getByName(host);
    DatagramSocket sock = new DatagramSocket();
    //sock.setSoTimeout(snmpTimeout);

    // create pdu.           
               
  
    snmpOctetString c = new snmpOctetString(community);
    snmpInteger requestId = new snmpInteger(0);
    snmpInteger errorIndex = new snmpInteger(0);
    snmpInteger errorStatus = new snmpInteger(0);
    if (! oidParamInvoked)  varList.addVar( new Var("1.3.6.1.2.1.1.1.0") );
    Pdu pdu = new Pdu(tag ,requestId,errorStatus,errorIndex, varList);
    Message m = new Message(c,pdu);

    // send
    byte[] b = m.codeBer();
    DatagramPacket packet = new DatagramPacket(b,b.length,snmpHost,snmpPort);    
   
    sock.send(packet);      
        
    Handler.listen(sock);
   
       
    } catch (UnknownHostException ex){ 
      System.out.println(host + ": unknown host." );   
    } catch ( Exception ex) { 
      System.out.println("Error in : ");
      System.out.println(ex);
    }
    System.exit(0);
  }  
}

