package isabel.gwflash.xuggle;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

import com.xuggle.ferry.IBuffer;
import com.xuggle.mediatool.IMediaListener;
import com.xuggle.mediatool.MediaGeneratorAdapter;
import com.xuggle.mediatool.event.AudioSamplesEvent;
import com.xuggle.xuggler.IAudioSamples;
import com.xuggle.xuggler.IRational;

public class RTPAudioGenerator extends MediaGeneratorAdapter {

	private static final int STREAM_INDEX = 0;
	
	private static final int RTP_HEADER_LENGTH = 12;
	private static final int PACKET_DATA_LENGTH = 640; 
	private static final int SAMPLES_PER_PACKET = 320;
	private static final int SAMPLE_RATE = 16000;

	private final DatagramSocket socket;
	
	private Thread t;

	public RTPAudioGenerator(int port) throws SocketException {
		socket = new DatagramSocket(port);

		t = new Thread(new Runnable() {

			@Override
			public void run() {
				DatagramPacket p = new DatagramPacket(new byte[1024], 1024);
				IRational timebase = IRational.make(1, 1000000);
				long ts = 0;
				
				while(true) {
					try {
						socket.receive(p);
						if (p.getLength() == RTP_HEADER_LENGTH + PACKET_DATA_LENGTH) {
							IBuffer buffer = IBuffer.make(null, IBuffer.Type.IBUFFER_SINT16, 1024, false);
							buffer.put(p.getData(), RTP_HEADER_LENGTH, 0, PACKET_DATA_LENGTH);
							IAudioSamples samples = IAudioSamples.make(SAMPLES_PER_PACKET, 1);							
							long time = IAudioSamples.samplesToDefaultPts(ts, SAMPLE_RATE);
							samples.setTimeBase(timebase);
							samples.setTimeStamp(time);
							samples.setData(buffer);
							samples.setComplete(true, SAMPLES_PER_PACKET, SAMPLE_RATE, 1, IAudioSamples.Format.FMT_S16, time);
							ts+=SAMPLES_PER_PACKET;
							AudioSamplesEvent evt = new AudioSamplesEvent(RTPAudioGenerator.this, samples, STREAM_INDEX);
							
							for (IMediaListener lis: getListeners()) {
								lis.onAudioSamples(evt);
							}
							
							buffer.delete();
							samples.delete();
							
						}
						else {
							System.out.println("Paquete raro");
						}
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}

				}
			}				
		});
	}
	
	public void go() {
		t.start();
	}
	
	public static void main(String[] args) throws Exception {
		OutputStream outstream = new FileOutputStream("/tmp/audio.flv");
		RTPAudioGenerator generator = new RTPAudioGenerator(6000);
		Resampler resampler = new Resampler(22050);
		GenericWriter writer = GenericWriter.getFlvWriter(outstream, 0, 32000, 0, 0, 0, true, false, false);
		generator.addListener(resampler);
		resampler.addListener(writer);
		
		generator.go();
	}
}
