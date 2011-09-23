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
//******************************************************************
//
//FILE: shDisplayDataBase_t.java
//
//DESCRIPTION:
// This file contains the classes that let me store the data of the
// shdis module of ISABEL. All the threads access this classes via
// the shDisplayDataBaseHandler, via the DataBaseHandler.
//


import java.io.*;
import java.util.*;

public class shDisplayDataBase_t extends DataBase_t {

    // Number of columns,
    int rowElements = shDisplayTableModel_t.SHDIS_STATS.length;

    Hashtable shDisplayTable;

    shDisplayDataBase_t () {
        super();
        shDisplayTable = new Hashtable();
    }

    //Function that give information about the MIB elements for the Monitor GUI:
    //--------------------------------------------------------------------------

    // This function is used by the Monitor_GUI when a graphic is required.
    // It maps from the position on the visual table
    // to the real channel number in the MIB.
    int getChIndexFromPos (int pos) {
        Object [] keys = shDisplayTable.keySet().toArray();
        return ((Integer)keys[pos]).intValue();
    }

    // This function tells the visual table how many rows it will have to paint.
    int countEntries() {
        return shDisplayTable.size();
    }

    //Function for deleting the data that hasn't been actualizated:
    //-------------------------------------------------------------

    // If time goes by and nobody sends information about the shared display,
    // the ttl will be incremented for all the channels.
    void IncrementAllChannelsTTL() {
        Enumeration keys= shDisplayTable.keys();
        while(keys.hasMoreElements()) {
            Integer auxkey = (Integer)keys.nextElement();
            shDisplayEntry_t auxEntry = (shDisplayEntry_t)shDisplayTable.get(auxkey);
            if ((auxEntry.getshDisplayTTL())<10) {
                auxEntry.incrementTTL();
                shDisplayTable.put(auxkey,auxEntry);
            } else {
                deleteChannel(auxkey);
            }
        }
    }

    // If a channel is not actualized for a long time
    // it will be deleted from de MIB with this function.
    boolean deleteChannel (Integer shdChIndex) {
        if (shDisplayTable.containsKey(shdChIndex)) {
            shDisplayTable.remove(shdChIndex);
            delOldOids(shdChIndex.intValue());
            return true;
        }
        return false;
    }

    boolean deleteChannel (int shdChIndex) {
        if (shDisplayTable.containsKey(new Integer(shdChIndex))) {
            shDisplayTable.remove(new Integer(shdChIndex));
            delOldOids(shdChIndex);
            isSorted=false;
            return true;
        }
        return false;
    }

    // Private functions to add and delete Oids to the static vector:
    // --------------------------------------------------------------

    // This function add the new MIB elements to a vector that is used when
    // a GET_NEXT request arrives to the agent.
    private void addNewOids (int shdChIndex) {
        try {
            for (int i=1; i<=rowElements; i++) {
                AddOid(new sck.Oid("1.3.6.1.4.1.2781.1.7.1.1."+i+"."+shdChIndex));
            }
        } catch (Exception e) {}
        isSorted=false;
    }

    // This function deletes the new MIB elements to a vector that is used when
    // a GET_NEXT request arrives to the agent.
    private void delOldOids (int shdChIndex) {
        try {
            for (int i=1;i<=rowElements;i++) {
                DelOid(new sck.Oid("1.3.6.1.4.1.2781.1.7.1.1."+i+"."+shdChIndex));
            }
        } catch (Exception e) {}
        isSorted=false;
    }


    // Functions to add new elements to the shared display MIB:
    // -----------------------------------------------

    void newShDisplayEntry (int shdChIndex) {
        shDisplayEntry_t shDisplayEntry = new shDisplayEntry_t (shdChIndex,null,null,0,null,0,0,0,0,0,0,0,0);
        shDisplayTable.put (new Integer(shdChIndex), shDisplayEntry);
        addNewOids (shdChIndex);
        isSorted=false;
    }

