package isabel.gwflash.red5client.utils;
import isabel.gwflash.red5client.oldred5.InvokeHandler;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Map;


import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


import org.apache.mina.common.ByteBuffer;
import org.red5.annotations.DeclarePrivate;
import org.red5.annotations.DeclareProtected;
import org.red5.io.ITag;
import org.red5.io.amf.AMF;
import org.red5.io.amf.Input;
import org.red5.io.amf.Output;
import org.red5.io.amf3.ByteArray;
import org.red5.io.flv.impl.Tag;
import org.red5.io.flv.meta.MetaData;
import org.red5.io.object.DataTypes;
import org.red5.io.object.Deserializer;
import org.red5.io.object.Serializer;
import org.red5.server.api.service.IPendingServiceCall;
import org.red5.server.exception.ClientDetailsException;
import org.red5.server.net.protocol.ProtocolState;
import org.red5.server.net.rtmp.Channel;
import org.red5.server.net.rtmp.DeferredResult;
import org.red5.server.net.rtmp.RTMPClient;
import org.red5.server.net.rtmp.RTMPConnection;
import org.red5.server.net.rtmp.codec.RTMP;
import org.red5.server.net.rtmp.event.AudioData;
import org.red5.server.net.rtmp.event.IRTMPEvent;
import org.red5.server.net.rtmp.event.Invoke;
import org.red5.server.net.rtmp.event.Notify;
import org.red5.server.net.rtmp.event.Ping;
import org.red5.server.net.rtmp.event.VideoData;
import org.red5.server.net.rtmp.message.Header;
import org.red5.server.net.rtmp.message.Packet;
import org.red5.server.service.Call;
import org.red5.server.service.NotAllowedException;
import org.red5.server.service.PendingCall;
import org.red5.server.service.ServiceUtils;

/**
 * Sample class that uses the client mode of the RTMP library
 * to connect to the "oflaDemo" application on the current server.
 *
 * @see http://mirror1.cvsdude.com/trac/osflash/red5/ticket/94
 * @author ce@publishing-etc.de
 */
public class NetConnection extends RTMPClient {
	public int status = 0;
	private RTMPConnection conn;
	//private int stream;


	private String hostname;
	private String appName;
	private int port;

	private long lastPingSent;

	// Estados de la conexin.
	public final static int NOT_CONNECTED = 0;
	public final static int CONNECTING = 1;
	public final static int CONNECTED = 2;
	public final static int CREATINGSTREAM = 21;
	public final static int STREAMCREATED = 22;
	public final static int CLOSING = 3;
	public final static int CLOSED = 4;

	private static final Logger log = LoggerFactory.getLogger( NetConnection.class );

    private Hashtable<String, InvokeHandler> invokeHandlers= new Hashtable<String,InvokeHandler>();
    private int invokeId = 1;

    private boolean flagg = true;
    
    private static boolean factAlreadySet = false;
    
    public Object client = this;
    //private MetaData metadata;


    // Encargado de enviar y recibir los bytes de RTMP.

	// Lista de manejadores de eventos de estado.
	private ArrayList<NetStatusEventHandler> netStatusHandlers = new ArrayList<NetStatusEventHandler>();
	private Hashtable<String,CallResultHandler> callResultHandlers = new Hashtable<String,CallResultHandler>();

    /**
     * Inicia un cliente RTMP hacia el servidor indicado en hostname. Para ello una vez
     * que invoquemos al mtodo connect() se conectar al puerto indicado en port y a la
     * aplicacin facilitada en applicationName.
     *
     * @param hostname
     * @param port
     * @param applicationName
     */
    public NetConnection() {

    	super();

    }

