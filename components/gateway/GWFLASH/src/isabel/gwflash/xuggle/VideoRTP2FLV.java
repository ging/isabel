
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
import java.util.logging.Logger;


/**
 * Clase encargada de convertir un flujo de video RTP a FLV
 * @author Fernando Escribano
 */
public class VideoRTP2FLV extends Thread {

    /**
     * Fichero sdp que describe el flujos RTP de video
     */
    private String sdpfile;
    
    /**
     * OutputStream donde se escribe el FLV
     */
    private OutputStream out;
    
    /**
     * Constructor de la clase
     * @param sdpfile Fichero sdp que describe el flujo RTP
     * @param out OutputStream donde se escribe el FLV
     */
    public VideoRTP2FLV(String sdpfile, OutputStream out) {
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

        int h = rtpreader.getContainer().getStream(0).getStreamCoder().getHeight();
        int w = rtpreader.getContainer().getStream(0).getStreamCoder().getWidth();
        IRational fr = rtpreader.getContainer().getStream(0).getStreamCoder().getFrameRate();
        String name = rtpreader.getContainer().getStream(0).getStreamCoder().getCodec().getName();
        System.out.printf("VIDEO Codec=%s W=%d H=%d FR=%d\n", name, w , h , fr.getNumerator());
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
                
        IMediaTool flvwriter = GenericWriter.getFlvWriter(out, bitrate, audiobitrate, h, w, fr.getNumerator(), false, true, false);

        // Make the chain (rtp->flv)
        rtpreader.addListener(flvwriter);

        //flvwriter.addListener(ToolFactory.makeDebugListener());
        //rtpreader.addListener(ToolFactory.makeDebugListener());
        //keyfinder.addListener(ToolFactory.makeDebugListener());
        //int i = 0;
        while(rtpreader.readPacket() == null) {
            //System.out.println(i++);
            //if (i == 100) break;
        }
        Logger.getLogger("VideoRTP2FLV").info("Thread Finished");
    }

    /**
     * Metodo de prueba de la clase
     */
    public static void main(String[] args) throws Exception {
        String sdp = "/tests/video.sdp";
        OutputStream outstream = new FileOutputStream("tests/video.flv");
        new VideoRTP2FLV(sdp, outstream).start();
    }
}
