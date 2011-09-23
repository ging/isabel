
package isabel.gwflash.xuggle;

import com.xuggle.mediatool.IMediaReader;
import com.xuggle.mediatool.IMediaTool;
import com.xuggle.mediatool.ToolFactory;
import com.xuggle.xuggler.IContainer;
import com.xuggle.xuggler.IContainerFormat;
import com.xuggle.xuggler.IRational;

import isabel.gwflash.FlashGatewayEnvConfig;

import java.io.FileOutputStream;
import java.io.OutputStream;

/**
 * Clase encargada de convertir RTP (audio y video) a FLV
 * @author Fernando Escribano
 */
public class RTP2FLV extends Thread {

	/**
	 * Rate de audio en el flujo FLV
	 */
    private static final int OUT_RATE = 16000;

    /**
     * Fichero sdp que describe los flujos RTP de audio
     * y video
     */
    private String sdpfile;
    
    /**
     * OutputStream donde se escribe el FLV
     */
    private OutputStream out;

    /**
     * Constructor de la clase
     * @param sdpfile Fichero sdp que describe los flujos RTP
     * @param out OutputStream donde se escribe el FLV
     */
    public RTP2FLV(String sdpfile, OutputStream out) {
        this.sdpfile = sdpfile;
        this.out = out;
    }

    @Override
    public void run() {

        // Read container
        IContainer rtpIn = IContainer.make();
        IContainerFormat format = IContainerFormat.make();

        // read rtp data as specified in the sdp
        format.setInputFormat("sdp");
        rtpIn.open(sdpfile, IContainer.Type.READ, format);

        IMediaReader rtpreader = ToolFactory.makeReader(rtpIn);
        
        try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}

        int h = rtpreader.getContainer().getStream(0).getStreamCoder().getHeight();
        int w = rtpreader.getContainer().getStream(0).getStreamCoder().getWidth();
        IRational fr = rtpreader.getContainer().getStream(0).getStreamCoder().getFrameRate();
        String name = rtpreader.getContainer().getStream(0).getStreamCoder().getCodec().getName();        
        System.out.printf("VIDEO Codec=%s W=%d H=%d FR=%d\n", name, w , h , fr.getNumerator());
        
        String aName = rtpreader.getContainer().getStream(1).getStreamCoder().getCodec().getName();
        int sr = rtpreader.getContainer().getStream(1).getStreamCoder().getSampleRate();
        System.out.printf("AUDIO Codec=%s SR=%d \n", aName, sr);
        
        // VALORES POR DEFECTO SI NO LOS PILLA DEL RTP
        if (h == 0) {
        	System.out.println("OJO!!! USANDO VALORES POR DEFECTO de WxH y FR");
        	h = 768;
        	w = 1024;
        	fr = IRational.make(10,1);
        }
        int bitrate = FlashGatewayEnvConfig.getFlashBitRate();
        int audiobitrate = FlashGatewayEnvConfig.getFlashAudioBitRate();
        
       
        System.out.println("VIDEO BW = " + bitrate + " AUDIO BW = " + audiobitrate);
                        
        IMediaTool flvwriter = GenericWriter.getFlvWriter(out, bitrate, audiobitrate, h, w, fr.getNumerator(), true, true, true);


        // resampler
        Resampler resampler = new Resampler(OUT_RATE);

        // Make the chain (rtp->resampler->flv)
        rtpreader.addListener(resampler);
        resampler.addListener(flvwriter);

        //flvwriter.addListener(ToolFactory.makeDebugListener());
        //rtpreader.addListener(ToolFactory.makeDebugListener());
        //keyfinder.addListener(ToolFactory.makeDebugListener());
        //int i = 0;
        while(rtpreader.readPacket() == null) {
            //System.out.println(i++);
            //if (i == 100) break;
        }
    }

    /**
     * Metodo de prueba de la clase
     */
    public static void main(String[] args) throws Exception {
        String sdp = "/tests/todo.sdp";
        OutputStream outstream = new FileOutputStream("/tests/todo.flv");
        new RTP2FLV(sdp, outstream).start();
    }
}