    /**
     * Conecta con el servidor, y le enva los argumentos que le pasamos.
     *
     * @param url   URL a la que nos conectamos
     * @param args
     */
    public void connect(String url, Object[] args) throws MalformedURLException{
    	// Primero comprobamos que el URL est bien formado.
    	if (!factAlreadySet){
    		URL.setURLStreamHandlerFactory(new RTMPHandlerFactory());
    		factAlreadySet = true;
    	}
 		
		URL urlConnect = new URL(url);
		// Guardamos los valores obtenidos.
		this.appName = urlConnect.getPath().substring(1);
		this.hostname = urlConnect.getHost();
		if (urlConnect.getPort() == -1)
			this.port = urlConnect.getDefaultPort();
		else this.port = urlConnect.getPort();

		this.connectArguments = args;
		log.info("Connecting to server: " +this.hostname + ":" + this.port);

		// Conector que utilizamos para la sesin RTMP.
//		connector = new SocketConnector();
//		connector.connect(new InetSocketAddress(this.hostname,this.port), ioHandler);
		//super.connect(this.hostname,this.port,this.appName);
		
		try{
			super.connect(this.hostname, this.port, super.makeDefaultConnectionParams(this.hostname, this.port, this.appName), null, this.connectArguments);
		}catch(Exception e){
			e.printStackTrace();
			throw new RuntimeException("Unable to connect, URL does not exist: " + e.getMessage());
		}
    }

    /**
     * Conecta con el servidor, sin enviarle argumentos.
     *
	 * @param url   URL a la que nos conectamos
     */
    public void connect(String url) throws MalformedURLException {
    	this.connect(url, null);
    }

	/**
	 * {@inheritDoc}
	 */
    public void connectionOpened(RTMPConnection conn, RTMP state) {
		super.connectionOpened(conn, state);
		this.conn = conn;
	}

