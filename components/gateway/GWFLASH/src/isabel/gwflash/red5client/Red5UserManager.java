package isabel.gwflash.red5client;

import isabel.gwflash.FlashUser;
import isabel.gwflash.FlowsBool;
import isabel.gwflash.FlashGatewayEnvConfig;
import isabel.gwflash.red5client.IsabelToRed5Stream.StreamType;
import isabel.gwflash.red5client.utils.NetStatusEvent;
import isabel.gwflash.red5client.utils.NetStatusEventHandler;

import java.net.MalformedURLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Logger;

/**
 * Clase encargada de manejar las representaciones de 
 * los usuarios de red5
 * @author pedro
 *
 */
public class Red5UserManager implements NetStatusEventHandler {

	private IGWRed5UserManagerListener listener = null;
	private String red5ServerURL = null;
	private Logger log = Logger.getLogger("isabel.gwflash.Red5UserManager");
	private HashMap<String, Red5User> usersFlows;
	private GWFlashClient client;
	private Map<String, ArrayList<String>> currentFlashUserList= null;
	private	IsabelToRed5Stream videoStream = null;
	private IsabelToRed5Stream audioStream = null;
	private int reconnections = 0;
	
	/**
	 * Construye un nuevo Manager
	 * @param red5ServerURL
	 * URL para conectarse al red5
	 * @throws MalformedURLException
	 */
	public Red5UserManager(String red5ServerURL, String[] buttonNames) throws MalformedURLException{
		this.red5ServerURL = red5ServerURL;
		log.info("Starting Red5UserManager: " + red5ServerURL);
		client = new GWFlashClient(red5ServerURL, new String[]{"IsabelClient"});
		client.getNetConnection().client = this;
		client.getNetConnection().addNetStatusEventHandler(this);
		this.setButtonNames(buttonNames);

		usersFlows = new HashMap<String, Red5User>();
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		this.isabelToRed5("IsabelClient");
	}
	
	/**
	 * Crear representacion de nuevo usuario de red5
	 * los flujos se inicializan como false ambos
	 * @param userName
	 * Nombre de usuario
	 * @param SSRC
	 * ssrc con el que se enviara a isabel
	 */
	public void newUser(String userName, int SSRC){		
		log.info("New User added " + userName + " SSRC " + SSRC);
		FlowsBool flowsChapu =  new FlowsBool();
		flowsChapu.audio = false;
		flowsChapu.video = false;
		usersFlows.put(userName, new Red5User(userName, flowsChapu, SSRC, client.getNetConnection()));
	}
	
	/**
	 * Elimina a un usuario que se ha desconectado de la sesion
	 * @param userName
	 * Nombre del usuario
	 * @param reason
	 * Razon de desconexion a enviar al usuario 
	 */
	public void disconnectUser(String userName, String reason){
		log.info("Disconnecting user " + userName + " reason " + reason);
		FlowsBool flowsFalse = new FlowsBool();

		if (usersFlows.get(userName) != null) {
			log.info("Stopping " + userName + "'s streams b/c of disconnection");
		    usersFlows.get(userName).stopStreams(flowsFalse);
		}
		usersFlows.remove(userName);
		client.getNetConnection().call("rejectUser", new Object[]{userName,reason});		
	}
	
	
	public void setListener(IGWRed5UserManagerListener listener){
		this.listener= listener;
	}
	
	/**
	 * Actualiza los flujos permitidos desde Isabel,
	 * se actualizan los transcoders y se informa al Red5
	 * @param streams
	 * Mapa con la información necesaria
	 */
	public void updateStreams(HashMap<FlashUser, FlowsBool> usersMap){
		log.info("Actualizando capacidades de los clientes");
		
		if (usersMap.isEmpty()){
			log.info("No hay clientes");
			return;
		}
		
		// Mandando informacion al Red5
		HashMap<String, Boolean[]> newFlows = new HashMap<String, Boolean[]>();
				
		for (FlashUser userTemp: usersMap.keySet()){			
			FlowsBool flowsTemp = usersMap.get(userTemp);
			
			log.info("FlashUser " + userTemp.getMemberInfo().getNickName() + " audio " + flowsTemp.audio + " video " + flowsTemp.video);
			
			newFlows.put(userTemp.getMemberInfo().getNickName(), new Boolean[]{flowsTemp.audio, flowsTemp.video});	
		}
		
		client.getNetConnection().call("updateStreams", new Object[]{newFlows});
		
		//Actualizando transcoders
						
		for (Red5User red5User: usersFlows.values()){
			
			for (FlashUser flashUser: usersMap.keySet()){
				
				if (flashUser.getMemberInfo().getNickName().equals(red5User.getUsername())){
					log.info("Actualizando Flows usuario " + red5User.getUsername());
					red5User.setFlows(usersMap.get(flashUser));
				}
			}
		}	
	}
	
	private void setButtonNames (String[] buttonNames) {
		List<String> params = new ArrayList<String>();
		
		for (String s: buttonNames) {
			params.add(s);
		}
		client.getNetConnection().call("enableButtons", new Object[]{params});
	}
	
	
	/**
	 * Llamada al red5 para informar de usuarios con el desktop activo
	 * 
	 * @param users
	 */
	public void activateDesktop(List<String> users){
		log.info("Activating users Desktops : " + users);
		client.getNetConnection().call("activateDesktop", new Object[]{users});
	}
	
	public void parameterChange(String user, String name, Object value) {
		log.fine("setting parameter " + name + " for user " + user + " to value " + value);
		client.getNetConnection().call("parameterChange", new Object[]{user, name, value});
	}
	
