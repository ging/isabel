/*
 * ISABEL: A group collaboration tool for the Internet
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

package isabel.gwflash;

import java.io.*;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

/**
 *
 * @author Fernando
 */
public class SilenceSender {
    
    private static final int MS_PER_PACKET = 20;
    private static final int FREC = 16;
    private static final int BYTES_PER_PACKET = FREC*MS_PER_PACKET*2;
    
    private static final int localport = 61008;
    private static final int remoteport = 51001; //Same as LocalMixer(GAPI) LOCAL_AUDIO_PORT
    private static final int ssrc = 2;
    
    private static final String host = "localhost";
    
    BufferedInputStream reader;
    
    DatagramSocket socket;
    SocketAddress address;
    
    byte[] buffer = new byte[BYTES_PER_PACKET + 12];
    
    Random rand = new Random();
    
     
    TimerTask task = new TimerTask() {
       
        int ts = 100;
        int seqnumber = 1;
        
        public void run() {
            try {
                ByteArrayOutputStream baos = new ByteArrayOutputStream();
                DataOutputStream daos = new DataOutputStream(baos);
                
                // Cabecera RTP                
                daos.write(0x80);
                daos.write(0x66); //102 NONE 16KHz
                daos.writeShort(seqnumber++);
                daos.writeInt(ts+=FREC*MS_PER_PACKET);
                daos.writeInt(ssrc);
                
                byte[] header = baos.toByteArray();
                
                for (int i = 0; i < 12; i++)
                    buffer[i] = header [i];                
                
                DatagramPacket packet = new DatagramPacket(buffer, BYTES_PER_PACKET + 12, address);
                
                socket.send(packet);
                
            }catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    };
        
    void go() throws Exception {        
        socket = new DatagramSocket(localport);
        address =  new InetSocketAddress(host, remoteport);        
        new Timer().scheduleAtFixedRate(task, 0, MS_PER_PACKET);
    }
   
    public SilenceSender() {
        for(int i = 0; i < buffer.length; i+=2) {
            buffer[i] = (byte)rand.nextInt(3);
        }
    } 
    
    /**
     * Test method
     */
    public static void main(String[] args) throws Exception {
        
        SilenceSender ss = new SilenceSender();                
        ss.go();   
    }
}
