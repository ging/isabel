
package isabel.gwflash.xuggle;

import com.xuggle.mediatool.MediaToolAdapter;
import com.xuggle.mediatool.event.AudioSamplesEvent;
import com.xuggle.mediatool.event.IAudioSamplesEvent;
import com.xuggle.xuggler.IAudioResampler;
import com.xuggle.xuggler.IAudioSamples;

/**
 * MediaTool que se encarga de resamplea un flujo de audio
 * de la frecuancia que tenga a la deseada como salida
 * @author Fernando Escribano
 */
public class Resampler extends MediaToolAdapter {

	/**
	 * Resampleador
	 */
    private IAudioResampler resampler;
    
    /**
     * Rate deseado a la salida
     */
    private int outrate;

    /**
     * Constructor de la clase.
     * @param outrate Rate de salida
     */
    public Resampler(int outrate) {
        this.outrate = outrate;
    }
    
    @Override
    public void onAudioSamples(IAudioSamplesEvent event) {
        IAudioSamples samples = event.getAudioSamples();

        if (resampler == null) {
             resampler = IAudioResampler.make(1, 1, outrate, samples.getSampleRate());
        }

        if (event.getAudioSamples().getNumSamples() > 0) {
            IAudioSamples out = IAudioSamples.make(samples.getNumSamples(), samples.getChannels());
            resampler.resample(out, samples, samples.getNumSamples());

            AudioSamplesEvent asc = new AudioSamplesEvent(event.getSource(), out, event.getStreamIndex());
            super.onAudioSamples(asc);
            out.delete();
        }
    }
    
    public void delete() {
    	resampler.delete();
    }
}
