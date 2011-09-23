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
//FILE: audioDataBase_t.java
//
//DESCRIPTION:
// This file contains the classes that let me store the data of the
// audio module of ISABEL. All the threads access this classes via
// the audioDataBaseHandler, via the DataBaseHandler.
//


import java.io.*;
import java.util.*;

public class audioDataBase_t extends DataBase_t {

    // Number of columns,
    int rowElements = audioTableModel_t.AUDIO_STATS.length;

    Hashtable audioTable;

    audioDataBase_t () {
        super();
        audioTable = new Hashtable();
    }

    // Gives information about the MIB elements for the Monitor GUI:
    //
    // This function is used by the Monitor_GUI when a graphic is required.
    // It maps from the position on the visual table
    // to the real channel number in the MIB.
    int getChIndexFromPos (int pos) {
        Object [] keys = audioTable.keySet().toArray();
        return ((Integer)keys[pos]).intValue();
    }

    // Tells the visual table how many rows it will have to paint.
    int countEntries() {
        return audioTable.size();
    }

    // Delete the data that hasn't been actualizated:
    //
    // If time goes by and nobody sends information about the audio,
    // the ttl will be incremented for all the channels.
    void IncrementAllChannelsTTL() {
        Enumeration keys= audioTable.keys();
        while(keys.hasMoreElements()) {
            Integer auxkey = (Integer)keys.nextElement();
            audioEntry_t auxEntry = (audioEntry_t)audioTable.get(auxkey);
            if ((auxEntry.getAudioTTL())<10) {
                auxEntry.incrementTTL();
                audioTable.put(auxkey,auxEntry);
            } else {
                deleteChannel(auxkey);
            }
        }
    }

    // If a channel is not actualized for a long time
    // it will be deleted from de MIB with this function.
    boolean deleteChannel (Integer audChIndex) {
        if (audioTable.containsKey(audChIndex)) {
            audioTable.remove(audChIndex);
            delOldOids(audChIndex.intValue());
            return true;
        }
        return false;
    }

    boolean deleteChannel (int audChIndex) {
        if (audioTable.containsKey(new Integer(audChIndex))) {
            audioTable.remove(new Integer(audChIndex));
            delOldOids(audChIndex);
            isSorted=false;
            return true;
        }
        return false;
    }

    // Private functions to add and delete Oids to the static vector:
    // --------------------------------------------------------------

    // This function add the new MIB elements to a vector that is used when
    // a GET_NEXT request arrives to the agent.
    private void addNewOids (int audChIndex) {
        try {
            for (int i=1; i<=rowElements; i++) {
                AddOid(new sck.Oid("1.3.6.1.4.1.2781.1.4.1.1."+i+"."+audChIndex));
            }
        } catch (Exception e) {}
        isSorted=false;
    }

    // This function deletes the new MIB elements to a vector that is used when
    // a GET_NEXT request arrives to the agent.
    private void delOldOids (int audChIndex) {
        try {
            for (int i=1;i<=rowElements;i++) {
                DelOid(new sck.Oid("1.3.6.1.4.1.2781.1.4.1.1."+i+"."+audChIndex));
            }
        } catch (Exception e) {}
        isSorted=false;
    }


    // Functions to add new elements to the audio MIB:
    // -----------------------------------------------

    void newAudioEntry (int audChIndex) {
        audioEntry_t audioEntry = new audioEntry_t (audChIndex,null,0,0,0,0,0,0,0,0,0,0);
        audioTable.put (new Integer(audChIndex), audioEntry);
        addNewOids (audChIndex);
        isSorted=false;
    }

    void newAudioEntry (int    audChIndex,
                        String audCodec,
                        int    audSentBW,
                        int    audReceivedBW,
                        int    audReceivedPackets,
                        int    audLostPackets,
                        int    audDupPackets,
                        int    audDisorderedPackets,
                        int    audThrownPackets,
                        int    audJitter,
                        int    audBuffered,
                        int    audBuffering
                       ) {
        audioEntry_t audioEntry=
            new audioEntry_t(audChIndex,
                             audCodec,
                             audSentBW,
                             audReceivedBW,
                             audReceivedPackets,
                             audLostPackets,
                             audDupPackets,
                             audDisorderedPackets,
                             audThrownPackets,
                             audJitter,
                             audBuffered,
                             audBuffering
                            );
        audioTable.put (new Integer(audChIndex), audioEntry);
        addNewOids (audChIndex);
        isSorted=false;
    }

