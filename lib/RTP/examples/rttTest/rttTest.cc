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
// $Id: rttTest.cc 20206 2010-04-08 10:55:00Z gabriel $
//
// (C) Copyright 1993-99. Dpto Ingenieria Sistemas Telematicos UPM Spain
// Dec 31, 1999 Transfered to Agora Systems S.A.
// (C) Copyright 2000-2001. Agora Systems S.A.
//
/////////////////////////////////////////////////////////////////////////


#include <icf2/general.h>
#include <icf2/stdTask.hh>
#include <rtp/RTPSender.hh>
#include <rtp/RTPSession.hh>
#include <rtp/RTPReceiver.hh>
#include <rtp/RTCPReceiver.hh>

#define EXAMPLE_TS_UNIT 0.0000625

// from sanchez
static char key[] = "Hola caracola y mas cosas";


class myApp_t: public application_t
{
    class myRTPSession_t: public RTPSession_t {
        public:

        myRTPSession_t (myApp_t *mApp, char *rtpHost, char *rtpPort, char *rtcpHost, char *rtcpPort, double tsUnit):
        RTPSession_t(static_cast<sched_t*>(mApp), rtpHost, rtpPort, rtcpHost, rtcpPort, tsUnit), myApp(mApp) 
        {
        }

        virtual void receivedData (RTPPacket_ref packet) {
            char message[512];
            u8 *buffer = packet->getData();
            int index;
            long long int diff;
            struct timeval tvRec, tv;

            gettimeofday(&tv, NULL);
            memcpy(message, buffer, sizeof(key));
            memcpy(&index, buffer+sizeof(key), sizeof(index));
            memcpy(&tvRec, buffer+sizeof(key)+sizeof(index), sizeof(tvRec));
            diff = (tv.tv_sec*1000000+tv.tv_usec) - 
                   ( tvRec.tv_sec*1000000+tvRec.tv_usec);
            printf("------------------------------------------------------\n");
            printf("Receiving seq=%d\t", index);
            printf("%ld:%ld -> %ld:%ld\n", 
                    tv.tv_sec, tv.tv_usec, tvRec.tv_sec, tvRec.tv_usec);
            printf("   ELAPSED=%llu ms \n", diff/1000);
            printf("------------------------------------------------------\n");
            fflush(stdout);
        }

        virtual void eliminateReceiver (u32 chId, char *reason) {}

        friend class smartReference_t<myRTPSession_t>;

    private:
        myApp_t *myApp;

    public:
        virtual const char *className(void) const { return "myRTPSession_t"; }
    };

    class senderTask_t: public simpleTask_t
    {
        myRTPSession_t *rtpSession;
        int __pktSize;
        u16 contador; // numero de secuencia
        int ssrc;
        struct timeval lastTime;
        bool firstTime;

    public:
        senderTask_t(myRTPSession_t *rtpS, char *rHost, char *rPort, 
                     int delay, int pktS)
        : simpleTask_t(delay) 
        {
            rtpSession=rtpS;
            __pktSize=pktS;
#if 1
            contador=0;
#else
            RTPRandom_t random;
            contador = random.random16();
#endif 
            ssrc=1;
            firstTime=true;
        }

        virtual void heartBeat(void) {
            struct timeval tv;

            gettimeofday(&tv, NULL);

            if (firstTime) {
                firstTime =false;
            } else {
                // long long int period =(tv.tv_sec*1000000 + tv.tv_usec) -
                //                     (lastTime.tv_sec*1000000 + lastTime.tv_usec);
                //printf("PERIOD=%lld\n", period);
            }
            memcpy(&lastTime, &tv, sizeof(tv));

            u8 *buffer = (u8 *)malloc(__pktSize);

            memcpy(buffer, key, sizeof(key));
            memcpy(buffer+sizeof(key), &contador, sizeof(contador));
            memcpy(buffer+sizeof(key)+sizeof(contador), &tv, sizeof(tv));

            contador++;
            u32 usec = tv.tv_sec*1000000 + tv.tv_usec;
            u32 timestamp = (u32)(((double)usec)/EXAMPLE_TS_UNIT);

            rtpSession->sendData(ssrc,
                                 buffer,
                                 __pktSize,
                                 4,        // payloadtype=none
                                 true,     // marker bit
                                 contador, // sequence number (it may be generated randomly)
                                 timestamp // timestamp
				);
        }

        virtual const char *className(void) const { return "senderTask_t"; }
    };

    public:
        myRTPSession_t *rtpSession;

        myApp_t(int &argc, argv_t &argv): application_t(argc,argv)
        {
            char *lPort=NULL;
            char *rPort=NULL;
            char *rHost=NULL;
            int delay=0;
            int pktSize =0;

            enum myOptions { optLocalPort, optRemotePort, 
                             optRemoteHost, optDelay, optPktSize};
            optionDefList_t opt;
            appParamList_t *parList;

            opt
                << new optionDef_t("@localPort",  optLocalPort)
                << new optionDef_t("@remotePort", optRemotePort)
                << new optionDef_t("@remoteHost", optRemoteHost)
                << new optionDef_t("@pktSize",    optPktSize)
                << new optionDef_t("@delay",      optDelay);

            parList= getOpt(opt, argc, argv);

            for( ; parList->len(); parList->behead()) {
                switch(parList->head()->parId) {
                    case optLocalPort:
                        lPort= strdup(parList->head()->parValue);
                        break;
                    case optRemotePort:
                        rPort= strdup(parList->head()->parValue);
                        break;
                    case optRemoteHost:
                        rHost= strdup(parList->head()->parValue);
                        break;
                    case optDelay:
                        delay= atoi(parList->head()->parValue);
                        break;
                    case optPktSize:
                        pktSize= atoi(parList->head()->parValue);
                        break;
                    default:
                        fprintf(stderr,
                                "myApp_t::myApp_t():: bogus "
                                "parList received from "
                                "application_t::getOpt()\n");
                        abort();
                        break;
                }
            }

            if ((!lPort) || (!rHost) || (!rPort) ||
                (delay==0) || (pktSize==0))
            { 
                fprintf(stderr,
                        "Usage: ./sender -localPort <localPort> "
                        "-remotePort <remotePort> -remoteHost <remoteHost> "
                        "-delay <delay> -pktSize <pktSize>\n"
                        " ... try again :)\n");
                exit(-1);
            }

#if 0
            rtpSession= new myRTPSession_t (this, NULL, lPort, NULL, "23444", 0.000125); // 60000
#else
            rtpSession= new myRTPSession_t (this, NULL, lPort, NULL, "22444", EXAMPLE_TS_UNIT); 
#endif
            rtpSession->assignSender(1, false, this);
            rtpSession->newFlow(1);

            rtpSession->rtpBind(1, rHost, rPort);

            *this << new senderTask_t(rtpSession, rHost, rPort, delay, pktSize);

        }
};

int
main(int argc, char *argv[])
{
   myApp_t app(argc, argv);
   app.run();
   return 0;
}
