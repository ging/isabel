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
import java.util.Hashtable;
import java.util.Set;
import java.util.logging.Logger;
import java.util.ArrayList;

import isabel.gw.flows.ComponentFlowTypes;
import isabel.gw.flows.FlowTypePayloads;
import isabel.gw.flows.ComponentFlowTypes.IsabelComponent;
import isabel.lib.tasksock.*;

/**
 * The mcu manager class.
 */
class McuManager implements DeadTaskSockListener {
	
	private String mcuExec;
	private InetAddress mcuAddress;
	private int mcuPort;
	/**
	 * Historia de comandos mandados a la MCU.
	 */
	private ArrayList<String> commands = new ArrayList<String>();
	/**
	 * Booleano que indica si la MCU esta conectada.
	 */
	private boolean connected = false;
	/**
	 * Logger en el que se escribirán los mensajes de depuración
	 */
	private Logger logger;
	/**
	 * Orden No Operation
	 */
	private final String NOP_COMMAND = "nop()";
	
	/**
	 * Orden de salida
	 */
	private final String QUIT_COMMAND = "quit()";
	
	/**
	 * Nombre del TaskSock que se creará
	 */
	private final String MCU_NAME = "MCU";
	
	/**
	 * El TaskSock que controlará la MCU
	 */
	private TaskSock mcuTask;
			
	/**
	 * Tabla que relaciona los modos de audio con los strings que enviamos a la MCU.
	 */
	static private Hashtable<AudioMode, String> audioModeTable;
	
	/**
	 * Tabla que relaciona los modos de video con los strings que enviamos a la MCU.
	 */
	static private Hashtable<VideoMode, String> videoModeTable;
	
	/**
	 * Constructor.
	 * @param mcuAddress
	 */
	public McuManager(String exec,InetAddress addr,int cport){
		
		logger = Logger.getLogger("isabel.gw.MCU");
		logger.info("Launching MCU manager...");
	
		audioModeTable = new Hashtable<AudioMode, String>();
		videoModeTable = new Hashtable<VideoMode, String>();
		
		audioModeTable.put(AudioMode.MIX_MODE, "MIX_MODE");
		audioModeTable.put(AudioMode.SWITCH_MODE, "SWITCH_MODE");
		
		videoModeTable.put(VideoMode.SWITCH_MODE, "SWITCH_MODE");
		videoModeTable.put(VideoMode.GRID_MODE, "GRID_MODE");
		videoModeTable.put(VideoMode.COMMON_GRID_MODE, "GRID2_MODE");
		
		mcuExec = exec;
		mcuAddress = addr;
		mcuPort = cport;
		LaunchMCU();
	}
	
