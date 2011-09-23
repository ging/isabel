package isabel.gwflash.red5client.utils;
import java.net.URLStreamHandler;
import java.net.URLStreamHandlerFactory;


public class RTMPHandlerFactory implements URLStreamHandlerFactory {

	public URLStreamHandler createURLStreamHandler(String protocol) {
		if (protocol.equalsIgnoreCase(RTMPHandler.DEFAULT_RTMP_PROTOCOL)) {
			return new RTMPHandler();
		}
		// TODO Auto-generated method stub
		return null;
	}

}
