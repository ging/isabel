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

public class irouterDataBase_t extends DataBase_t {

    // Number of columns,

    int RCVTrowElements= irouterRCVTableModel_t.IROUTER_RCV_STATS.length;
    int SNDTrowElements= irouterSNDTableModel_t.IROUTER_SND_STATS.length;

    //MIB elements:
    //-------------
    Hashtable recvIrouterTable;
    Hashtable sendIrouterTable;
    IntegerDataBaseEntry_t irouterTotalRecv;
    IntegerDataBaseEntry_t irouterTotalSend;

    //Constructor:
    //------------
    irouterDataBase_t () {

        super();

        //initialize the MIB elements:
        //----------------------------
        recvIrouterTable = new Hashtable();
        sendIrouterTable = new Hashtable();
        //irouterTotalRecv = 0;
        //irouterTotalRecvTTL = 0;
        //try  { AddOid (new sck.Oid("1.3.6.1.4.1.2781.1.2.2.0")); }catch (Exception e) {};
        //irouterTotalSend = 0;
        //irouterTotalRecvTTL = 0;
        //try  { AddOid (new sck.Oid("1.3.6.1.4.1.2781.1.2.4.0")); }catch (Exception e) {};
        //isSorted=false;
    }

    //Functions that give information about the elements in the MIB for the visualizationThread:
    //--------------------------------------------------------------
    String getRCVMediaFromRow(int pos) {
        Object [] keys = recvIrouterTable.keySet().toArray();
        //irouterEntryKey_t auxIrtEK = (irouterEntryKey_t)keys[pos];
        //return auxIrtEK.getMediaIndex();
        String auxKey=(String)keys[pos];
        return getMediaFromSt(auxKey);
    }

    String getSNDMediaFromRow(int pos) {
        Object [] keys = sendIrouterTable.keySet().toArray();
        //irouterEntryKey_t auxIrtEK = (irouterEntryKey_t)keys[pos];
        //return auxIrtEK.getMediaIndex();
        String auxKey=(String)keys[pos];
        return getMediaFromSt(auxKey);
    }

    int getRCVSSRCFromRow(int pos) {
        Object [] keys = recvIrouterTable.keySet().toArray();
        //irouterEntryKey_t auxIrtEK = (irouterEntryKey_t)keys[pos];
        //return auxIrtEK.getMediaIndex();
        String auxKey=(String)keys[pos];
        return getSSRCFromSt(auxKey);
    }

    int getSNDSSRCFromRow(int pos) {
        Object [] keys = sendIrouterTable.keySet().toArray();
        //irouterEntryKey_t auxIrtEK = (irouterEntryKey_t)keys[pos];
        //return auxIrtEK.getMediaIndex();
        String auxKey=(String)keys[pos];
        return getSSRCFromSt(auxKey);
    }

    int countRCVEntries() {
        return recvIrouterTable.size();
    }
    int countSNDEntries() {
        return sendIrouterTable.size();
    }

    int getSSRCFromSt (String st) {
        StringTokenizer tok = new StringTokenizer(st,"|");
        Integer v= new Integer((String)tok.nextToken());
        return v.intValue();
    }

    String getMediaFromSt (String st) {
        StringTokenizer tok = new StringTokenizer(st,"|");
        tok.nextToken();
        String v= new String((String)tok.nextToken());
        return v;
    }


