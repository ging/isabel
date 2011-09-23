package isabel.gwflash;

/**
 * Clase que se encarga de conseguir la configuracion de la pasarela
 * contenida en variables de entorno
 * 
 * @author pedro
 *
 */
public abstract class FlashGatewayEnvConfig {
	//Nombres de las variables de entorno
	private final static String flashVideoWidth = "FLASH_VIDEO_WIDTH";
	private final static String flashVideoHeight = "FLASH_VIDEO_HEIGHT";
	private final static String flash2streams = "FLASH_GATEWAY_2_STREAMS";
	private final static String flashClientBitRate = "FLASH_CLIENT_BITRATE";
	private final static String flashBitRate = "FLASH_BITRATE";
	private final static String flashFramerate = "FLASH_FRAMERATE";
	private final static String flashAudioBitRate = "FLASH_AUDIO_BITRATE";
	private final static String flashServerURL = "FLASH_SERVER_URL";
	private final static String flashMetadataURL = "FLASH_METADATA_URL";
	private final static String flashuseH264 = "FLASH_USE_H264";
	private final static String flashuseMp3 = "FLASH_USE_MP3";
	private final static String flashDisable = "FLASH_DISABLE";
	private final static String iGWEnable = "IGW_ENABLE";
	private final static String iGWBEnable = "IGWB_ENABLE";
	private final static String iGWCaptureDisplay = "IGW_CAPTURE_DISPLAY";
	private final static String iGWVideoBitRate = "IGW_VBITRATE";
    private final static String iGWAudioCaptureAddress = "IGW_AUDIO_CAPTURE_ADDR";
    private final static String iGWAudioCapturePort = "IGW_AUDIO_CAPTURE_PORT";
	private final static String iGWAudioBitRate = "IGW_ABITRATE";
	private final static String iGWPrefix = "IGW_PREFIX";
	private final static String iGWBaseURL = "IGW_BASE_URL";
	private final static String iGWOutputDir = "IGW_OUTPUT_DIR";
	private final static String isabelSessionID = "ISABEL_SESSION_ID";
	

	//Parametros por defecto, si no se especifica aqui se devuelve null
	private final static int defaultFlashWidth = 1024;
	private final static int defaultFlashHeight = 768;
	private final static int defaultClientBitRate = 100000;
	private final static int defaultBitRate = 400000;
	private final static int defaultAudioBitRate = 32000;
	private final static int defaultFramerate = 10;
	private final static String defaultServerURL = "rtmp://localhost/IsabelWebGWApp/session";
	private final static String defaultiGWCaptureDisplay = ":33";
	private final static int defaultiGWVideoBitRate = 300;
	private final static String defaultiGWAudioCaptureAddress = "0.0.0.0";
	private final static int defaultiGWAudioCapturePort = 61102;
	private final static int defaultiGWAudioBitRate = 32;
	private final static String defaultiGWPrefix = "igwstream";
	private final static String defaultiGWBaseURL = "http://localhost/live/";
	private final static String defaultiGWOutputDir = "/var/www/live";


	
	/**
	 * Obtiene el ancho del video Flash
	 * @return
	 */
	public static int getFlashVideoWidth() {
		int width = defaultFlashWidth ;
        try {
        	width = Integer.parseInt(System.getenv(flashVideoWidth));
        }catch (Exception e) {
        }
        return width;		
	}
	
	/**
	 * Obtiene el valor del alto del video Flash 
	 * @return
	 */
	public static int getFlashVideoHeight(){
		int height = defaultFlashHeight;
        try {
        	height = Integer.parseInt(System.getenv(flashVideoHeight));
        }catch (Exception e) {
        }
        return height;
	}
	
	/**
	 * Obtiene el framerate del video flash
	 * @return
	 */
	public static int getFlashFramerate() {
		int fr = defaultFramerate;
        try {
        	fr = Integer.parseInt(System.getenv(flashFramerate));
        }catch (Exception e) {
        }
        return fr;		
	}
	
	/**
	 * Obtiene el parametro que indica es necesario dividir los flujos hacia el red5
	 * en dos, audio y video
	 * @return
	 * true si hay que dividir los videos
	 */
	public static boolean isFlash2Streams(){
		return ((System.getenv(flash2streams)!=null));	
	}

	/**
	 * Obtiene el parametro que indica si se utiliza h264
	 * @return
	 * true si hay que usarlo
	 */
	public static boolean useH264(){
		return ((System.getenv(flashuseH264)!=null));	
	}

	/**
	 * Obtiene el parametro que indica si se utiliza mp3
	 * @return
	 * true si hay que usarlo
	 */
	public static boolean useMp3(){
		return ((System.getenv(flashuseMp3)!=null));	
	}
	
	/**
	 * Obtiene el parametro que indica si se habilita la igw
	 * @return
	 * true si hay que usarlo
	 */
	public static boolean enableIGW(){
		return ((System.getenv(iGWEnable)!=null));	
	}
	
	/**
	 * Obtiene el parametro que indica si se habilita la igw BUENA
	 * @return
	 * true si hay que usarlo
	 */
	public static boolean enableIGWB(){
		return ((System.getenv(iGWBEnable)!=null));	
	}
	
