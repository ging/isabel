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
package isabel.gw;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.logging.Logger;
import java.util.Hashtable;
import java.util.Vector;

/**
 * This class manages the MCU application. It receives commands from the main
 * module (Gateway).
 */
public class McuWrapper implements IMCUWrapper {

	/**
	 * MCU session.
	 */
	int session;
	/**
	 * HashTable <SSRCs,ID>. Relaciona identificadores de fuente con ids de miembros.
	 */
	Hashtable<Integer,Integer> members =  new Hashtable<Integer,Integer>();
	/**
	 * The Mcu manager.
	 */
	McuManager mcuManager;
	/**
	 * Logger.
	 */
	Logger log;
	
	/**
	 * Constructor.
	 * @param exec Fichero ejecutable de la MCU.
	 * @param addr Direccion IP de control.
	 * @param cport Puerto de control.
	 */
	public McuWrapper(String exec,String addr,int cport)
	{
		log = Logger.getLogger("isabel.gw.McuWrapper");
		log.info("MCUWrapper:: Initializing MCU...");		
		try
		{
			mcuManager = new McuManager(exec, InetAddress.getByName(addr), cport);			
			session = 1;
			Result result = mcuManager.createSession(session);
			if (!result.succeded())
			{
				log.severe("Error creating session: " + result.getDescription());
				System.exit(-1);
			}
		}catch(UnknownHostException e){
			log.severe("Unknown mcu address");
			System.exit(-1);
		}		
	}
	
	/**
	 * Crea un miembro en la MCU.
	 * @param info Informacion del miembro.
	 * @return Resultado de la operacion.
	 */
	public Result addMember(MemberInfo info)
	{		
		Result result = new Result(Result.OK_CODE,Result.OK_DESC);
		int[] ports = new int[info.getFlowList().size()];
		int i = 0;
		Vector<Integer> processed_ports = new Vector<Integer>();
		for (FlowInfo flowInfo : info.getFlowList())
		{
			if (!processed_ports.contains(flowInfo.getLocalPort()) && flowInfo.getLocalPort()>0)
			{
				ports[i++] = flowInfo.getLocalPort(); 
				processed_ports.add(flowInfo.getLocalPort());
			}
		}
		if (i==0)
		{
			return new Result(Result.ERROR,"Client without RX ports.");
		}
		try {
			int id = mcuManager.newParticipant(this.session, InetAddress.getByName(info.getAddr()), ports);
			if (id >= 0){
				members.put(info.ssrc, id); //guardamos el miembro
				//configuramos los flujos
				for (FlowInfo flowInfo : info.getFlowList())
				{
					//Configuramos flujos.					
					mcuManager.bindRTP(this.session, id, flowInfo.getPT(), flowInfo.getRemotePort(), flowInfo.getLocalPort());					
					//No activamos alive packets para abrir puertos.
					mcuManager.configureParticipant(this.session, id, flowInfo.getPT(),0,0,0);					
				}
			}
		} catch (UnknownHostException e) {
			log.severe(e.getMessage());
			e.printStackTrace();
			result = new Result(Result.ERROR,e.getMessage());
		}
		return result;
	}
	
	/**
	 * Elimina un miembro de la MCU.
	 * @param SSRC ID del miembro.
	 * @return Resultado de la operacion.
	 */
	public Result delMember(int SSRC)
	{
		Result result;
		Integer id = members.get(SSRC);
		if (id != null)
		{
			result = mcuManager.removeParticipant(this.session, id); 
		}else{
			result = new Result(Result.OK_CODE,"WARNING: Participant doesn't exists.");
		}
		return result;
	}
	
	/**
	 * See IMCUWrapper interface documentation.
	 * @return       Result
	 * @param        id
	 * @param        video_mode
	 * @param        pt
	 * @param        bw
	 * @param        fr
	 * @param        Q
	 * @param        width
	 * @param        height
	 * @param        ssrc
	 * @param        grid_mode
	 */
	public Result setupVideo( int id, VideoMode video_mode, int pt, int bw, int fr, int Q, int width, int height, int ssrc, GridMode grid_mode )
	{
		Result ret;
		//Buscamos origen y destino
		Integer mcuId = members.get(id);		
		if (mcuId != null)
		{
			ret = mcuManager.receiveVideoMode(this.session, mcuId, video_mode, pt, bw, fr, Q, width, height, ssrc, grid_mode);
		}else{
			ret = new Result(Result.ERROR,"Participant not found");
		}
		return ret;		
	}


	/**
	 * See IMCUWrapper interface documentation.
	 * @return       Result
	 * @param        id
	 * @param        audioMode
	 * @param        pt
	 * @param        sl
	 * @param        ssrc
	 */
	public Result setupAudio( int id, AudioMode audioMode, int pt, int sl)
	{
		Result ret;
		//Buscamos origen y destino
		Integer mcuId = members.get(id);		
		if (mcuId != null)
		{
			ret = mcuManager.receiveAudioMode(session, mcuId, audioMode, pt, sl);
		}else{
			ret = new Result(Result.ERROR,"Participant not found");
		}
		return ret;
	}

	/**
	 * See IMCUWrapper interface documentation.
	 * @return       Result
	 * @param        PT
	 * @param        origID
	 * @param        destID
	 * @param        changeSSRC 
	 */
	public Result enableFlow( int PT, int origID, int destID, boolean changeSSRC )
	{
		Result ret;
		//Buscamos origen y destino
		Integer mcuID1 = members.get(origID);
		Integer mcuID2 = members.get(destID);
		int ssrc = 0;
		if (changeSSRC == true)
		{
			ssrc = origID;
		}
		if (mcuID1 != null && mcuID2 != null)
		{
			ret = mcuManager.receive(this.session, mcuID2, mcuID1, PT, ssrc);
		}else{
			ret = new Result(Result.ERROR,"Participant not found");
		}
		return ret;
	}


	/**
	 * See IMCUWrapper interface documentation.
	 * @return       Result
	 * @param        PT
	 * @param        origID
	 * @param        destID
	 */
	public Result disableFlow( int PT, int origID, int destID )
	{
		
		Result ret;
		//Buscamos origen y destino
		Integer mcuID1 = members.get(origID);
		Integer mcuID2 = members.get(destID);
		if (mcuID1 != null && mcuID2 != null)
		{
			ret = mcuManager.discard(this.session, mcuID2, mcuID1,PT);
		}else{
			ret = new Result(Result.ERROR,"Participant not found");
		}
		return ret;
	}
}
