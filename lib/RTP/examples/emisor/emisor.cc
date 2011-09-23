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
#include <icf2/smartReference.hh>
#include <icf2/stdTask.hh>
#include <icf2/ql.hh>
#include <icf2/sockIO.hh>
#include <rtp/RTPSession.hh>


class myApplication_t: public application_t{
public:
        class myRTPSession_t: public RTPSession_t{
        public:
                myRTPSession_t (myApplication_t *myApp, 
				char *rtpHost,
				char *rtpPort, 
				char *rtcpHost,
				char *rtcpPort, 
				double bw, 
				double tsUnit
			       ): RTPSession_t (static_cast<sched_t*>(myApp), rtpHost, rtpPort, rtcpHost, rtcpPort, bw, tsUnit){
			myApplication = myApp;
                }

                myRTPSession_t (myApplication_t *myApp,
				char *rtpHost,
				char *rtpPort,
				char *rtcpHost,
				char *rtcpPort,
				double bw, 
				double tsUnit, 
				double fragment, 
				int max
			       ): RTPSession_t(static_cast<sched_t*>(myApp), rtpHost, rtpPort, rtcpHost, rtcpPort, bw, tsUnit, fragment, max){ 
                	myApplication = myApp;
		}

                void receivedData (RTPPacket_t *packet){
                }

                void newRRReceived (unsigned long ssrc){
                }

                void eliminateReceiver(u32 ssrc, char *reason){
		}

                bool dispatchMsg (tlMsg_t *m){
                        bool status = RTPSession_t::dispatchMsg (m);
                        return status;
                }
		
		friend smartReference_t<myRTPSession_t>;
        private:
		myApplication_t *myApplication;


        };


	class mySenderTask_t: public simpleTask_t{
	public:
		mySenderTask_t (myApplication_t *myApp): simpleTask_t (1000000){
			myApplication = myApp;
		}

		void heartBeat(void){
                    	u32 ssrc = 1;
                	u8 message[] = "Esto es un mensaje de prueba a ver si los algoritmos y la estructura del programa funciona bien.";
                	u8 payloadtype = 30;
                	bool mark = false;
                	bool incrementTimestamp = true;

                	myApplication -> session -> sendData (ssrc,
                                                              message,
                                                              sizeof(message),
                                                              payloadtype,
                                                              mark,
                                                              incrementTimestamp
                                                             );
        	}
	private:
		myApplication_t *myApplication;
	};


        myApplication_t (): application_t(128){

		session = new myRTPSession_t (this, 
					      NULL, 
					      "5000",
					      NULL,
					      "5001",
					      128000.0, 
					      1/90000
					     );
                session -> tlRun();

                session -> assignSender (1, 500000, this);

		char rtpHost[] = "maquina";
		char rtpPort[] = "puerto_datos";
		session -> rtpBind (1, rtpHost, rtpPort);	

		char rtcpHost[] = "maquina";
		char rtcpPort[] = "puerto_control";
		session -> rtcpBind (rtcpHost, rtcpPort);	

                session -> tlPostMsg (new RTPMsgSetSDESInfo_t (1, TYPE_SDES_NAME, "Eduardo Moro Carrizosa"));

                session -> tlPostMsg (new RTPMsgSetSDESInfo_t (1, TYPE_SDES_EMAIL, "emoro@dit.upm.es"));

                session -> tlPostMsg (new RTPMsgSetSDESInfo_t (1, TYPE_SDES_PHONE, "473"));

                session -> tlPostMsg (new RTPMsgSetSDESInfo_t (1, TYPE_SDES_LOC, "Despacho B-203"));

                session -> tlPostMsg (new RTPMsgSetSDESInfo_t (1, TYPE_SDES_TOOL, "Isabel"));

                session -> tlPostMsg (new RTPMsgSetSDESInfo_t (1, TYPE_SDES_NOTE, "Hola, hola, caracola"));

                session -> tlPostMsg (new RTPMsgSetSDESInfo_t (1, TYPE_SDES_PRIV, "Transmitiendo", "Isabel"));


		mySenderTask = new mySenderTask_t (this);
		(*this)<< mySenderTask;
        }

        friend class smartReference_t<myApplication_t>;
public:
        RTPSession_t *session;
	mySenderTask_t *mySenderTask;	

};
typedef smartReference_t<myApplication_t> myApplication_ref;


int main (){

        myApplication_t myApplication;
        myApplication.run();

        return 0;
}