    void newShDisplayEntry (int    shdChIndex,
                            String shdAcronym,
                            String shdCodec,
                            int    shdQuality,
                            String shdImageSize,
                            int    shdDesiredFR,
                            int    shdDesiredBW,
                            int    shdCodecBW,
                            int    shdSentFR,
                            int    shdSentBW,
                            int    shdReceivedBW,
                            int    shdEnsembledFR,
                            int    shdPaintFR
                           ) {
        shDisplayEntry_t shDisplayEntry=
            new shDisplayEntry_t(shdChIndex,
                                 shdAcronym,
                                 shdCodec,
                                 shdQuality,
                                 shdImageSize,
                                 shdDesiredFR,
                                 shdDesiredBW,
                                 shdCodecBW,
                                 shdSentFR,
                                 shdSentBW,
                                 shdReceivedBW,
                                 shdEnsembledFR,
                                 shdPaintFR
                                );
        shDisplayTable.put (new Integer(shdChIndex), shDisplayEntry);
        addNewOids (shdChIndex);
        isSorted=false;
    }

    void setOrCreateNewShDisplayEntry(int    shdChIndex,
                                      String shdAcronym,
                                      String shdCodec,
                                      int    shdQuality,
                                      String shdImageSize,
                                      int    shdDesiredFR,
                                      int    shdDesiredBW,
                                      int    shdCodecBW,
                                      int    shdSentFR,
                                      int    shdSentBW,
                                      int    shdReceivedBW,
                                      int    shdEnsembledFR,
                                      int    shdPaintFR
                                     ) {
        shDisplayEntry_t auxShDisplayEntry=
            new shDisplayEntry_t(shdChIndex,
                                 shdAcronym,
                                 shdCodec,
                                 shdQuality,
                                 shdImageSize,
                                 shdDesiredFR,
                                 shdDesiredBW,
                                 shdCodecBW,
                                 shdSentFR,
                                 shdSentBW,
                                 shdReceivedBW,
                                 shdEnsembledFR,
                                 shdPaintFR
                                );
        if (shDisplayTable.containsKey(new Integer(shdChIndex))) {
            shDisplayTable.remove(new Integer(shdChIndex));
            shDisplayTable.put(new Integer(shdChIndex), auxShDisplayEntry);
        } else {
            newShDisplayEntry(shdChIndex,
                              shdAcronym,
                              shdCodec,
                              shdQuality,
                              shdImageSize,
                              shdDesiredFR,
                              shdDesiredBW,
                              shdCodecBW,
                              shdSentFR,
                              shdSentBW,
                              shdReceivedBW,
                              shdEnsembledFR,
                              shdPaintFR
                             );
        }
    }

    // Function to get individualy each of the values in each table
    // (used by the SNMP_Server via the DataBaseHandler):
    // ----------------------------------------------------------------

    sck.Integer getChIndex (int shdChIndex) {
        if (shDisplayTable.containsKey(new Integer(shdChIndex))) {
            shDisplayEntry_t auxShDisplayEntry = (shDisplayEntry_t) shDisplayTable.get(new Integer(shdChIndex));
            return auxShDisplayEntry.getshDisplayChIndex();
        }
        else {
            return null;
        }
    }

    sck.OctetString getAcronym (int shdChIndex) {
        if (shDisplayTable.containsKey(new Integer(shdChIndex))) {
            shDisplayEntry_t auxShDisplayEntry = (shDisplayEntry_t) shDisplayTable.get(new Integer(shdChIndex));
            return auxShDisplayEntry.getshDisplayAcronym();
        }
        else {
            return null;
        }
    }

    sck.OctetString getCodec (int shdChIndex) {
        if (shDisplayTable.containsKey(new Integer(shdChIndex))) {
            shDisplayEntry_t auxShDisplayEntry = (shDisplayEntry_t) shDisplayTable.get(new Integer(shdChIndex));
            return auxShDisplayEntry.getshDisplayCodec();
        }
        else {
            return null;
        }
    }

    sck.Integer getQuality (int shdChIndex) {
        if (shDisplayTable.containsKey(new Integer(shdChIndex))) {
            shDisplayEntry_t auxShDisplayEntry = (shDisplayEntry_t) shDisplayTable.get(new Integer(shdChIndex));
            return auxShDisplayEntry.getshDisplayQuality();
        }
        else {
            return null;
        }
    }

