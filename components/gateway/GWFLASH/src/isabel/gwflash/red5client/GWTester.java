package isabel.gwflash.red5client;

import isabel.gwflash.FlashManager;

public class GWTester {

	/**
	 * @param args
	 */
	private FlashManager fm = null;
	
	public GWTester(){
		
		fm = new FlashManager(new String[]{"QUESTION_REQUEST"});
						
	}
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		new GWTester();

	}
}
