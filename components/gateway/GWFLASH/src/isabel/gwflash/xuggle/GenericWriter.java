package isabel.gwflash.xuggle;

import isabel.gwflash.FlashGatewayEnvConfig;

import java.io.OutputStream;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.atomic.AtomicInteger;

import com.xuggle.mediatool.MediaToolAdapter;
import com.xuggle.mediatool.event.IAudioSamplesEvent;
import com.xuggle.mediatool.event.ICloseEvent;
import com.xuggle.mediatool.event.IVideoPictureEvent;
import com.xuggle.xuggler.Configuration;
import com.xuggle.xuggler.IAudioSamples;
import com.xuggle.xuggler.ICodec;
import com.xuggle.xuggler.IContainer;
import com.xuggle.xuggler.IContainerFormat;
import com.xuggle.xuggler.IPacket;
import com.xuggle.xuggler.IPixelFormat;
import com.xuggle.xuggler.IRational;
import com.xuggle.xuggler.IStream;
import com.xuggle.xuggler.IStreamCoder;

public class GenericWriter extends MediaToolAdapter {

	/**
	 * Rate de salida del audio
	 */
    public static final int SPEEX_OUT_RATE = 16000;
    public static final int MP3_OUT_RATE = 44100;
	
    /**
	* Formatos de los contenedores
	*/
	private static final String FLV_FORMAT = "flv";
	private static final String TS_FORMAT = "mpegts";
	
	/**
	 * Presets h264
	 */
	private static final String PRESET_FLV = "/etc/isabel/gw/preseth264";
	private static final String PRESET_HTTPLIVE = "/etc/isabel/gw/preseth264HttpLive";
	    
	private static final long MAX_DIFF = 2000000;
    /**
     * Para mejorar(?) la sinconizacion descartamos los primeros segundos
     */
    private static final int T_DISCARD = 10;
    
    /**
     * Periodo en segundos de la tarea periodica que imprime trazas
     */
    private static final int T_SYNC = 100;

    /**
     * Tiempo de inicio a partir del cual se empieza a descartar
     */
    private static long tinit = 0;
    
    /**
     * Indica si se trata de sincronizar audio y video 
     */
    private boolean sync;
    
    /**
     * Indica si se ha encontrado el primer keyframe
     */
    private boolean keyFinded = false; 
    
    /**
     * IContainer de Xuggle que se utiliza para codificar a FLV
     */
    private IContainer outCont;
    
    /**
     * Codec de video
     */
    private IStreamCoder vCoder;
    /**
     * Codec de audio
     */
    private IStreamCoder aCoder;
    
    /**
     * Indica si ya se ha escrito la cabecera del archivo
     */
    private boolean headerWritten = false;
    
    /**
     * Guarda el TimeStamp del ultimo frame de audio procesado (Para sincronizar el video)
     */
    private long lastAudioTs = 0;
    
    //--------------------------------------------------------------//
    //           ESTO SOLO SE USA PARA PINTAR UNA TRAZAS            //
    //--------------------------------------------------------------//
    /**
     * Numero de frames de video procesados cada T_SYNC
     */
    private AtomicInteger vframes = new AtomicInteger(0);
    
    /**
     * Numero de frames de audio procesados cada T_SYNC
     */
    private AtomicInteger aframes = new AtomicInteger(0);
    
    /**
     * Numero de frames de video que de menos (no se usa)
     */
    private AtomicInteger missingFrames = new AtomicInteger(0);
    
    /**
     * Numero de frames de video que se esperan cada T_SYNC
     */
    private int expectedFrames;
    
    /**
     * Numero totals de frames de video procesados
     */
    private int vframest = 0;
    
    /**
     * Numero total de frames de audio procesados
     */
    private int aframest = 0;
    
    /**
     * Contador del numero de periodos
     */
    private int beats = -1;
    
    /**
     * Magic number to sync audio and video
     */
    private int tsScale;
    
    
    // Factory methods
    public static GenericWriter getFlvWriter(OutputStream out, int bitrate, int audiobitrate, int heigth, int width,
    		                                 int framerate, boolean hasAudio, boolean hasVideo, boolean sync){
    	System.out.println("Devolviendo FLVWriter H264: " + FlashGatewayEnvConfig.useH264() + " preset " + PRESET_FLV );
    	return new GenericWriter(out, bitrate, audiobitrate, heigth, width, framerate, hasAudio, hasVideo, sync, FLV_FORMAT, SPEEX_OUT_RATE, ICodec.ID.CODEC_ID_SPEEX, 1, PRESET_FLV);
    }

