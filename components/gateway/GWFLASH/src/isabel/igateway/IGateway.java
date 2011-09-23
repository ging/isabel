package isabel.igateway;

import java.io.IOException;

import isabel.gwflash.FlashGatewayEnvConfig;
import isabel.lib.tasks.Task;

public class IGateway {

	public final static int PT = 103;
	public final static int ID = 10;
	public final static int PORT = FlashGatewayEnvConfig.getIGWAudioCapturePort();
	public final static String IP = "127.0.0.8";
	private static final int VBITRATE = FlashGatewayEnvConfig.getIGWVideoBitRate();
	private static final int ABITRATE = FlashGatewayEnvConfig.getIGWAudioBitRate();
	private static final String C_DISPLAY = FlashGatewayEnvConfig.getIGWCaptureDisplay();
	private static final String C_IP = FlashGatewayEnvConfig.getIGWAudioCaptureAddress();
	private static final String PREFIX = FlashGatewayEnvConfig.getIGWPrefix();
	private static final String BASE_URL = FlashGatewayEnvConfig.getIGWBaseURL();
	private static final String OUT_PATH = FlashGatewayEnvConfig.getIGWOutputDir();
	
	
	public void go() throws IOException {
		System.out.println("Stating iGateway");
		String ISABEL_DIR = System.getProperty("isabel.dir");
		if (ISABEL_DIR == null)
			ISABEL_DIR = "/usr/local/isabel";
		String cmd = ISABEL_DIR + "/bin/httpliveencoding.sh "+
				        "start "+
				        C_DISPLAY + " "+
				        Integer.toString(VBITRATE)+" "+
				        C_IP+" "+
				        Integer.toString(PORT)+" "+
				        Integer.toString(ABITRATE)+" "+
				        PREFIX+" "+
				        BASE_URL + " " +
				        OUT_PATH;
		String[] cmd2 = {"sh", "-c", cmd}; 
				        
		new Task(cmd2, false);
	}
}