	/**
	 * {@inheritDoc}
	 */
    @Override
	public void messageReceived(RTMPConnection conn, ProtocolState state, Object message) throws Exception {
    	// Si no estamos conectados no haremos caso a ningn mensaje que no sea el de xito en la conexin.
//		log.debug("Mensage Recibido: " + state.toString());
    	if (status < NetConnection.CONNECTED) {
			if(message instanceof Packet) {
				Packet p=(Packet)message;
				log.info(p.getMessage().toString());
				if (p.getMessage() instanceof Invoke) {
					Invoke inv = (Invoke) p.getMessage();
					if (inv.getCall() instanceof PendingCall) {

						PendingCall call = (PendingCall) inv.getCall();
						Object[] arguments = call.getArguments();
						if (arguments.length ==0){
						}
						else if (arguments[0] instanceof HashMap) {
							@SuppressWarnings("unchecked")
							HashMap<String,String> arg = (HashMap<String,String>)arguments[0];
							log.info(arg.get("code"));
							if (arg.get("code").equals("NetConnection.Connect.Success")) {
								this.status = NetConnection.CONNECTED;

								// Avisamos de que la conexin ha tenido xito.
								NetStatusEvent event = new NetStatusEvent(NetStatusEvent.NETCONNECTION_CONNECT_SUCCESS);
								this.dispatchNetStatusEvent(event);
							}

							else if (arg.get("code").equals("NetConnection.Connect.Failed")) {
								// Avisamos de que la conexin ha fallado.
								NetStatusEvent event = new NetStatusEvent(NetStatusEvent.NETCONNECTION_CONNECT_FAILED);
								this.dispatchNetStatusEvent(event);
							}

							else if (arg.get("code").equals("NetConnection.Connect.Rejected")) {
								// Avisamos de que la conexin no ha sido aceptada.
								NetStatusEvent event = new NetStatusEvent(NetStatusEvent.NETCONNECTION_CONNECT_FAILED);
								this.dispatchNetStatusEvent(event);
							}

						}
					}
				} else return;
			}
			// Si an no estamos conectados pero no hemos recibido ningn mensaje de xito o fracaso
			// en la conexin no lo aceptamos.
			else {
				return;
			}
		}
		else if (status == NetConnection.CONNECTED) {
			if(message instanceof Packet) {
				Packet packet = (Packet)message;
				if (packet.getMessage() instanceof Invoke) {
					Invoke inv = (Invoke) packet.getMessage();

				}

				if (packet.getMessage() instanceof VideoData){
					VideoData video = (VideoData) packet.getMessage();
					netStreams.get("stream"+packet.getHeader().getStreamId()).videoBytesReceived(video);
				}
				else if (packet.getMessage() instanceof AudioData) {
					AudioData audio = (AudioData) packet.getMessage();

					netStreams.get("stream"+packet.getHeader().getStreamId()).audioBytesReceived(audio);

				} else if(packet.getMessage() instanceof Invoke) {
				
					Invoke inv = (Invoke) packet.getMessage();
					if (inv.getCall() instanceof PendingCall) {
						PendingCall call = (PendingCall) inv.getCall();
						Object[] arguments = call.getArguments();
						if (arguments.length > 0  && arguments[0] instanceof HashMap && call.getServiceMethodName().equals("onStatus")) {
							@SuppressWarnings("unchecked")
							HashMap<String, String> hashMap = (HashMap<String,String>)arguments[0];
							HashMap<String,String> arg = hashMap;							
								netStreams.get("stream"+packet.getHeader().getStreamId()).onStatus(arg);
							
						} else if (call.getServiceMethodName().equals("_result")) {
							this.dispatchCallResult(inv.getInvokeId() + "", call.getArguments());
						} else {
							Object[] methodResult = ServiceUtils.findMethodWithExactParameters(client, call.getServiceMethodName(), call.getArguments());
							Object result = null;
			                Method method = (Method) methodResult[0];
			                Object[] params = (Object[]) methodResult[1];
			                try {
		                        if (method.isAnnotationPresent(DeclarePrivate.class)) {
		                                // Method may not be called by clients.
		                                log.debug("Method {} is declared private.", method);
		                                throw new NotAllowedException("you are not allowed to execute this method");
		                        }
		                        
		                        final DeclareProtected annotation = method.getAnnotation(DeclareProtected.class);
		                        if (annotation != null) {
		                                if (!conn.getClient().hasPermission(conn, annotation.permission())) {
		                                        // Client doesn't have required permission
		                                        log.debug("Client {} doesn't have required permission {} to call {}", new Object[]{conn.getClient(), annotation.permission(), method});
		                                        throw new NotAllowedException("you are not allowed to execute this method");
		                                }
		                        }
		                        log.debug("Invoking method: {}", method.toString());

		                        if (method.getReturnType() == Void.class) {
		                                method.invoke(client, params);
		                                call.setStatus(Call.STATUS_SUCCESS_VOID);
		                        } else {
		                                result = method.invoke(client, params);
		                                log.debug("result: {}", result);
		                                call.setStatus(result == null ? Call.STATUS_SUCCESS_NULL
		                                                : Call.STATUS_SUCCESS_RESULT);
		                        }
		                        if (call instanceof IPendingServiceCall) {
		                                ((IPendingServiceCall) call).setResult(result);
		                        }
		                } catch (NotAllowedException e) {
		                        call.setException(e);
		                        call.setStatus(Call.STATUS_ACCESS_DENIED);
		                } catch (IllegalAccessException accessEx) {
		                        call.setException(accessEx);
		                        call.setStatus(Call.STATUS_ACCESS_DENIED);
		                        log.error("Error executing call: {}", call);
		                        log.error("Service invocation error", accessEx);
		                } catch (InvocationTargetException invocationEx) {
		                        call.setException(invocationEx);
		                        call.setStatus(Call.STATUS_INVOCATION_EXCEPTION);
		                        if (!(invocationEx.getCause() instanceof ClientDetailsException)) {
		                                // Only log if not handled by client
		                                log.error("Error executing call: {}", call);
		                                log.error("Service invocation error", invocationEx);
		                        }
		                } catch (Exception ex) {
		                        call.setException(ex);
		                        call.setStatus(Call.STATUS_GENERAL_EXCEPTION);
		                        log.error("Error executing call: {}", call);
		                        log.error("Service invocation error", ex);
		                }
                        IPendingServiceCall psc = (IPendingServiceCall) call;
                        Object result2 = psc.getResult();
                        if (result2 instanceof DeferredResult) {
                        } else {
                                Invoke reply = new Invoke();
                                reply.setCall(call);
                                reply.setInvokeId(inv.getInvokeId());
                                conn.getChannel(packet.getHeader().getChannelId()).write(reply);
                        }

						}
					}


				} else if (packet.getMessage() instanceof Notify && flagg) {
					flagg = false;
					Notify not = (Notify) packet.getMessage();
					if (not.getDataType()==Notify.TYPE_STREAM_METADATA) {
						// En este momento leeramos los metadatos.

						/*MetaService meta = new MetaService();
						meta.readMetaCue();
						metadata = meta.readMetaData(not.getData());*/
					}
				}
				if (packet.getMessage() instanceof Ping){

					// Devolvemos el Ping.
					long newPingTime = System.currentTimeMillis();
					Ping pingRequest = new Ping();
					pingRequest.setValue1((short) Ping.PONG_SERVER);
					lastPingSent = newPingTime;
					int now = (int) (lastPingSent & 0xffffffff);
					pingRequest.setValue2(now);
					pingRequest.setValue3(Ping.UNDEFINED);
					conn.ping(pingRequest);
				}

			}

		}
	}