	@SuppressWarnings("unchecked")
	void LaunchMCU()
	{
		logger.info("Launching MCU...");
		connected = true;
		try {
			String[] mcuCommand = new String[]{mcuExec, "-cport", String.valueOf(mcuPort)};
			mcuTask=new TaskSock(MCU_NAME, mcuCommand, mcuAddress, mcuPort, NOP_COMMAND, QUIT_COMMAND);
			mcuTask.addDeadTaskSockListener(this);
			ArrayList<String> tmp = (ArrayList<String>)commands.clone();
			commands.clear();
			for (String cmd : tmp)
			{
				Result ret = sendCommand(cmd);
				if (!ret.succeded())
				{
					logger.severe("Error in mcu: " + ret.getDescription());
				}
			}
		}
		catch (Exception e){
			logger.severe("Can't launch MCU: "+e.getMessage());
			connected = false;
			System.exit(-1);
		}
		logger.info("MCU launched.");		
	}
	/**
	 * Detiene la MCU.
	 * @return Resultado de la operacion.
	 */
	public Result quit(){
		logger.info("Killing MCU...");		
		Result ret = new Result(Result.OK_CODE,Result.OK_DESC);
		if (connected)
		{
			try
			{
				mcuTask.quit();
			}catch(Exception e)
			{
				logger.severe("Exception: " + e.getMessage());
				logger.severe("Stack: " + e.getStackTrace());
				ret = new Result(Result.ERROR,e.getMessage());
			}
			connected = false;
		}
		return ret; 
	}
	/**
	 * Crea una sesión de medios.
	 * @param sessionId El id de la sesión de medios
	 * @return Resultado de la operacion.
	 */
	public Result createSession(int sessionId){
		return sendCommand("create_session("+sessionId+")");
	}
	/**
	 * Elimina una sesión de medios.
	 * @param sessionId El id de la sesión de medios
	 * @return Resultado de la operacion.
	 */
	public Result removeSession(int sessionId)
	{
		return sendCommand("remove_session("+sessionId+")");
	}
	/**
	 * Añade un participante a la sesión de medios
	 * @param sessionId El id de la sesión de medios a la que se va a añadir el participante
	 * @param ip La dirección IP del participante
	 * @param ports Los puertos locales en los que se escucharán los flujos del participante
	 * @return El id del participante
	 */
	public int newParticipant(int sessionId, InetAddress ip, int ports[]){
		
		int id = -1;
		
		String portsString="";		
		for (int i=0; i<ports.length; i++){
			if (ports[i] > 0)
			{
				portsString+=","+ports[i];
			}
		}
			
		Result ret = sendCommand("new_participant("+sessionId+","+ip.getHostAddress()+portsString +")");
		try {
			if (ret.succeded())
			{
				id = Integer.parseInt(ret.getDescription().substring(4));
			}
		}catch (Exception e){
			logger.severe("new_participant exception parsing id.\n");
			id = -1;
		}
		return id;
	}
	/**
	 * Elimina un participante de la sesión de medios
	 * @param sessionId El id de la sesión de la que se va a eliminar el participante
	 * @param participantId El id del participante que se quiere eliminar 
	 * @return Resultado de la operacion.
	 */
	public Result removeParticipant(int sessionId, int participantId)
	{
		return sendCommand("remove_participant("+sessionId+","+ participantId+")");
	}
	/**
	 * Especifica el puerto al que se enviará un flujo con un payload determinado para un participante determinado.
	 * Además, también determina el puerto local donde se recibirán notificaciones RTCP acerca de ese flujo. 
	 * @param sessionId El id de la sesión
	 * @param participantId El id del participante
	 * @param PT El payload del flujo que se enviará a ese participante
	 * @param remoteRTPPort El puerto RTP del participante al que se enviará el flujo
	 * @param localRTPPort El puerto RTP local desde el que se enviará el flujo
	 * @param localRTCPPort El puerto en el que se escucharán notificaciones RTCP para ese flujo
	 * @return Resultado de la operacion.
	 */
	public Result bindRTP(int sessionId, int participantId, int PT, int remoteRTPPort, int localRTPPort, int localRTCPPort)
	{
		return sendCommand("bind_rtp("+sessionId+","+participantId+"," +
				""+PT+","+remoteRTPPort+","+localRTPPort+","+localRTCPPort+")");
	}
	/**
	 * Especifica el puerto al que se enviará un flujo con un payload determinado para un participante determinado.
	 * No se abrirá un puerto para notificaciones RTCP acerca de ese flujo.
	 * @param sessionId El id de la sesión
	 * @param participantId El id del participante
	 * @param PT El payload del flujo que se enviará a ese participante
	 * @param remoteRTPPort El puerto RTP del participante al que se enviará el flujo
	 * @param localRTPPort El puerto RTP local desde el que se enviará el flujo
	 * @return Resultado de la operacion.
	 */
	public Result bindRTP(int sessionId, int participantId, int PT, int remoteRTPPort, int localRTPPort)
	{
		return bindRTP(sessionId, participantId, PT, remoteRTPPort, localRTPPort, 0);
	}
	/**
	 * Deja de enviar el flujo con un payload determinado a un participante determinado.
	 * @param sessionId El id de la sesión
	 * @param participantId El id del participante
	 * @param PT El payload del flujo que se dejará de enviar
	 * @return Resultado de la operacion.
	 */
	public Result unbindRTP(int sessionId, int participantId, int PT)
	{
		return sendCommand("unbind_rtp("+sessionId+","+participantId+","+PT+")");
	}
	/**
	 * Envía información RTCP acerca de un flujo que se esté recibiendo en el puerto RTCP especificao
	 * @param sessionId El id de la sesión
	 * @param participantId El id del participante
	 * @param localRTPPort El puerto local en el que se está recibiendo el flujo RTP del que se quiere informar
	 * @param remoteRTCPPort El puerto remoto al que se enviarán las notifiaciones RTCP
	 * @param localRTCPPort El puerto local desde el que se enviarán las notificaciones RTCP
	 * @return Resultado de la operacion.
	 */
	public Result bindRTCP(int sessionId, int participantId, int localRTPPort, int remoteRTCPPort, int localRTCPPort){
		return sendCommand("bind_rtp("+sessionId+","+participantId+","+localRTPPort+","+remoteRTCPPort+","+localRTCPPort+")");
	}
	/**
	 * Deja de enviar información RTCP para un flujo determinado
	 * @param sessionId El id de la sesión
	 * @param participantId El id del participante
	 * @param localRTPPort El puerto local en el que se está recibiendo el flujo RTP del que se quiere dejar de informar
	 * @param remoteRTCPPort El puerto remoto al que se enviarán las notifiaciones RTCP
	 * @return Resultado de la operacion.
	 */
	public Result unbindRTCP(int sessionId, int participantId, int localRTPPort,int remoteRTCPPort){
		return sendCommand("unbind_rtp("+sessionId+","+participantId+","+localRTPPort+","+remoteRTCPPort+")");
	}
	/**
	 * Configura el pipeline de salida del participante para cierto flujo
	 * @param sessionId El id de la sesión
	 * @param participantId El id del participante que está recibiendo el flujo que se quiere configurar
	 * @param PT El payload del flujo que se quiere configurar
	 * @param FEC El nivel de redundancia que se enviará (0 para off)
	 * @param bandwidth El ancho de banda que se enviará
	 * @return Resultado de la operacion.
	 */
	public Result configureParticipant(int sessionId, int participantId, int PT, int ALIVE, int FEC, int bandwidth){
		return sendCommand("configure_participant("+sessionId+","+participantId+","+PT+","+ALIVE+","+FEC+","+bandwidth+")");
	}
	
