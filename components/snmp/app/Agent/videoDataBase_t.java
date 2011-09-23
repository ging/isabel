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
// FILE: videoDataBase_t.java
//
// DESCRIPTION:
// This file contains the classes that stores the data of the
// video module of ISABEL. All the threads access this classes
// via the videoDataBaseHandler, via the DataBaseHandler.
//


import java.io.*;
import java.util.*;

public class videoDataBase_t extends DataBase_t{

    // Number of columns,
    int rowElements = videoTableModel_t.VIDEO_STATS.length;

    Hashtable videoTable;

    videoDataBase_t () {
        super();
        videoTable = new Hashtable();
    }

    //Function that give information about the MIB elements for the Monitor GUI:
    //--------------------------------------------------------------------------

    // This function is used by the Monitor_GUI when a graphic is required.
    // It maps from the position on the visual table
    // to the real channel number in the MIB.
    int getChIndexFromPos (int pos) {
        Object [] keys = videoTable.keySet().toArray();
        return ((Integer)keys[pos]).intValue();
    }

    // This function tells the visual table how many rows it will have to paint.
    int countEntries() {
        return videoTable.size();
    }

    //Function for deleting the data that hasn't been actualizated:
    //-------------------------------------------------------------

    // If time goes by and nobody sends information about the video,
    // the ttl will be incremented for all the channels.
    void IncrementAllChannelsTTL() {
        Enumeration keys= videoTable.keys();
        while(keys.hasMoreElements()) {
            Integer auxkey = (Integer)keys.nextElement();
            videoEntry_t auxEntry = (videoEntry_t)videoTable.get(auxkey);
            if ((auxEntry.getvideoTTL())<10) {
                auxEntry.incrementTTL();
                videoTable.put(auxkey,auxEntry);
            } else {
                deleteChannel(auxkey);
            }
        }
    }

    // If a channel is not actualized for a long time
    // it will be deleted from de MIB with this function.
    boolean deleteChannel (Integer ivChIndex) {
        if (videoTable.containsKey(ivChIndex)) {
            videoTable.remove(ivChIndex);
            delOldOids(ivChIndex.intValue());
            return true;
        }
        return false;
    }

    boolean deleteChannel (int ivChIndex) {
        if (videoTable.containsKey(new Integer(ivChIndex))) {
            videoTable.remove(new Integer(ivChIndex));
            delOldOids(ivChIndex);
            return true;
        }
        return false;
    }

    // Private functions to add and delete Oids to the static vector:
    // --------------------------------------------------------------

    // This function add the new MIB elements to a vector that is used when
    // a GET_NEXT request arrives to the agent.
    private void addNewOids (int ivChIndex) {
        try {
            for (int i=1; i<=rowElements; i++) {
                AddOid(new sck.Oid("1.3.6.1.4.1.2781.1.5.1.1."+i+"."+ivChIndex));
            }
        } catch (Exception e) {}
        isSorted=false;
    }

    // This function deletes the new MIB elements to a vector that is used when
    // a GET_NEXT request arrives to the agent.
    private void delOldOids (int ivChIndex) {
        try {
            for (int i=1;i<=rowElements;i++) {
                DelOid(new sck.Oid("1.3.6.1.4.1.2781.1.5.1.1."+i+"."+ivChIndex));
            }
        } catch (Exception e) {}
        isSorted=false;
    }


    // Functions to add new elements to the video MIB:
    // -----------------------------------------------

    private void newVideoEntry (int ivChIndex) {
        videoEntry_t videoEntry = new videoEntry_t (ivChIndex,null,null,0,null,0,0,0,0,0,0,0,0);
        videoTable.put (new Integer(ivChIndex), videoEntry);
        addNewOids (ivChIndex);
        isSorted=false;
    }

    void newVideoEntry (int    ivChIndex,
                        String ivAcronym,
                        String ivCodec,
                        int    ivQuality,
                        String ivImageSize,
                        int    ivDesiredFR,
                        int    ivDesiredBW,
                        int    ivCodecBW,
                        int    ivSentFR,
                        int    ivSentBW,
                        int    ivReceivedBW,
                        int    ivEnsembledFR,
                        int    ivPaintFR
                       ) {
        videoEntry_t videoEntry=
            new videoEntry_t(ivChIndex,
                             ivAcronym,
                             ivCodec,
                             ivQuality,
                             ivImageSize,
                             ivDesiredFR,
                             ivDesiredBW,
                             ivCodecBW,
                             ivSentFR,
                             ivSentBW,
                             ivReceivedBW,
                             ivEnsembledFR,
                             ivPaintFR
                            );
        videoTable.put (new Integer(ivChIndex), videoEntry);
        addNewOids (ivChIndex);
        isSorted=false;
    }

