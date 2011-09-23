package isabel.gwflash.red5client.utils;

/**
 * Las clases que implementen esta interfaz ser�n capaces de procesar eventos de conexi�n.
 *
 * @author Javier
 *
 */
public interface NetStatusEventHandler {

	/**
	 * M�todo invocado cuando ocurra un nuevo evento de conexi�n.
	 *
	 * @param event
	 *				Evento que ha ocurrido en la conexi�n RTMP.
	 */
	public void newNetStatusEvent(NetStatusEvent event);
}
