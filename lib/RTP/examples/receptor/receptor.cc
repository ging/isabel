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
#include <icf2/stdTask.hh>
#include <icf2/ql.hh>
#include <icf2/sockIO.hh>
#include <rtp/RTPSession.hh>


class myApplication_t:public application_t
{
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
                       )
        : RTPSession_t (static_cast<sched_t*>(myApp),
                        rtpHost,
                        rtpPort,
                        rtcpHost,
                        rtcpPort,
                        bw,
                        tsUnit
                       )
        { }

        myRTPSession_t (myApplication_t *myApp,
                        char *rtpHost,
                        char *rtpPort,
                        char *rtcpHost,
                        char *rtcpPort,
                        double bw,
                        double tsUnit,
                        double fragment,
                        int max
                       )
        : RTPSession_t (static_cast<sched_t*>(myApp),
                        rtpHost,
                        rtpPort,
                        rtcpHost,
                        rtcpPort,
                        bw,
                        tsUnit,
                        fragment,
                        max
                       )
        { }

        void receivedData (RTPPacket_t *packet)
        {
            printf ("SSRC: %d\n", packet -> getSSRC());
            printf ("Datos %s\n", packet -> getData());
            printf ("Longitud: %d\n", packet -> getDataLength());
        }

        void newRRReceived (unsigned long ssrc)
        {
        }

        void eliminateReceiver(u32 ssrc, char *reason)
        {
        }

        bool dispatchMsg (tlMsg_t *m)
        {
            bool status = RTPSession_t::dispatchMsg (m);
            return status;
        }

        friend smartReference_t<myRTPSession_t>;
    };

    typedef smartReference_t<myRTPSession_t> myRTPSession_ref;

    myApplication_t (void)
    : application_t(128)
    {
        session = new myRTPSession_t (this,
                                      NULL,
                                      "5000",
                                      NULL,
                                      "5001",
                                      128000.0,
                                      1/90000
                                     );
        session -> tlRun();
    }

    friend class smartReference_t<myApplication_t>;

private:
    RTPSession_t *session;

};
typedef smartReference_t<myApplication_t> myApplication_ref;



int
main ()
{
    myApplication_t myApplication;
    myApplication.run ();

    return 0;
}

