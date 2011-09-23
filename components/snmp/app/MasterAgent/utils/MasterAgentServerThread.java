
//---------------- MasterAgentServerThread.java

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

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.JOptionPane;

import sck.*;


/** This is the thread that is started by the object of class MasterAgent
 *  every time an SNMP message arrives, and deals with the just arrived message
 *  by creating a hashtable called toSend, whose key is one of the port numbers
 *  included in slaveAgentsPorts, and whose value is a VarList object that contains
 *  all the variables of the original recieved message that must be sent to that
 *  port.
 * 
 *  @author Oscar J. Martin Vega
 *
 */
class MasterAgentServerThread extends Thread {
    byte[] b = new byte[1024];
    int packetLength;
    
    InetAddress master;
    int clientPort;
    
    Vector orderedPrefixes = new Vector();
    
    int i;

/** Constructor of the thread.
 *  @param b array of bytes where the SNMP message has been stored
 *  @param packetLength, length of the message in bytes.
 */
     public MasterAgentServerThread(byte[] b, int packetLength, InetAddress master, int clientPort) {
       this.b = b;
       this.packetLength = packetLength;
       this.master = master;
       this.clientPort = clientPort;
        
       try {
        orderedPrefixes = OidPrefix.sort(MasterAgent.slaveAgentsPorts.keys());                 
       } catch(java.io.InvalidObjectException e) {
            System.out.println("\n\n\tFatal error: invalid Oid prefix in configuration file!!\n");
            System.out.println("This error has ocurred because the contents of the configuration file\nagentports.dat have been externally changed in a wrong way.\n");
            System.out.println("You should delete the file agentports.dat and run the master agent again; it\nwill ask you to introduce the configuration information again.\n\n");          
            System.exit(0);
         } 
     }
/** Actions performed during execution.
 */
     synchronized public void run() {
        try {
          ByteArrayInputStream ber =  new ByteArrayInputStream(b,1,packetLength - 1);
                                                                                                                          // without tag, like all smi object !
          Message m = new Message(ber);

          snmpOctetString community = new snmpOctetString( m.getCommunity() );
          byte pduTag = m.getPdu().getTag();
          snmpInteger requestId = new snmpInteger(m.getPdu().getRequestId() );
          snmpInteger errorStatus = new snmpInteger(m.getPdu().getErrorStatus() );
          snmpInteger errorIndex = new snmpInteger(m.getPdu().getErrorIndex() );

          VarList requestVarList = m.getPdu().getVarList();
          
          VarList responseVarList = new VarList(); //The responded varlist, yet to be created           
          
          //Enumeration keys;
          Hashtable toSend = new Hashtable();  /* key =(String) puerto destino; valor= VarList a enviar         
                                                * expresada como vector de Var's    */
          boolean allPrefixesRecognized = true; //If any prefix is not recognized. it becomes false
                                               
          // toSend construction

          for ( i =0 ;  i < requestVarList.getSize() ;  i++  ) {
            //keys = MasterAgent.slaveAgentsPorts.keys();
            Var variable =  requestVarList.elementAt(i);
            Oid oid = variable.getName();
            String prefix;
            boolean samePrefix;
            /*  do {
                  prefix = new String( (String) keys.nextElement() );
                  samePrefix = oid.sameTree(prefix);
              } while( (keys. hasMoreElements() ) & ( ! samePrefix ) );
            */
            int j = 0;
            do {
              prefix = (String) orderedPrefixes.get(j).toString();
              samePrefix = oid.sameTree(prefix);
              j++;
            } while( ( j < orderedPrefixes.size()) & ( ! samePrefix ));
            
            if ( ! samePrefix ) {                   
              // This Var has a prefix not included in slaveAgentsPorts              
              
               // Case get-next
              if (pduTag == PduInterface.GET_NEXT) {
                // oid
                int k = 0;  
                 
                // By now we eliminate the case default              
                Vector orderedPrefixesWithoutDefault = orderedPrefixes;
                if(orderedPrefixes.contains(new String("default")))
                  orderedPrefixesWithoutDefault.removeElement(new String("default"));
                
                // If the only configuration information was the default String,
                // don't execute the while, and jump to enter the next if!
                if (orderedPrefixesWithoutDefault.size() == 0)
                  k = orderedPrefixesWithoutDefault.size(); // So the condition of the next if will be met. ( 0 - 1 = -1 )
                else {       
                  while ( (((OidPrefix) orderedPrefixesWithoutDefault.get(k)).minorThan(oid.toString())) & (k < orderedPrefixesWithoutDefault.size()-1) ) {                     
                     k++;                                 
                  } 
                }                  
                            
                if (k == orderedPrefixesWithoutDefault.size()) {
                
                  // If there is a default port number, send it it to it.
                  if (MasterAgent.slaveAgentsPorts.containsKey(new String("default"))) {
                    //send it to the default port number
                    String _port = (String) MasterAgent.slaveAgentsPorts.getProperty(new String("default"));

                    VarList _varList;
 
                    if (toSend.get(_port) != null) _varList =(VarList) toSend.get(_port);
                    else _varList = new VarList();
  
                    _varList.addVar(variable);
                    toSend.put(_port, _varList);

                    continue;                  
                  }
                  else {
                    // send an error response indicating noSuchName
                    allPrefixesRecognized = false;
                    
                       // Generate message                     
                    errorStatus = new snmpInteger(2);
                    errorIndex = new snmpInteger(requestVarList.indexOf(variable));
                    responseVarList = requestVarList; //The responded varlist, yet to be created                    
                
                    break;
                  }  
                }
                else {
                    //send it to the desired port number
                String _port = 
                    (String) MasterAgent.slaveAgentsPorts.getProperty((String) ((OidPrefix)orderedPrefixes.get(k)).toString() );

                VarList _varList;

                if (toSend.get(_port) != null) _varList =(VarList) toSend.get(_port);
                else _varList = new VarList();
  
                _varList.addVar(variable);
                toSend.put(_port, _varList);

                continue;

                }
              }
              else if (MasterAgent.slaveAgentsPorts.containsKey(new String("default"))) {
                  //send it to the default port number
                String _port = (String) MasterAgent.slaveAgentsPorts.getProperty(new String("default"));

                VarList _varList;

                if (toSend.get(_port) != null) _varList =(VarList) toSend.get(_port);
                else _varList = new VarList();
  
                _varList.addVar(variable);
                toSend.put(_port, _varList);

                continue;
              }
              else  {                                 
                   // Responds the master with a message indicating a noSuchName error in this var                                                                               
                allPrefixesRecognized = false;
                    
                   // Generate message                     
                errorStatus = new snmpInteger(2);
                errorIndex = new snmpInteger(requestVarList.indexOf(variable));
                responseVarList = requestVarList; //The responded varlist, yet to be created                    
                
                break;
              }
            }
                 
                // Now we are sure the Oid prefix is included in Properties object slaveAgentsPorts
              String _port = (String) MasterAgent.slaveAgentsPorts.getProperty(prefix);

              VarList _varList;

              if (toSend.get(_port) != null) _varList =(VarList) toSend.get(_port);
              else _varList = new VarList();

              _varList.addVar(variable);
              toSend.put(_port, _varList);

          } // for          
        
            if (allPrefixesRecognized) {            
            

                //responses is an array which will be filled with the responded messages
                //from sub-agents; it's up to the subagent communication threads to
                //fill it. It will be used to construct the errorStatus and
                //errorIndex field on the returned message. 
            Vector responses = new Vector(toSend.size());
         
                 //Each error status of the received messages is stored in
                 //this array. It will be used to construct the errorStatus 
                 //and errorIndex field on the returned message.               

            Vector errorStatusOfEachPdu = new Vector(toSend.size());
          
                 //We'll construct the varList to be returned to the master, 
                 //using the requested array    
                 
            Var[] variablesRespondidas = new Var[requestVarList.getSize()];     
                             
                 //Used to construct respondVarList
            int[] positionsInResponseVarList;
          
            Enumeration ports = toSend.keys();
            Pdu finstro;
            Var pecadorl;            
 
            while (ports.hasMoreElements() ) {
              String port2slaveString = (String) ports.nextElement();
              int port2slave =  Integer.valueOf(port2slaveString).intValue();
              VarList varList2slave = (VarList) toSend.get(port2slaveString);
                   
                   //Construct positionsInResponseVarList
              positionsInResponseVarList = new int[varList2slave.getSize()];                                      
              
              for (i=0; i<varList2slave.getSize(); i++) {          
                pecadorl = varList2slave.elementAt(i);
                positionsInResponseVarList[i] = requestVarList.indexOf(pecadorl);                                          
              }  
                   //Create pdu sent locally to the corresponding slave agent
              Pdu pdu2slave = new Pdu(pduTag,requestId,errorStatus,errorIndex,varList2slave);
 
                         //Create SNMP message sent locally to the corresponding slave agent
              Message message2slave = new Message(community,pdu2slave);         
                         
              finstro = (new SubAgentCommunicator(message2slave,port2slave)).run() ;
              responses.addElement(finstro);                          
              errorStatusOfEachPdu.addElement( new Integer( ((Pdu)responses.lastElement()).getErrorStatus() ));                                                         
           
              VarList torpedo = finstro.getVarList();                          
              for (i=0; i<varList2slave.getSize(); i++) {              
                variablesRespondidas[positionsInResponseVarList[i]] = torpedo.elementAt(i);              
              }              

            } //while            
                                    
                              
             //Rellenar campos de error 
          int numeroDePduQueCausoError = -1; //Por darle un valor inicial cualquiera
          if (errorStatusOfEachPdu.contains(new Integer(2))) {
            //errorStatus = noSuchName 
            errorStatus = new snmpInteger(2);           
            numeroDePduQueCausoError = errorStatusOfEachPdu.indexOf(new Integer(2));                              
          }
          else if (errorStatusOfEachPdu.contains(new Integer(3))) {
            //errorStatus = badValue
            errorStatus = new snmpInteger(3);          
            numeroDePduQueCausoError = errorStatusOfEachPdu.indexOf(new Integer(3));                   
          }
          else if (errorStatusOfEachPdu.contains(new Integer(4))) {
            //errorStatus = readOnly
            errorStatus = new snmpInteger(4);          
            numeroDePduQueCausoError = errorStatusOfEachPdu.indexOf(new Integer(4));                       
          }
          else if (errorStatusOfEachPdu.contains(new Integer(1))) {
            //errorStatus = tooBig
            errorStatus = new snmpInteger(1);
            numeroDePduQueCausoError = errorStatusOfEachPdu.indexOf(new Integer(1));                            
          }
          else if (errorStatusOfEachPdu.contains(new Integer(5))) {
            //errorStatus = genErr
            errorStatus = new snmpInteger(5);
            numeroDePduQueCausoError = errorStatusOfEachPdu.indexOf(new Integer(5));                
          }           
          else if (errorStatusOfEachPdu.contains(new Integer(0))) {
            //errorStatus = noError
            errorStatus = new snmpInteger(0);             
          } 
           
              //Construct errorIndex            
          if (errorStatus.equals(new snmpInteger(0))) {
                // if noError
                
             //Create responseVarList             
            responseVarList = new VarList(variablesRespondidas); //The responded varlist
    
            errorIndex = new snmpInteger(0);  
            
                  
          }
          else { //if exists an error
          
              //The returned varList is the same than the varList of the request
            responseVarList = requestVarList;
 
            Pdu pduErronea = (Pdu) responses.get(numeroDePduQueCausoError);          
            VarList varListErronea = pduErronea.getVarList();                 
            Var variableErronea = varListErronea.elementAt(pduErronea.getErrorIndex());
        
            errorIndex = new snmpInteger((new VarList(variablesRespondidas)).indexOf(variableErronea));            
          }           
                       
        }  //del if (allPrefixesRecognized)      
                
            //Construct the respond message              
        Pdu pdu2master = new Pdu(Pdu.RESPONSE, requestId,  errorStatus, errorIndex, responseVarList);        
        Message message2master = new Message(community, pdu2master); 
        
            //send it
        b = new byte[1024];    
        b = message2master.codeBer();
        DatagramPacket packet = new DatagramPacket(b, b.length, master, clientPort);    
               
        DatagramSocket socket = new DatagramSocket();
        //socket.setSoTimeout(3000);  //TIMEOUT = 3 seconds 
        
        try {
          socket.send(packet);
        } catch (IOException e) {
            System.err.println("An error ocurred while sending the response message to the master.");
            System.err.println(e.toString());
            System.exit(0);
        }                     
                   
        }catch(InterruptedIOException e2) {
          /* This exception is thrown when one of the objects of class
           * ubAgentCommunicator has not received a response from the
           * slave agent 3 minutes after it sent to it the corresponding
           * request -that is the setSoTimeOut value of 1800 ms-. This can
           * happen in two situations: if the authentication service of the
           * subagent thought the information included in the request message
           * was wrong (tipically: when the community name is not valid);
           * the second chance is that the subagent does not recognize the 
           * PDU within the message. The second case will never happen 
           * because the master agent discards any wrong PDU before 
           * re-sending it to the slave agent. In the first situation the
           * slave agent will generate a trap (which is not handled by
           * the master agent, just like any other trap, because traps are
           * always sent to the IP address of the master included in the
           * slave agent's configuration files. What to do in this case?
           * The answer is NOTHING!!
           */             
                                               
        }catch(IOException e3) {

        /* The expected syntax, as defined in SMI's rules for message creation,
         * has not been satisfied by the received message, or the SNMP version
         * is not version 1 so it will be discarded and no more actions will be
         * done
         */

         // A Swing window will alert the user of this fact:
            JOptionPane.showMessageDialog(null,
                              "Error: received message does not comply\nSNMPv1 protocol's rules, so it will be discarded.   ",
                              "Master agent",
                              JOptionPane.ERROR_MESSAGE);

          }catch (ArrayIndexOutOfBoundsException e4){
            System.err.println(e4.toString());
            System.exit(0);
          }
          }         
       
