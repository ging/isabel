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
//************************************************************************
//
//FILE: internalCommsServer_t
//
//DESCRIPTION:
//    This thread has to receive all the data the different Isabel modules
//send to the monitor and include it in the database.
//

import java.net.*;
import java.io.*;

public class internalCommsServer_t extends java.lang.Thread {

    int               internalPort;
    threadsManager_t  threadsManager;
    DatagramSocket    internalSocket;
    DatagramSocket    visualizationSocket;
    DataBaseHandler_t DataBaseHandler;
    boolean           finish;

    internalCommsServer_t (int internalPort,
                           threadsManager_t threadsManager,
                           DataBaseHandler_t DataBaseHandler
                          )
                          throws java.net.SocketException {
        this.internalPort    = internalPort;
        this.threadsManager  = threadsManager;
        this.DataBaseHandler = DataBaseHandler;
        finish               = false;

        // communication with other Isabel's modules.
        internalSocket = new DatagramSocket (internalPort);
        internalSocket.setSoTimeout(1000);

    }

    public void run() {

        //I start listening at the UDP port:
        //----------------------------------
        while ((threadsManager.doIkeepAlive())&&(!finish)) {
            try {

                //1.- I get what arrives to the port:
                //-----------------------------------
                byte[] internalData = new byte[1024];
                DatagramPacket internalDataPkt = new DatagramPacket (internalData,internalData.length);
                internalSocket.receive(internalDataPkt);

                //System.out.print("Received "
                //  + internalDataPkt.getLength() + " bytes");


                //2.- I analize the contents of the datagram:
                //-------------------------------------------
                ByteArrayInputStream Bst = new ByteArrayInputStream (internalDataPkt.getData());
                DataInputStream dst = new DataInputStream (Bst);
                byte type = dst.readByte();
                switch (type) {
                    case (0):
                        AnalizeControlMessage(dst);
                        break;
                    case (1):
                        AnalizeDataMessage(dst);
                        break;
                    default:
                }

            } catch (java.io.InterruptedIOException e) {
                //Scoket TimeOut.
                //Actualizate the channels TTL:
                DataBaseHandler.IncrementAllChannelsTTL();
                //See if I must delete any channel:
                //DataBaseHandler.DeleteDeadChannels();
            } catch (java.net.SocketException e) {
                System.err.println("Excepción capturada:"+e);

            } catch (java.io.IOException e) {
                System.err.println("Excepción capturada:"+e);
            }

        }
    }

    //function to analize control messages from an Isabel's module:
    //-------------------------------------------------------------
    void AnalizeControlMessage (DataInputStream dst) {
        try{

            // 1.- read the diferent fields of the packet:
            byte [] componentId = new byte [3];
            dst.read(componentId,0,3);
            String compId= new String (componentId);
            int channelId= dst.readInt();

            //System.out.println(" CONTROL: chId=" + channelId +
            //                   " compId=" + compId);

            //2.- pass the data to the database
            DataBaseHandler.AnalizeControlMessage(compId, channelId);

        } catch (java.io.IOException e) {
            System.out.println("Excepcion en AnalizeCtrlMessage");
            System.out.println(e);
        }
    }

    //Function to analize data messages from an Isabel's module:
    //----------------------------------------------------------
    void AnalizeDataMessage(DataInputStream dst) {
        try {
            // 1.- read the diferent fields of the packet:
            //
            // -----------------------------
            // | compId | channelId | Data |
            // -----------------------------
            byte [] componentId = new byte [3];
            dst.read(componentId,0,3);
            String compId= new String (componentId);
            int channelId= dst.readInt();

            //System.out.println(" DATOS: chId=" + channelId +
            //                   " compId=" + compId);

            //2.- pass the data to the database
            //---------------------------------
            DataBaseHandler.AnalizeDataMessage(compId, channelId, dst);

        } catch (java.io.IOException e) {
            System.out.println("Excepcion en AnalizeDataMessage");
            System.out.println(e);
        }
    }

    //Function to stop this thread:
    //-----------------------------
    public void setFinish(boolean value) {
        finish = value;
    }
}