    //Function for deleting the data that hasn't been actualizated:
    //-------------------------------------------------------------
    void IncrementAllChannelsTTL() {
        Enumeration keys;

        //ReceivedDataTable:
        keys = recvIrouterTable.keys();
        while(keys.hasMoreElements()) {
            String auxkey = (String)keys.nextElement();
            irouterEntry_t auxEntry =
                (irouterEntry_t)recvIrouterTable.get(auxkey);
            if ((auxEntry.getirouterTTL())<10) {
                auxEntry.incrementTTL();
                recvIrouterTable.put(auxkey,auxEntry);
            } else {
                deleteRecvChannel(auxkey);
            }
        }

        //SendDataTable:
        keys = sendIrouterTable.keys();
        while(keys.hasMoreElements()) {
            String auxkey = (String)keys.nextElement();
            irouterEntry_t auxEntry =
                (irouterEntry_t)sendIrouterTable.get(auxkey);
            if ((auxEntry.getirouterTTL())<10) {
                auxEntry.incrementTTL();
                sendIrouterTable.put(auxkey,auxEntry);
            } else {
                deleteSendChannel(auxkey);
            }
        }

        //Totals:
        //-------
        if (irouterTotalSend!=null) {
            if (irouterTotalSend.getTTL()<10) irouterTotalSend.incrementTTL();
                {
                    irouterTotalSend.takeOut();
                    irouterTotalSend=null;
                }
        }
        if (irouterTotalRecv!=null) {
            if (irouterTotalRecv.getTTL()<10) irouterTotalRecv.incrementTTL();
                {
                    irouterTotalRecv.takeOut();
                    irouterTotalRecv= null;
                }
        }
    }

    boolean deleteRecvChannel(String irtEK) {
        if (recvIrouterTable.containsKey(irtEK)) {
            recvIrouterTable.remove(irtEK);
            delOldOids(1, getSSRCFromSt(irtEK), getMediaFromSt(irtEK));
            return true;
        }
        return false;
    }

    boolean deleteSendChannel(String irtEK) {
        if (sendIrouterTable.containsKey(irtEK)) {
            sendIrouterTable.remove(irtEK);
            delOldOids(3,getSSRCFromSt(irtEK), getMediaFromSt(irtEK));
            return true;
        }
        return false;
    }

    //Private functions to add and delete Oids to the static vector:
    //--------------------------------------------------------------
    void addNewOids (int tableRef, int irtSSRC, String irtMedia) {
        try {
            for (int i= 1; i <= RCVTrowElements; i++) {
                AddOid(new sck.Oid("1.3.6.1.4.1.2781.1.2."+tableRef+".1."+i+"."+irtSSRC+"."+irouterMediaID.getMediaID(irtMedia)));
            }
        }catch (Exception e) {}
    }

    void delOldOids (int tableRef, int irtSSRC, String irtMedia) {
        try {
            for (int i= 1; i <= RCVTrowElements; i++) {
                DelOid(new sck.Oid("1.3.6.1.4.1.2781.1.2."+tableRef+".1."+i+"."+irtSSRC+"."+irouterMediaID.getMediaID(irtMedia)));
            }
        }catch (Exception e) {}
    }

