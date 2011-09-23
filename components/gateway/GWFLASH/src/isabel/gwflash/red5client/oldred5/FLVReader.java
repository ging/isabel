package isabel.gwflash.red5client.oldred5;
import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;

import org.apache.mina.common.ByteBuffer;
import org.red5.io.IStreamableFile;
import org.red5.io.ITag;
import org.red5.io.ITagReader;
import org.red5.io.flv.impl.Tag;


public class FLVReader implements ITagReader{


	InputStream is;
	
	public FLVReader(InputStream in){
		System.out.println("Nuevo FLVReader");
		this.is = in;
		try {
			is.skip(9);		// Saltamos la cabecera del archivo. 
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
	}
	
	public ITag readTag(){
		try{
//		System.out.println("READTAG");
		byte[] header = new byte[15];
		int num = read(header);
//		System.out.println("Leidos -----  " +  num);
		if(num == -1) {
			System.out.println("Sin Datos -------------------");
			return null;
			
		}
		FLVTag tag = new FLVTag(header);
		byte[] body = new byte[tag.dataSize];
		int readBody = read(body);
		
//		System.out.println("Leidos Body ------ " + readBody);
		
		ByteBuffer buf =  ByteBuffer.allocate(tag.dataSize, false);
		buf.put(body);
		buf.flip();
		
//		System.out.println(tag);
//		System.out.println(System.currentTimeMillis());
		
		
		Tag red5Tag = new Tag(tag.dataType, tag.timestamp, tag.dataSize, buf, tag.previousTagSize);
		
		return red5Tag;
		
		}catch(Exception e){
			e.printStackTrace();
			return null;			
		}
	}
	
	
	@Override
	public void close() {
		// TODO Auto-generated method stub
		try {
			is.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}

	@Override
	public void decodeHeader() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public long getBytesRead() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public long getDuration() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public IStreamableFile getFile() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public int getOffset() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public long getTotalBytes() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public boolean hasMoreTags() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean hasVideo() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void position(long arg0) {
		// TODO Auto-generated method stub
		
	}
	
	private int read(byte[] buf) throws IOException {
		int read = 0; 
		while(read < buf.length) {
			int num = is.read(buf, read, buf.length - read);
			if (num == -1)
				return -1;
			else
				read += num;
		}
		return read;
	}
}