  public void notifica() {
    notify();
  }     

}

 class SubAgentCommunicator  {

  Message requestMessage;
  int port;  
  
  DatagramSocket socket;   

  SubAgentCommunicator(Message requestMessage, int port) {
    this.requestMessage = requestMessage;
    this.port = port;    
  }

  public Pdu run() throws InterruptedIOException { 
    
    try { 
             // send it      Por fin!!!! 
      byte[] b1 = new byte[1024]; 
      byte[] b2 = new byte[1024];                 
      b1 = requestMessage.codeBer();           

      DatagramPacket requestPacket = new DatagramPacket(b1,b1.length,MasterAgent.local,port);
      DatagramPacket responsePacket = new DatagramPacket(b2, b2.length);
      socket = new DatagramSocket();       
 
      socket.setSoTimeout(180000);   //Si no recibe respuesta del subagente en 3 minutos, lanza una InterruptedIOException

      socket.send(requestPacket);         
      socket.receive(responsePacket);          
      
      ByteArrayInputStream ber = new ByteArrayInputStream(b2,1,responsePacket.getLength()-1); // without tag, like all smi object !
      Message responseMessage = new Message(ber);
                
      return responseMessage.getPdu();        
        
    }catch (IOException e2) {
        //If the requestSocket fails
      System.err.println("The local datagramSocket between the master agent and a slave agent has falied.\n");  
      System.err.println(e2.toString());
      System.exit(0);
      return null;
    }
  }
  
  protected void finalize() {
    socket.close();   
  }
 
}  //class SubAgentCommunicationThread