	private boolean isType(IsabelComponent type, int PT) {

		Set<ComponentFlowTypes.FlowType> fts = ComponentFlowTypes
				.getFlowTypes(type);

		for (ComponentFlowTypes.FlowType ft : fts) {
			if (FlowTypePayloads.exists(ft, PT))
			{
					return true;
			}
		}
		return false;
	}
	/**
	 * Indica a la MCU que reenvie a cierto participante el flujo recibido de otro participante con un payload
	 * determinado
	 * @param sessionId El id de la sesión
	 * @param receiveParticipantId El participante que va a recibir el flujo
	 * @param sendParticipantId El participante que está enviando el flujo
	 * @param PT El payload del flujo que se quiere reenviar
	 * @return Resultado de la operacion.
	 */
	public Result receive(int sessionId, int receiveParticipantId, int sendParticipantId, int PT, int SSRC){
		//Si es audio o video, redireccionamos correctamente
		if (isType(IsabelComponent.Audio,PT)==true)
		{
			return receiveAudio(sessionId, receiveParticipantId, sendParticipantId, SSRC);
		}else if (isType(IsabelComponent.Video,PT)==true)
		{
			return receiveVideo(sessionId, receiveParticipantId, sendParticipantId, PT, 0, 0, 0, 0, SSRC);
		}
		return sendCommand("receive("+ sessionId+","+receiveParticipantId+","+sendParticipantId+","+PT+","+SSRC+")");
	}
	/**
	 * Detiene el reenvio de cierto flujo
	 * @param sessionId El id de la sesión
	 * @param receiveParticipantId El participante que va a dejar de recibir el flujo
	 * @param sendParticipantId El participante que está enviando el flujo
	 * @param PT El payload del flujo que se quiere reenviar
	 * @return Resultado de la operacion.
	 */
	public Result discard(int sessionId, int receiveParticipantId, int sendParticipantId, int PT){
		//Si es audio o video, redireccionamos correctamente
		if (isType(IsabelComponent.Audio,PT)==true)
		{
			return discardAudio(sessionId, receiveParticipantId, sendParticipantId);
		}else if (isType(IsabelComponent.Video,PT)==true)
		{
			return discardVideo(sessionId, receiveParticipantId, sendParticipantId);
		}
		return sendCommand("discard("+ sessionId+","+receiveParticipantId+","+sendParticipantId+","+PT+")");
	}
	/**
	 * @param sessionId
	 * @param participantId
	 * @param mode
	 * @param PT
	 * @return
	 */
	public Result receiveAudioMode(int sessionId, int participantId, AudioMode mode, int PT,int sl){
		
		return sendCommand("receive_audio_mode("+ sessionId+","+participantId+","+audioModeTable.get(mode)+","+PT+","+sl+")");
	}
	