    public void invoke(Invoke invoke, InvokeHandler handler) {
    	invoke.setInvokeId(invokeId);
    	Channel channel = conn.getChannel((byte)3);
    	invokeHandlers.put(""+invokeId, handler);
    	invokeId++;
    	channel.write(invoke);
    }

    public int getNextInvokeId() {
    	return invokeId;
    }

    private Hashtable<String,NetStream> netStreams = new Hashtable<String, NetStream>();

    public void streamPlayCall(NetStream netStream, int stream, String streamName) {
    	int chann = 4 + (stream - 1) + 4;
    	netStreams.put("stream"+stream, netStream);
    	Object[] args = new Object[1];
    	args[0] = streamName;
    	PendingCall pendingCall=new PendingCall("play");
		pendingCall.setArguments(args);

		Invoke invoke=new Invoke(pendingCall);
		this.callResultHandlers.put(""+invokeId, netStream);
		invoke.setInvokeId(invokeId++);
		IRTMPEvent event = invoke;
		final Header header = new Header();
		final Packet packet = new Packet(header, event);

		header.setChannelId(chann);
		header.setTimer(event.getTimestamp());
		header.setStreamId(stream);
		header.setDataType(event.getDataType());
		if (event.getHeader() != null) {
			header.setTimerRelative(event.getHeader().isTimerRelative());
		}

		conn.write(packet);
    }
    
    public void streamClose(NetStream netStream, int stream, String streamName){
    	int chann = 4 + (stream - 1) + 4;
    	//Object[] args = new Object[0];
    	//args[0] = streamName;
    	PendingCall pendingCall = new PendingCall("closeStream");
    	//pendingCall.setArguments(args);
    	Invoke invoke=new Invoke(pendingCall);
    	
		this.callResultHandlers.put(""+invokeId, netStream);
		invoke.setInvokeId(invokeId++);
		IRTMPEvent event = invoke;
		final Header header = new Header();
		final Packet packet = new Packet(header, event);

		header.setChannelId(chann);
		header.setTimer(event.getTimestamp());
		header.setStreamId(stream);
		header.setDataType(event.getDataType());
		if (event.getHeader() != null) {
			header.setTimerRelative(event.getHeader().isTimerRelative());
		}

		conn.write(packet);
    	
    }

    public void streamPublishCall(NetStream netStream, int stream, String streamName) {
    	int chann = 4 + (stream - 1) + 4;
    	netStreams.put("stream"+stream, netStream);
    	Object[] args = new Object[2];
    	args[0] = streamName;
    	args[1] = "live";    	
    	PendingCall pendingCall=new PendingCall("publish");
		pendingCall.setArguments(args);

		Invoke invoke=new Invoke(pendingCall);
		this.callResultHandlers.put(""+invokeId, netStream);
		invoke.setInvokeId(invokeId++);
		IRTMPEvent event = invoke;
		final Header header = new Header();
		final Packet packet = new Packet(header, event);

		header.setChannelId(chann);
		header.setTimer(event.getTimestamp());
		header.setStreamId(stream);
		header.setDataType(event.getDataType());		
		if (event.getHeader() != null) {
			header.setTimerRelative(event.getHeader().isTimerRelative());
		}

		conn.write(packet);
    }
    