    void setOrCreateNewAudioEntry(int    audChIndex,
                                  String audCodec,
                                  int    audSentBW,
                                  int    audReceivedBW,
                                  int    audReceivedPackets,
                                  int    audLostPackets,
                                  int    audDupPackets,
                                  int    audDisorderedPackets,
                                  int    audThrownPackets,
                                  int    audJitter,
                                  int    audBuffered,
                                  int    audBuffering
                                 ) {
        audioEntry_t auxAudioEntry=
            new audioEntry_t(audChIndex,
                             audCodec,
                             audSentBW,
                             audReceivedBW,
                             audReceivedPackets,
                             audLostPackets,
                             audDupPackets,
                             audDisorderedPackets,
                             audThrownPackets,
                             audJitter,
                             audBuffered,
                             audBuffering
                            );
        if (audioTable.containsKey(new Integer(audChIndex))) {
            audioTable.remove(new Integer(audChIndex));
            audioTable.put(new Integer(audChIndex), auxAudioEntry);
        } else {
            newAudioEntry(audChIndex,
                          audCodec,
                          audSentBW,
                          audReceivedBW,
                          audReceivedPackets,
                          audLostPackets,
                          audDupPackets,
                          audDisorderedPackets,
                          audThrownPackets,
                          audJitter,
                          audBuffered,
                          audBuffering
                         );
        }
    }

    // Function to get individualy each of the values in each table
    // (used by the SNMP_Server via the DataBaseHandler):
    // ----------------------------------------------------------------

    sck.Integer getChIndex (int audChIndex) {
        if (audioTable.containsKey(new Integer(audChIndex))) {
            audioEntry_t auxAudioEntry =
                (audioEntry_t) audioTable.get(new Integer(audChIndex));
            return auxAudioEntry.getAudioChIndex();
        } else {
            return null;
        }
    }

    sck.OctetString getCodec (int audChIndex) {
        if (audioTable.containsKey(new Integer(audChIndex))) {
            audioEntry_t auxAudioEntry =
                (audioEntry_t) audioTable.get(new Integer(audChIndex));
            return auxAudioEntry.getAudioCodec();
        } else {
            return null;
        }
    }

    sck.Integer getSentBW (int audChIndex) {
        if (audioTable.containsKey(new Integer(audChIndex))) {
            audioEntry_t auxAudioEntry =
                (audioEntry_t) audioTable.get(new Integer(audChIndex));
            return auxAudioEntry.getAudioSentBW();
        } else {
            return null;
        }
    }

    sck.Integer getReceivedBW (int audChIndex) {
        if (audioTable.containsKey(new Integer(audChIndex))) {
            audioEntry_t auxAudioEntry =
                (audioEntry_t) audioTable.get(new Integer(audChIndex));
            return auxAudioEntry.getAudioReceivedBW();
        } else {
            return null;
        }
    }

    sck.Integer getReceivedPackets (int audChIndex) {
        if (audioTable.containsKey(new Integer(audChIndex))) {
            audioEntry_t auxAudioEntry =
                (audioEntry_t) audioTable.get(new Integer(audChIndex));
            return auxAudioEntry.getAudioReceivedPackets();
        } else {
            return null;
        }
    }

    sck.Integer getLostPackets (int audChIndex) {
        if (audioTable.containsKey(new Integer(audChIndex))) {
            audioEntry_t auxAudioEntry =
                (audioEntry_t) audioTable.get(new Integer(audChIndex));
            return auxAudioEntry.getAudioLostPackets();
        } else {
            return null;
        }
    }

    sck.Integer getDupPackets (int audChIndex) {
        if (audioTable.containsKey(new Integer(audChIndex))) {
            audioEntry_t auxAudioEntry =
                (audioEntry_t) audioTable.get(new Integer(audChIndex));
            return auxAudioEntry.getAudioDupPackets();
        } else {
            return null;
        }
    }

    sck.Integer getDisorderedPackets (int audChIndex) {
        if (audioTable.containsKey(new Integer(audChIndex))) {
            audioEntry_t auxAudioEntry =
                (audioEntry_t) audioTable.get(new Integer(audChIndex));
            return auxAudioEntry.getAudioDisorderedPackets();
        } else {
            return null;
        }
    }

    sck.Integer getThrownPackets (int audChIndex) {
        if (audioTable.containsKey(new Integer(audChIndex))) {
            audioEntry_t auxAudioEntry =
                (audioEntry_t) audioTable.get(new Integer(audChIndex));
            return auxAudioEntry.getAudioThrownPackets();
        } else {
            return null;
        }
    }

    sck.Integer getJitter (int audChIndex) {
        if (audioTable.containsKey(new Integer(audChIndex))) {
            audioEntry_t auxAudioEntry =
                (audioEntry_t) audioTable.get(new Integer(audChIndex));
            return auxAudioEntry.getAudioJitter();
        } else {
            return null;
        }
    }

