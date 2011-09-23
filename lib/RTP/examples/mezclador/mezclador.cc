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
 #include <sys/types.h>
#include <icf2/smartReference.hh>

#include <icf2/ql.hh>
#include <icf2/sockIO.hh>
#include <rtp/RTPInclude.hh>
                                                             
                                      
                                                                 
class component_t: public simpleTask_t{
public:

        class myRTPSession_t: public RTPSession_t{                 
        public:
                myRTPSession_t (double bw, component_t *comp): RTPSession_t (bw){
                        component = comp;
                }

                myRTPSession_t (double bw, double fragment, int max, component_t *comp): RTPSession_t (bw, fragment, max){
                        component = comp;
                }

                /*void receivedData (unsigned long ssrc, unsigned char *data, int length){

                } */

                void receivedData (RTPPacket_t *packet){
                        component -> mix (packet -> getSSRC(), packet -> getData(), packet -> getLength());
                }

                void newRRReceived (unsigned long ssrc){
                        printf ("Ya puedo pedir estadisticas\n");
                }

                void lastJitter (unsigned long ssrc, unsigned long jitter){
                }

                void fractionLost (unsigned long ssrc, unsigned char fractionLost){
                }

                void roundTripTime (unsigned long ssrc, struct timeval rttime){
                }

                void packetsLost (unsigned long ssrc, unsigned long lost){
                }

                void SDESInfo (unsigned long ssrc, char *info){
                }

                ql_t<unsigned long> *getRRList(){
                        return NULL;
                }

                bool dispatchMsg (tlMsg_t *m){
                        bool status = RTPSession_t::dispatchMsg (m);
                        //printf ("STATUS: %d\n", status);
                        return status;
                }

                friend smartReference_t<myRTPSession_t>;
        private:
                component_t *component;

        };
        typedef smartReference_t<myRTPSession_t> myRTPSession_ref;


        component_t (sched_t &s): simpleTask_t(){

                contador = 0;

                int maxPacketSize = RTP_MAX_PACKET_SIZE;
                session = new myRTPSession_t (128000.0, this);
                session -> tlRun();

                char *host = NULL;
                char rtpPort[] = "2222";
                char rtcpPort[] = "2223";
                inetAddr_t rtpAddress (host, rtpPort, serviceAddr_t::DGRAM);
                io_ref rtp = new dgramSocket_t (rtpAddress);
                inetAddr_t rtcpAddress (host, rtcpPort, serviceAddr_t::DGRAM);
                io_ref rtcp = new dgramSocket_t (rtcpAddress);
                unsigned char payloadtype1 = 7;
                unsigned char payloadtype2 = 20;
                ql_t<unsigned char> payloadtypeList;
                payloadtypeList.insert (payloadtype1);
                payloadtypeList.insert (payloadtype2);

                rtpReceiver = new RTPReceiver_t (rtp, session, maxPacketSize, &payloadtypeList);

                rtpReceiver -> tlRun ();
                s.insertTask (rtpReceiver);

                rtcpReceiver = new RTCPReceiver_t (rtcp, session);
                rtcpReceiver -> tlRun ();
                s.insertTask (rtcpReceiver);


                session -> tlPostMsg (new RTPMsgInitialize_t ());

                sleep(1);
                session -> tlPostMsg (new RTPMsgCreateMixer_t (20, false, 5));


                char *localhost = NULL;
                char remotePort[] = "2224";
                char remoteControlPort[] = "2225";


                inetAddr_t localRTPAddress (localhost, NULL, serviceAddr_t::DGRAM);
                inetAddr_t remoteRTPAddress (localhost, remotePort, serviceAddr_t::DGRAM);
                io_ref rtpSocket = new dgramSocket_t (localRTPAddress, remoteRTPAddress);

                inetAddr_t localRTCPAddress (localhost, NULL, serviceAddr_t::DGRAM);
                inetAddr_t remoteRTCPAddress (localhost, remoteControlPort, serviceAddr_t::DGRAM);
                io_ref rtcpSocket = new dgramSocket_t (localRTCPAddress, remoteRTCPAddress);

                session -> tlPostMsg (new RTPMsgAddDestination_t (rtpSocket, rtcpSocket));

                //session -> tlPostMsg (new RTPMsgCreateSender_t ());



                //char NAME[256] = "Eduardo Moro Carrizosa, casi ingeniero de teleco";
                //session -> tlPostMsg (new RTPMsgSetSDESInfo_t (TYPE_SDES_NAME, NAME));
                /*char EMAIL[256] = "emoro@dit.upm.es";
                session -> tlPostMsg (new RTPMsgSetSDESInfo_t (TYPE_SDES_EMAIL, EMAIL));
                char PHONE[256] = "620 83 81 35";
                session -> tlPostMsg (new RTPMsgSetSDESInfo_t (TYPE_SDES_PHONE, PHONE));
                char LOC[256] = "Despacho B-203, la peor maquina de todas";
                session -> tlPostMsg (new RTPMsgSetSDESInfo_t (TYPE_SDES_LOC, LOC));
                char TOOL[256] = "Isabel n.p.i de la version";
                session -> tlPostMsg (new RTPMsgSetSDESInfo_t (TYPE_SDES_TOOL, TOOL));
                char NOTE[256] = "Por desgracia, delante de la pantalla";
                session -> tlPostMsg (new RTPMsgSetSDESInfo_t (TYPE_SDES_NOTE, NOTE));
                */



        }

