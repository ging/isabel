
package isabel.gwflash.xuggle;

import isabel.gwflash.FlashGatewayEnvConfig;

import java.io.FileOutputStream;
import java.io.OutputStream;

import com.xuggle.mediatool.IMediaTool;

/**
 * Clase que se encarga de generar FLV a partir de una conexion VNC
 * @author Fernando Escribano
 */
public class VNC2FLV implements Transcoder {

	/**
	 * Framerate deseado
	 */
	private static final int FRAMERATE = FlashGatewayEnvConfig.getFlashFramerate();
		
	/**
	 * Ancho del VNC
	 */
	private int width;
	
	/**
	 * Alto del VNC
	 */
	private int height;
	
	/**
	 * Generador de imagenes a partir de un cliente VNC
	 */
	private VNCImageGenerator vncgenerator;
		
	/**
	 * Constructor de la clase
	 * @param host Servidor VNC
	 * @param password Password para conectarse al VNC
	 * @param port Puerto del servidor VNC
	 * @param width Ancho del VNC
	 * @param height Alto del VNC
	 */
	public VNC2FLV(String host, String password, String port, int width, int height) {
		this.width = width;
		this.height = height;
		
		vncgenerator = new VNCImageGenerator(host, password, port, 1000 / FRAMERATE, width, height);
	}
	
	@Override
	public void keyboardEvent(String type, int keyCode, int keyChar) {		
		vncgenerator.keyboardEvent(type, keyCode, keyChar);
	}
	
	@Override
	public void mouseEvent(String type, int x, int y) {
		vncgenerator.mouseEvent(type, x, y);		
	}

	// Cosas del Xuggler
	private IMediaTool flvwriter;
	
	/**
	 * Empieza a generar FLV. (Habria que hacer un stop?)
	 * @param out OutputStream donde se escribe el FLV
	 */
	@Override
	public void go(OutputStream out) {
		if (flvwriter != null) {
			vncgenerator.removeListener(flvwriter);
			flvwriter.onClose(null);
		}
		
		System.out.printf("VNC W=%d H=%d FR=%d\n", width, height, FRAMERATE);

		int bitrate = FlashGatewayEnvConfig.getFlashBitRate();
		System.out.println("VIDEO BW = " + bitrate);
		flvwriter = GenericWriter.getTsWriter(out, bitrate, 32000, height, width, FRAMERATE, false, true, false);

		// Make the chain (rtp->flv)
		vncgenerator.addListener(flvwriter);
	}

	/**
	 * Metodo de prueba de la clase
	 */
	public static void main(String[] args) throws Exception {
		OutputStream outstream = new FileOutputStream("/tmp/vnc.ts");
		VNC2FLV vnc = new VNC2FLV("chotis2.dit.upm.es", "fr0d0", "5900", 1024, 768);
		vnc.go(outstream);
//        Thread.sleep(20000);
//        OutputStream outstream2 = new FileOutputStream("/tmp/vnc2.flv");
//        vnc.go(outstream2);		
	}
}
