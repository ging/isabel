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

import java.io.*;
import java.util.*;

public class irouterDataBaseHandler_t {

    irouterDataBase_t irouterDataBase;

    irouterDataBaseHandler_t (irouterDataBase_t irouterDataBase) {
        this.irouterDataBase = irouterDataBase;
    }

    // Functions that help the Monitor GUI to find data in the dataBase:

    irouterDataBase_t getIrouterDataBase () { return irouterDataBase; };

    int getRCVSSRCFromRow(int row) {
        return irouterDataBase.getRCVSSRCFromRow(row);
    }

    int getSNDSSRCFromRow(int row) {
        return irouterDataBase.getSNDSSRCFromRow(row);
    }

    String getRCVMediaFromRow(int row) {
        return irouterDataBase.getRCVMediaFromRow(row);
    }

    String getSNDMediaFromRow(int row) {
        return irouterDataBase.getSNDMediaFromRow(row);
    }

    sck.Oid getNextOid(sck.Oid OID) {
        return irouterDataBase.getNext(OID);
    }

    int countRCVEntries() {
        return irouterDataBase.countRCVEntries();
    }

    int countSNDEntries() {
        return irouterDataBase.countSNDEntries();
    }

    boolean contains (sck.Oid OID) {
        return irouterDataBase.contains(OID);
    }

    // Function used by the internalServer via the DataBaseHandler to
    // decode the messages that come from the irouter
    // It also introduces the new data in the MIB.

    void AnalizeMessage (int irtChIndex, DataInputStream dst)
                        throws java.io.IOException {
            int irtTableIndex = dst.readInt();
            if (irtTableIndex == 2) {
                byte [] irtMediaB     = new byte [6];
                dst.read(irtMediaB,0,6);
                String irtMediaStr= new String (irtMediaB).trim();

                int irtDataNumPack    = dst.readInt();
                int irtDataBW         = dst.readInt();
                int irtFecNumPack     = dst.readInt();
                int irtFecBW          = dst.readInt();
                int irtRecpPkts       = dst.readInt();
                int irtLostPkts       = dst.readInt();
                int irtDisorderedPkts = dst.readInt();
                int irtDuplicatedPkts = dst.readInt();
                
                int IPlen= (int)dst.readByte();
                byte [] irtSourceIPB = new byte[IPlen];
                dst.read(irtSourceIPB,0,IPlen);
                String irtSourceIP = new String(irtSourceIPB);

                //System.out.println("IRORCV: "+irtChIndex+
                //                   " irtTable="+irtTableIndex+
                //                   " irtMediaStr="+irtMediaStr+
                //                   " irtDataNumPack="+irtDataNumPack+
                //                   " irtDataBW="+irtDataBW+
                //                   " irtFecNumPack="+irtFecNumPack+
                //                   " irtFecBW="+irtFecBW+
                //                   " irtLostPkts="+irtLostPkts+
                //                   " irtDisorderedPkts="+irtDisorderedPkts+
                //                   " irtDuplicatedPkts="+irtDuplicatedPkts+
                //                   " SourceIP="+irtSourceIP
                //                  );

                irouterDataBase.setOrCreateNewRCVEntry(
                                        irtChIndex,
                                        irtMediaStr,
                                        irtSourceIP,
                                        irtDataNumPack,
                                        irtDataBW,
                                        irtFecNumPack,
                                        irtFecBW,
                                        irtRecpPkts,
                                        irtLostPkts,
                                        irtDisorderedPkts,
                                        irtDuplicatedPkts
                                        );
            }
            if (irtTableIndex == 1) {
                irouterDataBase.setOrCreateRCVTotal(dst.readInt()*100/8);
            }
            if (irtTableIndex == 4) {
                byte [] irtMediaB    = new byte [6];
                dst.read(irtMediaB,0,6);
                String irtMediaStr= new String (irtMediaB).trim();

                int irtDataNumPack   = dst.readInt();
                int irtDataBW        = dst.readInt();
                int irtFecNumPack    = dst.readInt();
                int irtFecBW         = dst.readInt();
                int irtEnqueuedPkts  = dst.readInt();
                int irtDiscardedPkts = dst.readInt();
               
                int IPlen= (int)dst.readByte();
                byte [] irtSourceIPB = new byte[IPlen];
                dst.read(irtSourceIPB,0,IPlen);
                String irtSourceIP = new String(irtSourceIPB);

                //System.out.println("IROSND: "+irtChIndex+
                //                   " irtTable="+irtTableIndex+
                //                   " irtMediaStr="+irtMediaStr+
                //                   " irtDataNumPack="+irtDataNumPack+
                //                   " irtDataBW="+irtDataBW+
                //                   " irtFecNumPack="+irtFecNumPack+
                //                   " irtFecBW="+irtFecBW+
                //                   " irtDiscardedPkts="+irtDiscardedPkts+
                //                   " SourceIP="+irtSourceIP
                //                  );

                irouterDataBase.setOrCreateNewSNDEntry(
                                        irtChIndex,
                                        irtMediaStr,
                                        irtSourceIP,
                                        irtDataNumPack,
                                        irtDataBW,
                                        irtFecNumPack,
                                        irtFecBW,
                                        irtEnqueuedPkts,
                                        irtDiscardedPkts
                                        );
            }
            if (irtTableIndex == 3) {
                irouterDataBase.setOrCreateSNDTotal(dst.readInt()*100/8);
            }
    }