        void mix (unsigned long csrc, unsigned char *data, int length){
                session -> tlPostMsg (new RTPMsgAddCSRC_t (csrc));
                //printf ("MIX: Voy a mandar un paquete mezclado\n");

                session -> tlPostMsg (new RTPMsgDefaultSendData_t (data, length));
                contador++;
                if (contador == 20){
                        char reason[256] = "Porque si";
                        session -> tlPostMsg (new RTPMsgDestroy_t (reason));
                        contador = 0;
                }
        }


        /*void heartBeat(void){
                unsigned char message[] = "Eduardo Moro Carrizosa: Ok. Esto funciona. Parece que las cosas van saliendo y esto me pone bastante contento aunque todavia queda lo peor, bueno Edu no deseperes que seguro que te sale bien";
                printf ("Los datos tienen una longitud: %d\n", sizeof (message));
                session -> tlPostMsg (new RTPMsgSendData_t (message, sizeof(message)));
                */
                //printf ("Envio un paquete RTP\n");
        //}

        friend class smartReference_t<component_t>;
private:
        RTPSession_t *session;
        RTPReceiver_t *rtpReceiver;
        RTCPReceiver_t *rtcpReceiver;

        int contador;

};
typedef smartReference_t<component_t> component_ref;




int main (){

        sched_t s(128);

        //RTPSession_ref session = new component_t::myRTPSession_t (128000.0);

        component_t component (s);

        s.insertTask (&component);


        s.run ();
        /*RTPSender_t *sender = session -> getSender ();
        char *NAME = new char [256];
        NAME = "Eduardo Moro Carrizosa, casi ingeniero de teleco";
        sender -> setSDESInfo (NAME, TYPE_SDES_NAME);
        char *EMAIL = new char [256];
        EMAIL = "emoro@dit.upm.es";
        sender -> setSDESInfo (EMAIL, TYPE_SDES_EMAIL);
        char *PHONE = new char [256];
        PHONE = "620 83 81 35";
        sender -> setSDESInfo (PHONE, TYPE_SDES_PHONE);
        char *LOC = new char [256];
        LOC = "Despacho B-203, la peor maquina de todas";
        sender -> setSDESInfo (LOC, TYPE_SDES_LOC);
        char *TOOL = new char [256];
        TOOL = "Isabel n.p.i de la version";
        sender -> setSDESInfo (TOOL, TYPE_SDES_TOOL);
        char *NOTE = new char [256];
        NOTE = "Por desgracia, delante de la pantalla";
        sender -> setSDESInfo (NOTE, TYPE_SDES_NOTE);
        */






        return 0;
}
