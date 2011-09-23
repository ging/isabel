package isabel.igateway;

import isabel.gwflash.FlashGatewayEnvConfig;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.OutputStream;

public class Segmenter extends Thread{
	
	//Ejecutable del segmenter
	private static final String SEG_EXECUTABLE="/usr/local/segmenter/segmenter";
	//Numero de segmentos que van rotando
	private static final int SEG_NUMBER = 10;
	
	private Process p;
	private BufferedOutputStream out;
	private BufferedInputStream err, std;
	
	public Segmenter(){
		String command = SEG_EXECUTABLE + " pipe: 5 " + FlashGatewayEnvConfig.getIGWPrefix() 
				+ " " + FlashGatewayEnvConfig.getIGWPrefix() + ".m3u8 " + FlashGatewayEnvConfig.getIGWBaseURL()+ " " + SEG_NUMBER;

		System.out.println("STARTING SEGMENTER Params: " + command);
		try {
			p = Runtime.getRuntime().exec(command, null, new File(FlashGatewayEnvConfig.getIGWOutputDir()));
			
			out = new BufferedOutputStream(p.getOutputStream());
			
			std = new BufferedInputStream(p.getInputStream());
			err = new BufferedInputStream(p.getErrorStream());
			this.start();
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
	}
	
	public OutputStream getOutput(){
		return out;
	}
	public void run(){
		int result = 0;
		byte[] buf = new byte[5000];		
		while(result >= 0){
			try {
				
				if (std.available()>0){
					result = std.read(buf);			
					System.out.println(new String(buf, 0, result));
				}
				
				if (err.available()>0){
					result = err.read(buf);
					System.out.println(new String(buf, 0, result));
				}
				Thread.sleep(500);
				
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				result=-1;
			}			
		}
	}
	
	public static void main (String[] args){
		new Segmenter();
	}
}