    sck.OctetString getImageSize (int shdChIndex) {
        if (shDisplayTable.containsKey(new Integer(shdChIndex))) {
            shDisplayEntry_t auxShDisplayEntry = (shDisplayEntry_t) shDisplayTable.get(new Integer(shdChIndex));
            return auxShDisplayEntry.getshDisplayImageSize();
        }
        else {
            return null;
        }
    }

    sck.Integer getDesiredFR (int shdChIndex) {
        if (shDisplayTable.containsKey(new Integer(shdChIndex))) {
            shDisplayEntry_t auxShDisplayEntry = (shDisplayEntry_t) shDisplayTable.get(new Integer(shdChIndex));
            return auxShDisplayEntry.getshDisplayDesiredFR();
        }
        else {
            return null;
        }
    }

    sck.Integer getDesiredBW (int shdChIndex) {
        if (shDisplayTable.containsKey(new Integer(shdChIndex))) {
            shDisplayEntry_t auxShDisplayEntry = (shDisplayEntry_t) shDisplayTable.get(new Integer(shdChIndex));
            return auxShDisplayEntry.getshDisplayDesiredBW();
        }
        else {
            return null;
        }
    }

    sck.Integer getCodecBW (int shdChIndex) {
        if (shDisplayTable.containsKey(new Integer(shdChIndex))) {
            shDisplayEntry_t auxShDisplayEntry = (shDisplayEntry_t) shDisplayTable.get(new Integer(shdChIndex));
            return auxShDisplayEntry.getshDisplayCodecBW();
        }
        else {
            return null;
        }
    }

    sck.Integer getSentFR (int shdChIndex) {
        if (shDisplayTable.containsKey(new Integer(shdChIndex))) {
            shDisplayEntry_t auxShDisplayEntry = (shDisplayEntry_t) shDisplayTable.get(new Integer(shdChIndex));
            return auxShDisplayEntry.getshDisplaySentFR();
        }
        else {
            return null;
        }
    }

    sck.Integer getSentBW (int shdChIndex) {
        if (shDisplayTable.containsKey(new Integer(shdChIndex))) {
            shDisplayEntry_t auxShDisplayEntry = (shDisplayEntry_t) shDisplayTable.get(new Integer(shdChIndex));
            return auxShDisplayEntry.getshDisplaySentBW();
        }
        else {
            return null;
        }
    }

    sck.Integer getReceivedBW (int shdChIndex) {
        if (shDisplayTable.containsKey(new Integer(shdChIndex))) {
            shDisplayEntry_t auxShDisplayEntry = (shDisplayEntry_t) shDisplayTable.get(new Integer(shdChIndex));
            return auxShDisplayEntry.getshDisplayReceivedBW();
        }
        else {
            return null;
        }
    }

    sck.Integer getEnsembledFR (int shdChIndex) {
        if (shDisplayTable.containsKey(new Integer(shdChIndex))) {
            shDisplayEntry_t auxShDisplayEntry = (shDisplayEntry_t) shDisplayTable.get(new Integer(shdChIndex));
            return auxShDisplayEntry.getshDisplayEnsembledFR();
        }
        else {
            return null;
        }
    }

    sck.Integer getPaintFR (int shdChIndex) {
        if (shDisplayTable.containsKey(new Integer(shdChIndex))) {
            shDisplayEntry_t auxShDisplayEntry = (shDisplayEntry_t) shDisplayTable.get(new Integer(shdChIndex));
            return auxShDisplayEntry.getshDisplayPaintFR();
        }
        else {
            return null;
        }
    }
}

class shDisplayEntry_t {
    sck.Integer     shDisplayChIndex;
    sck.OctetString shDisplayAcronym;
    sck.OctetString shDisplayCodec;
    sck.Integer     shDisplayQuality;
    sck.OctetString shDisplayImageSize;
    sck.Integer     shDisplayDesiredFR;
    sck.Integer     shDisplayDesiredBW;
    sck.Integer     shDisplayCodecBW;
    sck.Integer     shDisplaySentFR;
    sck.Integer     shDisplaySentBW;
    sck.Integer     shDisplayReceivedBW;
    sck.Integer     shDisplayEnsembledFR;
    sck.Integer     shDisplayPaintFR;

    int ttl;