    //Public functions to add new elements in the MIB:
    //------------------------------------------------
    void setOrCreateNewRCVEntry (int    irtSSRC,
                                 String irtMedia,
                                 String irtSourceIP,
                                 int    irtDataNumPack,
                                 int    irtDataBW,
                                 int    irtFecNumPack,
                                 int    irtFecBW,
                                 int    irtRecpPkts,
                                 int    irtLostPkts,
                                 int    irtDisorderedPkts,
                                 int    irtDuplicatedPkts
                                ) {
            irouterEntry_t irtE =
                new irouterEntry_t (irtSSRC,
                                    irtMedia,
                                    irtSourceIP,
                                    irtDataNumPack,
                                    irtDataBW,
                                    irtFecNumPack,
                                    irtFecBW,
                                    0,           //emission parameters
                                    0,           //emission parameters
                                    irtRecpPkts,
                                    irtLostPkts,
                                    irtDisorderedPkts,
                                    irtDuplicatedPkts
                                   );
            if (recvIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
                recvIrouterTable.remove(irtSSRC+"|"+irtMedia);
                recvIrouterTable.put(irtSSRC+"|"+irtMedia,irtE);
            } else {
                newRCVIrouterEntry (irtSSRC,
                                    irtMedia,
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
    }

 void setOrCreateNewSNDEntry (int    irtSSRC,
                              String irtMedia,
                              String irtSourceIP,
                              int    irtDataNumPack,
                              int    irtDataBW,
                              int    irtFecNumPack,
                              int    irtFecBW,
                              int    irtEnqueuedPkts,
                              int    irtDiscardedPkts
                             ) {
         irouterEntry_t irtE =
            new irouterEntry_t (irtSSRC,
                                irtMedia,
                                irtSourceIP,
                                irtDataNumPack,
                                irtDataBW,
                                irtFecNumPack,
                                irtFecBW,
                                irtEnqueuedPkts,
                                irtDiscardedPkts,
                                0,           //reception parameters
                                0,           //reception parameters
                                0,           //reception parameters
                                0            //reception parameters
                               );
         if (sendIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            sendIrouterTable.remove(irtSSRC+"|"+irtMedia);
            sendIrouterTable.put(irtSSRC+"|"+irtMedia,irtE);
         } else {
             newSNDIrouterEntry (irtSSRC,
                                 irtMedia,
                                 irtSourceIP,
                                 irtDataNumPack,
                                 irtDataBW,
                                 irtFecNumPack,
                                 irtFecBW,
                                 irtEnqueuedPkts,
                                 irtDiscardedPkts
                                );
         }
   }

    void newRCVIrouterEntry (int    irtSSRC, String irtMedia) {
        irouterEntry_t irouterEntry =
            new irouterEntry_t (irtSSRC, irtMedia, null, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        recvIrouterTable.put (irtSSRC+"|"+irtMedia,irouterEntry);
        addNewOids (1,irtSSRC, irtMedia);
    }
    void newRCVIrouterEntry (int    irtSSRC,
                             String irtMedia,
                             String irtSourceIP,
                             int    irtDataNumPack,
                             int    irtDataBW,
                             int    irtFecNumPack,
                             int    irtFecBW,
                             int    irtRecpPkts,
                             int    irtLostPkts,
                             int    irtDisorderedPkts,
                             int    irtDuplicatedPkts
                            ) {
        irouterEntry_t irouterEntry =
            new irouterEntry_t (irtSSRC,
                                irtMedia,
                                irtSourceIP,
                                irtDataNumPack,
                                irtDataBW,
                                irtFecNumPack,
                                irtFecBW,
                                0,           //emission parameters
                                0,           //emission parameters
                                irtRecpPkts,
                                irtLostPkts,
                                irtDisorderedPkts,
                                irtDuplicatedPkts
                               );
        recvIrouterTable.put(irtSSRC+"|"+irtMedia,irouterEntry);
        addNewOids (1,irtSSRC, irtMedia);
        isSorted=false;
    }

    void newSNDIrouterEntry (int    irtSSRC, String irtMedia) {
        irouterEntry_t irouterEntry =
            new irouterEntry_t (irtSSRC, irtMedia, null, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        sendIrouterTable.put (irtSSRC+"|"+irtMedia,irouterEntry);
        addNewOids (3,irtSSRC, irtMedia);
    }
    void newSNDIrouterEntry (int    irtSSRC,
                             String irtMedia,
                             String irtSourceIP,
                             int    irtDataNumPack,
                             int    irtDataBW,
                             int    irtFecNumPack,
                             int    irtFecBW,
                             int    irtEnqueuedPkts,
                             int    irtDiscardedPkts
                            ) {
        irouterEntry_t irouterEntry =
            new irouterEntry_t (irtSSRC,
                                irtMedia,
                                irtSourceIP,
                                irtDataNumPack,
                                irtDataBW,
                                irtFecNumPack,
                                irtFecBW,
                                irtEnqueuedPkts,
                                irtDiscardedPkts,
                                0,           
                                0,           
                                0,           
                                0            
                               );
        sendIrouterTable.put (irtSSRC+"|"+irtMedia,irouterEntry);
        addNewOids (3,irtSSRC, irtMedia);
        isSorted=false;
    }
    void setOrCreateRCVTotal (int irtRCVTotal) {
        if (irouterTotalRecv==null) {
            irouterTotalRecv = new IntegerDataBaseEntry_t (irtRCVTotal,"1.3.6.1.4.1.2781.1.2.2.0");
            return;
        }
        irouterTotalRecv.setValue(irtRCVTotal);
    }

    void setOrCreateSNDTotal (int irtSNDTotal) {
        if (irouterTotalSend==null) {
            irouterTotalSend = new IntegerDataBaseEntry_t (irtSNDTotal,"1.3.6.1.4.1.2781.1.2.4.0");
            return;
        }
        irouterTotalSend.setValue(irtSNDTotal);
    }
    //void setIrouterRCVTotal (int irtRCVTotal) {
    //                irouterTotalRecv = irtRCVTotal;
    //                irouterTotalRecvTTL = 0;
    //                }
    //void setIrouterSNDTotal (int irtSNDTotal) {
    //                irouterTotalSend = irtSNDTotal;
    //                irouterTotalSendTTL = 0;
    //                }

    //Functions to get each of the data elements individualy:
    //-------------------------------------------------------
    sck.Integer getIrouterRCVTotal () { return irouterTotalRecv.getValue();};
    sck.Integer getIrouterSNDTotal () { return irouterTotalSend.getValue();};

    sck.Integer getSSRC (int irtTable, int irtSSRC, String irtMedia) {
      if (irtTable==1) {
         if (recvIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) recvIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterSSRC();
         }
         else return null;
      }
      if (irtTable==3) {
         if (sendIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) sendIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterSSRC();
         }
         else return null;
      }
      else return null;
    }

    sck.OctetString getMedia (int irtTable, int irtSSRC, String irtMedia) {
      if (irtTable==1) {
         if (recvIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) recvIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterMedia();
         }
         else return null;
      }
      if (irtTable==3) {
         if (sendIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) sendIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterMedia();
         }
         else return null;
      }
      else return null;
    }