	/**
	 * Llamado desde el Red5 con las capacidades de los usuarios
	 * se llama cada vez que cambian y/o entran o salen
	 * @param newUserList
	 */
	public void  updateFlashUserList(Map<String, ArrayList<String>> newUserList){ //Nombre de Usuario - Audio, Video (none si no hay nada)
		
		log.info(" Nueva Lista desde Red5" + newUserList.toString());
		
		if (currentFlashUserList == null){
			log.info("Creada nueva currentFlashUserList");
			currentFlashUserList = new HashMap<String, ArrayList<String>>();
		}
		if (newUserList.isEmpty()){ //Llega lista vacia
			log.info("Lista vacia ha llegado");
			for (String userTemp : currentFlashUserList.keySet()){
				listener.onFlashClientDisconnect(userTemp);
				currentFlashUserList.remove(userTemp);
				log.info("Eliminando cliente " + userTemp);
			}
		}
		
		for (String userTemp: newUserList.keySet()){ // Comprobamos si hay usuarios nuevos
			if(!currentFlashUserList.containsKey(userTemp)){
				ArrayList<String> flows = newUserList.get(userTemp);
				log.info("Nuevo cliente encontrado " + userTemp + " flows " + flows.size() + " talcual " + flows );
				currentFlashUserList.put(userTemp, newUserList.get(userTemp));				
				listener.onFlashClientConnect(userTemp,flows.get(0),flows.get(1));
			}
		}
		
		@SuppressWarnings("unchecked")
		HashMap<String, ArrayList<String>> tmpCurrentFlashUserList = 
			(HashMap<String, ArrayList<String>>)((HashMap<String, ArrayList<String>>)currentFlashUserList).clone();
		
		for (String userTemp:tmpCurrentFlashUserList.keySet()){ //Comprobamos si se ha ido algun usuario
			if(!newUserList.containsKey(userTemp)){
				currentFlashUserList.remove(userTemp);
				log.info("Usuario " + userTemp + " se va");
				listener.onFlashClientDisconnect(userTemp);
			}			
		}
		
		for (String userTemp: currentFlashUserList.keySet()){
			ArrayList<String> currentFlows = currentFlashUserList.get(userTemp);
			ArrayList<String> newFlows = newUserList.get(userTemp);
			if ((currentFlows.get(0).equals(newFlows.get(0)))&&(currentFlows.get(1).equals(newFlows.get(1))) ){ //El usuario no ha cambiado
				continue;
			}else{	
				log.info("El usuario " + userTemp + " ha cambiado, no hacemos nada");								
			}
		}
		currentFlashUserList = newUserList;		
		

	}
	
	/**
	 * Llamado desde el Red5 Cuando un usuario pulsa un boton
	 * @param buttonName
	 * @param UserName
	 */
	public void buttonPressed(String userName, String buttonName){
		log.info("Button from flash user " + userName + " : " + buttonName);
		if (listener!=null){
			listener.onFlashClientButtonPressed(buttonName, userName);		
		}
	}
	
	public void setParameter(String userName, String name, Object value) {
		log.fine("Parameter set user " + userName + " name " + name + " value " + value);
		if(listener != null)
		{
			listener.onFlashClientSetParameter(userName, name, value);
		}
	}
	
	/**
	 * Llamado desde el Red5 cuando se toca el teclado
	 * @param type press o release
	 * @param keyCode La tecla
	 * @param keyChar La tecla
	 */
	public void vncKeyboardEvent(String type, int keyCode, int keyChar) {
		videoStream.getTranscoder().keyboardEvent(type, keyCode, keyChar);
	}

	/**
	 * Llamado desde el Red5 cuando se toca el raton
	 * @param type press, release o move
	 * @param x coordenada x
	 * @param y coordenada y
	 */
	public void vncMouseEvent(String type, int x, int y) {
		videoStream.getTranscoder().mouseEvent(type, x, y);
	}
	
	/**
	 * 
	 * Inicia el flujo de video y audio hacia el red5
	 * 
	 * @param streamName
	 */
	public void isabelToRed5(String streamName){
		log.info("Publishing " + streamName);		
		
		videoStream = new IsabelToRed5Stream(streamName+"_VIDEO", client.getNetConnection(),StreamType.VIDEO );
		if (FlashGatewayEnvConfig.isFlash2Streams())
			audioStream = new IsabelToRed5Stream(streamName+"_AUDIO", client.getNetConnection(), StreamType.AUDIO);


		videoStream.startSending();
		if (FlashGatewayEnvConfig.isFlash2Streams())
			audioStream.startSending();
	}
	
	public void reconnect(){

		log .info ("Reconectando al red5. Desconectamos");
		log.info("Parando Flujos existentes");
		for (String userTemp: currentFlashUserList.keySet()){ // Comprobamos si hay usuarios nuevos				
				listener.onFlashClientDisconnect(userTemp);		
		}
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
			
		client.getNetConnection().disconnect();
		
		currentFlashUserList = null;
		
		try {			
				Thread.sleep(1000);
		} catch (InterruptedException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		log.info("Reconectando, reconexiones: " + (++reconnections));
		
		try {
			log.info("Iniciando nueva connection");
			client = new GWFlashClient(red5ServerURL, new String[]{"IsabelClient"});
			client.getNetConnection().client = this;
			client.getNetConnection().addNetStatusEventHandler(this);
			
		} catch (MalformedURLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	
		log.info("Reiniciando IsabeltoRed5");
		if(videoStream != null)
			videoStream.restart(client.getNetConnection());
		if(audioStream != null)
			audioStream.restart(client.getNetConnection());
		
	}


	@Override
	public void newNetStatusEvent(NetStatusEvent event) {
		log.info("EVENT " + event.getCode());
		if (event.getCode() == NetStatusEvent.NETCONNECTION_CONNECT_CLOSED){
			log.info("Caida Conexion con Red5");
			reconnect();
		}
	}
	

	
}
