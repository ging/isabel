
package isabel.gwflash.xuggle;

import isabel.gwflash.FlashGatewayEnvConfig;

import java.io.FileInputStream;
import java.io.InputStream;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.util.logging.Level;
import java.util.logging.Logger;

import com.xuggle.mediatool.IMediaReader;
import com.xuggle.mediatool.ToolFactory;
import com.xuggle.xuggler.IContainer;
import com.xuggle.xuggler.IContainerFormat;

/**
 * Clase que lee FLV de un input stream y lo pasa a dos flujos RTP
 * @author Fernando Escribano
 */
public class FLV2RTP extends Thread {
	
	/**
	 * Payload Type del codec que se utiliza (en el audio de isabel)
	 * None-16KHz
	 */
    private static final int NONE_16_PT = 102;
    
    /**
     * Rate del codec que se utiliza
     */
    private static final int RATE = 16000; //16KHz
    
    /**
     * Tamaño de los paquetes de audio en el codec utilizado
     */
    private static final int MS_PER_FRAME = 20;
    
    /**
     * SSRC del flujo RTP de audio y video
     */
    private int ssrc;

    /**
     * InputStream de donde se lee el FLV
     */
    InputStream input;
    
    /**
     * Direccion de destino de los paquetes de video
     */
    SocketAddress videoAddress = null;
    
    /**
     * Direccion de destino de los paquetes de audio
     */
    SocketAddress audioAddress = null;
    
    /**
     * Controla si la hebra debe parar
     */
    private volatile boolean running = true;


    /**
     * Constructor de la clase. Si alguna de las direcciones destino es null
     * no se envia ese flujo
     * @param input Inputstream de donde se lee FLV
     * @param videoAddr Destino de los paqutes RTP de video
     * @param audioAddr Destino de los paquetes RTP de audio
     * @param ssrc SSRC de los flijos RTP
     */
    public FLV2RTP(InputStream input, SocketAddress videoAddr, SocketAddress audioAddr, int ssrc) {
        this.input = input;
        this.videoAddress = videoAddr;
        this.audioAddress = audioAddr;
        this.ssrc = ssrc;
        
        System.out.println("¿Y esto?" + RATE*MS_PER_FRAME*NONE_16_PT);
    }


    @Override
    public void run() {
	        try {
	    		int bitrate = FlashGatewayEnvConfig.getFlashClientBitRate();
        	
	            IContainer container = IContainer.make();
	            IContainerFormat format = IContainerFormat.make();
	            format.setInputFormat("flv");
	            container.open(input, format, true, false);
	            IMediaReader fileReader = ToolFactory.makeReader(container);
	            DatagramSocket sock = new DatagramSocket();
	            if (videoAddress !=null){
	            	Logger.getLogger(FLV2RTP.class.getName()).log(Level.INFO,"Empezando Video ");
	            	VideoFLV2RTP videortpwriter = new VideoFLV2RTP(sock, videoAddress, ssrc, 320, 240, bitrate);
	            	fileReader.addListener(videortpwriter);
	            }
	            if (audioAddress!=null){
	            	Logger.getLogger(FLV2RTP.class.getName()).log(Level.INFO,"Empezando Audio ");
	            	/*
		            AudioFLV2RTP audiortpwriter = new AudioFLV2RTP(sock, audioAddress, ssrc, RATE, MS_PER_FRAME, (byte) NONE_16_PT);
		            Resampler resampler = new Resampler(RATE);
		            fileReader.addListener(resampler);
		            resampler.addListener(audiortpwriter);		            
		            */
	            	AudioFLVSpeex2RTP audiortpwriter = new AudioFLVSpeex2RTP(sock, audioAddress, ssrc);
		            fileReader.addListener(audiortpwriter);
	            }
	            
	            while (running && fileReader.readPacket() == null) {
	            	//System.out.println("FLV2RTP packet readed");
	            }
	            fileReader.close();
	            Logger.getLogger(FLV2RTP.class.getName()).log(Level.INFO, "Terminado");

	        } catch (Exception ex) {
	            Logger.getLogger(FLV2RTP.class.getName()).log(Level.SEVERE, null, ex);
	        }
	}
    
    /**
     * Detiene la hebra principal
     */
    public void stopRunning(){
    	this.running = false;    	
    }

    /**
     * Metodo de prueba de la clase
     */
    public static void main(String[] args) throws Exception {
        InputStream fin = new FileInputStream("/tests/video.flv");
        SocketAddress videoaddress = new InetSocketAddress("triton.dit.upm.es", 51017);
        SocketAddress audioaddress = new InetSocketAddress("localhost", 51001);

        new FLV2RTP(fin, videoaddress, audioaddress,15).start();
    }
}
