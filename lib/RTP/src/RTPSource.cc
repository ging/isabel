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
// $Id: RTPSource.cc 20746 2010-07-02 12:44:00Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <icf2/icfTime.hh>

#include <rtp/RTPSource.hh>

RTPSourceStatistics_t::RTPSourceStatistics_t (void)
{
    debugMsg(dbg_App_Verbose,
             "RTPSourceStatistics_t",
             "Creating RTPSourceStatistics_t"
            );
    srreceived = false;
    senderInfo = NULL;
}

RTPSourceStatistics_t::~RTPSourceStatistics_t (void)
{
    debugMsg(dbg_App_Verbose, "~RTPSourceStatistics_t", "Deleting Statistics");

    if (senderInfo != NULL)
    {
        delete senderInfo;
    }
}

void
RTPSourceStatistics_t::initStatistics (u16 seqnum, u32 tstamp)
{
    baseSeq = (unsigned long) (seqnum - 1);
    badSeq = RTP_SEQ_MOD + 1;
    maxSeq = seqnum;
    cycles = 0;
    packetsReceived = 0;
    expectedPrior = 0;
    receivedPrior = 0;
    jitter = 0;
    gettimeofday (&tsTime, NULL);
    timestamp = tstamp;
}

void
RTPSourceStatistics_t::computeJitter (u32 tstamp, double tsunit)
{
    struct timeval currentTime;
    long diffsec, diffusec;
    double diff1, diff2, diff;

    if (tstamp != timestamp)
    {
        debugMsg(dbg_App_Verbose, "computeJitter", "Computing Jitter");
        gettimeofday (&currentTime, NULL);
        diffsec = currentTime.tv_sec - tsTime.tv_sec;
        diffusec = currentTime.tv_usec - tsTime.tv_usec;
        while (diffusec < 0)
        {
            diffsec--;
            diffusec += 1000000;
        }

        diff1 = ( ( (double)diffsec + ( (double)diffusec ) / 1000000.0 ) / tsunit );
        diff2 = (double) tstamp - (double)timestamp;
        diff = diff1 - diff2;

        if (diff < 0)
            diff = -diff;

        diff -= (double)jitter;
        diff /= 16.0;
        jitter += (u32)diff;
        timestamp = tstamp;
        tsTime.tv_sec = currentTime.tv_sec;
        tsTime.tv_usec = currentTime.tv_usec;
    }
}

RTPSource_t::RTPSource_t (u32 id, double tsUnit)
{
    debugMsg(dbg_App_Verbose, "RTPSource_t", "Creating RTPSource_t");
    statistics = new RTPSourceStatistics_t ();
    ssrc = id;
    proving = RTP_MIN_SEQUENTIAL;
    timestampUnit = tsUnit;
    packetReceived = false;
    isaCSRC = false;
    sdesInfo = NULL;
    sdesInfoDict = NULL;
}

RTPSource_t::~RTPSource_t (void)
{
    delete statistics;
    if (sdesInfo != NULL)
    {
        for (int i = 0; i < TYPE_SDES + 1; i++)
        {
            free (sdesInfo[i]);
        }
        debugMsg(dbg_App_Verbose, "~RTPSource_t", "Deleting SDESInfo");
        delete [] sdesInfo;
    }
    if (sdesInfoDict != NULL)
    {
        ql_t<u32> *tmp = sdesInfoDict -> getKeys();
        for (ql_t<u32>::iterator_t i = tmp -> begin(); i != tmp -> end(); i++)
        {
            u32 csrc = static_cast<u32> (i);
            char **sdes = sdesInfoDict -> lookUp (csrc);
            if (sdes != NULL)
            {
                for (int i = 0; i < TYPE_SDES + 1; i++)
                {
                    free (sdes[i]);
                }
                debugMsg(dbg_App_Verbose, "~RTPSource_t", "Deleting SDESInfo");
                delete sdes;
            }
        }

        delete tmp;
        delete sdesInfoDict;
    }

    debugMsg(dbg_App_Verbose, "~RTPSource_t", "Deleting RTPSource");

}

bool
RTPSource_t::getProving (void)
{
    return (proving != 0);
}

unsigned long
RTPSource_t::getLastPacketTime (void)
{
    return lastPacketTime.tv_sec;
}

