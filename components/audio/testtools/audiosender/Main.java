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
/*
 * Main.java
 *
 * Created on 6 de septiembre de 2007, 11:50
 * 
 */

package audiosender;

import java.io.*;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.util.Timer;
import java.util.TimerTask;

/**
 *
 * @author Fernando
 */
public class Main {
    
    static int MS_PER_PACKET = 30;
    static int FREC = 16;
    static int BYTES_PER_PACKET = FREC*MS_PER_PACKET*2;
    
    int localport;
    int remoteport;
    int ssrc;
    
    String filename;
    String host;
    
    BufferedInputStream reader;
    
    DatagramSocket socket;
    SocketAddress address;
    
    byte[] buffer = new byte[BYTES_PER_PACKET + 12];
    
     
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
                    
                // Escribo los datos del fichero
                if (reader.available() < BYTES_PER_PACKET)
                    reader.reset();
                
                reader.read(buffer, 12, BYTES_PER_PACKET);  
                
                DatagramPacket packet = new DatagramPacket(buffer, BYTES_PER_PACKET + 12, address);
                
                socket.send(packet);
                
            }catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    };
    
    /** Creates a new instance of Main */
    public Main() throws Exception {
        localport = 30000;
        remoteport = 40000;
        ssrc = 1000;
        
        host = "localhost";
    }
    
    void go() throws Exception {
        if(filename != null) {
            reader = new BufferedInputStream(new FileInputStream(filename));
            reader.mark(100000000);

        }
        else {
            System.out.println("Falta elegir fichero");
            System.exit(0);
        }
        
        socket = new DatagramSocket(localport);
        address =  new InetSocketAddress(host, remoteport);        
        new Timer().scheduleAtFixedRate(task, 0, MS_PER_PACKET);
    }
    
    
    /**
     * Programa para enviar audio en RTP leido desde un fichero.
     * Envia en NONE-16KHz con PT 102.
     * Parametros:
     * -lport Puerto desde el que se envian los paquetes
     * -rport Puerto al que se envian los paquetes
     * -host  Host al que se envian los paquetes
     * -ssrc  Numero de canal
     * -file Nombre del fichero. El fichero tiene que tener el audio en crudo
     * con muestras de 16 bits a 16KHz.
     *
     */
    public static void main(String[] args) throws Exception {
        
        Main main = new Main();        
        
        for(int i = 0; i < args.length; i++) {
            if (args[i].equals("-lport")) {
                main.localport = Integer.parseInt(args[++i]);
                System.out.println("localport:" + main.localport);
            }
            if (args[i].equals("-rport")) {
                main.remoteport = Integer.parseInt(args[++i]);
                System.out.println("remoteport:" + main.remoteport);
            }
            if (args[i].equals("-ssrc")) {
                main.ssrc = Integer.parseInt(args[++i]);
                System.out.println("ssrc:" + main.ssrc);
            }
            if (args[i].equals("-file")) {
                main.filename = args[++i];
                System.out.println("file:" + main.filename);
            }
            if (args[i].equals("-host")) {
                main.host = args[++i];
                System.out.println("host:" + main.host);
            }
            
        }
        
        main.go();
        
    }
}
        