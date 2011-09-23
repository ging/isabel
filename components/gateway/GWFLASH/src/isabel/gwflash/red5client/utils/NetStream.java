package isabel.gwflash.red5client.utils;
import isabel.gwflash.red5client.oldred5.FLV;
import isabel.gwflash.red5client.oldred5.FLVReader;

import java.io.File;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.HashMap;


import org.red5.io.ITag;

import org.red5.io.ITagWriter;
import org.red5.io.flv.impl.Tag;
import org.red5.server.net.rtmp.event.AudioData;
import org.red5.server.net.rtmp.event.VideoData;


public class NetStream extends Thread implements CallResultHandler{

	private NetConnection connection;

	public enum Status { STOPPED, WAITING, PLAYING, PUBLISHING }

	private Status status;

	private Status action;

	private int streamId;

	private String streamName;

	private FLV flv;

	private ITagWriter flvWriter;

	private int videoTimeStamp;

	private int previousTagSize;

	private int audioTimeStamp;

	private OutputStream os;

	private InputStream is;

	private FLVReader flvReader = null;
	
	private boolean restart;

	public NetStream(NetConnection connection) {
//		this.connection = connection;
//		status = Status.STOPPED;
		setNetConnection(connection);
	}
	
	public void setNetConnection(NetConnection connection){
		this.connection = connection;
		status = Status.STOPPED;
		action = null;
		restart = true;
	}

	/**
	 * Inicia la ejecuci�n del stream.
	 * @param streamName
	 */
	public void play(String streamName, OutputStream os) {
		this.os = os;
		if (status.equals(Status.PLAYING) || status.equals(Status.WAITING)
				|| status.equals(Status.PUBLISHING))
			return;

		// Primero creamos un nuevo identificador de stream.

		this.streamName = streamName;
		int callId = connection.call("createStream");
		this.status = Status.WAITING;
		this.action = Status.PLAYING;
		connection.addCallResultHandler(""+callId,this);

	}
	
	public void stopStream(){
		if (!status.equals(Status.PLAYING)){
			return;
		}
		connection.streamClose(this, streamId, streamName);
		this.close();
	}

	/**
	 *
	 */
	public void publish(String streamName, InputStream is) {
		System.out.println("PUBLISH status " + status  + "action " + action);
		this.is = is;
		if (status.equals(Status.PLAYING) || status.equals(Status.WAITING)
				|| status.equals(Status.PUBLISHING))
			return;

		this.streamName = streamName;
		int callId = connection.call("createStream");
		this.status = Status.WAITING;
		this.action = Status.PUBLISHING;
		connection.addCallResultHandler(""+callId,this);

	}


	/**
	 * {@inheritDoc}
	 */
	public void resultHandler(Object[] args) {
		if (status.equals(Status.WAITING)) {
			if (action.equals(Status.PLAYING)) {
				// Obtenemos nuesto Identificador.
				streamId = (Integer)args[0];
				this.setupWriteFiles();
				connection.streamPlayCall(this, streamId, streamName);

			} else if (action.equals(Status.PUBLISHING)) {
				streamId = (Integer)args[0];
				status = Status.PUBLISHING;
				this.setupReadFiles();
				connection.streamPublishCall(this, streamId, streamName);
			}
		}
	}

	public void videoBytesReceived(VideoData video) {
		try {
			videoTimeStamp  += video.getTimestamp();
			Tag itag = new Tag(video.getDataType(),videoTimeStamp,video.getData().limit(),video.getData(),previousTagSize);
			this.previousTagSize = video.getData().limit();
			flvWriter.writeTag(itag );
		} catch (Exception e) {
			System.out.println("FLVWRITER Video tag FAIL");
			e.printStackTrace();
		}
	}

	public void audioBytesReceived(AudioData audio) {
		try {
			audioTimeStamp  += audio.getTimestamp();
			Tag itag = new Tag(audio.getDataType(),audioTimeStamp,audio.getData().limit(),audio.getData(),previousTagSize);
			this.previousTagSize = audio.getData().limit();

			
			
			flvWriter.writeTag(itag );
		} catch (Exception e) {
			System.out.println("FLVWRITER Audio tag FAIL");
			e.printStackTrace();
		}
	}

	private void setupWriteFiles() {
		try {
			flv = new FLV(new File(streamName), true);
			flvWriter = ((FLV)flv).getWriter(os);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private void setupReadFiles() {
		try {
			flv = new FLV(new File(streamName), true);
//			FileInputStream fin = new FileInputStream(new File(streamName));
			if(flvReader != null){
				flvReader.close();
			}
			flvReader = (FLVReader) ((FLV)flv).getReader(is);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void close() {
//		flvWriter.close();
		this.status = Status.STOPPED;
	}

	public Status getStatus() {

		
		
		return this.status;
	}

	public void onStatus(HashMap<String, String> arg) {
		String code = arg.get("code");
//		System.out.println(code);
		if (code.equals("NetStream.Play.Start")) {
//			System.out.println("Comienza a ejecutar.");
			status = Status.PLAYING;
		} else if (code.equals("NetStream.Play.Stop")) {
//			System.out.println("Termina la ejecucion.");
			this.close();
		} else if (code.equals("NetStream.Play.UnpublishNotify")) {
//			System.out.println("Termina la ejecucion.");			
//			this.close(); // NO CLOSE, lo cierro siempre cuando me avisa isabel
		} else if(code.equals("NetStream.Publish.Start")){
			System.out.println("Empezamos a publicar");
			status = Status.PUBLISHING;
			if (!this.isAlive())
				this.start();
		}

	}

	@Override
	public void run() {
		ITag tag = null;		

		while(true){
			if(status!= Status.PUBLISHING){
//				System.out.println("NOT PUBLISHING");
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				continue;
			}			
			tag = flvReader.readTag();
			connection.sendPublishTag(tag, streamId, streamName);
		}
		
	}

}