    // Functions to control the channels that are not actualizated:
    void IncrementAllChannelsTTL() {
        irouterDataBase.IncrementAllChannelsTTL();
    }

    //
    // Function used by the SNMPServer to get data from the MIB:
    //
    sck.smi GET (sck.Oid RequestedOid) {
        try {
            if (RequestedOid.equals(new sck.Oid ("1.3.6.1.4.1.2781.1.2.2.0")))
                return irouterDataBase.getIrouterRCVTotal();
            if (RequestedOid.equals(new sck.Oid ("1.3.6.1.4.1.2781.1.2.4.0")))
                return irouterDataBase.getIrouterSNDTotal();
        } catch (Exception e) { return null; }
        if (RequestedOid.sameTree("1.3.6.1.4.1.2781.1.2.1.1")) {
            if (RequestedOid.getSubIdentifiers().length == 14) {
                int Column  = RequestedOid.getSubIdentifiers() [11];
                int SSRC    = RequestedOid.getSubIdentifiers() [12];
                int MediaID = RequestedOid.getSubIdentifiers() [13];
                String Media= irouterMediaID.getMediaStr(MediaID);
                switch (Column) {
                    case 1:
                        return irouterDataBase.getSSRC (1, SSRC, Media);
                    case 2:
                        return irouterDataBase.getMedia (1, SSRC, Media);
                    case 3:
                        return irouterDataBase.getSourceIP (1, SSRC, Media);
                    case 4:
                        return irouterDataBase.getDataNumPack (1, SSRC, Media);
                    case 5:
                        return irouterDataBase.getDataBW (1, SSRC, Media);
                    case 6:
                        return irouterDataBase.getFecNumPack (1, SSRC, Media);
                    case 7:
                        return irouterDataBase.getFecBW (1, SSRC, Media);
                    case 8:
                        return irouterDataBase.getRecpPkts (1, SSRC, Media);
                    case 9:
                        return irouterDataBase.getLostPkts (1, SSRC, Media);
                    case 10:
                        return irouterDataBase.getDisorderedPkts (1, SSRC, Media);
                    case 11:
                        return irouterDataBase.getDuplicatedPkts (1, SSRC, Media);
                    default:
                        return null;
                }
            } else return null;
        }
        if (RequestedOid.sameTree("1.3.6.1.4.1.2781.1.2.3.1")) {
            if (RequestedOid.getSubIdentifiers().length == 14) {
                int Column  = RequestedOid.getSubIdentifiers() [11];
                int SSRC    = RequestedOid.getSubIdentifiers() [12];
                int MediaID = RequestedOid.getSubIdentifiers() [13];
                String Media= irouterMediaID.getMediaStr(MediaID);
                switch (Column) {
                   case 1:
                       return irouterDataBase.getSSRC (3, SSRC, Media);
                   case 2:
                       return irouterDataBase.getMedia (3, SSRC, Media);
                   case 3:
                       return irouterDataBase.getSourceIP (3, SSRC, Media);
                   case 4:
                       return irouterDataBase.getDataNumPack (3, SSRC, Media);
                   case 5:
                       return irouterDataBase.getDataBW (3, SSRC, Media);
                   case 6:
                       return irouterDataBase.getFecNumPack (3, SSRC, Media);
                   case 7:
                       return irouterDataBase.getFecBW (3, SSRC, Media);
                   case 8:
                       return irouterDataBase.getEnqueuedPkts (3, SSRC, Media);
                   case 9:
                       return irouterDataBase.getDiscardedPkts (3, SSRC, Media);
                   default:
                       return null;
                }
            } else return null;
        }
        return null;
    }

}