	/**
	 * Obtiene el parametro que indica si se deshabilita el flash
	 * @return
	 * true si hay que usarlo
	 */
	public static boolean disableFlash(){
		return ((System.getenv(flashDisable)!=null));	
	}
	
	
	/**
	 * Devuelve la URL del servidor web para el igw
	 * @return
	 * La url del servidor web, localhost por defecto
	 */
	public static String getIGWBaseURL(){		
		String url = System.getenv(iGWBaseURL);
		if (url==null){
			url = defaultiGWBaseURL; 
		}
		return url;
	}

    /**
      * Devuelve el numero de la pantalla (display) a capturar
      * @return
      * Numero de la pantalla de capturar, :33 por defecto
      */
    public static String getIGWCaptureDisplay(){
    	String display = System.getenv(iGWCaptureDisplay);
        if (display==null){
           display = defaultiGWCaptureDisplay;
        }
        return display;
     }
	
    /**
     * Devuelve la direccion donde se esperan los paquetes de audio para el igw
     * @return
     * La direccion del donde se espera el audio, "0.0.0.0" por defecto
     */
    public static String getIGWAudioCaptureAddress(){
        String addr = System.getenv(iGWAudioCaptureAddress);
        if (addr==null){
            addr = defaultiGWAudioCaptureAddress;
        }
        return addr;
    }

    /**
     * Devuelve el puerto donde se esperan los paquetes de audio para el igw
     * @return
	 * El puerto donde se espera el audio, 5004 por defecto
     */
    public static int getIGWAudioCapturePort() {
        int port = defaultiGWAudioCapturePort;
        try {
                port = Integer.parseInt(System.getenv(iGWAudioCapturePort));
        }catch (Exception e) {
        }
        return port;
    }

    /**
     * Devuelve el bitrate deseado para el flujo de audio del igw
     * @return
     * El bitrate deseado para el audio, 32 por defecto
     */
    public static int getIGWAudioBitRate() {
        int abr = defaultiGWAudioBitRate;
        try {
            abr = Integer.parseInt(System.getenv(iGWAudioBitRate));
        }catch (Exception e) {
        }
        return abr;
    }

    /**
     * Devuelve el bitrate deseado para el flujo de video del igw
     * @return
     * El bitrate deseado para el video, 300 por defecto
     */
    public static int getIGWVideoBitRate() {
        int vbr = defaultiGWVideoBitRate;
        try {
                vbr = Integer.parseInt(System.getenv(iGWVideoBitRate));
        }catch (Exception e) {
        }
        return vbr;
    }


    /**
     * Devuelve el prefijo para los ficheros de streaming
     * @return
     * El prefijo para los ficheros de streaming, "igwstream" por defecto
     */
    public static String getIGWPrefix(){
        String prefix = System.getenv(iGWPrefix);
        if (prefix==null){
            prefix = defaultiGWPrefix;
        }
        return prefix;
    }

    /**
     * Devuelve el directorio donde se dejaran los ficheros de streaming
     * @return
     * El directorio donde se dejaran los ficheros, "/var/www/live/" por defecto
     */
    public static String getIGWOutputDir(){
        String dir = System.getenv(iGWOutputDir);
        if (dir==null){
            dir = defaultiGWOutputDir;
        } else{
        	if (!dir.endsWith("/"))
        		dir = dir.concat("/");
        }
        return dir;
    }

	/**
	 * Devuelve el objetivo del BitRate de los clientes Flash
	 * @return
	 */
	public static int getFlashClientBitRate(){
		int bitrate = defaultClientBitRate;
		try {
			bitrate = Integer.parseInt(System.getenv(flashClientBitRate));
		} catch (Exception e) {
		}
		return bitrate;
	}
	
	/**
	 * Devuelve el objetivo del Bitrate hacia el Red5
	 * @return
	 */
	public static int getFlashBitRate (){
		int bitrate = defaultBitRate;
		try {
			bitrate = Integer.parseInt(System.getenv(flashBitRate));
		} catch (Exception e) {
		}		
		return bitrate;		
	}
	
	/**
	 * Devuelve el objetivo del Bitrate de audio hacia el Red5
	 * @return
	 */
	public static int getFlashAudioBitRate (){
		int bitrate = defaultAudioBitRate;
		try {
			bitrate = Integer.parseInt(System.getenv(flashAudioBitRate));
		} catch (Exception e) {
		}
		return bitrate;		
	}
	
	/**
	 * Devuelve la URL del servidor Flash
	 * @return
	 * La url del servidor flash, localhost por defecto
	 */
	public static String getFlashServerURL(){		
		String url = System.getenv(flashServerURL);
		if (url==null){
			url = defaultServerURL; 
		}
		return url;
	}
	
	/**
	 * Devuelve la URL a la que se deben enviar los metadatos
	 * @return
	 * la url o null si no esta configurada
	 */
	public static String getFlashMetadataURL(){
		String url = System.getenv(flashMetadataURL);
		return url;
	}
	/**
	 * Devuelve la Id de sesion de isabel necesaria para los
	 * metadatos
	 * @return
	 * la id o null si no existe
	 */
	public static String getFlashSessionID(){
		String id = System.getenv(isabelSessionID);
		return id;
	}
}
