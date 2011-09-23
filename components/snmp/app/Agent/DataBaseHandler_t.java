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
//*********************************************************************
//CLASE PERTENECIENTE A: ISABELAgent
//
//DESCRIPCIÓN: Clase encargada de proporcionar acceso a los datos a
//    los distintos Threads, de forma que en estas no deban hacerse
//    modificaciones para añadir la gestion de nuevos compoenetes
//    de Isabel.
//
//CLASES RELACIONADAS:
//

import java.io.*;

public class DataBaseHandler_t {
    //CAMPOS:
    //1.- Debe haber una base de Datos por cada rama de la MIB implementada:
    //----------------------------------------------------------------------

    //-->Audio:
    audioDataBase_t audioDataBase;
    audioDataBaseHandler_t audioDataBaseHandler;

    //-->Video:
    videoDataBase_t videoDataBase;
    videoDataBaseHandler_t videoDataBaseHandler;

    //-->SharedDisplay:
    shDisplayDataBase_t shDisplayDataBase;
    shDisplayDataBaseHandler_t shDisplayDataBaseHandler;

    //-->IRouter:
    irouterDataBase_t irouterDataBase;
    irouterDataBaseHandler_t irouterDataBaseHandler;

    DataBaseHandler_t () {

        //-->Audio:
        audioDataBase = new audioDataBase_t();
        audioDataBaseHandler = new audioDataBaseHandler_t(audioDataBase);

        //-->Video:
        videoDataBase = new videoDataBase_t();
        videoDataBaseHandler = new videoDataBaseHandler_t(videoDataBase);

        //-->ShDisplay:
        shDisplayDataBase = new shDisplayDataBase_t();
        shDisplayDataBaseHandler = new shDisplayDataBaseHandler_t(shDisplayDataBase);

        //-->IRouter:
        irouterDataBase = new irouterDataBase_t();
        irouterDataBaseHandler = new irouterDataBaseHandler_t(irouterDataBase);
    }

    //3.- Funciones que invoca la thread internalCommServer_t:
    //--------------------------------------------------------

    //Analizar mensajes que llegan de los modulos de ISAEBEL.
    //Aqui se distribuyen al modulo de datos que corresponda.
    void AnalizeDataMessage(String compId, int channelId, DataInputStream dst) {
        try {
            //-->Audio:
            if (compId.equals("AUD")) {
                audioDataBaseHandler.AnalizeMessage(channelId,dst);
                return;
            }

            //-->Video:
            if (compId.equals("VID")) {
                videoDataBaseHandler.AnalizeMessage(channelId,dst);
                return;
            }

            //-->shDisplay:
            if (compId.equals("SHD")) {
                shDisplayDataBaseHandler.AnalizeMessage(channelId,dst);
                return;
            }

            //-->Irouter:
            if (compId.equals("IRT")) {
                irouterDataBaseHandler.AnalizeMessage(channelId,dst);
                return;
            }

        } catch (java.io.IOException e) {
            System.out.println("Excepcion en AnalizeDataMessage " +
                               "de DataBaseHandler:"+e);
        };
    }

    //Analizar mensajes de control que llegan de los modulos de ISABEL.
    //Aqui se distribuyen al modulo de datos que corresponda.
    void AnalizeControlMessage(String compId, int channelId) {
        // A dia de hoy los mensajes de control son unicos
        // e indican el borrado de un determinado canal.

        //-->Audio:
        if (compId.equals("AUD")) {
            audioDataBaseHandler.deleteChannel(channelId);
            return;
        }

        //-->Video:
        if (compId.equals("VID")) {
            videoDataBaseHandler.deleteChannel(channelId);
            return;
        }

        //-->ShDisplay:
        if (compId.equals("SHD")) {
            shDisplayDataBaseHandler.deleteChannel(channelId);
            return;
        }

        //-->IRouter:
        //if (compId.equals("IRT")) {
        //    irouterDataBaseHandler.AnalizeControlMessage(channelId);
        //    return;
        //}
    }

    // Para actualizar el tiempo que llevan sin modificarse los datos:
    // y borrar aquellos que ya estan obsoletos.
    void IncrementAllChannelsTTL() {
        audioDataBaseHandler.IncrementAllChannelsTTL();
        videoDataBaseHandler.IncrementAllChannelsTTL();
        shDisplayDataBaseHandler.IncrementAllChannelsTTL();
        irouterDataBaseHandler.IncrementAllChannelsTTL();
    }

    // Para eliminar de la Base de Datos aquellos canales que no se
    // han actualizado en un largo periodo de tiempo y por tanto han
    // debido morir
    //void DeleteDeadChannels() {
    //    audioDataBase.DeleteDeadChannels();
    //    videoDataBase.DeleteDeadChannels();
    //    shDisplayDataBaseHandler.DeleteDeadChannels();
    //}