    sck.OctetString getSourceIP (int irtTable, int irtSSRC, String irtMedia) {
      if (irtTable==1) {
         if (recvIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) recvIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterSourceIP();
         }
         else return null;
      }
      if (irtTable==3) {
         if (sendIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) sendIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterSourceIP();
         }
         else return null;
      }
      else return null;
    }

    sck.Integer getDataNumPack (int irtTable, int irtSSRC, String irtMedia) {
      if (irtTable==1) {
         if (recvIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) recvIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterDataNumPack();
         }
         else return null;
      }
      if (irtTable==3) {
         if (sendIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) sendIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterDataNumPack();
         }
         else return null;
      }
      else return null;
    }

    sck.Integer getDataBW (int irtTable, int irtSSRC, String irtMedia) {
      if (irtTable==1) {
         if (recvIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) recvIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterDataBW();
         }
         else return null;
      }
      if (irtTable==3) {
         if (sendIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) sendIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterDataBW();
         }
         else return null;
      }
      else return null;
    }

    sck.Integer getFecNumPack (int irtTable, int irtSSRC, String irtMedia) {
      if (irtTable==1) {
         if (recvIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) recvIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterFecNumPack();
         }
         else return null;
      }
      if (irtTable==3) {
         if (sendIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) sendIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterFecNumPack();
         }
         else return null;
      }
      else return null;
    }

     sck.Integer getFecBW (int irtTable, int irtSSRC, String irtMedia) {
      if (irtTable==1) {
         if (recvIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) recvIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterFecBW();
         }
         else return null;
      }
      if (irtTable==3) {
         if (sendIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) sendIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterFecBW();
         }
         else return null;
      }
      else return null;
    }

    sck.Integer getEnqueuedPkts (int irtTable, int irtSSRC, String irtMedia) {
      if (irtTable==1) {
         if (recvIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) recvIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterEnqueuedPkts();
         }
         else return null;
      }
      if (irtTable==3) {
         if (sendIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) sendIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterEnqueuedPkts();
         }
         else return null;
      }
      else return null;
    }

    sck.Integer getDiscardedPkts (int irtTable, int irtSSRC, String irtMedia) {
      if (irtTable==1) {
         if (recvIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) recvIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterDiscardedPkts();
         }
         else return null;
      }
      if (irtTable==3) {
         if (sendIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) sendIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterDiscardedPkts();
         }
         else return null;
      }
      else return null;
    }

    sck.Integer getRecpPkts (int irtTable, int irtSSRC, String irtMedia) {
      if (irtTable==1) {
         if (recvIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) recvIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterRecpPkts();
         }
         else return null;
      }
      if (irtTable==3) {
         if (sendIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) sendIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterRecpPkts();
         }
         else return null;
      }
      else return null;
    }

    sck.Integer getLostPkts (int irtTable, int irtSSRC, String irtMedia) {
      if (irtTable==1) {
         if (recvIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) recvIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterLostPkts();
         }
         else return null;
      }
      if (irtTable==3) {
         if (sendIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) sendIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterLostPkts();
         }
         else return null;
      }
      else return null;
    }

    sck.Integer getDisorderedPkts (int irtTable, int irtSSRC, String irtMedia) {
      if (irtTable==1) {
         if (recvIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) recvIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterDisorderedPkts();
         }
         else return null;
      }
      if (irtTable==3) {
         if (sendIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) sendIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterDisorderedPkts();
         }
         else return null;
      }
      else return null;
    }

    sck.Integer getDuplicatedPkts (int irtTable, int irtSSRC, String irtMedia) {
      if (irtTable==1) {
         if (recvIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) recvIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterDuplicatedPkts();
         }
         else return null;
      }
      if (irtTable==3) {
         if (sendIrouterTable.containsKey(irtSSRC+"|"+irtMedia)) {
            irouterEntry_t auxIrouterEntry = (irouterEntry_t) sendIrouterTable.get(irtSSRC+"|"+irtMedia);
            return auxIrouterEntry.getirouterDuplicatedPkts();
         }
         else return null;
      }
      else return null;
    }

}//End of irouterDataBase_t