    public static GenericWriter getFlvMp3Writer(OutputStream out, int bitrate, int audiobitrate, int heigth, int width,
            int framerate, boolean hasAudio, boolean hasVideo, boolean sync){
		System.out.println("Devolviendo FLVWriter H264 + MP3: " + FlashGatewayEnvConfig.useH264() + " preset " + PRESET_FLV);
		return new GenericWriter(out, bitrate, audiobitrate, heigth, width,	framerate, hasAudio, hasVideo, sync, FLV_FORMAT, MP3_OUT_RATE, ICodec.ID.CODEC_ID_MP3, 1, PRESET_FLV);
}
    
    public static GenericWriter getTsWriter(OutputStream out, int bitrate, int audiobitrate, int heigth, int width,
    		                                 int framerate, boolean hasAudio, boolean hasVideo, boolean sync){
    	System.out.println("Devolviendo TsWriter, preset " + PRESET_HTTPLIVE );
    	return new GenericWriter(out, bitrate, audiobitrate, heigth, width, framerate, hasAudio, hasVideo, sync, TS_FORMAT, MP3_OUT_RATE, ICodec.ID.CODEC_ID_MP3, 1000, PRESET_HTTPLIVE);
    }
    

    /**
     * Crea un GenericWriter
     * 
     * @param out OutputStream donde se escribe
     * @param bitrate Bitrate de video
     * @param audiobitrate Bitrate del audio (32000 o 16000)
     * @param heigth Alto del video
     * @param width Ancho del audio
     * @param framerate Framerate del video
     * @param hasAudio Indica si el FLV tiene audio
     * @param hasVideo Indica si el FLV tiene video
     * @param sync Indica si se quiere que se intente sincronizar audio y video
     * @param contFormat Formato del contenedor
     * @param audioSampleRate Sample Rate del audio
     * @param audioCodec Codec de Audio
     * @param tsScale escala para sincronizacion audio en Ts
     * @param presetFile Archivo de preset para codificacion en h264
     */
    private GenericWriter(OutputStream out, int bitrate, int audiobitrate, int heigth, int width, int framerate, boolean hasAudio, boolean hasVideo, boolean sync, String contFormat,
    					 int audioSampleRate, ICodec.ID audioCodec, int tsScale, String presetFile) {
    	this.expectedFrames = framerate*T_SYNC;
    	this.sync = sync;
    	this.tsScale = tsScale;
    	
    	outCont = IContainer.make();
        IContainerFormat format = IContainerFormat.make();
        format.setOutputFormat(contFormat, null, null);
        
        outCont.open(out, format);

        if (hasVideo) {
            // Video Stream
            IStream vStream = outCont.addNewStream(1);
            vCoder = vStream.getStreamCoder();
            ICodec vcodec;

            if (FlashGatewayEnvConfig.useH264()|| contFormat.equals(TS_FORMAT)) {
            	vcodec = ICodec.findEncodingCodec(ICodec.ID.CODEC_ID_H264);
            	vCoder.setCodec(vcodec);
                int result = Configuration.configure(presetFile, vCoder);
            
                if (result < 0)
            	    throw new RuntimeException("Ha fallado la configuracion: " + result);
            }
            else {
            	vcodec = ICodec.findEncodingCodec(ICodec.ID.CODEC_ID_FLV1);
            	vCoder.setCodec(vcodec);
                vCoder.setNumPicturesInGroupOfPictures(100);
            }
                        
            vCoder.setPixelType(IPixelFormat.Type.YUV420P);
            vCoder.setFlag(IStreamCoder.Flags.FLAG_QSCALE, false);
            vCoder.setBitRate(bitrate);
            vCoder.setBitRateTolerance(bitrate/4);
            vCoder.setFrameRate(IRational.make(framerate,1));
            vCoder.setTimeBase(IRational.make(1, framerate));
            vCoder.setHeight(heigth);
            vCoder.setWidth(width);
            vCoder.open();      
        }

        if (hasAudio) {
            // Audio stream
            IStream aStream = outCont.addNewStream(0);
            aCoder = aStream.getStreamCoder();
            ICodec acodec = ICodec.findEncodingCodec(audioCodec);
            aCoder.setCodec(acodec);
            aCoder.setChannels(1);
            aCoder.setSampleRate(audioSampleRate);
            aCoder.setFlag(IStreamCoder.Flags.FLAG_QSCALE, false);
            aCoder.setBitRate(audiobitrate);
            aCoder.open();
            
         }

        new Timer().scheduleAtFixedRate(new TimerTask() {
            public void run() {
                if (beats++ >= 0) {
                	int vf = vframes.get();
                	int af = aframes.get();
                    vframest+=vf;
                    aframest+=af;
                    System.out.print("Frames de video: " + vf + ". Media = " + (float)vframest/(float)beats);
                    System.out.println("  Frames de audio: " + af + ". Media = " + (float)aframest/(float)beats);
                    missingFrames.addAndGet(expectedFrames - vf);
                }
                vframes.set(0);
                aframes.set(0);
            }
        }, 10000, T_SYNC * 1000);
    }
    
        
    @Override
    public void onVideoPicture(IVideoPictureEvent event) {    	
    	if (!keyFinded) {
    		if (event.getPicture().isKeyFrame())
    			keyFinded = true;
    		else {
    			return;
    		}
    	}

        IPacket packet = IPacket.make();
        vCoder.encodeVideo(packet, event.getPicture(), -1);
        if (packet.isComplete()) {
            writePacket(packet, false);
        }
        super.onVideoPicture(event);
        packet.delete();
    	vframes.incrementAndGet();
    }

