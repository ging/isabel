
package isabel.gwflash.xuggle;

import isabel.gwflash.FlashGatewayEnvConfig;
import isabel.igateway.Segmenter;

import java.io.FileOutputStream;
import java.io.OutputStream;
import java.net.SocketException;

import com.xuggle.mediatool.IMediaTool;

/**
 * Clase encargada de generar FLV a partir de un VNC y un flujo de audio RTP
 * @author Fernando Escribano
 */
public class VNCAndRTP2FLV  implements Transcoder {
    
    /**
     * Framerate de video
     */
    private static final int FRAMERATE = FlashGatewayEnvConfig.getFlashFramerate();
	
	/**
	 * Ancho del VNC
	 */
	private int width;
	
	/**
	 * Alto del VNC
	 */
	private int height;
	
	/**
	 * Generador de imagenes a partir de un cliente VNC
	 */
	private VNCImageGenerator vncgenerator;
		
	/**
	 * Generador de muestras de audio a partir de un flujo RTP
	 */
	private RTPAudioGenerator audiogenerator;
	
	/**
	 * Constructor de la clase
	 * @param audioPort Puerto RTP de audio
	 * @param host Servidor VNC
	 * @param password Password para conectarse al VNC
	 * @param port Puerto del servidor VNC
	 * @param width Ancho del VNC
	 * @param height Alto del VNC
	 */
    public VNCAndRTP2FLV(int audioPort, String host, String password, String port, int width, int height) {
		this.width = width;
		this.height = height;
		
		vncgenerator = new VNCImageGenerator(host, password, port, 1000 / FRAMERATE, width, height );
		
		try {
			audiogenerator = new RTPAudioGenerator(audioPort);
	    	audiogenerator.go();
		} catch (SocketException e) {
			e.printStackTrace();
			throw new RuntimeException(e);
		}		
    }
    
    @Override
	public void keyboardEvent(String type, int keyCode, int keyChar) {
		vncgenerator.keyboardEvent(type, keyCode, keyChar);
	}
	
    @Override
	public void mouseEvent(String type, int x, int y) {
		vncgenerator.mouseEvent(type, x, y);
	}
    
    /**
     * Dice a la hebra que empiece a escribir por el outputstream que se pasa como parametro
	 * @param out OutputStream donde se escribe el FLV
     */
    @Override
    public void go(OutputStream out) {
    	reset(out);
    }
     
    // Cosas del Xuggler
    private IMediaTool flvwriter;
    private IMediaTool tswriter;
    private Resampler resampler;
    private Resampler resampler2;
    
    private void reset(OutputStream out) {
        try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		
    
        int bitrate = FlashGatewayEnvConfig.getFlashBitRate();
        int audiobitrate = FlashGatewayEnvConfig.getFlashAudioBitRate();
        System.out.println("VIDEO BW = " + bitrate + " AUDIO BW = " + audiobitrate + " iGATEWAY " + FlashGatewayEnvConfig.enableIGWB());
                       

        int outSampleRate;
        if (FlashGatewayEnvConfig.useMp3()) {
            flvwriter = GenericWriter.getFlvMp3Writer(out, bitrate, audiobitrate, height, width, FRAMERATE, true, true, true);
            outSampleRate = GenericWriter.MP3_OUT_RATE;
        }
        else {        	
        	flvwriter = GenericWriter.getFlvWriter(out, bitrate, audiobitrate, height, width, FRAMERATE, true, true, true);
        	outSampleRate = GenericWriter.SPEEX_OUT_RATE;
        }
  
        // resampler
        resampler = new Resampler(outSampleRate);


        // Make audio the chain (generator->resampler->flv)
        audiogenerator.addListener(resampler);
        resampler.addListener(flvwriter);        
        
        // Make the video chain
         vncgenerator.addListener(flvwriter);
        
        if (FlashGatewayEnvConfig.enableIGWB()) {
    		Segmenter seg = new Segmenter();
    		tswriter = GenericWriter.getTsWriter(seg.getOutput(), bitrate, audiobitrate, height, width, FRAMERATE, true, true, true);
        	resampler2 = new Resampler(GenericWriter.MP3_OUT_RATE);
        	
            // Make audio the chain (generatosr->resampler->ts)
            audiogenerator.addListener(resampler2);        
            resampler2.addListener(tswriter);        
            
            // Make the video chain
            vncgenerator.addListener(tswriter);
        }        
    }

    /**
     * Metodo de prueba de la clase
     */
    public static void main(String[] args) throws Exception {
        OutputStream outstream = new FileOutputStream("/tmp/todo.flv");
        VNCAndRTP2FLV vnc = new VNCAndRTP2FLV(61004, "chotis2", "fr0d0","5900", 1024, 768);
        vnc.go(outstream);
//        Thread.sleep(20000);
//        OutputStream outstream2 = new FileOutputStream("/tmp/todo2.flv");
//        vnc.go(outstream2);
    }
}
