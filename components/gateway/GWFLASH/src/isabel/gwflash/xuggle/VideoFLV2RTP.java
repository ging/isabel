
package isabel.gwflash.xuggle;

import com.xuggle.mediatool.IMediaReader;
import com.xuggle.mediatool.MediaToolAdapter;
import com.xuggle.mediatool.ToolFactory;
import com.xuggle.mediatool.event.ICloseEvent;
import com.xuggle.mediatool.event.IVideoPictureEvent;
import com.xuggle.xuggler.ICodec;
import com.xuggle.xuggler.IContainer;
import com.xuggle.xuggler.IContainerFormat;
import com.xuggle.xuggler.IPacket;
import com.xuggle.xuggler.IPixelFormat.Type;
import com.xuggle.xuggler.IRational;
import com.xuggle.xuggler.IStreamCoder;
import com.xuggle.xuggler.IVideoPicture;

import java.io.*;
import java.net.*;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Clase que se encarga de pasar video en FLV a RTP
 * Utiliza MPEG-4 como codec
 * @author Fernando Escribano
 */
public class VideoFLV2RTP extends MediaToolAdapter {

	/**
	 * Tamaño en bytes de los paquetes RTP
	 */
    private static final int PACKET_SIZE = 1000;
    
	/**
	 * Payload Type del codec que se utiliza (en el video de isabel)
	 * MPEG-4
	 */
    private static final int MPEG_4_PT = 96;


    /**
     * SSRC del flujo RTP de video
     */
    private int ssrc;

    /**
     * Socket desde el que se envian paquetes RTP
     */
    private DatagramSocket socket;
    
    /**
     * Destino de los paquetes RTP
     */
    private SocketAddress address;

    /**
     * Numero de secuencia de los paquetes RTP
     */
    private int seqnumber = 0;
    
    /**
     * Codec de video
     */
    private IStreamCoder coder;

    /**
     * Constructor de la clase
     * @param sock Socket de envio de RTP
     * @param address Destino del flujo RTP
     * @param ssrc SSRC del flujo RTP
     * @param width Ancho del video
     * @param height Alto del video
     * @param bitrate Bitrate de salida del video
     */
    public VideoFLV2RTP(DatagramSocket sock, SocketAddress address, int ssrc, int width, int height, int bitrate) {
        this.socket = sock;
        this.address = address;
        this.ssrc = ssrc;
        //this.width = width;
        //this.height = height;

        configureCodec(bitrate, width, height);

    }

    /**
     * Crea y configura el codec MPEG-4
     * @param bitrate Bitrate de salida del codec
     */
    private void configureCodec(int bitrate, int width, int height) {

        coder = IStreamCoder.make(IStreamCoder.Direction.ENCODING);
        coder.setCodec(ICodec.ID.CODEC_ID_MPEG4);
        coder.setPixelType(Type.YUV420P);
        coder.setFlag(IStreamCoder.Flags.FLAG_QSCALE, false);
        coder.setWidth(width);
        coder.setHeight(height);
        coder.setBitRate(bitrate);
        coder.setBitRateTolerance(bitrate/2);
        coder.setNumPicturesInGroupOfPictures(30);

        IRational frameRate = IRational.make(15,1); // OJO el video se supone que llega a 15 FPS
        coder.setFrameRate(frameRate);
        coder.setTimeBase(IRational.make(frameRate.getDenominator(), frameRate.getNumerator()));

        coder.open();
    }

    @Override
    public void onVideoPicture(IVideoPictureEvent event) {
        try {
            IVideoPicture image = event.getMediaData();
            
            IPacket packet = IPacket.make();

            coder.encodeVideo(packet, image, -1);

            if (packet.getSize() > 0)
                sendNextImage(packet.getData().getByteArray(0, packet.getSize()), image.getTimeStamp());
            
            super.onVideoPicture(event);
            packet.delete();

        } catch (Exception ex) {
            Logger.getLogger(VideoFLV2RTP.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    @Override
    public void onClose(ICloseEvent event) {
    	coder.delete();
    	super.onClose(event);
    }

    /**
     * Envia una imagen codificada partiendola en tantos paquetes RTP como sea necesario
     * @param image Buffer con la imagen
     * @param time Timestamp de la imagen
     * @throws IOException
     */
    private void sendNextImage(byte[] image, long time) throws IOException {
        
        int ts = (int)(time*9/100); // RTP TIMEBASE = 1/90000
        int sended = 0;
        while (sended < image.length) {
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            DataOutputStream daos = new DataOutputStream(baos);
            
            boolean lastFragment = (image.length - sended) < PACKET_SIZE;

            // Cabecera RTP
            daos.write(0x80);
            
            // byte con el PT y el marker
            if (lastFragment)
                daos.write(128+MPEG_4_PT);
            else
                daos.write(MPEG_4_PT);

            daos.writeShort(seqnumber++);
            daos.writeInt(ts);
            daos.writeInt(ssrc);

            byte[] header = baos.toByteArray();
            byte[] buffer;

            if (lastFragment) {
                int remaining = image.length - sended;
                buffer = new byte[remaining + 12];
                System.arraycopy(header, 0, buffer, 0, 12);
                System.arraycopy(image, sended, buffer, 12, remaining);
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length , address);
                socket.send(packet);
                sended += remaining;
            }
            else {
                buffer = new byte[PACKET_SIZE + 12];
                System.arraycopy(header, 0, buffer, 0, 12);
                System.arraycopy(image, sended, buffer, 12, PACKET_SIZE);
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length , address);
                socket.send(packet);
                sended += PACKET_SIZE;
            }
        }
    }


    /**
     * Metodo de prueba de la clase
     */
    public static void main(String[] args) throws Exception {
        
        IContainer container = IContainer.make();
        IContainerFormat format = IContainerFormat.make();
        format.setInputFormat("flv");
        container.open(new FileInputStream("/tests/video.flv"), format);

        IMediaReader fileReader = ToolFactory.makeReader(container);
        DatagramSocket sock = new DatagramSocket();
        SocketAddress address = new InetSocketAddress("triton.dit.upm.es", 51017);
        VideoFLV2RTP rtpwriter = new VideoFLV2RTP(sock, address, 11, 320, 240, 50000);

        fileReader.addListener(rtpwriter);

        //int i = 0;
        while (fileReader.readPacket() == null) {
            Thread.sleep(30);
            //System.out.println(i++);
        }
    }
}