int
RTPSource_t::processPacket (u16 seqnum, u32 tstamp, ql_t<u32>* list)
{
    if (!packetReceived)
    {
        debugMsg(dbg_App_Verbose, "processPacket", "First packet for this source");
        if (list -> len() == 0)
        {
            debugMsg(dbg_App_Verbose, "processPacket", "Source not mixed");
            sdesInfoDict = NULL;
            sdesInfo = new char*[TYPE_SDES+1];

            for (int i = 0; i < TYPE_SDES+1; i++)
            {
                sdesInfo[i] = NULL;
            }
        }
        else
        {
            debugMsg(dbg_App_Verbose, "processPacket", "Source mixed");
            sdesInfo = NULL;
            sdesInfoDict = new dictionary_t<u32, char**>;
            for (ql_t<u32>::iterator_t i = list -> begin(); i != list -> end(); i++)
            {
                u32 aux = static_cast<u32> (i);
                char **sdes = new char*[TYPE_SDES+1];

                for (int i = 0; i < TYPE_SDES+1; i++)
                {
                    sdes[i] = NULL;
                }
                sdesInfoDict -> insert (aux, sdes);
            }
        }
        statistics -> initStatistics (seqnum, tstamp);
        proving--;
        packetReceived = true;

        return 0;
    }
    else
    {
        int result = updateStatistics (seqnum, tstamp);
        if (list -> len() != 0)
            updateCSRC (list);
        return result;
    }
}

int
RTPSource_t::updateStatistics (u16 seqnum, u32 tstamp)
{
    int diff, diff2;
    if (proving > 0)
    {
        if ( seqnum == (statistics -> maxSeq + 1) )
        {
            proving--;
            statistics -> maxSeq = seqnum;
            if (proving == 0)
            {
                debugMsg(dbg_App_Verbose,
                         "updateStatistics",
                         "2 packets in order: Correct source"
                        );
                statistics -> packetsReceived++;
                return 0;
            }
        }
        else
        {
            debugMsg(dbg_App_Verbose,
                     "updateStatistics",
                     "2nd packets not in order"
                    );
            proving = RTP_MIN_SEQUENTIAL - 1;
            statistics -> maxSeq = seqnum;
            statistics -> baseSeq =  (unsigned long) (seqnum - 1);
        }
        return -1;
    }
    else
    {
        gettimeofday (&lastPacketTime, NULL);

        if ( seqnum > statistics -> maxSeq )
        {
            statistics -> packetsReceived++;
            diff = seqnum - statistics -> maxSeq;
            if ( diff < RTP_MAX_DROPOUT )
            {
                statistics -> maxSeq = seqnum;
                statistics -> computeJitter (tstamp, timestampUnit);
                return 0;
            }
        }
        else
        {
            diff = statistics -> maxSeq - seqnum;
            if (!diff)
                return 0;
            statistics -> packetsReceived++;
            if ( diff < RTP_MAX_MISORDER )
            {
                statistics -> computeJitter (tstamp, timestampUnit);
                return 0;
            }

            diff2 = ( RTP_SEQ_MOD  - statistics -> maxSeq ) + seqnum;
            if ( diff2 < RTP_MAX_DROPOUT )
            {
                debugMsg(dbg_App_Verbose,
                         "updateStatistics",
                         "Sequence number overflows: cycles++"
                        );

                statistics -> cycles += RTP_SEQ_MOD;
                statistics -> maxSeq = seqnum;
                //statistics -> baseSeq = statistics -> cycles + (seqnum - 1);
                statistics -> computeJitter (tstamp, timestampUnit);
                return 0;
            }
        }

        if ( seqnum == statistics -> badSeq )
        {
            debugMsg(dbg_App_Verbose,
                     "updateStatistics",
                     "Statistics reinitialized"
                    );
            statistics -> initStatistics (seqnum, tstamp);
            statistics -> packetsReceived++;

            return 0;
        }
        else
        {
            statistics -> badSeq = (seqnum + 1) & (RTP_SEQ_MOD - 1);
            return -1;
        }
    }
}

