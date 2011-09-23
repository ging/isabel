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
/**
 <head> 
   <name>BWControl.cpp</name> 
   <version>1.0</version>
   <author>Vicente Sirvent Orts</author>
   <mail>sirvent@dit.upm.es</mail>
   <descr>
   Body of BWControl.h
   </descr>
//</head>
**/
#include "BWControl.h"
#include "recChannel.h"
#include "videoRTPSession.h"
#include "W32reg.h"

// BandWidth Control
	    
BWController_t::BWController_t(channel_t * channel,
							   long BW,
							   u32 period):
							   simpleTask_t(period)
{
    __CONTEXT("BWController_t::BWController_t");

#if DBG_SCHED   // Heartbeat Test
	dbgFile = new fileIO_t("Test.txt",modeWrite|modeCreate);
#endif

	data = 0;
	actualBW = 0;
	this->BW = BW;
	this->channel = channel;
	s->insertTask(this);
}

BWController_t::~BWController_t(void)
{
    __CONTEXT("BWController_t::~BWController_t");
#if DBG_SCHED
	dbgFile->close();
	delete dbgFile;
#endif

	debugMsg(dbg_App_Normal, 
			 "BWController_t",
			 "~BWController_t"
			 );
    

}

void
BWController_t::Release(void)
{
   __CONTEXT("BWController_t::Release");
   //sched_t has the only smartReference, so 
   //when removing this task, automatically calls
   //delete.
   s->removeTask(this);
}

bool
BWController_t::setBW(long BW)
{
    __CONTEXT("BWController_t::setBW");

    if (this->BW != BW)
    {
        this->BW = BW;
    	refreshBW();	
        return true;
    }
    return false;
}

bool
BWController_t::refreshBW(void)
{
    __CONTEXT("BWController_t::refreshBW");
    recChannel_t * recChannel = static_cast<recChannel_t *>(channel);
    // if codec == ffvfw we can access BW CBR usage
    // in windows register.
    if (strcmp(recChannel->pSender->GetActualCodec(),"ffvfw MPEG-4 Codec")==0) 
    {

        registry_t winReg("HKEY_CURRENT_USER/Software/GNU/ffvfw");
        DWORD codecBW = 0;
        winReg.read("rcBitrate",(BYTE *)&codecBW,sizeof(DWORD));
        if(codecBW!=BW)
        {
            NOTIFY("ControlBW::codecBW = %d, changing to selected BW = %d\n",codecBW,BW);
            codecBW = BW;
            winReg.write("rcBitRate",(BYTE *)&codecBW,sizeof(DWORD),REG_DWORD);
            return true;
        }

    }
    
    // if codec == ffdshow we can access BW CBR usage
    // in windows register.
    if (strcmp(recChannel->pSender->GetActualCodec(),"ffdshow Video Codec")==0) 
    {

        registry_t winReg("HKEY_CURRENT_USER/Software/GNU/ffdshow_enc");
        DWORD codecBW = 0;
        winReg.read("bitrate1000",(BYTE *)&codecBW,sizeof(DWORD));
        if(codecBW!=BW)
        {
           NOTIFY("ControlBW::codecBW = %d, changing to selected BW = %d\n",codecBW,BW);
           codecBW = BW;
           this->BW = BW;
           winReg.write("bitrate1000",(BYTE *)&codecBW,sizeof(DWORD),REG_DWORD);
           return true;
        }
    }
    return false;
}

void
BWController_t::watchBW(long Data)
{
    __CONTEXT("BWController_t::watchBW");
	data = data + Data; // Data in Kbps
}