    @Override
    public void onAudioSamples(IAudioSamplesEvent event) {
    	IPacket packet = IPacket.make();
    	IAudioSamples as = event.getAudioSamples();
    	long num = as.getNumSamples();
    	long index = 0;

    	while (num > 0) {
    		long cons = aCoder.encodeAudio(packet, as, index);
    		if (cons < 0) {
    			System.out.println("Error encoding audio");
    			return;
    		}
    		num -=cons;
    		index +=cons;
    		if (packet.getSize() > 0) {
    			writePacket(packet, true);
    			aframes.incrementAndGet();
    		}
    	}
    	super.onAudioSamples(event);
    	packet.delete();
    }
    
    private boolean closed = false;
    @Override
    public synchronized void onClose(ICloseEvent event) {
    	if (!closed) {
	    	outCont.writeTrailer();
	    	outCont.close();
	    	if (vCoder != null)
	    	    vCoder.delete();
	    	if (aCoder != null)
	    	    aCoder.delete();
	    	outCont.delete();
	    	closed = true;
	    	super.onClose(event);
    	}
    }
    
    /**
     * Escribe un frame de audio o video codificado al Icontainer
     * @param packet IPacket salido del codificador
     * @param isAudio Indica si el Ipacket es de audio o de video
     * @param doTs Indica si el Ipacket hay que escribirlo al TScontainer
     */
    private synchronized void writePacket(IPacket packet, boolean isAudio) {
    	if (!headerWritten) {
    		outCont.writeHeader();
    		headerWritten = true;
    	}
    	
    	if (checkTime() || !sync) {
    		if (isAudio) {
    			long ts = packet.getTimeStamp();
    			if (ts > lastAudioTs || ts - lastAudioTs > MAX_DIFF)
    			    lastAudioTs = ts;
    		}
    		else {
    			if (sync) {
//    			    if (aCoder.getCodecID() == ICodec.ID.CODEC_ID_MP3)
//    			        System.out.println("TS = " + packet.getTimeStamp() + " PTS =" + packet.getPts() + " DTS = " + packet.getDts() + " Audio =" + lastAudioTs);    			    
    				
    				lastAudioTs += tsScale;
    				int tmp = Math.round(lastAudioTs/tsScale) * tsScale;
    			    packet.setTimeStamp(tmp);
    			    packet.setPts(tmp);
    			    packet.setDts(tmp);
    			    
//    			    if (aCoder.getCodecID() == ICodec.ID.CODEC_ID_MP3)
//    			        System.out.println("TS = " + packet.getTimeStamp() + " PTS =" + packet.getPts() + " DTS = " + packet.getDts() + " Audio =" + lastAudioTs);    			    
    			}
    		}
    		outCont.writePacket(packet);
    	}
    }

    /**
     * Comprueba si ya ha pasado el tiempo de descarte
     * @return True si ya a pasado y false en caso contrario.
     */
    private boolean checkTime() {
    	// Discard the first T_DISCARD SECONDS
    	if (tinit == 0)
    		tinit = System.currentTimeMillis();
    	if ( (System.currentTimeMillis() - tinit > T_DISCARD*1000)) {
    		return true;
    	}
    	else {
    		return false;
    	}
    }    

}