    shDisplayEntry_t(int    shdChIndex,
                     String shdAcronym,
                     String shdCodec,
                     int    shdQuality,
                     String shdImageSize,
                     int    shdDesiredFR,
                     int    shdDesiredBW,
                     int    shdCodecBW,
                     int    shdSentFR,
                     int    shdSentBW,
                     int    shdReceivedBW,
                     int    shdEnsembledFR,
                     int    shdPaintFR
                    ) {

        shDisplayChIndex     = new sck.Integer     (shdChIndex);
        shDisplayAcronym     = new sck.OctetString (shdAcronym);
        shDisplayCodec       = new sck.OctetString (shdCodec);
        shDisplayQuality     = new sck.Integer     (shdQuality);
        shDisplayImageSize   = new sck.OctetString (shdImageSize);
        shDisplayDesiredFR   = new sck.Integer     (shdDesiredFR);
        shDisplayDesiredBW   = new sck.Integer     (shdDesiredBW);
        shDisplayCodecBW     = new sck.Integer     (shdCodecBW);
        shDisplaySentFR      = new sck.Integer     (shdSentFR);
        shDisplaySentBW      = new sck.Integer     (shdSentBW);
        shDisplayReceivedBW  = new sck.Integer     (shdReceivedBW);
        shDisplayEnsembledFR = new sck.Integer     (shdEnsembledFR);
        shDisplayPaintFR     = new sck.Integer     (shdPaintFR);
        ttl= 0;
    }

    shDisplayEntry_t(sck.Integer     shDisplayChIndex,
                     sck.OctetString shDisplayAcronym,
                     sck.OctetString shDisplayCodec,
                     sck.Integer     shDisplayQuality,
                     sck.OctetString shDisplayImageSize,
                     sck.Integer     shDisplayDesiredFR,
                     sck.Integer     shDisplayDesiredBW,
                     sck.Integer     shDisplayCodecBW,
                     sck.Integer     shDisplaySentFR,
                     sck.Integer     shDisplaySentBW,
                     sck.Integer     shDisplayReceivedBW,
                     sck.Integer     shDisplayEnsembledFR,
                     sck.Integer     shDisplayPaintFR
                    ) {

        this.shDisplayChIndex     = shDisplayChIndex;
        this.shDisplayAcronym     = shDisplayAcronym;
        this.shDisplayCodec       = shDisplayCodec;
        this.shDisplayQuality     = shDisplayQuality;
        this.shDisplayImageSize   = shDisplayImageSize;
        this.shDisplayDesiredFR   = shDisplayDesiredFR;
        this.shDisplayDesiredBW   = shDisplayDesiredBW;
        this.shDisplayCodecBW     = shDisplayCodecBW;
        this.shDisplaySentFR      = shDisplaySentFR;
        this.shDisplaySentBW      = shDisplaySentBW;
        this.shDisplayReceivedBW  = shDisplayReceivedBW;
        this.shDisplayEnsembledFR = shDisplayEnsembledFR;
        this.shDisplayPaintFR     = shDisplayPaintFR;
        ttl= 0;
    }

    sck.Integer     getshDisplayChIndex    () { return shDisplayChIndex;     }
    sck.OctetString getshDisplayAcronym    () { return shDisplayAcronym;     }
    sck.OctetString getshDisplayCodec      () { return shDisplayCodec;       }
    sck.Integer     getshDisplayQuality    () { return shDisplayQuality;     }
    sck.OctetString getshDisplayImageSize  () { return shDisplayImageSize;   }
    sck.Integer     getshDisplayDesiredFR  () { return shDisplayDesiredFR;   }
    sck.Integer     getshDisplayDesiredBW  () { return shDisplayDesiredBW;   }
    sck.Integer     getshDisplayCodecBW    () { return shDisplayCodecBW;     }
    sck.Integer     getshDisplaySentFR     () { return shDisplaySentFR;      }
    sck.Integer     getshDisplaySentBW     () { return shDisplaySentBW;      }
    sck.Integer     getshDisplayReceivedBW () { return shDisplayReceivedBW;  }
    sck.Integer     getshDisplayEnsembledFR() { return shDisplayEnsembledFR; }
    sck.Integer     getshDisplayPaintFR    () { return shDisplayPaintFR;     }

    int         getshDisplayTTL         () { return ttl;                  }
    void        incrementTTL            () { ttl++;                       }
}
