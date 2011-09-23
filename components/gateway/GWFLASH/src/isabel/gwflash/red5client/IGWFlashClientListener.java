package isabel.gwflash.red5client;

public interface IGWFlashClientListener {
	public void onFlashClientConnect(String Username);
	public void onFlashClientDisconnect(String Username);
	public void onFlashClientQuestion(String Username);
}
