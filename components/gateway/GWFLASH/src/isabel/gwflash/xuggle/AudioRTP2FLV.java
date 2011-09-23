
package isabel.gwflash.xuggle;

import isabel.gwflash.FlashGatewayEnvConfig;

import java.io.FileOutputStream;
import java.io.OutputStream;
import java.net.SocketException;

import com.xuggle.mediatool.IMediaTool;

/**
 * Clase encarga de convertir un flujo de audio RTP a un flujo FLV
 * @author Fernando Escribano
 */
public class AudioRTP2FLV implements Transcoder {
    
	/**
	 * Generador de muestras de audio a partir de un flujo RTP
	 */
	private RTPAudioGenerator audiogenerator;
    
    /**
     * Constructor de la clase
     * @param audioPort Puerto RTP de audio
     */
    public AudioRTP2FLV(int audioPort) {
		try {
			audiogenerator = new RTPAudioGenerator(audioPort);
	    	audiogenerator.go();
		} catch (SocketException e) {
			e.printStackTrace();
			throw new RuntimeException(e);
		}		
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
    private Resampler resampler;

    public void reset(OutputStream out) {    	
        
        int audiobitrate = 32000;
        
        try {
        	audiobitrate = Integer.parseInt(System.getenv("FLASH_AUDIO_BITRATE"));
        }catch (Exception e) {
        }
        
        System.out.println("AUDIO BW = " + audiobitrate);
        
        int outSampleRate;
        if (FlashGatewayEnvConfig.useMp3()) {
            flvwriter = GenericWriter.getFlvMp3Writer(out, 100000, audiobitrate, 0, 0, 25, true, false, false);
            outSampleRate = GenericWriter.MP3_OUT_RATE;
        }
        else {
        	flvwriter = GenericWriter.getFlvWriter(out, 100000, audiobitrate, 0, 0, 25, true, false, false);
        	outSampleRate = GenericWriter.SPEEX_OUT_RATE;
        }
        
        // resampler
        resampler = new Resampler(outSampleRate);

        // Make the chain (rtp->resapler->flv)
        audiogenerator.addListener(resampler);
        resampler.addListener(flvwriter);
    }

    /**
     * Metodo de prueba de la clase
     */
    public static void main(String[] args) throws Exception {
        OutputStream outstream = new FileOutputStream("/tmp/audio.flv");
        AudioRTP2FLV audio = new AudioRTP2FLV(61004);
        audio.go(outstream);
        Thread.sleep(10000);
        OutputStream outstream2 = new FileOutputStream("/tmp/audio2.flv");
        audio.go(outstream2);
    }

	@Override
	public void keyboardEvent(String type, int keyCode, int keyChar) {
	}

	@Override
	public void mouseEvent(String type, int x, int y) {
	}
}
