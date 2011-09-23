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
//**********************************************************************
//
//FILE: threadsManager_t.java:
//
//        this is the class which manages the threads, that means, is
// responsable of the correct dead of all of them when the program
// finish.
//

import java.util.*;
public class threadsManager_t {

    //Attributes:
    //-----------

    DataBaseHandler_t      DataBaseHandler;
    SNMPServer_t           SNMPServer;
    ctrlServer_t           ctrlServer;
    internalCommsServer_t  internalServer;
    MonitorGUI_t           MonitorGUI;
    Vector                 frames;

    boolean keepAllAlive;

    boolean noexitbutton;

    //Constructor:
    //------------

    threadsManager_t (DataBaseHandler_t DataBaseHandler, boolean noexitbutton) {
        this.DataBaseHandler = DataBaseHandler;
        SNMPServer           = null;
        ctrlServer           = null;
        internalServer       = null;
        MonitorGUI           = null;
        frames               = new Vector();
        keepAllAlive         = true;
        this.noexitbutton    = noexitbutton;
    }

    //Functions:
    //----------

   //1.- set-functions:
    void setSNMPServer (SNMPServer_t SNMPServer) {
        this.SNMPServer = SNMPServer;
    }
    void setCtrlServer (ctrlServer_t ctrlServer) {
        this.ctrlServer = ctrlServer;
    }
    void setInternalServer (internalCommsServer_t internalServer) {
        this.internalServer = internalServer;
    }
    void setMonitorGUI (MonitorGUI_t MonitorGUI) {
        this.MonitorGUI = MonitorGUI;
    }

    //2.- get-functions: if there is no thread of that kind initializated,
    //        those functions will return null.
    SNMPServer_t getSNMPServer ()              { return SNMPServer;      }
    ctrlServer_t getCtrlServer ()              { return ctrlServer;      }
    internalCommsServer_t getInternalServer () { return internalServer;  }
    MonitorGUI_t getMonitorGUI ()              { return MonitorGUI;      }
    DataBaseHandler_t getDataBaseHandler()     { return DataBaseHandler; }
    boolean noExitButton()                     { return noexitbutton;    }

    //3.- functions to inform of the death of a thread:
    void SNMPServerOff     () { SNMPServer = null;     }
    void CtrlServerOff     () { ctrlServer = null;     }
    void InternalServerOff () { internalServer = null; }
    void MonitorGUIOff     () { MonitorGUI = null;     }

    //4.- functions to control threaded-frames:
    //    If any of those has started another thread for himself, it will be
    //responsible of its management.
    int countFrames () { return frames.size(); }
    void insertFrame (FrameRef frame) {
        frames.add(frame);
    }
    void deleteFrame (FrameRef frameToDelete) {
        frames.removeElement(frameToDelete);
    }

    void deleteAllFrames() {
        Object [] FrameRefArr  = frames.toArray();
        for (int i=0 ; i<FrameRefArr.length ; i++ ) {
            ((FrameRef)FrameRefArr[i]).setFinish(true);
        }
        frames.clear();
    }

    //5.- functions to control all threads at once:
    void setKeepAllAlive (boolean keepAllAlive) {this.keepAllAlive=keepAllAlive; }
    boolean doIkeepAlive() { return keepAllAlive; }
   void killAll () {
        keepAllAlive=false;
        //System.exit(0);
    }

    //6.- functions to re-launch the threads:
    void SNMPServerOnAgain (int SNMPPort) throws java.net.SocketException {
        SNMPServer_t auxSNMPServer = new SNMPServer_t (SNMPPort,this,DataBaseHandler);
        SNMPServer.setFinish(true);
        SNMPServer = null;
        System.out.println("SNMPServer parado     [ OK ]");
        SNMPServer = auxSNMPServer;
        SNMPServer.start();
        System.out.println("SNMPServer relanzado  [ OK ]");
    }

    void InternalServerOnAgain(int internalPort) throws java.net.SocketException {
        internalCommsServer_t auxIntrServer = new internalCommsServer_t (internalPort,this,DataBaseHandler);
        internalServer.setFinish(true);
        internalServer = null;
        System.out.println("InternalServer parado     [ OK ]");
        internalServer = auxIntrServer;
        internalServer.start();
        System.out.println("InternalServer relanzado  [ OK ]");
    }
}//End of serversManager_t