    @SuppressWarnings("unchecked")
	public void sendPublishTag (ITag tag, int stream, String streamName){
    	int chann = 4 + (stream - 1) + 4;    	

		final Header header = new Header();		
    	final Packet packet = new Packet(header);
    	
    	header.setChannelId(chann);
    	header.setStreamId(stream);    	
    	header.setDataType(tag.getDataType());
    	header.setTimer(tag.getTimestamp()); 
    	header.setTimerRelative(false);
    	
		if(tag.getDataType() == Tag.TYPE_AUDIO){
			AudioData data = new AudioData(tag.getBody());
			data.setTimestamp(tag.getTimestamp());
	    	packet.setMessage(data);

		}else if (tag.getDataType() == Tag.TYPE_VIDEO){
			VideoData data = new VideoData(tag.getBody());
			data.setTimestamp(tag.getTimestamp());
	    	packet.setMessage(data);
		}else if (tag.getDataType() == Tag.TYPE_METADATA){
			System.out.println("Metadata lo mando con un par");
	        
            ByteBuffer buf = ByteBuffer.allocate(1024);
            buf.setAutoExpand(true);
            Output out = new Output(buf);
            out.writeString("@setDataFrame");
            Deserializer des = new Deserializer();
            Input in = new Input(tag.getBody());
            String type = (String)des.deserialize(in, null);
            Map<Object, Object> metadata = (Map<Object,Object>)des.deserialize(in, null);
            out.writeString(type);
            out.writeMap(metadata, new Serializer());
            buf.flip();
            Notify notify = new Notify(buf);	        
			packet.setMessage(notify);
            
		}
    	
	
    	conn.write(packet);

    }

	/**
	 * {@inheritDoc}
	 */
    public void messageSent(RTMPConnection conn, Object message) {
    	// TODO Llevar algn recuento de mensajes.
	}

	/**
	 * {@inheritDoc}
	 */
    public void connectionClosed(RTMPConnection conn, RTMP state) {
    	// Avisamos a los manejadores de eventos.
    	NetStatusEvent event = new NetStatusEvent(NetStatusEvent.NETCONNECTION_CONNECT_CLOSED);
    	this.dispatchNetStatusEvent(event);
	}

    public void addNetStatusEventHandler(NetStatusEventHandler handler) {
		this.netStatusHandlers.add(handler);
	}

    public boolean removeNetStatusEventHandler(NetStatusEventHandler handler) {
    	return this.netStatusHandlers.remove(handler);
    }

    public void dispatchNetStatusEvent(NetStatusEvent event) {
    	for (int i = 0; i < this.netStatusHandlers.size(); i++) {
    		this.netStatusHandlers.get(i).newNetStatusEvent(event);
    	}
    }

    public void addCallResultHandler(String id, CallResultHandler handler) {
    	this.callResultHandlers.put(id, handler);
    }

    public void dispatchCallResult(String id, Object[] args) {
    	this.callResultHandlers.remove(id).resultHandler(args);
    }

	public int call(String method) {
    	return call(method,null);
	}	
	
	public int call(String method,Object[] args) {
		Channel channel=conn.getChannel((byte)3);
		PendingCall pendingCall;
		if (args != null) pendingCall = new PendingCall(method, args);
		else pendingCall = new PendingCall(method);
    	Invoke invoke=new Invoke(pendingCall);
		invoke.setInvokeId(invokeId);
		channel.write(invoke);
		
		return invokeId++;
	}
	
	public int call(String method,Object[] args, CallResultHandler handler) {
		Channel channel=conn.getChannel((byte)3);
		PendingCall pendingCall;
		if (args != null) pendingCall = new PendingCall(method, args);
		else pendingCall = new PendingCall(method);
		addCallResultHandler(""+invokeId,handler);
    	Invoke invoke=new Invoke(pendingCall);
		invoke.setInvokeId(invokeId);
		channel.write(invoke);
		return invokeId++;
	}
	
	public void close() {
		super.disconnect();
	}

}
