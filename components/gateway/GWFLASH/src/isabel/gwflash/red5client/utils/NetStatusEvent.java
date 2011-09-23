package isabel.gwflash.red5client.utils;

public class NetStatusEvent {

	public static final int NETCONNECTION_CONNECT_SUCCESS = 1;
	public static final int NETCONNECTION_CONNECT_FAILED = 2;
	public static final int NETCONNECTION_CONNECT_REJECTED = 3;
	public static final int NETCONNECTION_CONNECT_CLOSED = 4;

	/**
	 * C�digo del evento.
	 */
	private int code;

	/**
	 * Crea un nuevo evento del tipo indicado por el par�metro type.
	 * @param type
	 */
	public NetStatusEvent(int type) {
		code = type;
	}

	/**
	 * Devuelve el codigo del evento.
	 * @return
	 */
	public int getCode() {
		return code;
	}
}
