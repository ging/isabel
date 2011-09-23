package isabel.gwflash.red5client;

import java.util.logging.Logger;

import isabel.gwflash.FlowsBool;
import isabel.gwflash.red5client.Red5ToIsabelStream.StreamType;
import isabel.gwflash.red5client.utils.NetConnection;

/**
 * Representa a un usuario de Red5 con sus correspondientes flujos
 * @author pedro
 *
 */
public class Red5User {
	private FlowsBool flows;
	private Red5ToIsabelStream videostream;
	private Red5ToIsabelStream audiostream;	
	private String userName;
	private NetConnection conn;
	private int SSRC;
	private Logger log = Logger.getLogger("isabel.gwflash.Red5User");

	
	
	/**
	 * Representa un usuario del red5 de cara a la pasarela
	 * contiene los transcodificadores necesarios
	 * @param userName
	 * Nombre de usuario en el Red5
	 * @param flows
	 * flujos que va a enviar (audio y/o video)
	 * @param SSRC
	 * ssrc con el que enviara hacia Isabel
	 * @param conn
	 * Conexion a traves de la que se recibiran los flujos del red5
	 */
	public Red5User(String userName, FlowsBool flows, int SSRC, NetConnection conn){
		log.info("new User created " + userName);
		this.userName = userName;
		this.flows = flows;		
		this.SSRC = SSRC;
		videostream = null;
		audiostream = null;
		this.conn = conn;
	}
	
	/**
	 * Arranca los streams que se han configurado con setFlows
	 * 
	 */
	public void startStreams(){
		log.info("startStreams audio " + flows.audio + " video " + flows.video);
		if(flows.video && videostream==null){
			log.info("user " + this.userName + " empezando stream de video");
			videostream = new Red5ToIsabelStream(userName + "_VIDEO", conn, SSRC, StreamType.VIDEO);
			videostream.startSending();

		}
		if (flows.audio && audiostream == null){
			log.info("user " + this.userName + " empezando stream de audio");			
			audiostream = new Red5ToIsabelStream(userName + "_AUDIO", conn, SSRC, StreamType.AUDIO);
			audiostream.startSending();
		}
	}
	
	/**
	 * Para todos los streams
	 */
	public void stopStreams(FlowsBool newFlows){
		log.info("StopStreams de " + this.userName);

		if(audiostream!=null && !newFlows.audio){
			log.info("user " + userName + " parando stream de audio");
			audiostream.stopSending();
			audiostream = null;
		}
		if(videostream!=null && !newFlows.video){
			log.info("user " + userName + " parando stream de video");
			videostream.stopSending();
			videostream = null;
		}
		this.flows = newFlows;
	}
	
	/**
	 * Permite definir los flujos que va a enviar este usuario
	 * @param newFlows
	 * 
	 */
	public void setFlows(FlowsBool newFlows){
		log.info("setFlows --------------------");
		if (newFlows.audio==flows.audio && newFlows.video == flows.video){
			log.info("setFlows no cambia nada");
			return;
		}
		
		if (flows.video||flows.audio){
			log.info("setFlows hay algo activo, lo paro");
			this.stopStreams(newFlows);
		}else{
			this.flows = newFlows;
		}
		
		this.startStreams();
	}
	public FlowsBool getFlows(){
		return flows;
	}
	
	public String getUsername(){
		return userName;
	}
	
	public int getSSRC(){
		return SSRC;
	}
	
}