void
RTPSource_t::processSenderInfo (SenderInfo_t *si)
{
    statistics -> srreceived = true;
    gettimeofday(&(statistics -> srtime),NULL);

    if (statistics -> senderInfo == NULL)
        statistics -> senderInfo = new SenderInfo_t;

    statistics -> senderInfo -> NTPmsw = si -> NTPmsw;
    statistics -> senderInfo -> NTPlsw = si -> NTPlsw;
    statistics -> senderInfo -> rtptimestamp = si -> rtptimestamp;
    statistics -> senderInfo -> senderpacketcount = si -> senderpacketcount;
    statistics -> senderInfo -> senderoctetcount = si -> senderoctetcount;
    debugMsg(dbg_App_Normal,
             "processSenderInfo",
              "Sender Report processed correctly"
            );
}

void
RTPSource_t::processSDES (ql_t<SDESItem_t *> *list)
{
    if (sdesInfo != NULL)
    {
        for (ql_t<SDESItem_t *>::iterator_t i = list -> begin();
             i != list -> end();
             i++
            )
        {
            SDESItem_t *aux = static_cast<SDESItem_t *> (i);
            if (aux -> identifier != TYPE_SDES_PRIV)
            {
                if (sdesInfo[aux -> identifier] != NULL)
                {
                    free (sdesInfo[aux -> identifier]);
                }
                char *info = (char*)malloc (aux -> length + 1);
                memcpy (info, aux -> info, aux -> length);
                info[aux -> length] = '\0';
                sdesInfo[aux -> identifier] = info;
                debugMsg(dbg_App_Verbose,
                         "processSDES",
                         "Item: %s",
                         sdesInfo[aux->identifier]
                        );
            }
            else
            {
                if (sdesInfo[aux -> identifier] != NULL)
                {
                    free (sdesInfo[aux -> identifier]);
                    free (sdesInfo[aux -> identifier + 1]);
                }
                int prefixLength = aux -> info[0];
                char *prefix = (char*)malloc (prefixLength + 1);
                memcpy (prefix, aux -> info + 1, prefixLength);
                prefix[prefixLength] = '\0';
                int infoLength = aux -> length - prefixLength;
                char *info = (char*)malloc (infoLength + 1);
                memcpy (info, aux -> info + 1 + prefixLength, infoLength);
                info[infoLength] = '\0';
                sdesInfo[aux -> identifier] = info;
                sdesInfo[aux -> identifier + 1] = prefix;
                debugMsg(dbg_App_Verbose,
                         "processSDES",
                         "Prefix: %s - Item: %s", sdesInfo[aux->identifier + 1],
                         sdesInfo[aux -> identifier]
                        );
             }
         }
    }
}

void
RTPSource_t::processSDES (u32 csrc, ql_t<SDESItem_t *> *list)
{
    if (sdesInfoDict != NULL)
    {
        char **sdes = sdesInfoDict -> lookUp (csrc);
        if (sdes != NULL)
        {
            for (ql_t<SDESItem_t *>::iterator_t i = list -> begin();
                 i != list -> end();
                 i++
                )
            {
                SDESItem_t *aux = static_cast<SDESItem_t *> (i);
                if (aux -> identifier != TYPE_SDES_PRIV)
                {
                    if (sdes[aux -> identifier] != NULL)
                    {
                        free (sdes[aux -> identifier]);
                    }
                    char *info = (char*)malloc (aux -> length + 1);
                    memcpy (info, aux -> info, aux -> length);
                    info[aux -> length] = '\0';
                    sdes[aux -> identifier] = info;
                    debugMsg(dbg_App_Verbose,
                             "processSDES",
                             "Item: %s\n",
                             sdes[aux->identifier]
                            );
                }
                else
                {
                    if (sdes[aux -> identifier] != NULL)
                    {
                        free (sdes[aux -> identifier]);
                        free (sdes[aux -> identifier + 1]);
                    }
                    int prefixLength = aux -> info[0];
                    char *prefix = (char*)malloc (prefixLength + 1);
                    memcpy (prefix, aux -> info + 1, prefixLength);
                    prefix[prefixLength] = '\0';
                    int infoLength = aux -> length - prefixLength;
                    char *info = (char*)malloc (infoLength + 1);
                    memcpy (info, aux -> info + 1 + prefixLength, infoLength);
                    info[infoLength] = '\0';
                    sdes[aux -> identifier] = info;
                    sdes[aux -> identifier + 1] = prefix;
                    debugMsg(dbg_App_Verbose,
                             "processSDES",
                             "Prefix: %s -- Item: %s",
                             sdes[aux -> identifier + 1],
                             sdes[aux -> identifier]
                            );
                 }
            }
        }
    }
}