void 
BWController_t::heartBeat(void)
{
    __CONTEXT("BWController_t::heartBeat");
    recChannel_t * recChannel = static_cast<recChannel_t *>(channel);

#if DBG_SCHED

	struct _timeb time;
	_ftime(&time);
	char string[50];
	sprintf(string,"mark : %d\r\n",time.time*1000 + time.millitm);
	dbgFile->write(string,strlen(string));
	return;

#endif
	
	if (recChannel->getMapping())
	{

#if 0        

		    if (recChannel->get_camInfo()->getKind() == SHARED)
			    recChannel->get_camInfo()->RunSource();
		    
		    if (recChannel->get_camInfo()->getKind() == CAM)
            {
                debugMsg(dbg_App_Paranoic, 
				    "BWAlarm", 
				    "Time to refresh BW");

		        actualBW = (data)/(get_period()/1000);
                float newFR = recChannel->get_rate()*30;
                int deltaBW = abs(actualBW - BW);
                if (actualBW > BW && deltaBW > 30)
                {
                    if (deltaBW>100)
                    {
                        if(recChannel->get_camInfo()->getKind()==CAM && 
                           BW < 150 &&
                           recChannel->get_captureInfo().heigth >120 &&
                           recChannel->get_captureInfo().width  >160)
                        {
                            captureInfo_t auxCapInfo;
                            auxCapInfo.heigth = 120;
                            auxCapInfo.width  = 160;
                            recChannel->set_captureInfo(auxCapInfo);
                            recChannel->set_all(false);
                            recChannel->select_source(recChannel->get_camInfo());
                        }

                        newFR = (BW*newFR)/actualBW;
                
                    }else{
                    
                        newFR--;
                    }

                    // Actually there isn't an efficient method to control
			        // file bandwitdh. Here we only control fps.
			        // Partial solution: use a CBR codec
			    
                    debugMsg(dbg_App_Normal, 
                        "BWControl", 
                        "MAX BW: %d Kbps, ACTUAL BW: %d Kbps:: Decreasing bandwidth to %2.1f fps",
                        BW, 
                        actualBW,
                        newFR);
                
                    NOTIFY( "BWControl::MAX BW: %d Kbps, ACTUAL BW: %d Kbps:: Decreasing bandwidth to %2.1f fps\r\n",
					     BW, 
					     actualBW,
					     newFR);
			    
			    recChannel->set_rate(newFR);
						    
            }else{

			    if ((actualBW < BW && deltaBW > 30 && newFR<30))
			    {
                    int deltaBW = abs(actualBW - BW);
                    if (deltaBW>100)
                    {
                        newFR = (BW*newFR)/actualBW;
                        if (newFR>30.0) newFR=30;
                
                    }else{
                    
                        newFR++;
                    }
                
                    debugMsg(dbg_App_Normal, 
                        "BWControl", 
                        "MAX BW: %d Kbps, ACTUAL BW: %d Kbps:: Incrementing bandwidth to %2.1f fps",
                        BW, 
                        actualBW,
                        newFR);
                
                    NOTIFY( "BWControl::MAX BW: %d Kbps, ACTUAL BW: %d Kbps:: Incrementing bandwidth to %2.1f fps\r\n",
                        BW, 
                        actualBW,
                        newFR);
            
                    recChannel->set_rate(newFR);

                }else{
                
                    if (deltaBW>100)
                    {
                        if(recChannel->get_camInfo()->getKind()==CAM && 
                            BW > 1000 &&
                            recChannel->get_captureInfo().heigth <480 &&
                            recChannel->get_captureInfo().width  <640)
                        {
                            captureInfo_t auxCapInfo;
                            auxCapInfo.heigth = 480;
                            auxCapInfo.width  = 640;
                            recChannel->set_captureInfo(auxCapInfo);
                            recChannel->set_all(false);
                            recChannel->select_source(recChannel->get_camInfo());
                    
                        }else{
                        
                            if(recChannel->get_camInfo()->getKind()==CAM && 
                                BW > 150 &&
                                recChannel->get_captureInfo().heigth <320 &&
                                recChannel->get_captureInfo().width  <240)
                            {
                                captureInfo_t auxCapInfo;
                                auxCapInfo.heigth = 240;
                                auxCapInfo.width  = 320;
                                recChannel->set_captureInfo(auxCapInfo);
                                recChannel->set_all(false);
                                recChannel->select_source(recChannel->get_camInfo());
                            }
                        }

                    }
                    debugMsg(dbg_App_Normal, 
                        "BWControl", 
                        "MAX BW: %d Kbps, ACTUAL BW: %d Kbps:: Bandwidth OK",
                        BW, 
                        actualBW);
                
                    NOTIFY  ("BWControl::MAX BW: %d Kbps, ACTUAL BW: %d Kbps\r\n",
                        BW, 
                        actualBW);

                }
            }
        }
        }
#endif
    }
	
    data=0;
    return;
}

			
