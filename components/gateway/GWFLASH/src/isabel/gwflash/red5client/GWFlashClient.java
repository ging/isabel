package isabel.gwflash.red5client;

import isabel.gwflash.red5client.utils.NetConnection;
import isabel.gwflash.red5client.utils.NetStatusEvent;
import isabel.gwflash.red5client.utils.NetStatusEventHandler;

import java.net.MalformedURLException;
import java.util.Map;

import java.util.logging.Logger;

public class GWFlashClient implements NetStatusEventHandler {
	
	private boolean connected = false; 	
	private NetConnection connection;

	private Logger log = Logger.getLogger("isabel.gwflash.GWFlashClient");
	
	public GWFlashClient(String server, int port, Map<String,Object> connectionParams) throws MalformedURLException{
		log.info("RTMPClient(String server, int port, Map<String,Object> connectionParams) Constructor");		
		connection = new NetConnection();
		//connection.client = this;
		connection.connect(server, port, connectionParams);		
		waitForConnection();
	}
	
	public GWFlashClient (String url) throws MalformedURLException {
		log.info("RTMPClient (String url) Constructor");		
		connection = new NetConnection();
		connection.addNetStatusEventHandler(this);
		connection.client = this;
		connection.connect(url);
		waitForConnection();
	
	}
	
	public GWFlashClient (String url, Object[] args) throws MalformedURLException {
		log.info("RTMPClient (String url) Constructor");		
		connection = new NetConnection();
		connection.addNetStatusEventHandler(this);
		connection.client = this;
		connection.connect(url, args);
		waitForConnection();
	}
	
	private void waitForConnection() throws MalformedURLException {
		int i = 0;
		while(!connected){			
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			if (i++ == 100) throw new MalformedURLException("Unable to connect");
		}
	}
	
	public boolean isConnected(){
		return connected;
	}
	
	public NetConnection getNetConnection(){
		return connection;
	}

	public void newNetStatusEvent(NetStatusEvent event) {
		System.out.println("event " + event.getCode());
		
		switch (event.getCode()) {
		
		case NetStatusEvent.NETCONNECTION_CONNECT_SUCCESS:
			log.info("Ya estamos conectados");
			connected = true;
			break;
		case NetStatusEvent.NETCONNECTION_CONNECT_FAILED:
			log.info("La conexion ha fallado");
			break;
		case NetStatusEvent.NETCONNECTION_CONNECT_REJECTED:
			log.info("La conexion no ha sido aceptada");
			break;
		case NetStatusEvent.NETCONNECTION_CONNECT_CLOSED:
			log.info("La conexion ha terminado");
			connected=false;			
			break;
		default:
			break;
		}
		
	}
}
