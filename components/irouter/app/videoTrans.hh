/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
// Copyright (C) 2009 Agora System S.A.
// 
// This file is part of Isabel.
// 
// Isabel is free software: you can redistribute it and/or modify
// it under the terms of the Affero GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Isabel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details.
// 
// You should have received a copy of the Affero GNU General Public License
// along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// $Id: videoTrans.hh 20770 2010-07-07 13:19:44Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#ifndef __video_transcoder_hh__
#define __video_transcoder_hh__

// vCodecs includes
#include <vCodecs/codecs.h>

#include <icf2/general.h>

#include <rtp/RTPDefinitions.hh>
#include <rtp/RTPPacket.hh>
#include <rtp/fragmenter.hh>

#include <vUtils/coderWrapper.h>
#include <vUtils/rtp2image.h>

#include "RTPFecHolder.hh"

const int __INTERVAL = 200;  // intervalo de histeresis

typedef smartReference_t < ql_t <RTPPacket_t *> > RTPPacketList_ref;

class videoTranscodeInfo_t
{
public:
    u32 SSRC;
    u32 BW;
    u32 lineFmt;

    videoTranscodeInfo_t(void)
    : SSRC(0), BW(0), lineFmt(0) // illegal video format
    {}

    videoTranscodeInfo_t(u32 pSSRC, u32 pBW, u32 plineFmt)
    : SSRC(pSSRC), BW(pBW), lineFmt(plineFmt)
    {}

    bool operator==(const videoTranscodeInfo_t &info) const
    {
        return (SSRC    == info.SSRC &&
                BW      == info.BW   &&
                lineFmt == info.lineFmt
               );
    }

    bool operator!=(const videoTranscodeInfo_t &info) const
    {
        return (SSRC    != info.SSRC ||
                BW      != info.BW   ||
                lineFmt != info.lineFmt
               );
    }
};

class tgtInfo_t
{
public:
    void *tgt;
    ql_t<RTPPacket_t *> outputBuffer;

    tgtInfo_t(void *newTgt);
    ~tgtInfo_t(void);
};

class videoTranscoder_t: public item_t
{
private:

    //-------------------
    // PARTE COMUN
    //-------------------

    // informacion de transcodificacion
    videoTranscodeInfo_t Info;

    // ancho de banda que se gasta en cada momento
    u32 actualBW;

    // para comprobar que el paquete no es repetido
    u32 lastSeqNum;
    u32 lastTS;

    // booleano que activa/desactiva la transcodificacion
    bool working;

    // contador para cambio de situacion
    u8  nActive;

    // para procesar el ancho de banda
    u32 t1, t2; // t1 = ts de keyFrame, t2 = ts de ultimo paquete diferencial
    u32 dataCounter;  // contador de bytes

    // defragmenter y decoder para la transcodificacion
    rtp2image_t *rtp2image;

    // fragmenter y coder para la transcodificacion
    fragmenter_t *fragmenter;
    vCoderArgs_t coderArgs;
    coderWrapper_t *coder;

    unsigned short seqNum;
     
    // Parametros codificador
    u32 lastFmt, actualFmt;


    // last defragmenter [compressed] frame
    u8 *newFrameBuf;
    int newFrameLen;

    //-------------------
    // WINDOWS
    //-------------------
#ifdef WIN32
    // pipeLine_t *pipeLine;

#endif

private:

    // Este metodo se encarga de activar/desactivar la transcodificacion,
    // utiliza un ciclo de histeresis para obtener cierta estabilidad,
    // en caso de estar desactivado simplemente se copian los paquetes
    // a las colas de los targets registrados al transcodificador
    u32 processBW(u8 *data, u32 dataLen, u32 TS);

    // este metodo se encarga de activar la transcodificacion
    // dependiendo del BW en cada momento
    // si se transcodifica de un codec a otro siempre se activa
    void checkActive(u32 lineFmt);

public:

    // en el constructor pasamos la informacion de transcodificacion necesaria
    // tambien generaremos dos clases, una decodificadora de video y otra
    // codificadora.
    videoTranscoder_t(videoTranscodeInfo_t);
    virtual ~videoTranscoder_t(void);

    // Procesamos el paquete que nos pasa el target, necesitamos su
    // direccion de memoria para devolverle un fragmento de su lista,
    // si existe. Antes de procesar el paquete comprobamos que no se
    // ha recibido ya mediante la dupla <numSeq,TS>, tambien se comprueba
    // el booleano working para ver si tenemos activado la transcodificacion.
    RTPPacketList_ref processData(RTPPacket_t *,void *);
    // para obtener la informacion del transcodificador
    videoTranscodeInfo_t getInfo(void);
    bool isWorking(void);
};
#endif