    void setOrCreateNewVideoEntry(int    ivChIndex,
                                  String ivAcronym,
                                  String ivCodec,
                                  int    ivQuality,
                                  String ivImageSize,
                                  int    ivDesiredFR,
                                  int    ivDesiredBW,
                                  int    ivCodecBW,
                                  int    ivSentFR,
                                  int    ivSentBW,
                                  int    ivReceivedBW,
                                  int    ivEnsembledFR,
                                  int    ivPaintFR
                                 )  {
        videoEntry_t auxVideoEntry=
            new videoEntry_t(ivChIndex,
                             ivAcronym,
                             ivCodec,
                             ivQuality,
                             ivImageSize,
                             ivDesiredFR,
                             ivDesiredBW,
                             ivCodecBW,
                             ivSentFR,
                             ivSentBW,
                             ivReceivedBW,
                             ivEnsembledFR,
                             ivPaintFR
                            );
        if (videoTable.containsKey(new Integer(ivChIndex))) {
            videoTable.remove(new Integer(ivChIndex));
            videoTable.put(new Integer(ivChIndex), auxVideoEntry);
        } else {
            newVideoEntry(ivChIndex,
                          ivAcronym,
                          ivCodec,
                          ivQuality,
                          ivImageSize,
                          ivDesiredFR,
                          ivDesiredBW,
                          ivCodecBW,
                          ivSentFR,
                          ivSentBW,
                          ivReceivedBW,
                          ivEnsembledFR,
                          ivPaintFR
                         );
        }
    }

    // Function to get individualy each of the values in each table
    // (used by the SNMP_Server via the DataBaseHandler):
    // ----------------------------------------------------------------

    sck.Integer getChIndex (int ivChIndex) {
        if (videoTable.containsKey(new Integer(ivChIndex))) {
            videoEntry_t auxVideoEntry = (videoEntry_t) videoTable.get(new Integer(ivChIndex));
            return auxVideoEntry.getvideoChIndex();
            }
        else return null;
    }

    sck.OctetString getAcronym (int ivChIndex) {
        if (videoTable.containsKey(new Integer(ivChIndex))) {
            videoEntry_t auxVideoEntry = (videoEntry_t) videoTable.get(new Integer(ivChIndex));
            return auxVideoEntry.getvideoAcronym();
        }
        else return null;
    }

    sck.OctetString getCodec (int ivChIndex) {
        if (videoTable.containsKey(new Integer(ivChIndex))) {
            videoEntry_t auxVideoEntry = (videoEntry_t) videoTable.get(new Integer(ivChIndex));
            return auxVideoEntry.getvideoCodec();
        }
        else return null;
    }

    sck.Integer getQuality (int ivChIndex) {
        if (videoTable.containsKey(new Integer(ivChIndex))) {
            videoEntry_t auxVideoEntry = (videoEntry_t) videoTable.get(new Integer(ivChIndex));
            return auxVideoEntry.getvideoQuality();
        }
        else return null;
    }

    sck.OctetString getImageSize (int ivChIndex) {
        if (videoTable.containsKey(new Integer(ivChIndex))) {
            videoEntry_t auxVideoEntry = (videoEntry_t) videoTable.get(new Integer(ivChIndex));
            return auxVideoEntry.getvideoImageSize();
        }
        else return null;
    }

    sck.Integer getDesiredFR (int ivChIndex) {
        if (videoTable.containsKey(new Integer(ivChIndex))) {
            videoEntry_t auxVideoEntry = (videoEntry_t) videoTable.get(new Integer(ivChIndex));
            return auxVideoEntry.getvideoDesiredFR();
            }
        else {
            return null;
            }
    }

    sck.Integer getDesiredBW (int ivChIndex) {
        if (videoTable.containsKey(new Integer(ivChIndex))) {
            videoEntry_t auxVideoEntry = (videoEntry_t) videoTable.get(new Integer(ivChIndex));
            return auxVideoEntry.getvideoDesiredBW();
            }
        else return null;
    }

    sck.Integer getCodecBW (int ivChIndex) {
        if (videoTable.containsKey(new Integer(ivChIndex))) {
            videoEntry_t auxVideoEntry = (videoEntry_t) videoTable.get(new Integer(ivChIndex));
            return auxVideoEntry.getvideoCodecBW();
        }
        else return null;
    }

    sck.Integer getSentFR (int ivChIndex) {
        if (videoTable.containsKey(new Integer(ivChIndex))) {
            videoEntry_t auxVideoEntry = (videoEntry_t) videoTable.get(new Integer(ivChIndex));
            return auxVideoEntry.getvideoSentFR();
            }
        else return null;
    }

    sck.Integer getSentBW (int ivChIndex) {
        if (videoTable.containsKey(new Integer(ivChIndex))) {
            videoEntry_t auxVideoEntry = (videoEntry_t) videoTable.get(new Integer(ivChIndex));
            return auxVideoEntry.getvideoSentBW();
        }
        else return null;
    }

    sck.Integer getReceivedBW (int ivChIndex) {
        if (videoTable.containsKey(new Integer(ivChIndex))) {
            videoEntry_t auxVideoEntry = (videoEntry_t) videoTable.get(new Integer(ivChIndex));
            return auxVideoEntry.getvideoReceivedBW();
        }
        else return null;
    }

