package isabel.gwflash.red5client;

import java.io.IOException;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;

import org.mortbay.log.Log;

import isabel.gwflash.FlashGatewayEnvConfig;
import isabel.gwflash.red5client.utils.NetConnection;
import isabel.gwflash.red5client.utils.NetStream;
import isabel.gwflash.xuggle.AudioRTP2FLV;
import isabel.gwflash.xuggle.Transcoder;
import isabel.gwflash.xuggle.VNC2FLV;
import isabel.gwflash.xuggle.VNCAndRTP2FLV;

/**
 * Clase que representa un stream de audio o video desde Isabel hacia el red5
 * @author pedro
 *
 */
public class IsabelToRed5Stream extends NetStream {
	
	private String streamName;
	private PipedInputStream pipIn = null;
	private PipedOutputStream pipOut = null;
	private Transcoder transcoder= null;
	
	private static final String VNC_HOST = "localhost";
	private static final String VNC_PASSWORD = "bella00";
	private static final String VNC_PORT = "53018";
	private static final int AUDIO_PORT = 61004;
	
	public enum StreamType{
		VIDEO,AUDIO
	}
	
	/**
	 * Devuelve el transcoder utilizado internamente
	 * @return
	 */
	public Transcoder getTranscoder() {
		return transcoder;
	}

	private StreamType type;
		
	/**
	 * Constructor de un nuevo stream
	 * @param streamName
	 * Nombre con el que se publicara el stream
	 * @param nc
	 * NetConnection a traves del cual se enviara el flujo
	 * @param type
	 * Tipo del stream: Audio o Video
	 */
	public IsabelToRed5Stream(String streamName, NetConnection nc, StreamType type) {
		super(nc);
		this.streamName = streamName;
		this.type = type;
		
		initPipes();

		int width = FlashGatewayEnvConfig.getFlashVideoWidth();
		int height = FlashGatewayEnvConfig.getFlashVideoHeight();
		
		if (FlashGatewayEnvConfig.isFlash2Streams()) {
			Log.info("Arrancando streams de Isabel hacia Red5: 2 Streams");
			if (type.equals(StreamType.VIDEO)){				
				transcoder = new VNC2FLV(VNC_HOST, VNC_PASSWORD, VNC_PORT, width, height);
			}else if(type.equals(StreamType.AUDIO)){
				transcoder = new AudioRTP2FLV(AUDIO_PORT);
			}
		} else {
			Log.info("Arrancando streams de Isabel hacia Red5: 1 Stream");
			transcoder = new VNCAndRTP2FLV(AUDIO_PORT, VNC_HOST, VNC_PASSWORD, VNC_PORT, width, height);

		}
	}
	
	/**
	 * Devuelve el tipo de Stream
	 * @return
	 */
	public StreamType getType(){
		return type;
	}
	
	/**
	 * Empieza la transmision del flujo
	 */
	public void startSending(){
		transcoder.go(pipOut);
		this.publish(streamName, pipIn);
	}
	
	/**
	 * Reinicia la transmision por un NetConnection diferente
	 * @param nc
	 * Netconnection al que se mandara el flujo
	 */
	public void restart(NetConnection nc){
		System.out.println("IsabelToRed5Stream restart " + type);
		this.setNetConnection(nc);
		initPipes();
		startSending();		
	}
	
	/**
	 * Reinicia los pipes entre el netstream y el transcoder
	 */
	private void initPipes(){
		pipIn = new PipedInputStream();
		try{
			pipOut = new PipedOutputStream(pipIn);
		}catch(IOException e){
			e.printStackTrace();
		}
	}
}
