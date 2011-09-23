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
package isabel.gwflash;

import isabel.gw.AudioMode;
import isabel.gw.ConfigException;
import isabel.gw.FlowInfo;
import isabel.gw.GAPI;
import isabel.gw.GridMode;
import isabel.gw.IGAPI;
import isabel.gw.MemberInfo;
import isabel.gw.VideoMode;
import isabel.igateway.IGateway;

import java.util.Vector;

public class McuController {

	/**
	 * El gateway de Isabel.
	 */
	private IGAPI gapi = GAPI.GetGateway();	
	
	/**
	 * SSRC del cliente flash
	 */
	private static final int FlashID = 7; //... o 2, o 1, o ...
	
	/**
	 * Local Video port - (the mcu or gateway side).
	 */
	private final static int MCU_PORT = 61000;

	/**
	 * Remote Video port - (the irouter or isabel side).
	 */
	private final static int FLASH_PORT_VIDEO = 61002;
	/**
	 * Remote Audio port - (the irouter or isabel side).
	 */
	private final static int FLASH_PORT_AUDIO = 61004;
	
	/**
	 * Video PT
	 */
	private final static int VIDEO_PT = 96;
	
	/**
	 * Audio PT
	 */
	private final static int AUDIO_PT = 102;
			
	public void initMcu()
	{
		//configuramos flujos de medios
		int isabelSSRC = -1;
		try {
			isabelSSRC = gapi.getConfig().getIsabelSSRC();
		} catch (ConfigException e) {						
			e.printStackTrace();
		}
		
		if (isabelSSRC > 0)
		{
			if (!FlashGatewayEnvConfig.disableFlash()) {
				Vector<FlowInfo> flows = new Vector<FlowInfo>();
				flows.add(new FlowInfo(AUDIO_PT, MCU_PORT, FLASH_PORT_AUDIO, -1));
				flows.add(new FlowInfo(VIDEO_PT, MCU_PORT, FLASH_PORT_VIDEO, -1));
		
				//Añadir participante "fake" que transcodifica FLV.
				MemberInfo flash = new MemberInfo(FlashID, //Este dato da lo mismo, solo se usa para guardar en la lista
		                                          "flash",
		                                          "127.0.0.2",
		                                          flows);
				gapi.getMcuWrapper().addMember(flash);
				
				//Configurar flujos de audio y video desde ISABEL hacia el cliente Flash
				gapi.setupVideo(FlashID, VideoMode.SWITCH_MODE, -1, 0, 0, 0, 0, 0, 0, GridMode.GRID_AUTO);
				//Configuramos audio (Gateway --> Cliente SIP)
				//Transcodificamos el audio del gateway hacia Flash en codec AUDIO_PT.
				gapi.setupAudio(FlashID, AudioMode.MIX_MODE, AUDIO_PT,-50);
				
				//Confgiramos audio y video desde cliente Flash hacia Isabel
				gapi.setupVideo(isabelSSRC, VideoMode.SWITCH_MODE, -1, 0, 0, 0, 0, 0, 0, GridMode.GRID_AUTO);
				//Transcodificamos todo el audio que recibimos del cliente flash hacia el gateway con salida en AUDIO_PT
				gapi.setupAudio(isabelSSRC, AudioMode.SWITCH_MODE, -1 ,-50);
				
				//Ahora habilitamos los flujos de audio y video
				gapi.enableFlow(VIDEO_PT, isabelSSRC, FlashID,false);
				gapi.enableFlow(AUDIO_PT, isabelSSRC, FlashID,false);
				gapi.enableFlow(VIDEO_PT, FlashID, isabelSSRC,false);
				gapi.enableFlow(AUDIO_PT, FlashID, isabelSSRC,false);
			}
			// CONFIGURAMOS EL PARTICIPANTE iGateway si es necesario
			if (FlashGatewayEnvConfig.enableIGW()) {
				Vector<FlowInfo> iflows = new Vector<FlowInfo>();
				iflows.add(new FlowInfo(IGateway.PT, MCU_PORT, IGateway.PORT, -1));
				MemberInfo iGateway = new MemberInfo(IGateway.ID, "igateway", IGateway.IP, iflows);
				gapi.getMcuWrapper().addMember(iGateway);
				gapi.setupAudio(IGateway.ID, AudioMode.MIX_MODE, IGateway.PT,-50);
				gapi.enableFlow(IGateway.PT, isabelSSRC, IGateway.ID, false);
			}
		}
	}
	
	public void releaseMcu()
	{
		gapi.getMcuWrapper().delMember(FlashID);
	}

}