void
RTPSource_t::processBYE (u32 ssrc)
{
    sdesInfoDict -> remove (ssrc);
}

void
RTPSource_t::updateCSRC (ql_t<u32> *list)
{
    ql_t<u32> *tmp = sdesInfoDict -> getKeys();
    for (ql_t<u32>::iterator_t i = list -> begin();
         i != list -> end();
         i++)
         {
             u32 aux = static_cast<u32> (i);
             bool found = false;
             for (ql_t<u32>::iterator_t j = tmp -> begin();
                  j != tmp -> end();
                  j++)
                  {
                      u32 aux2 = static_cast<u32> (j);
                      if (aux == aux2)
                          found = true;
                  }

                  if (!found)
                  {
                      char **sdes = new char*[TYPE_SDES+1];
                      sdesInfoDict -> insert (aux, sdes);
                  }
         }

     delete tmp;
}

ql_t<u32> *
RTPSource_t::getCSRCList(void)
{
    if (sdesInfoDict != NULL)
    {
        ql_t<u32> *tmp = sdesInfoDict -> getKeys();
        return tmp;
    }
    else
        return NULL;
}

const char *
RTPSource_t::getSDESItem (u32 csrc, u8 identifier)
{
    if (sdesInfoDict != NULL)
    {
        char **sdes = sdesInfoDict -> lookUp (csrc);
        if (sdes != NULL)
        {
            if (identifier != TYPE_SDES_PRIV)
            {
                return sdes[identifier];
            }
        }
    }
    return NULL;
}

char *
RTPSource_t::getSDESItems (void)
{
    char aux[512];
    char *retVal= (char*)malloc(10*1024*sizeof(char));
    retVal[0]='\0';
    if (packetReceived)
    {
        for (int i = TYPE_SDES_CNAME; i < TYPE_SDES; i++)
        {
            if (i != TYPE_SDES_PRIV)
            {
                sprintf(aux, "Id=%d(%d), [%s]", i, ssrc, sdesInfo[i]);

            }
            else
            {
                sprintf(aux, "Id=%d(%d), [%s][%s]", i, ssrc, sdesInfo[i+1], sdesInfo[i]);
            }

            if (strlen(retVal) + strlen(aux) < 10*1024)
            {
                sprintf (retVal, "%s\n %s", retVal, aux);
            }
        }
    }

    if(strlen(retVal) == 0)
        sprintf(retVal, "No SDESInfo\n");
    else
        sprintf(retVal, "%s\n", retVal);

    return retVal;
}

char *
RTPSource_t::getJitter(void)
{
    char aux[512];
    char *retVal= (char*)malloc(1024*sizeof(char));
    retVal[0]='\0';

    if (packetReceived)
    {
        double timeJitter = (double)(statistics -> jitter) * timestampUnit;
        sprintf(aux, "(%d), Jitter: [%g]",  ssrc, timeJitter);
        if (strlen(retVal) + strlen(aux) < 1024)
        {
            sprintf (retVal, "%s\n %s", retVal, aux);
        }
    }

    sprintf(retVal, "%s\n", strlen(retVal) == 0 ? "No jitter" : retVal);

    return retVal;
}

char *
RTPSource_t::getPacketsLost(void)
{
    char aux[512];
    char *retVal= (char*)malloc(1024*sizeof(char));
    retVal[0]='\0';

    if (packetReceived)
    {
        u32 extendedsnr = statistics -> cycles + statistics -> maxSeq;
        u32 expected = extendedsnr - statistics -> baseSeq;
        u32 lost = expected - statistics -> packetsReceived;
        sprintf(aux, "(%d), Lost: [%u]",  ssrc, lost);
        if (strlen(retVal) + strlen(aux) < 1024)
            sprintf (retVal, "%s\n %s", retVal, aux);
    }

    if(strlen(retVal) == 0)
        sprintf(retVal, "No lost\n");
    else
        sprintf(retVal, "%s\n", retVal);

    return retVal;
}