class irouterEntry_t {
    sck.Integer     irouterSSRC;
    sck.OctetString irouterMedia;
    sck.OctetString irouterSourceIP;
    sck.Integer     irouterDataNumPack;
    sck.Integer     irouterDataBW;
    sck.Integer     irouterFecNumPack;
    sck.Integer     irouterFecBW;
    sck.Integer     irouterEnqueuedPkts;
    sck.Integer     irouterDiscardedPkts;
    sck.Integer     irouterRecpPkts;
    sck.Integer     irouterLostPkts;
    sck.Integer     irouterDisorderedPkts;
    sck.Integer     irouterDuplicatedPkts;
    int             ttl;

    //Constructors:
    //-------------
    irouterEntry_t (int    irtSSRC,
                    String irtMedia,
                    String irtSourceIP,
                    int    irtDataNumPack,
                    int    irtDataBW,
                    int    irtFecNumPack,
                    int    irtFecBW,
                    int    irtEnqueuedPkts,
                    int    irtDiscardedPkts,
                    int    irtRecpPkts,
                    int    irtLostPkts,
                    int    irtDisorderedPkts,
                    int    irtDuplicatedPkts
                   ) {
        try {
            irouterSSRC              = new sck.Integer(irtSSRC);
            irouterMedia             = new sck.OctetString(irtMedia);
            irouterSourceIP          = new sck.OctetString(irtSourceIP);
            irouterDataNumPack       = new sck.Integer(irtDataNumPack);
            irouterDataBW            = new sck.Integer(irtDataBW);
            irouterFecNumPack        = new sck.Integer(irtFecNumPack);
            irouterFecBW             = new sck.Integer(irtFecBW);
            irouterEnqueuedPkts      = new sck.Integer(irtEnqueuedPkts);
            irouterDiscardedPkts     = new sck.Integer(irtDiscardedPkts);
            irouterRecpPkts          = new sck.Integer(irtRecpPkts);
            irouterLostPkts          = new sck.Integer(irtLostPkts);
            irouterDisorderedPkts    = new sck.Integer(irtDisorderedPkts);
            irouterDuplicatedPkts    = new sck.Integer(irtDuplicatedPkts);
            ttl=0;
        } catch (Exception exc) {
            System.out.println("Excepcion creando entrada:"+exc);
        };
    }
    irouterEntry_t (sck.Integer     irtSSRC,
                    sck.OctetString irtMedia,
                    sck.OctetString irtSourceIP,
                    sck.Integer     irtDataNumPack,
                    sck.Integer     irtDataBW,
                    sck.Integer     irtFecNumPack,
                    sck.Integer     irtFecBW,
                    sck.Integer     irtEnqueuedPkts,
                    sck.Integer     irtDiscardedPkts,
                    sck.Integer     irtRecpPkts,
                    sck.Integer     irtLostPkts,
                    sck.Integer     irtDisorderedPkts,
                    sck.Integer     irtDuplicatedPkts
                   ) {
        irouterSSRC              = irtSSRC;
        irouterMedia             = irtMedia;
        irouterSourceIP          = irtSourceIP;
        irouterDataNumPack       = irtDataNumPack;
        irouterDataBW            = irtDataBW;
        irouterFecNumPack        = irtFecNumPack;
        irouterFecBW             = irtFecBW;
        irouterEnqueuedPkts      = irtEnqueuedPkts;
        irouterDiscardedPkts     = irtDiscardedPkts;
        irouterRecpPkts          = irtRecpPkts;
        irouterLostPkts          = irtLostPkts;
        irouterDisorderedPkts    = irtDisorderedPkts;
        irouterDuplicatedPkts    = irtDuplicatedPkts;
        ttl=0;
    }

