
package isabel.gwflash.xuggle;

import com.xuggle.mediatool.IMediaReader;
import com.xuggle.mediatool.MediaToolAdapter;
import com.xuggle.mediatool.ToolFactory;
import com.xuggle.mediatool.event.IAudioSamplesEvent;
import com.xuggle.xuggler.IAudioSamples;

import java.io.*;
import java.net.*;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Esta clase se encarga de pasar audio de un stream FLV a RTP
 * En RTP utiliza PCM con el tamaño de paquete que se pase como parametro
 * @author Fernando Escribano
 */
public class AudioFLV2RTP extends MediaToolAdapter {


    /**
     * Tamaño de los paquetes de audio en ms
     */
    private int msPerPacket;
    
    /**
     * Rate del codec
     */
    private int frec;
    
    /**
     * Payload type que se pondra a los paquetes RTP
     */
    private byte pt;

    /**
     * Numero de secuencia de los paquetes RTP
     */
    private int seqnumber = 0;
    
    /**
     * Timestamp de los paquetes RTP
     */
    private int ts = 100;
    
    /**
     * SSRC del flujo RTP
     */
    private int ssrc;
    
    /**
     * Tamaño en bytes de los paquetes RTP
     */
    private int packetSize;

    /**
     * Socket por el que se envian los paquetes RTP
     */
    private DatagramSocket socket;
    
    /**
     * Destino de los paquetes RTP
     */
    private SocketAddress address;

    // Buffers para manipulacion del audio
    private byte[] buffer;
    private byte[] buffer2;

    // Se usa para controlar cuanto audio queda en el buffer
    private int remaining = 0;

    /**
     * Constructor de la clase
     * @param sock Socket UDP para enviar los paquetes RTP 
     * @param address Destino de los paquetes RTP
     * @param ssrc SSRC que se pondra al flujo RTP
     * @param frec Rate de entrada y salida del audio (NO resamplea)
     * @param ms Tamaño en milisegundos de los paquetes RTP
     * @param pt Payload Type que se pondra a los paquetes RTP
     */
    public AudioFLV2RTP(DatagramSocket sock, SocketAddress address, int ssrc, int frec, int ms, byte pt) {
        this.socket = sock;
        this.address = address;
        this.ssrc = ssrc;
        this.frec = frec;
        this.msPerPacket = ms;
        this.pt = pt;
        packetSize = frec*ms*2/1000;

        buffer = new byte[3000];
        buffer2 = new byte[3000];
    }
    
    @Override
    public void onAudioSamples(IAudioSamplesEvent event) {
        IAudioSamples samples = event.getAudioSamples();

        int arrived = samples.getSize();
        byte[] buf = samples.getData().getByteArray(0, arrived);

        System.arraycopy(buf, 0, buffer2, remaining, arrived);

        remaining += arrived;

        while (remaining > packetSize) {
            try {
                sendNextPacket(buffer2);
                remaining -=packetSize;
                System.arraycopy(buffer2, packetSize, buffer2, 0, remaining);
            } catch (IOException ex) {
                Logger.getLogger(AudioFLV2RTP.class.getName()).log(Level.SEVERE, null, ex);
            }

        }
        super.onAudioSamples(event);
    }
    
    /**
     * Envia un paquete RTP
     * @param samples Buffer con el audio a enviar
     * @throws IOException
     */
    private void sendNextPacket(byte[] samples) throws IOException {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutputStream daos = new DataOutputStream(baos);

        // Cabecera RTP
        daos.write(0x80);
        daos.write(pt);
        daos.writeShort(seqnumber++);
        daos.writeInt(ts += (frec/1000) * msPerPacket);
        daos.writeInt(ssrc);

        byte[] header = baos.toByteArray();

        for (int i = 0; i < 12; i++) {
            buffer[i] = header[i];
        }

        System.arraycopy(samples, 0, buffer, 12, packetSize);

        DatagramPacket packet = new DatagramPacket(buffer, packetSize + 12, address);

        socket.send(packet);
    }

    /**
     * Metodo de prueba de la clase
     */
    public static void main(String[] args) throws Exception {
        IMediaReader fileReader = ToolFactory.makeReader("c:/peli.flv");
        Resampler resampler = new Resampler(16000);
        DatagramSocket sock = new DatagramSocket();
        SocketAddress address = new InetSocketAddress("localhost", 51001);
        AudioFLV2RTP rtpwriter = new AudioFLV2RTP(sock, address, 11, 16000, 20, (byte)102);
        
        fileReader.addListener(resampler);
        resampler.addListener(rtpwriter);

        int i = 0;
        while (fileReader.readPacket() == null) {
            System.out.println(i++);
        }
    }
}
