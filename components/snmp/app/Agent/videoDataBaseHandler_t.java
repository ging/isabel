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
//FILE: videoDataBaseHandler_t.java
//
//DESCRIPTION:
//    This class give acces to all the data of the video.
//    It's always used by the DataBaseHandler.
//

import java.io.*;

public class videoDataBaseHandler_t {

//Name of each of the MIB elements that this Handler manages:
//
//    String videoChIndexOid       = "1.3.6.1.4.1.2781.1.5.1.1.1";
//    String videoAcronymOid       = "1.3.6.1.4.1.2781.1.5.1.1.2";
//    String videoCodecOid         = "1.3.6.1.4.1.2781.1.5.1.1.3";
//    String videoQualityOid       = "1.3.6.1.4.1.2781.1.5.1.1.4";
//    String videoImageSizeOid     = "1.3.6.1.4.1.2781.1.5.1.1.5";
//    String videoDesiredFROid     = "1.3.6.1.4.1.2781.1.5.1.1.6";
//    String videoDesiredBWOid     = "1.3.6.1.4.1.2781.1.5.1.1.7";
//    String videoCodecBWOid       = "1.3.6.1.4.1.2781.1.5.1.1.8";
//    String videoSentFROid        = "1.3.6.1.4.1.2781.1.5.1.1.9";
//    String videoSentBWOid        = "1.3.6.1.4.1.2781.1.5.1.1.10";
//    String videoReceivedBWOid    = "1.3.6.1.4.1.2781.1.5.1.1.11";
//    String videoEnsembledFROid   = "1.3.6.1.4.1.2781.1.5.1.1.12";
//    String videoPaintFROid       = "1.3.6.1.4.1.2781.1.5.1.1.13";


    //Attributes:
    //-----------
    videoDataBase_t videoDataBase;

    //Constructor:
    //------------
    videoDataBaseHandler_t (videoDataBase_t videoDataBase) {
        this.videoDataBase = videoDataBase;
    }

    //Functions to control what is in the dataBase:
    //---------------------------------------------
    videoDataBase_t getVideoDataBase () { return videoDataBase; };
    sck.Oid getNextOid(sck.Oid OID) {
        return videoDataBase.getNext(OID);
    }
    int getChIndexFromPos(int row) {
        return videoDataBase.getChIndexFromPos(row);
    }
    int countEntries() {
        return videoDataBase.countEntries();
    }
    boolean contains (sck.Oid OID) {
        return videoDataBase.contains(OID);
    }

    //Function used by the internalServer via the DataBaseHandler to
    //decode the messages that come from the video ISABEL module.
    //It also introduces the new data in the MIB.
    //---------------------------------------------------------------

    void AnalizeMessage(int ivChIndex, DataInputStream dst)
                       throws java.io.IOException {
        int ivDesiredFR   = dst.readInt();
        int ivDesiredBW   = dst.readInt();
        int ivCodecBW     = dst.readInt();
        int ivSentFR      = dst.readInt();
        int ivSentBW      = dst.readInt();
        int ivReceivedBW  = dst.readInt();
        int ivEnsembledFR = dst.readInt();
        int ivPaintFR     = dst.readInt();

        int codecNameLen  = dst.readByte();
        byte [] codecNameB = new byte[codecNameLen];
        dst.read(codecNameB,0,codecNameLen);
        String ivCodec = new String(codecNameB);

        int ivQuality     = dst.readByte();

        int acronymLen  = dst.readByte();
        byte [] acronymB = new byte[acronymLen];
        dst.read(acronymB,0,acronymLen);
        String ivAcronym = new String(acronymB);

        int imageWidth = dst.readInt();
        int imageHeight= dst.readInt();
        String ivImageSize = new String(imageWidth + "x" + imageHeight);

        //System.out.println("VIDEO: "+ivChIndex+"    "
        //                   +ivDesiredFR+"    "+ivDesiredBW+"    "
        //                   +ivCodecBW+"    "
        //                   +ivSentFR+"    "+ivSentBW+"    "
        //                   +ivReceivedBW+" "
        //                   +ivEnsembledFR+"    "
        //                   +ivPaintFR+" "+ivCodec+" "+ivQuality+" "
        //                   +ivAcronym+" "+ivImageSize
        //                  );

        videoDataBase.setOrCreateNewVideoEntry(ivChIndex,
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

    //
    // Functions to delete all dat from the MIB and to control
    // if some data it's not being actualizated.
    //
    boolean deleteChannel (int ivChIndex) {
        return videoDataBase.deleteChannel(ivChIndex);
    }
    void IncrementAllChannelsTTL() {
        videoDataBase.IncrementAllChannelsTTL();
    }

    //
    // Function used by the SNMP_Server to get data from the MIB.
    //
    sck.smi GET (sck.Oid RequestedOid) {
        if (RequestedOid.sameTree("1.3.6.1.4.1.2781.1.5.1.1")) {
            int Columna = getColumn(RequestedOid);
            int ivChIndex = getInstanceOid(RequestedOid);
            switch (Columna) {
                case 1:
                    return videoDataBase.getChIndex(ivChIndex);
                case 2:
                    return videoDataBase.getAcronym(ivChIndex);
                case 3:
                    return videoDataBase.getCodec(ivChIndex);
                case 4:
                    return videoDataBase.getQuality(ivChIndex);
                case 5:
                    return videoDataBase.getImageSize(ivChIndex);
                case 6:
                    return videoDataBase.getDesiredFR(ivChIndex);
                case 7:
                    return videoDataBase.getDesiredBW(ivChIndex);
                case 8:
                    return videoDataBase.getCodecBW(ivChIndex);
                case 9:
                    return videoDataBase.getSentFR(ivChIndex);
                case 10:
                    return videoDataBase.getSentBW(ivChIndex);
                case 11:
                    return videoDataBase.getReceivedBW(ivChIndex);
                case 12:
                    return videoDataBase.getEnsembledFR(ivChIndex);
                case 13:
                    return videoDataBase.getPaintFR(ivChIndex);
                default:
                    return null;
            }
        }
        else return null;
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

