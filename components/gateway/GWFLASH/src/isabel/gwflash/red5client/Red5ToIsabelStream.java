package isabel.gwflash.red5client;

import java.io.IOException;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.net.InetSocketAddress;
import java.net.SocketAddress;

import isabel.gwflash.red5client.utils.NetConnection;
import isabel.gwflash.red5client.utils.NetStream;
import isabel.gwflash.xuggle.FLV2RTP;

import java.util.logging.Logger;

/**
 * Representa un flujo desde red5 hacia isabel
 * @author pedro
 *
 */
public class Red5ToIsabelStream extends NetStream {
	
	private String streamName;
	private FLV2RTP rtpTranscoder = null;
	PipedOutputStream pipOut = null;
	PipedInputStream pipIn = null;
	private int ssrc;
	public enum StreamType{
		VIDEO,AUDIO
	}
	
	private StreamType type;
	private Logger log = Logger.getLogger("Red5ToIsabelStream");
	
	/**
	 * Constructor de un nuevo stream hacia el Isabel
	 * @param streamName
	 * Nombre del stream
	 * @param nc
	 * NetConnection del que recibir el flujo
	 * @param ssrc
	 * SSRC que se utilizara de cara a isabel
	 * @param type
	 * Tipo de flujo: Audio o Video
	 */
	public Red5ToIsabelStream(String streamName, NetConnection nc, int ssrc, StreamType type){
		super(nc);
		this.streamName = streamName;
		this.type = type;
		
		this.ssrc = ssrc;
		log.info("Creando Red5ToIsabelStream " + streamName + " ssrc: " + ssrc + " tipo: " + type);

	}
	
	
	/**
	 * Inicia la transmision
	 */
	public void startSending(){
		pipIn = new PipedInputStream();
		try {
			pipOut = new PipedOutputStream(pipIn);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		SocketAddress videoaddress = null;
		SocketAddress audioaddress = null;
		switch(this.type){
			case VIDEO:
				videoaddress = new InetSocketAddress("127.0.0.2", 61000);				
				break;
			case AUDIO:
				audioaddress = new InetSocketAddress("127.0.0.2", 61000);
				break;
		}
        rtpTranscoder = new FLV2RTP(pipIn, videoaddress, audioaddress, ssrc);
		super.play(this.streamName, pipOut);
		
		try {
			Thread.sleep(500);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		rtpTranscoder.start();
	}
	
	/**
	 * Para la transmision 
	 */
	public void stopSending(){
		super.stopStream();

		if(rtpTranscoder!= null){
			rtpTranscoder.stopRunning();
		}
		
		log.info("Parando " + streamName + " ssrc: " + ssrc + " tipo: " + type);		
	}
}