    sck.Integer getBuffered (int audChIndex) {
        if (audioTable.containsKey(new Integer(audChIndex))) {
            audioEntry_t auxAudioEntry =
                (audioEntry_t) audioTable.get(new Integer(audChIndex));
            return auxAudioEntry.getAudioBuffered();
        } else {
            return null;
        }
    }

    sck.Integer getBuffering (int audChIndex) {
        if (audioTable.containsKey(new Integer(audChIndex))) {
            audioEntry_t auxAudioEntry =
                (audioEntry_t) audioTable.get(new Integer(audChIndex));
            return auxAudioEntry.getAudioBuffering();
        } else {
            return null;
        }
    }
}

class audioEntry_t {
    sck.Integer     audioChIndex;
    sck.OctetString audioCodec;
    sck.Integer     audioSentBW;
    sck.Integer     audioReceivedBW;
    sck.Integer     audioReceivedPackets;
    sck.Integer     audioLostPackets;
    sck.Integer     audioDupPackets;
    sck.Integer     audioDisorderedPackets;
    sck.Integer     audioThrownPackets;
    sck.Integer     audioJitter;
    sck.Integer     audioBuffered;
    sck.Integer     audioBuffering;

    int ttl;

    audioEntry_t(int    audChIndex,
                 String audCodec,
                 int    audSentBW,
                 int    audReceivedBW,
                 int    audReceivedPackets,
                 int    audLostPackets,
                 int    audDupPackets,
                 int    audDisorderedPackets,
                 int    audThrownPackets,
                 int    audJitter,
                 int    audBuffered,
                 int    audBuffering
                ) {

        audioChIndex          = new sck.Integer     (audChIndex);
        audioCodec            = new sck.OctetString (audCodec);
        audioSentBW           = new sck.Integer     (audSentBW);
        audioReceivedBW       = new sck.Integer     (audReceivedBW);
        audioReceivedPackets  = new sck.Integer     (audReceivedPackets);
        audioLostPackets      = new sck.Integer     (audLostPackets);
        audioDupPackets       = new sck.Integer     (audDupPackets);
        audioDisorderedPackets= new sck.Integer     (audDisorderedPackets);
        audioThrownPackets    = new sck.Integer     (audThrownPackets);
        audioJitter           = new sck.Integer     (audJitter);
        audioBuffered         = new sck.Integer     (audBuffered);
        audioBuffering        = new sck.Integer     (audBuffering);
        ttl= 0;
    }

    audioEntry_t(sck.Integer     audioChIndex,
                 sck.OctetString audioCodec,
                 sck.Integer     audioSentBW,
                 sck.Integer     audioReceivedBW,
                 sck.Integer     audioReceivedPackets,
                 sck.Integer     audioLostPackets,
                 sck.Integer     audioDupPackets,
                 sck.Integer     audioDisorderedPackets,
                 sck.Integer     audioThrownPackets,
                 sck.Integer     audioJitter,
                 sck.Integer     audioBuffered,
                 sck.Integer     audioBuffering
                ) {

        this.audioChIndex          = audioChIndex;
        this.audioCodec            = audioCodec;
        this.audioSentBW           = audioSentBW;
        this.audioReceivedBW       = audioReceivedBW;
        this.audioReceivedPackets  = audioReceivedPackets;
        this.audioLostPackets      = audioLostPackets;
        this.audioDupPackets       = audioDupPackets;
        this.audioDisorderedPackets= audioDisorderedPackets;
        this.audioThrownPackets    = audioThrownPackets;
        this.audioJitter           = audioJitter;
        this.audioBuffered         = audioBuffered;
        this.audioBuffering        = audioBuffering;
        ttl= 0;
    }

    sck.Integer getAudioChIndex          () { return audioChIndex;           }
    sck.OctetString getAudioCodec        () { return audioCodec;             }
    sck.Integer getAudioSentBW           () { return audioSentBW;            }
    sck.Integer getAudioReceivedBW       () { return audioReceivedBW;        }
    sck.Integer getAudioReceivedPackets  () { return audioReceivedPackets;   }
    sck.Integer getAudioLostPackets      () { return audioLostPackets;       }
    sck.Integer getAudioDupPackets       () { return audioDupPackets;        }
    sck.Integer getAudioDisorderedPackets() { return audioDisorderedPackets; }
    sck.Integer getAudioThrownPackets    () { return audioThrownPackets;     }
    sck.Integer getAudioJitter           () { return audioJitter;            }
    sck.Integer getAudioBuffered         () { return audioBuffered;          }
    sck.Integer getAudioBuffering        () { return audioBuffering;         }

    int         getAudioTTL              () { return ttl;                    }
    void        incrementTTL             () { ttl++;                         }
}
