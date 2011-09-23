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
//FILE: shDisplayDataBaseHandler_t.java
//
//DESCRIPTION:
//    This class give acces to all the data of the shared display.
//    It's always used by the DataBaseHandler.
//

import java.io.*;

public class shDisplayDataBaseHandler_t {
    String shDisplayChIndexOid       = "1.3.6.1.4.1.2781.1.7.1.1.1";
    String shDisplayAcronymOid       = "1.3.6.1.4.1.2781.1.7.1.1.2";
    String shDisplayCodecOid         = "1.3.6.1.4.1.2781.1.7.1.1.3";
    String shDisplayQualityOid       = "1.3.6.1.4.1.2781.1.7.1.1.4";
    String shDisplayImageSizeOid     = "1.3.6.1.4.1.2781.1.7.1.1.5";
    String shDisplayDesiredFROid     = "1.3.6.1.4.1.2781.1.7.1.1.6";
    String shDisplayDesiredBWOid     = "1.3.6.1.4.1.2781.1.7.1.1.7";
    String shDisplayCodecBWOid       = "1.3.6.1.4.1.2781.1.7.1.1.8";
    String shDisplaySentFROid        = "1.3.6.1.4.1.2781.1.7.1.1.9";
    String shDisplaySentBWOid        = "1.3.6.1.4.1.2781.1.7.1.1.10";
    String shDisplayReceivedBWOid    = "1.3.6.1.4.1.2781.1.7.1.1.11";
    String shDisplayEnsembledFROid   = "1.3.6.1.4.1.2781.1.7.1.1.12";
    String shDisplayPaintFROid       = "1.3.6.1.4.1.2781.1.7.1.1.13";

    shDisplayDataBase_t shDisplayDataBase;

    shDisplayDataBaseHandler_t (shDisplayDataBase_t shDisplayDataBase) {
        this.shDisplayDataBase = shDisplayDataBase;
    }

    shDisplayDataBase_t getShDisplayDataBase () {
        return shDisplayDataBase;
    }

    sck.Oid getNextOid(sck.Oid OID) {
        return shDisplayDataBase.getNext(OID);
    }
    int countEntries() {
        return shDisplayDataBase.countEntries();
    }
    int getChIndexFromPos(int row) {
        return shDisplayDataBase.getChIndexFromPos(row);
    }
    boolean contains (sck.Oid OID) {
       return shDisplayDataBase.contains(OID);
    }

    //Function used by the internalServer via the DataBaseHandler to
    //decode the messages that come from the shared display ISABEL module.
    //It also introduces the new data in the MIB.
    //---------------------------------------------------------------

    void AnalizeMessage (int shdChIndex, DataInputStream dst)
                        throws java.io.IOException {
        int shdDesiredFR   = dst.readInt();
        int shdDesiredBW   = dst.readInt();
        int shdCodecBW     = dst.readInt();
        int shdSentFR      = dst.readInt();
        int shdSentBW      = dst.readInt();
        int shdReceivedBW  = dst.readInt();
        int shdEnsembledFR = dst.readInt();
        int shdPaintFR     = dst.readInt();

        int codecNameLen   = dst.readByte();
        byte [] codecNameB = new byte[codecNameLen];
        dst.read(codecNameB,0,codecNameLen);
        String shdCodec = new String(codecNameB);

        int shdQuality     = dst.readByte();

        int acronymLen  = dst.readByte();
        byte [] acronymB = new byte[acronymLen];
        dst.read(acronymB,0,acronymLen);
        String shdAcronym = new String(acronymB);

        int imageWidth = dst.readInt();
        int imageHeight= dst.readInt();
        String shdImageSize = new String(imageWidth + "x" + imageHeight);

        //System.out.println("SHDISP: "+shdChIndex+"    "
        //                   +shdDesiredFR+"    "+shdDesiredBW+"    "
        //                   +shdCodecBW+"    "
        //                   +shdSentFR+"    "+shdSentBW+"    "
        //                   +shdReceivedBW+" "
        //                   +shdPaintFR+" "+shdCodec+" "+shdQuality+" "
        //                   +shdAcronym+" "+shdImageSize);

        shDisplayDataBase.setOrCreateNewShDisplayEntry(shdChIndex,
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
    void setshDisplayChIndex (int shdChIndex) {
        shDisplayDataBase.newShDisplayEntry(shdChIndex);
    }
    void shDisplaySetAll(int    shdChIndex,
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
        shDisplayDataBase.setOrCreateNewShDisplayEntry(shdChIndex,
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

    boolean deleteChannel (int ivChIndex) {
      return shDisplayDataBase.deleteChannel(ivChIndex);
    }

    void IncrementAllChannelsTTL() {
      shDisplayDataBase.IncrementAllChannelsTTL();
    }

    //FUNCIONES PARA BUSCAR DATOS EN LA BASE DE DATOS Y ASI PODER
    //RESPONDER A LAS PETICIONES SNMP.

    sck.smi GET (sck.Oid RequestedOid) {
        if (RequestedOid.sameTree("1.3.6.1.4.1.2781.1.7.1.1")) {
            int Columna = getColumn(RequestedOid);
            int shdChIndex = getInstanceOid(RequestedOid);
            //System.out.println("La peticion realizada ha sido:");
            //System.out.println("Columna: "+Columna+" --> Ejemplar: "+shdChIndex);
            switch (Columna) {
                case 1:
                    return shDisplayDataBase.getChIndex(shdChIndex);
                case 2:
                    return shDisplayDataBase.getAcronym(shdChIndex);
                case 3:
                    return shDisplayDataBase.getCodec(shdChIndex);
                case 4:
                    return shDisplayDataBase.getQuality(shdChIndex);
                case 5:
                    return shDisplayDataBase.getImageSize(shdChIndex);
                case 6:
                    return shDisplayDataBase.getDesiredFR(shdChIndex);
                case 7:
                    return shDisplayDataBase.getDesiredBW(shdChIndex);
                case 8:
                    return shDisplayDataBase.getCodecBW(shdChIndex);
                case 9:
                    return shDisplayDataBase.getSentFR(shdChIndex);
                case 10:
                    return shDisplayDataBase.getSentBW(shdChIndex);
                case 11:
                    return shDisplayDataBase.getReceivedBW(shdChIndex);
                case 12:
                    return shDisplayDataBase.getEnsembledFR(shdChIndex);
                case 13:
                    return shDisplayDataBase.getPaintFR(shdChIndex);
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

