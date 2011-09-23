package isabel.gwflash.red5client.oldred5;

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;

public class FLVTag {

	public enum TYPE {
		audio, video, data, unknown
	};

	public TYPE type;

	public byte dataType;

	public int dataSize;

	public int timestamp;

	public int previousTagSize;

	public byte[] body;

	public FLVTag(byte[] header) throws IOException {

		DataInputStream dis = new DataInputStream(new ByteArrayInputStream(
				header));
		previousTagSize = dis.readInt();

		Byte type = dis.readByte();
		dataType = type;
		if (type == 8)
			this.type = TYPE.audio;
		else if (type == 9)
			this.type = TYPE.video;
		else if (type == 18)
			this.type = TYPE.data;
		else
			this.type = TYPE.unknown;

		dataSize = ((dis.readByte() << 16) & 0x00FF0000)
				+ ((dis.readByte() << 8) & 0x0000FF00)
				+ ((dis.readByte()) & 0x000000FF);
		timestamp = ((dis.readByte() << 16) & 0x00FF0000)
				+ ((dis.readByte() << 8) & 0x0000FF00)
				+ ((dis.readByte()) & 0x000000FF);

	}

	public String toString() {
		String tagType = "unknown";
		switch (type) {
		case audio:
			tagType = "Audio";
			break;
		case video:
			tagType = "Video";
			break;
		case data:
			tagType = "data";
			break;
		}
		return "FLV Tag: " + tagType + "; size: " + dataSize + "; TS: "
				+ timestamp;
	}

}