    //Functions to get each of the elements:
    //--------------------------------------
    sck.Integer     getirouterSSRC          () { return irouterSSRC;          }
    sck.OctetString getirouterMedia         () { return irouterMedia;         }
    sck.OctetString getirouterSourceIP      () { return irouterSourceIP;      }
    sck.Integer     getirouterDataNumPack   () { return irouterDataNumPack;   }
    sck.Integer     getirouterDataBW        () { return irouterDataBW;        }
    sck.Integer     getirouterFecNumPack    () { return irouterFecNumPack;    }
    sck.Integer     getirouterFecBW         () { return irouterFecBW;         }
    sck.Integer     getirouterEnqueuedPkts  () { return irouterEnqueuedPkts;  }
    sck.Integer     getirouterDiscardedPkts () { return irouterDiscardedPkts; }
    sck.Integer     getirouterRecpPkts      () { return irouterRecpPkts;      }
    sck.Integer     getirouterLostPkts      () { return irouterLostPkts;      }
    sck.Integer     getirouterDisorderedPkts() { return irouterDisorderedPkts; }
    sck.Integer     getirouterDuplicatedPkts() { return irouterDuplicatedPkts; }
    int             getirouterTTL           () { return ttl;                  }
    void            incrementTTL            () { ttl++;                       }
}//End of irouterEntry_t


class irouterEntryKey_t {
    int    MediaIndex;
    String SourceIP;

    irouterEntryKey_t (int MI, String SIP) {
        MediaIndex = MI;
        SourceIP = SIP;
    }

    public boolean equals(Object obj) {
        irouterEntryKey_t aux = (irouterEntryKey_t)obj;

        boolean value = (    (MediaIndex==aux.MediaIndex)
                          &&  SourceIP.equals(aux.SourceIP)
                        );
        return value;
    }

    int getMediaIndex  () { return MediaIndex; };
    String getSourceIP () { return SourceIP;   };
}

