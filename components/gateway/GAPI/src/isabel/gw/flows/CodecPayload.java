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
package isabel.gw.flows;

import java.util.Map;
import java.util.HashMap;

/**
 * This class contains a map to associate codec names and its payload types.
 */
public class CodecPayload {

	/**
	 * Map to store codec names with its associated payload types.
	 * 
	 * The key is the codec name, and the value is the payload type.
	 */
	private static Map<String, Integer> codec_payload = new HashMap<String, Integer>();

	/**
	 * Static constructor to fill the codec_payload map.
	 */
	static {
		// video:
		add("CELLB", 25);
		add("MJPEG", 26);
		add("MPEG-1", 32);
		add("MPEG-4", 96);
		add("H263", 34);
		add("H264", 98);
		add("XVID", 118);
		add("XviD", 118);

		// audio:
		add("gsm-8KHz", 3);
		add("gsm-16KHz", 107);
		add("gsm-32KHz", 115);

		add("amr-8KHz", 116);
		add("amr-16KHz", 117);
		add("amr-32KHz", 121);

		add("mp3-8KHz", 14);
		add("mp3-16KHz", 15);
		add("mp3-32KHz", 16);
		add("mp3-44.1KHz", 17);
		add("mp3-48KHz", 18);

		add("none-8KHz", 100);
		add("none-16KHz", 102);
		add("none-22.05KHz", 108);
		add("L16-44.1KHz", 11);
		add("none-48KHz", 119);

		add("g711-ulaw-8KHz", 0);
		add("g711-alaw-8KHz", 8);
		add("g711-ulaw-16KHz", 103);
		add("g711-ulaw-22.05KHz", 109);
		add("g711-ulaw-44.1KHz", 112);

		add("g722-8KHz", 9);
		add("g722-16KHz", 104);

		add("g726_24-8KHz", 101);
		add("g726_24-16KHz", 105);
		add("speex-16KHz",124);

		// vumeter:
		add("VUMETER", 22);
	}

	/**
	 * Stores the given codec name and payload type into the payloadCodecs map.
	 * 
	 * @param codecName
	 *            Codec name.
	 * @param payloadType
	 *            Payload type.
	 */
	private static void add(String codecName, int payloadType) {
		codec_payload.put(codecName, payloadType);
	}

	/**
	 * Returns the payload type associated to the given codec name.
	 * 
	 * @codecName The name of the codec used to search its associated payload
	 *            type.
	 * @return The payload type of the given codec name. It the given codec name
	 *         has not been registered, then it returns -1.
	 */
	public static int getPayloadType(String codecName) {

		if (codec_payload.containsKey(codecName)) {
			return codec_payload.get(codecName);
		} else {
			return -1;
		}
	}
}