
package isabel.gwflash.xuggle;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketAddress;

import com.xuggle.mediatool.MediaToolAdapter;
import com.xuggle.mediatool.event.IReadPacketEvent;

/**
 * Esta clase se encarga de pasar audio de un stream FLV a RTP
 * Supone que el audio llega en SPEEX, en frames de 20 ms y lo envía tal cual a isabel.
 * @author Fernando Escribano
 */
public class AudioFLVSpeex2RTP extends MediaToolAdapter {

	private static final int SPEEX_PT = 124;
	private static final int SAMPLES_PER_FRAME = 320;
    /**
     * Numero de secuencia de los paquetes RTP
     */
    private int seqnumber = 1;
    
    /**
     * Timestamp de los paquetes RTP
     */
    private int ts = 100;
    
    /**
     * SSRC del flujo RTP
     */
    private int ssrc;
    
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

    /**
     * Constructor de la clase
     * @param sock Socket UDP para enviar los paquetes RTP 
     * @param address Destino de los paquetes RTP
     * @param ssrc SSRC que se pondra al flujo RTP
     */
    public AudioFLVSpeex2RTP(DatagramSocket sock, SocketAddress address, int ssrc) {
        this.socket = sock;
        this.address = address;
        this.ssrc = ssrc;

        buffer = new byte[3000];
    }
    
    @Override
    public void onReadPacket(IReadPacketEvent event) {
    	try {
			sendNextPacket(event.getPacket().getData().getByteArray(0, event.getPacket().getSize()), event.getPacket().getSize());
		} catch (IOException e) {
			e.printStackTrace();
		}
    	super.onReadPacket(event);
    }
        
    /**
     * Envia un paquete RTP
     * @param samples Buffer con el audio a enviar
     * @throws IOException
     */
    private void sendNextPacket(byte[] samples, int size) throws IOException {
    	
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutputStream daos = new DataOutputStream(baos);

        // Cabecera RTP
        daos.write(0x80);
        daos.write(SPEEX_PT);
        daos.writeShort(seqnumber++);
        daos.writeInt(ts += SAMPLES_PER_FRAME);
        daos.writeInt(ssrc);

        byte[] header = baos.toByteArray();

        for (int i = 0; i < 12; i++) {
            buffer[i] = header[i];
        }

        System.arraycopy(samples, 0, buffer, 12, size);

        DatagramPacket packet = new DatagramPacket(buffer, size + 12, address);

        socket.send(packet);
    }
}
