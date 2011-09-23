
import java.io.*;   
import java.net.*;

import java.util.*;
import java.util.regex.*;

import isabel.lib.tasksock.*;

/**
 *
 *
 */
public class LseServer {  

  public static void main (String args[]) {

      int port = 22569;

      ServerSocket serv = null;
      try {
	  serv = new ServerSocket(port);
	  System.out.println("LSE Server was created at "+port+".");
      } catch (IOException e) { 
	  System.err.println("Error: I can not create the LSE server: "+e);
	  System.exit(1); 
      }

      Socket c = null;
      SubTitles st = null;
      VideoAvatar va = null;
      while (true) {
	  try {
	      c = serv.accept();

	      System.out.println("Aceptada una nueva conexion.");

	      PrintWriter out = new PrintWriter(new OutputStreamWriter(c.getOutputStream(),
								       "ISO-8859-1"),
						true);
	      BufferedReader in = new BufferedReader(new InputStreamReader(c.getInputStream(),
									      "ISO-8859-1"));
	      
	      String line;
	      
	      while ( (line = in.readLine()) != null ) {
		  
		  System.out.println("He recibido: "+line);
		  
		  out.println("audio:138.4.4.209,51001");

		  Pattern p = Pattern.compile("^avatar:(.+),(\\d+)\\s*subtitles:(.+),(\\d+)\\s*$");
		  Matcher m = p.matcher(line);
		  if ( !m.matches()) {
		      System.out.println("He recibido un mensaje incorrecto: "+line);
		      break;
		  }

		  String avatar_host = m.group(1);
		  int avatar_port = Integer.parseInt(m.group(2));
		  String subtitles_host = m.group(3);
		  int subtitles_port = Integer.parseInt(m.group(4));

		  st = new SubTitles(subtitles_host,subtitles_port);
		  st.start();
		  
		  try {
		      va = new VideoAvatar(avatar_host,avatar_port);
		  }
		  catch (Exception e) { 
		      System.err.println("No puedo arrarcar el video: "+e); 
		  }
	      }
	  } catch (Exception e) { 
	      System.err.println(e); 
	  } finally {
	      try {c.close();} catch (Exception e) {} 
	      if (st != null) {
		  st.parar();
		  st = null;
	      }
	      if (va != null) {
		  va.quit();
		  va = null;
	      }
	  }
      }
  }
}


/**
 *
 *
 */
class SubTitles extends Thread {
    
    private boolean parar = false;
    
    private String host;
    private int port;
    
    private Random r;
    
    SubTitles(String host, int port) {
	this.host = host;
	this.port = port;

	r = new Random();
    }	
    
    public void parar() {
	parar = true;
    }
    
    public void run() {

	while (!parar) {
	    Socket s = null;
	    try {
		s = new Socket(host, port);
		PrintWriter out = new PrintWriter(s.getOutputStream(), true);
		int cont = 1;
		while (!parar) {
		    System.out.print("*");
		    out.println();
		    out.print(rgb() + "hola " + cont++);
		    if (out.checkError()) break;
		    try { sleep(1000); } catch (Exception e) {}	    
		}
	    } catch (Exception e) {
		System.err.println(e);
	    } finally {
		try {s.close();} catch (Exception e) {} 
	    }
	}
	System.out.println("Finalizo el generador de subtitulos");

    }

    private String rgb() {
	
	return String.format("#%02x%02x%02x#%02x%02x%02x",
			     r.nextInt(255),r.nextInt(255),r.nextInt(255),
			     r.nextInt(255),r.nextInt(255),r.nextInt(255));
    }
}



/**
 *
 *
 */
class VideoAvatar {

    private String host;
    private int port;

    private TaskSock vts;
    
    VideoAvatar(String host, int port) throws Exception {
	this.host = host;
	this.port = port;
	
	String cmd[] = {
	    "/usr/local/isabel/bin/isabel_video",
            "-noV4L",
	    "-cport", "10111",
	    "-rtpPort", "10112",
	    "-rtcpPort", "10113",
	    "-notify", "/dev/null"
	};

	vts = new TaskSock("Video",cmd, 10111,"video_nop()","video_quit()");
	vts.RDO("video_rec_channel(1)");
	vts.RDO("video_bind(1,"+host+","+port+")");
	vts.RDO("video_send(1,1)");
    }

    void quit() {

	vts.quit();
	vts = null;
    }
}

