package isabel.gwflash.red5client.utils;
import java.io.IOException;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLStreamHandler;


public class RTMPHandler extends URLStreamHandler{

	public static final int DEFAULT_RTMP_PORT = 1935;

	public static final String DEFAULT_RTMP_PROTOCOL = "rtmp";
	@Override
	protected URLConnection openConnection(URL u) throws IOException {
		return null;
	}

	/**
	 * @return the default RTMP port
	 */
	@Override
	protected int getDefaultPort()
	{
		return RTMPHandler.DEFAULT_RTMP_PORT;
	}


}
