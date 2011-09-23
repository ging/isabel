
package isabel.gwflash.xuggle;

import java.awt.Frame;
import java.awt.Image;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.util.Timer;
import java.util.TimerTask;


import isabel.gwflash.vncviewer.VncCanvas;
import isabel.gwflash.vncviewer.VncViewer;

import com.xuggle.mediatool.IMediaListener;
import com.xuggle.mediatool.MediaGeneratorAdapter;
import com.xuggle.mediatool.event.VideoPictureEvent;
import com.xuggle.xuggler.IRational;
import com.xuggle.xuggler.IVideoPicture;
import com.xuggle.xuggler.video.ConverterFactory;
import com.xuggle.xuggler.video.IConverter;

/**
 * MediaGenerator que saca imagenes de un VNC
 * @author Fernando Escribano
 *
 */
public class VNCImageGenerator extends MediaGeneratorAdapter {
	
	/**
	 * Index del stream (posiblemente de igual)
	 */
	private static final int STREAM_INDEX = 1;
	
	/**
	 * Conversor de BufferedImage a VideoPicture
	 */
	private IConverter converter;
	
	/**
	 * El VNC para pasarle eventos de raton y teclado
	 */
	private VncCanvas vnc;
		
	/**
	 * Constructor de la clase. Crea una hebra que va leyendo imagenes del VNC y se las pasa a los
	 * listeners
	 * @param host Host donde se conecta el VNC
	 * @param password Password para conectarse al VNC
	 * @param port Puerto donde se conecta el VNC
	 * @param period Tiempo entre las imagenes generadas
	 * @param width Ancho de las imagenes que se generaran
	 * @param height Alto de las imagenes que se generaran
	 */
	public VNCImageGenerator(final String host, final String password,final String port, final long period, final int width, final int height) {
				
		new Timer().scheduleAtFixedRate(new TimerTask() {

			IRational timebase = IRational.make(1, 1000000);
			VncViewer v;
			long i = 0;
			{
				String[] args = { "HOST", host, "PASSWORD", password, "PORT", port, "ENCODING", "RAW"};
				v = new VncViewer();
				v.mainArgs = args;
				v.inAnApplet = false;
				v.inSeparateFrame = true;

				v.init();
				v.start();
			}
			
			@Override
			public void run() {
				if (v.vc != null && v.vc.memImage != null) {
					synchronized (v.vc.memImage) {
						try {
							if (vnc == null) {
								vnc = v.vc;
								v.vncFrame.setState(Frame.ICONIFIED);
							}
							
							BufferedImage image = convertToType((BufferedImage)v.vc.memImage, BufferedImage.TYPE_3BYTE_BGR, width, height);
							long ts = i++*period*1000;
							IVideoPicture picture = toPicture(image, ts);
							picture.setTimeBase(timebase);
							VideoPictureEvent evt = new VideoPictureEvent(VNCImageGenerator.this,
								picture, STREAM_INDEX);
							
							for (IMediaListener lis: getListeners()) {
								lis.onVideoPicture(evt);
							}
							picture.delete();
						} catch (Exception e) {
							e.printStackTrace();
						}
					}
				}
			}
		}, 5000, period);		
	}

	/**
	 * Convierte una BufferedImage a un IVideoPicture
	 * @param image BufferedImage a convertir
	 * @param ts Timestamp de la imagen
	 * @return IVideoPicture con la imagen
	 */
	private IVideoPicture toPicture(BufferedImage image, long ts) {
		if (converter == null) {
			converter = ConverterFactory.createConverter(image, com.xuggle.xuggler.IPixelFormat.Type.YUV420P);
		}
		return converter.toPicture(image, ts);
	}

	/**
	 * Convierte una BufferedImage al formato que le gusta al Xuggle y la escala si hace falta
	 * @param sourceImage Imagen a convertir
	 * @param targetType Tipo deseado de la imagen de retorno
	 * @param width Ancho de la imagen
	 * @param height Alto de la imagen
	 * @return Imagen en el formato elegido
	 */
	private static BufferedImage convertToType(BufferedImage sourceImage, int targetType, int width, int height) {

		// Resize the image if necessary
		Image resizedImage;
		if (sourceImage.getWidth() == width && sourceImage.getHeight() == height)
			resizedImage = sourceImage;
		else
			resizedImage = sourceImage.getScaledInstance(width, height, Image.SCALE_SMOOTH);

		BufferedImage image = new BufferedImage(width, height, targetType);
		image.getGraphics().drawImage(resizedImage, 0, 0, null);
		return image;
	}
	
	public void keyboardEvent(String type, int keyCode, int keyChar) {
		if (vnc != null) {
		    if (type.equalsIgnoreCase("press")) {
			    KeyEvent evt = new KeyEvent(vnc, KeyEvent.KEY_PRESSED, System.currentTimeMillis(), 0, keyCode, (char)keyChar);
			    vnc.keyPressed(evt);
			} else if (type.equalsIgnoreCase("release")) {
			    KeyEvent evt = new KeyEvent(vnc, KeyEvent.KEY_RELEASED, System.currentTimeMillis(), 0, keyCode, (char)keyChar);
			    vnc.keyReleased(evt);
			}
		}
	}
	
	
	public void mouseEvent(String type, int x, int y) {

		if (vnc != null) {
		    if (type.equalsIgnoreCase("press")) {
		    	MouseEvent evt = new MouseEvent(vnc, MouseEvent.MOUSE_PRESSED, System.currentTimeMillis(),
		    			0, x, y, 1, false, MouseEvent.BUTTON1);
		    	vnc.mousePressed(evt);
			} else if (type.equalsIgnoreCase("release")) {
		    	MouseEvent evt = new MouseEvent(vnc, MouseEvent.MOUSE_RELEASED, System.currentTimeMillis(),
		    			0, x, y, 1, false, MouseEvent.BUTTON1);
		    	vnc.mouseReleased(evt);
				
			} else if (type.equalsIgnoreCase("move")) {
		    	MouseEvent evt = new MouseEvent(vnc, MouseEvent.MOUSE_MOVED, System.currentTimeMillis(),
		    			0, x, y, 1, false, MouseEvent.BUTTON1);
		    	vnc.mouseMoved(evt);
			}
		}
	}	
}
