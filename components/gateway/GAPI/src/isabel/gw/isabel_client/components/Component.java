/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */
package isabel.gw.isabel_client.components;

import java.util.*;

import isabel.lib.StringParser;

/**
 * Clase base de los componentes. Contiene atributos y metodos comunes de todos
 * los componentes.
 */
public abstract class Component {

	/**
	 * Nombre del componente (Video, Audio, ...)
	 */
	private String name;

	/**
	 * State: Indica si el sitio local, es decir yo, esta participando en la ejecucion del
	 * componente o no.
	 */
	private enum State {
		STOPPED, WAITING, RUNNING
	};

	/**
	 * Mi estado de ejecucion en el componente.
	 */
	private State myState = State.STOPPED;

	/**
	 * Otros sitios que estan ejecutando el componente. Nota: son otros sitios,
	 * es decir, yo no estoy en este conjunto.
	 * It contains site_id values.
	 */
	private Set<String> partnersRunning;

	/**
	 * When the component is running, this value indicates if this site is
	 * visible or not. Visible means video window mapped, audio capturing, etc.
	 */
	private boolean visible;

	/**
	 * Constructor base.
	 * 
	 * @param name
	 *            Nombre del componente (Video, Audio, ...).
	 */
	Component(String name) {
		this.name = name;

		partnersRunning = new HashSet<String>();
	}

	/**
	 * Se llama para indicar que estoy ejecutando el componente.
	 */
	void start() {

		myState = State.WAITING;
		visible = false;
	}

	/**
	 * Se llama para indicar que estoy no ejecutando el componente.
	 */
	void stop() {

		myState = State.STOPPED;
		visible = false;
	}

	/**
	 * He recibido del master el mensaje de notificacion de mi creacion
	 * satisfactoria, me uno a la ejecucion del componente.
	 */
	void addMe() {

		myState = State.RUNNING;
	}

	/**
	 * Informo al componente de que el sitio indicado se ha unido a la ejecucion
	 * del componente.
	 * 
	 * @param site_id
	 *            Sitio que se ha unido a la ejecucion del componente.
	 */
	void addRemoteSite(String site_id) {

		partnersRunning.add(site_id);
	}

	/**
	 * Informo al componente de que el sitio indicado ha dejado la ejecucion del
	 * componente.
	 * 
	 * @param site_id
	 *            Sitio que se ha dejado la ejecucion del componente.
	 */
	void delRemoteSite(String site_id) {

		partnersRunning.remove(site_id);
	}

	/**
	 * Set the visible status of the component. When the component is running, it
	 * can be visible or not.
	 * 
	 * @param visible
	 *            true if the component is visible.
	 */
	void setVisible(boolean visible) {

		if (myState != State.STOPPED) {
			this.visible = visible;
		} else {
			this.visible = false;
		}
	}

	/**
	 * Procesa un mensaje de tipo COMP.
	 * 
	 * @param sp
	 *            StringParser con el mensaje a procesar.
	 */
	abstract void processMessage(StringParser sp);

	/**
	 * Applies a interaction mode configuration.
	 * 
	 * @param imName
	 *            Interaction mode name.
	 * @param nsec
	 *            Interaction mode secuence number.
	 * @param site_id_list
	 *            - site_id of the selected sites.
	 *            - "*" to select all sites.
	 * @param options
	 *            List of option-value pairs (ex: -opt1 v1 -opt v2 ...)
	 *            configuring the interaction mode.
	 */
	abstract void setInteractionMode(String imName, int nsec,
			List<String> site_id_list, Map<String, String> options);

	// ---------------------------
	// Getters
	// ---------------------------

	/**
	 * Devuelve el valor del atributo name.
	 * 
	 * @return el valor del atributo name.
	 */
	String getName() {
		return name;
	}

	/**
	 * Devuelve el valor del atributo visible.
	 * 
	 * @return el valor del atributo visible.
	 */
	boolean getVisible() {
		return visible;
	}

	/**
	 * Devuelve true si el componente esta parado rn mi sitio.
	 * 
	 * @return true si el componente esta parado.
	 */
	boolean isStopped() {
		return myState == State.STOPPED;
	}

	/**
	 * Devuelve true si estoy en proceso de lanzar el componente en mi sitio.
	 * 
	 * @return true  si estoy en proceso de lanzar el componente.
	 */
	boolean isWaiting() {
		return myState == State.WAITING;
	}

	/**
	 * Devuelve true si estoy ejecutando el componente en mi sitio.
	 * 
	 * @return true si estoy ejecutando el componente.
	 */
	boolean isRunning() {
		return myState == State.RUNNING;
	}
	
    /**
     * Gets the value of the given option.
     * 
     * @param sp
     *            StringParser object where the option is searched.
     * @param optionName
     *            Option name;
     * @param defaultValue
     *            Value to return if ooption doesn't exists.
     */
    protected String getOptionValue(StringParser sp, String optionName,
				  String defaultValue) {
	
	int pos = sp.indexOf(optionName);
	if (pos == -1) {
	    return defaultValue;
	} else {
	    return sp.tokenAt(pos + 1);
	}
    }
	

}
