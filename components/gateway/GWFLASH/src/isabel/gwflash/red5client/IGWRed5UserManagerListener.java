package isabel.gwflash.red5client;

public interface IGWRed5UserManagerListener {
	public void onFlashClientConnect(String Username, String audio, String video);
	public void onFlashClientDisconnect(String Username);
	public void onFlashClientButtonPressed(String buttonName, String Username);
	public void onFlashClientSetParameter(String userName, String name, Object value);
}