    sck.Integer getEnsembledFR (int ivChIndex) {
        if (videoTable.containsKey(new Integer(ivChIndex))) {
            videoEntry_t auxVideoEntry = (videoEntry_t) videoTable.get(new Integer(ivChIndex));
            return auxVideoEntry.getvideoEnsembledFR();
        }
        else return null;
    }

    sck.Integer getPaintFR (int ivChIndex) {
        if (videoTable.containsKey(new Integer(ivChIndex))) {
            videoEntry_t auxVideoEntry = (videoEntry_t) videoTable.get(new Integer(ivChIndex));
            return auxVideoEntry.getvideoPaintFR();
        }
        else return null;
    }

}

class videoEntry_t {
    sck.Integer     videoChIndex;
    sck.OctetString videoAcronym;
    sck.OctetString videoCodec;
    sck.Integer     videoQuality;
    sck.OctetString videoImageSize;
    sck.Integer     videoDesiredFR;
    sck.Integer     videoDesiredBW;
    sck.Integer     videoCodecBW;
    sck.Integer     videoSentFR;
    sck.Integer     videoSentBW;
    sck.Integer     videoReceivedBW;
    sck.Integer     videoEnsembledFR;
    sck.Integer     videoPaintFR;

    int ttl;

    videoEntry_t(int    ivChIndex,
                 String ivAcronym,
                 String ivCodec,
                 int    ivQuality,
                 String ivImageSize,
                 int    ivDesiredFR,
                 int    ivDesiredBW,
                 int    ivCodecBW,
                 int    ivSentFR,
                 int    ivSentBW,
                 int    ivReceivedBW,
                 int    ivEnsembledFR,
                 int    ivPaintFR
                ) {

        videoChIndex       = new sck.Integer     (ivChIndex);
        videoAcronym       = new sck.OctetString (ivAcronym);
        videoCodec         = new sck.OctetString (ivCodec);
        videoQuality       = new sck.Integer     (ivQuality);
        videoImageSize     = new sck.OctetString (ivImageSize);
        videoDesiredFR     = new sck.Integer     (ivDesiredFR);
        videoDesiredBW     = new sck.Integer     (ivDesiredBW);
        videoCodecBW       = new sck.Integer     (ivCodecBW);
        videoSentFR        = new sck.Integer     (ivSentFR);
        videoSentBW        = new sck.Integer     (ivSentBW);
        videoReceivedBW    = new sck.Integer     (ivReceivedBW);
        videoEnsembledFR   = new sck.Integer     (ivEnsembledFR);
        videoPaintFR       = new sck.Integer     (ivPaintFR);
        ttl= 0;
    }

    videoEntry_t(sck.Integer     videoChIndex,
                 sck.OctetString videoAcronym,
                 sck.OctetString videoCodec,
                 sck.Integer     videoQuality,
                 sck.OctetString videoImageSize,
                 sck.Integer     videoDesiredFR,
                 sck.Integer     videoDesiredBW,
                 sck.Integer     videoCodecBW,
                 sck.Integer     videoSentFR,
                 sck.Integer     videoSentBW,
                 sck.Integer     videoReceivedBW,
                 sck.Integer     videoEnsembledFR,
                 sck.Integer     videoPaintFR
                ) {

        this.videoChIndex     = videoChIndex;
        this.videoAcronym     = videoAcronym;
        this.videoCodec       = videoCodec;
        this.videoQuality     = videoQuality;
        this.videoImageSize   = videoImageSize;
        this.videoDesiredFR   = videoDesiredFR;
        this.videoDesiredBW   = videoDesiredBW;
        this.videoCodecBW     = videoCodecBW;
        this.videoSentFR      = videoSentFR;
        this.videoSentBW      = videoSentBW;
        this.videoReceivedBW  = videoReceivedBW;
        this.videoEnsembledFR = videoEnsembledFR;
        this.videoPaintFR     = videoPaintFR;
        ttl= 0;
    }

    sck.Integer     getvideoChIndex     () { return videoChIndex ;       }
    sck.OctetString getvideoAcronym     () { return videoAcronym ;       }
    sck.OctetString getvideoCodec       () { return videoCodec ;         }
    sck.Integer     getvideoQuality     () { return videoQuality ;       }
    sck.OctetString getvideoImageSize   () { return videoImageSize ;     }
    sck.Integer     getvideoDesiredFR   () { return videoDesiredFR ;     }
    sck.Integer     getvideoDesiredBW   () { return videoDesiredBW ;     }
    sck.Integer     getvideoCodecBW     () { return videoCodecBW ;       }
    sck.Integer     getvideoSentFR      () { return videoSentFR ;        }
    sck.Integer     getvideoSentBW      () { return videoSentBW ;        }
    sck.Integer     getvideoReceivedBW  () { return videoReceivedBW ;    }
    sck.Integer     getvideoEnsembledFR () { return videoEnsembledFR ;   }
    sck.Integer     getvideoPaintFR     () { return videoPaintFR ;       }

    int         getvideoTTL         () { return ttl;                 }
    void        incrementTTL        () { ttl++;                      }

}