    //4.- Funciones que invoca la Thread SNMPServer_t:
    //------------------------------------------------
    sck.smi getData(sck.Oid RequestedOid) {
        if (!DataBase_t.contains(RequestedOid))
            return null;

        //-->Audio:
        if (RequestedOid.sameTree(DataBase_t.AUDIO)) {
            return audioDataBaseHandler.GET(RequestedOid);
        }

        //-->Video:
        if (RequestedOid.sameTree(DataBase_t.VIDEO)) {
            return videoDataBaseHandler.GET(RequestedOid);
        }

        //-->ShDisplay:
        if (RequestedOid.sameTree(DataBase_t.SHDISPLAY)) {
            return shDisplayDataBaseHandler.GET(RequestedOid);
        }

        //-->Irouter:
        if (RequestedOid.sameTree(DataBase_t.IROUTER)) {
            return irouterDataBaseHandler.GET(RequestedOid);
        }

        return null;
    }

    //5.- Funciones que invoca la thread de Visualizacion:
    //----------------------------------------------------
    int getCountOfAudioEntries () {
        return audioDataBaseHandler.countEntries();
    }

    int getCountOfVideoEntries () {
        return videoDataBaseHandler.countEntries();
    }

    int getCountOfShDisplayEntries() {
        return shDisplayDataBaseHandler.countEntries();
    }

    int getCountOfIrouterRCVEntries() {
        return irouterDataBaseHandler.countRCVEntries();
    }

    int getCountOfIrouterSNDEntries() {
        return irouterDataBaseHandler.countSNDEntries();
    }

    int getChIndexFromPos(String compId,int row) {
        if (compId.equals("AUD"))
            return audioDataBaseHandler.getChIndexFromPos(row);

        if (compId.equals("VID"))
            return videoDataBaseHandler.getChIndexFromPos(row);

        if (compId.equals("SHD"))
            return shDisplayDataBaseHandler.getChIndexFromPos(row);

        //if (compId.equals("IRT"))
        //    return irouterDataBaseHandler.getChIndexFromPos(row);

        return -1;
    }

    Object getData (String compId, int row, int dataIndex) {
        try {
            //-->Audio:
            if (compId.equals("AUD")) {
                int ChIndex = audioDataBaseHandler.getChIndexFromPos(row);
                return getData (new sck.Oid (DataBase_t.AUDIO+".1.1."+dataIndex+"."+ChIndex));
            }

            //-->Video:
            if (compId.equals("VID")) {
                int ChIndex = videoDataBaseHandler.getChIndexFromPos(row);
                return getData (new sck.Oid (DataBase_t.VIDEO+".1.1."+dataIndex+"."+ChIndex));
            }

            //-->ShDisplay:
            if (compId.equals("SHD")) {
                int ChIndex = shDisplayDataBaseHandler.getChIndexFromPos(row);
                return getData (new sck.Oid (DataBase_t.SHDISPLAY+".1.1."+dataIndex+"."+ChIndex));
            }

            //-->Irouter:
            if (compId.equals("IRTRCV")) {
                String irtMedia= irouterDataBaseHandler.getRCVMediaFromRow(row);
                int irtSSRC= irouterDataBaseHandler.getRCVSSRCFromRow(row);
                return getData(new sck.Oid (DataBase_t.IROUTER+".1.1."+dataIndex+"."+irtSSRC+"."+irouterMediaID.getMediaID(irtMedia)));
            }
            if (compId.equals("IRTSND")) {
                String irtMedia= irouterDataBaseHandler.getSNDMediaFromRow(row);
                int irtSSRC= irouterDataBaseHandler.getSNDSSRCFromRow(row);
                return getData(new sck.Oid (DataBase_t.IROUTER+".3.1."+dataIndex+"."+irtSSRC+"."+irouterMediaID.getMediaID(irtMedia)));
            }

            return null;
        } catch (java.io.InvalidObjectException e) {
            System.out.println("Excepcion al buscar un data " +
                               "para la thread visual; "+e);
        };
        return null;
    }

    Object getDataFromChannel (String compId, int ChIndex, int dataIndex) {
        try {
            //-->Audio:
            if (compId.equals("AUD")) {
               return getData (new sck.Oid (DataBase_t.AUDIO+".1.1."+dataIndex+"."+ChIndex));
            }

            //-->Video:
            if (compId.equals("VID")) {
               return getData (new sck.Oid (DataBase_t.VIDEO+".1.1."+dataIndex+"."+ChIndex));
            }

            //-->ShDisplay:
            if (compId.equals("SHD")) {
               return getData (new sck.Oid (DataBase_t.SHDISPLAY+".1.1."+dataIndex+"."+ChIndex));
            }

            //-->Irouter:
            // if (compId.equals("IRT")) {
            //    return getData (new sck.Oid (DataBase_t.IROUTER+".1.1."+dataIndex+"."+ChIndex));
            // }

            return null;
        } catch (java.io.InvalidObjectException e) {
            System.out.println("Excepcion al buscar un data " +
                               "para la thread visual; "+e);
        };
        return null;
    }

    audioDataBase_t getAudioDataBase () {
        return audioDataBaseHandler.getAudioDataBase();
    }

    videoDataBase_t getVideoDataBase () {
        return videoDataBaseHandler.getVideoDataBase();
    }

    shDisplayDataBase_t getShDisplayDataBase () {
        return shDisplayDataBaseHandler.getShDisplayDataBase();
    }

    irouterDataBase_t getIrouterDataBase () {
        return irouterDataBaseHandler.getIrouterDataBase();
    }

}
