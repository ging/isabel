/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
//*********************************************************************
//
//FILE: SNMPServer_t
//
//DESCRIPTION:
//
//    Thread who manages the SNMP petitions from managers who ask
// for data. It's a SNMP version 1 agent with only the GET and
//    GET_NEXT primitives implemented.
//    No SET primitives are acepted.
//

import java.net.*;
import java.io.*;

public class SNMPServer_t extends java.lang.Thread {

    //Attributes:
    //-----------

    int                     SNMPPort;
    threadsManager_t     threadsManager;
    DatagramSocket     SNMPSocket;
    DatagramSocket     SNMPSocketToSend;
    String                 PublicCommunity;
    String                 PrivateCommunity;
    String                 ReceivedCommunity;
    boolean                 publicAccess;
    boolean                 privateAccess;
    DataBaseHandler_t DataBaseHandler;
    boolean                 finish;
    //Constructor:
    //------------
    SNMPServer_t (int SNMPPort,
                  threadsManager_t threadsManager,
                  DataBaseHandler_t DataBaseHandler
                 )
                 throws java.net.SocketException {
        this.SNMPPort            = SNMPPort;
        this.threadsManager      = threadsManager;
        this.PublicCommunity     = "public";
        this.PrivateCommunity    = "Isabel";
        this.publicAccess        = false;
        this.privateAccess       = false;
        this.DataBaseHandler     = DataBaseHandler;
        this.finish              = false;

        SNMPSocket = new DatagramSocket (SNMPPort);
        SNMPSocketToSend = new DatagramSocket();
        SNMPSocket.setSoTimeout(800);
    }

