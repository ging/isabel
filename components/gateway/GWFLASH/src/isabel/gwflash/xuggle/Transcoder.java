package isabel.gwflash.xuggle;

import java.io.OutputStream;

public interface Transcoder {
	
	public void go(OutputStream o);
	public void keyboardEvent(String type, int keyCode, int keyChar);	
	public void mouseEvent(String type, int x, int y);
}
