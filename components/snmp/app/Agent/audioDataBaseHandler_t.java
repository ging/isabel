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
//***************************************************************************
//
//FILE: audioDataBaseHandler_t.java
//
//DESCRIPTION:
//    This class give acces to all the data of the audio.
//    It's always used by the DataBaseHandler.
//

import java.io.*;

public class audioDataBaseHandler_t {
    String audioChIndexOid          = "1.3.6.1.4.1.2781.1.4.1.1.1";
    String audioCodecOid            = "1.3.6.1.4.1.2781.1.4.1.1.2";
    String audioSentBWOid           = "1.3.6.1.4.1.2781.1.4.1.1.3";
    String audioReceivedBWOid       = "1.3.6.1.4.1.2781.1.4.1.1.4";
    String audioReceivedPacketsOid  = "1.3.6.1.4.1.2781.1.4.1.1.5";
    String audioLostPacketsOid      = "1.3.6.1.4.1.2781.1.4.1.1.6";
    String audioDupPacketsOid       = "1.3.6.1.4.1.2781.1.4.1.1.7";
    String audioDisorderedPacketsOid= "1.3.6.1.4.1.2781.1.4.1.1.8";
    String audioThrownPacketsOid    = "1.3.6.1.4.1.2781.1.4.1.1.9";
    String audioJitterOid           = "1.3.6.1.4.1.2781.1.4.1.1.10";
    String audioBufferedOid         = "1.3.6.1.4.1.2781.1.4.1.1.11";
    String audioBufferingOid        = "1.3.6.1.4.1.2781.1.4.1.1.12";

    audioDataBase_t audioDataBase;

    audioDataBaseHandler_t (audioDataBase_t audioDataBase) {
        this.audioDataBase = audioDataBase;
    }

    // Functions that help the Monitor GUI to find data in the dataBase:

    audioDataBase_t getAudioDataBase () {
        return audioDataBase;
    }

    sck.Oid getNextOid(sck.Oid OID) {
        return audioDataBase.getNext(OID);
    }
    int countEntries() {
        return audioDataBase.countEntries();
    }
    int getChIndexFromPos(int row) {
        return audioDataBase.getChIndexFromPos(row);
    }
    boolean contains (sck.Oid OID) {
       return audioDataBase.contains(OID);
    }

    // Function used by the internalServer via the DataBaseHandler to
    // decode the messages that come from the audio ISABEL module.
    // It also introduces the new data in the MIB.

    void AnalizeMessage (int audChIndex, DataInputStream dst)
                        throws java.io.IOException {
        int audSentBW           = dst.readInt();
        int audReceivedBW       = dst.readInt();
        int audReceivedPackets  = dst.readInt();
        int audLostPackets      = dst.readInt();
        int audDupPackets       = dst.readInt();
        int audDisorderedPackets= dst.readInt();
        int audThrownPackets    = dst.readInt();
        int audJitter           = dst.readInt();
        int audBuffered         = dst.readInt();
        int audBuffering        = dst.readInt();
        int codecNameLen        = dst.readByte();
        byte [] codecNameB = new byte[codecNameLen];
        dst.read(codecNameB,0,codecNameLen);
        String audCodec = new String(codecNameB);

        //System.out.println("AUDIO: chId=" + audChIndex +
        //                   "\tSentBW=" + audSentBW +
        //                   "\tReceivedBW=" + audReceivedBW +
        //                   "\tReceivedPackets=" + audReceivedPackets +
        //                   "\n\tLostPackets=" + audLostPackets +
        //                   "\tDupPackets=" + audDupPackets +
        //                   "\tDisorderedPackets=" + audDisorderedPackets +
        //                   "\n\tJitter=" + audJitter +
        //                   "\tBuffered=" + audBuffered +
        //                   "\tBuffering=" + audBuffering +
        //                   "\n\tThrownPackets=" + audThrownPackets +
        //                   "\tcodecNameLen=" + codecNameLen +
        //                   "\tCodec=" + audCodec);

        audioDataBase.setOrCreateNewAudioEntry(audChIndex,
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
    void setAudioChIndex (int audChIndex) {
        audioDataBase.newAudioEntry(audChIndex);
    }
    void audioSetAll(int    audChIndex,
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
        audioDataBase.setOrCreateNewAudioEntry(audChIndex,
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

    boolean deleteChannel (int ivChIndex) {
      return audioDataBase.deleteChannel(ivChIndex);
    }
    void IncrementAllChannelsTTL() {
      audioDataBase.IncrementAllChannelsTTL();
    }

    //FUNCIONES PARA BUSCAR DATOS EN LA BASE DE DATOS Y ASI PODER
    //RESPONDER A LAS PETICIONES SNMP.

    sck.smi GET (sck.Oid RequestedOid) {
        if (RequestedOid.sameTree("1.3.6.1.4.1.2781.1.4.1.1")) {
            int Columna = getColumn(RequestedOid);
            int audChIndex = getInstanceOid(RequestedOid);
            switch (Columna) {
                case 1:
                    return audioDataBase.getChIndex(audChIndex);
                case 2:
                    return audioDataBase.getCodec(audChIndex);
                case 3:
                    return audioDataBase.getSentBW(audChIndex);
                case 4:
                    return audioDataBase.getReceivedBW(audChIndex);
                case 5:
                    return audioDataBase.getReceivedPackets(audChIndex);
                case 6:
                    return audioDataBase.getLostPackets(audChIndex);
                case 7:
                    return audioDataBase.getDupPackets(audChIndex);
                case 8:
                    return audioDataBase.getDisorderedPackets(audChIndex);
                case 9:
                    return audioDataBase.getThrownPackets(audChIndex);
                case 10:
                    return audioDataBase.getJitter(audChIndex);
                case 11:
                    return audioDataBase.getBuffered(audChIndex);
                case 12:
                    return audioDataBase.getBuffering(audChIndex);
                default:
                    return null;
            }
        }
        else return null;
    }

    sck.Integer GET_NEXT (sck.Oid RequestedOid) {
        return null;
    }

    boolean  SET (sck.Oid RequestedOid, sck.smi Value) {
        return true;
    }

    //
    // Funciones de acceso a los Oid
    //
    private int getColumn (sck.Oid RequestedOid) {
        String ObjectOid = getObjectOid(RequestedOid).toString();
        int LastPoint = ObjectOid.lastIndexOf('.');
        return Integer.parseInt(ObjectOid.substring(LastPoint+1));
    }

    private sck.Oid getObjectOid (sck.Oid RequestedOid) {
        try {
            String OID = RequestedOid.toString();
            int LastPoint = OID.lastIndexOf('.');
            return new sck.Oid(OID.substring(0,LastPoint));
        } catch (Exception e) {
            System.out.println("Excepcion al sacar el getObjectOid de:"+RequestedOid);
            System.out.println(e);
            return null;
        }
    }

    private int getInstanceOid (sck.Oid RequestedOid) {
        String OID = RequestedOid.toString();
        int LastPoint = OID.lastIndexOf('.');
        return Integer.parseInt(OID.substring(LastPoint+1));
    }
}