    public void run() {

        //If I've been able to open the socket, I start to listen in
        //that port.
        int          LastRequestId = 0;
        int          RequestId;
        InetAddress  ManagerIP;
        int          ManagerPort;
        int          LastManagerPort=0;
        int          ErrorStatus;
        int          ErrorIndex;

        while ((threadsManager.doIkeepAlive())&&(!finish)) {
            try {
                //1.- I listen for any message that may arrive:
                //---------------------------------------------
                byte[] SNMPData = new byte[1024];
                DatagramPacket SNMPDataPkt = new DatagramPacket (SNMPData,SNMPData.length);
                SNMPSocket.receive(SNMPDataPkt);
                ManagerIP = SNMPDataPkt.getAddress();
                ManagerPort = SNMPDataPkt.getPort();

                //2.- I analize the content of the datagram:
                //------------------------------------------
                ByteArrayInputStream ber =new ByteArrayInputStream(SNMPData,1,SNMPDataPkt.getLength()-1);
                sck.Message SNMPMessage = new sck.Message(ber);

                ReceivedCommunity=SNMPMessage.getCommunity();
                sck.PduCmd ReceivedPDU = (sck.PduCmd) SNMPMessage.getPdu();
                sck.VarList ReceivedList = ReceivedPDU.getVarList();
                RequestId = ReceivedPDU.getRequestId();

                //2.1.- Don't answer to two or more petitions that come sucessive:
                //----------------------------------------------------------------
                if ((RequestId==LastRequestId)&&(LastManagerPort==ManagerPort)) {
                        //System.out.println("Repetition");
                        continue;
                        }
                ErrorStatus = 0;
                ErrorIndex = 0;

                //3.- Security policy:
                //--------------------
                if (ReceivedCommunity.equals(PublicCommunity)) publicAccess=true;
                if (ReceivedCommunity.equals(PrivateCommunity)) privateAccess=true;
                if ((!publicAccess)&&(!privateAccess)) continue;

                //4.- Analize the SNMP directive and get the response:
                //----------------------------------------------------
                byte            PDUType             = ReceivedPDU.getTag();
                int             NumOfVars         = ReceivedList.getSize();
                sck.Var []     ResponseList     = new sck.Var [NumOfVars];

                switch (PDUType) {//Decide the type of the primitive:

                    case (sck.Pdu.GET): {
                        int i = 0;
                        while ((NumOfVars>0)&&(ErrorIndex==0)) {
                            sck.Oid RequestedOid = ReceivedList.elementAt(i).getName();
                            /*if (DataBase_t.contains(RequestedOid)) ResponseList[i]=new sck.Var(RequestedOid,DataBaseHandler.getData(RequestedOid));
                            else {
                                    ErrorIndex=i;
                                    ErrorStatus=2; //No such element in the MIB.
                                    ResponseList[i]=new sck.Var (RequestedOid.toString());
                                    break; //I don't try to answer any of the other variable the manager asked in this message.
                                }*/
                            NumOfVars=NumOfVars-1;
                            i=i+1;
                        }//End of Get_While
                        break;
                    }//End of Get

                    case (sck.Pdu.GET_NEXT): {
                        int i=0;
                        while ((NumOfVars>0)&&(ErrorIndex==0)) {
                            sck.Oid RequestedOid = ReceivedList.elementAt(i).getName();
                            sck.Oid ResponseOid = DataBase_t.getNext(RequestedOid);
                            if (ResponseOid == null) {
                                //System.out.println("NO TENGO ESA VARIABLE");
                                ResponseList[i]=new sck.Var (RequestedOid.toString());
                                ErrorStatus=2;
                                ErrorIndex=i;
                                break;
                                }
                            else {
                                RequestedOid = ResponseOid;
                                ResponseList[i] = new sck.Var (RequestedOid,DataBaseHandler.getData(RequestedOid));
                                if (ResponseList[i].getValue()==null) {//If I can't answer to this variable:
                                    //System.out.println("Entro porque la respuesta es nula");
                                    ErrorIndex=i;
                                    ErrorStatus=2;
                                    break;
                                }
                            }
                            NumOfVars=NumOfVars-1;
                            i=i+1;
                        }//End of Get_Next_While
                        break;
                    }//End of Get_Next

                    case (sck.Pdu.SET): {
                        System.out.println("Ha llegado un SET");
                        //Not implemented: maybe in the future.
                        break;
                    }//End of Set

                    default: {//Something that we can't recognise
                        //System.out.println("No sabemos que ha llegado");
                    }//End of default
                }//End of switch

                sck.VarList Response = new sck.VarList (ResponseList);

                //5.- I answer to the manager sending a UDP datagram:
                //---------------------------------------------------
                sck.PduCmd ResponsePDU=new sck.PduCmd (sck.Pdu.RESPONSE,new sck.Integer(RequestId),new sck.Integer(ErrorStatus),new sck.Integer(ErrorIndex), Response);
                sck.Message ResponseMessage = new sck.Message (new sck.OctetString(ReceivedCommunity), ResponsePDU);
                DatagramPacket DPEnvio=new DatagramPacket (ResponseMessage.codeBer(), ResponseMessage.codeBer().length, ManagerIP, ManagerPort);
                SNMPSocketToSend.send(DPEnvio);

                //6.- Actualizations concerning with detecting repetitions:
                //---------------------------------------------------------
                LastRequestId=RequestId;
                LastManagerPort=ManagerPort;

            }catch (java.io.InterruptedIOException e) {
                //This is catch when the timeout of the socket
                //happens. This is necesary for the thread to
                //die.
                publicAccess=false;
                privateAccess=false;
                LastManagerPort=0;

            }catch (java.net.SocketException e) {
                publicAccess=false;
                privateAccess=false;
                System.err.println("Excepción capturada 2:"+e);
                System.exit(0);

            }catch (java.io.IOException e) {
                publicAccess=false;
                privateAccess=false;
                System.err.println("Excepción capturada 3:"+e);
            }

            publicAccess=false;
            privateAccess=false;

        }//End of while
    }//End of run
    public void setFinish(boolean value) {
        finish=value;
    }
}//End of ctrlServer_t