	/**
	 * @param sessionId
	 * @param participantId
	 * @param mode
	 * @param pt
	 * @param bw
	 * @param fr
	 * @param q
	 * @param width
	 * @param height
	 * @param ssrc
	 * @param grid_mode
	 * @return
	 */
	public Result receiveVideoMode(int sessionId, int participantId, VideoMode mode,int pt, int bw,int fr,int q,int width,int height,int ssrc , GridMode grid_mode){
		return sendCommand("receive_video_mode("+ sessionId + "," + participantId + ","+ videoModeTable.get(mode) + ","+ pt+ ","+ bw+ ","+ fr+ ","+q+ ","+width+ ","+height+ ","+ssrc+ ","+grid_mode+")");
	}
	
	/**
	 * @param sessionId
	 * @param receiveParticipantId
	 * @param sendParticipantId
	 * @param SSRC
	 * @return
	 */
	public Result receiveAudio(int sessionId, int receiveParticipantId, int sendParticipantId,int SSRC){
	   	return sendCommand("receive_audio("+sessionId+","+receiveParticipantId+","+sendParticipantId+","+SSRC+")");
	}
	
	/**
	 * @param sessionId
	 * @param receiveParticipantId
	 * @param sendParticipantId
	 * @param PT
	 * @param width
	 * @param height
	 * @param q
	 * @param bandwidth
	 * @param SSRC
	 * @return
	 */
	public Result receiveVideo (int sessionId, int receiveParticipantId, int sendParticipantId, int PT, int width,int height,int q,int bandwidth,int SSRC){
		return sendCommand("receive_video("+ sessionId+ ","+ receiveParticipantId+ ","+ sendParticipantId+","+ PT+","+ bandwidth+",0,"+ q +","+ width+","+ height+","+ SSRC+")");
	}
	
	/**
	 * @param sessionId
	 * @param receiveParticipantId
	 * @param sendParticipantId
	 * @return
	 */
	public Result discardAudio(int sessionId, int receiveParticipantId, int sendParticipantId){
		return sendCommand("discard_audio("+ sessionId+","+receiveParticipantId+","+sendParticipantId+")");
	}
	
	/**
	 * @param sessionId
	 * @param receiveParticipantId
	 * @param sendParticipantId
	 * @return
	 */
	public Result discardVideo(int sessionId, int receiveParticipantId, int sendParticipantId){
		return sendCommand("discard_video("+ sessionId+","+receiveParticipantId+","+sendParticipantId+")");
	}
	
	/**
	 * @param command
	 * @return
	 */
	private Result sendCommand(String command)
	{
		Result ret = null;
		if (connected)
		{
			try {
				String answer = mcuTask.RPC(command);
				if (answer==null){
					logger.fine("Comando para la MCU: "+ command +"\n\t" + "Respuesta nula");
					ret = new Result(Result.ERROR,"MCU answered null");
				}
				else if (answer.toUpperCase().startsWith("ERROR")){
					logger.fine("Comando para la MCU: "+ command +"\n\t" + "Respuesta: "+ answer);
					ret = new Result(Result.ERROR,answer);			
				}else {
					logger.fine("Comando para la MCU: "+ command +"\n\t" + "Respuesta: "+ answer);
					ret = new Result(Result.OK_CODE,answer);
				}
				
			}
			catch (NoRespondingTaskSockException e){
				logger.warning("Comando para la MCU: "+ command +"\n\t" + "La MCU no responde");
				ret = new Result(Result.ERROR,"MCU not responding");
			}
		}else{
			ret = new Result(Result.ERROR,"MCU is not launched");
		}
		if (ret.succeded())
		{
			commands.add(command);
		}
		return ret;
	}
	
	/* (non-Javadoc)
	 * @see isabel.gw.DeadTaskSockListener#dead()
	 */
	public void dead() {
		logger.severe("MCU is dead");
		LaunchMCU();		
	}

	/**
	 * Informa de si hay conexion con la MCU.
	 */
	public boolean isConnected()
	{
		return connected;
	}
}
