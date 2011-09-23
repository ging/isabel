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
package services.isabel.lib;



import java.io.*;

import java.net.*; 

import java.util.*; 
import java.util.List;



// Para el manejo de ZIP:

import java.util.zip.*;



// Para el manejo de XERL

import xedl.lib.XEDLsplitter.XEDLsplitter;
import xedl.lib.jaxb.SITE;
import xedl.lib.xerl.*;


// Para el manejo de XEDL

import xedl.lib.xedl.*;
import xedl.lib.xpathParser.XPathParser;



// Para el manejo de menus

import java.awt.*;

import java.awt.event.*;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

import javax.swing.*;

import javax.swing.event.*;



// Para el manejo de horas..

import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;
import javax.xml.parsers.*;

import javax.xml.transform.*;




import isabel.lib.NetworkManagerConstants;
import isabel.lib.ZipUtils;
import isabel.tunnel.TunnelUtils;
import isabel.tunnel.Tunnelator;



//import isabel.xlim.Client;

//import isabel.xlim.services.crypt.Crypt;





/**

 * Esta clase reune una serie de funciones que son utilizadas por los distintos servicios xlim.

 * @Author ebarra

 */

public class Servutilities { 




	// METODOS METODOS METODOS METODOS METODOS METODOS METODOS METODOS METODOS METODOS METODOS METODOS METODOS METODOS 

	// --------------------------------------------------------------------------------------------------

	// ------------------ METODO PARA SACAR EL LOCATION DEL PUBLIC NAME ---------------------------------

	// --------------------------------------------------------------------------------------------------

	/**

	 * Dado un Public Name devuelve la parte correspondiente a location (se supone que publicname = ID.LOCATION) 

	 * @param publicName el public name del que se quiere sacar el location

	 * @return la parte correspondiente al location dentro del public name dado

	 * @Author lailoken

	 */

	static public String getLocation (String publicName){

		int beginIndex = publicName.indexOf(".");

		if (beginIndex != -1) return publicName.substring(beginIndex+1);

		else return publicName;

	}



	/**

	 * Dado un Public Name y un id devuelve la parte correspondiente a location (se supone que publicname = ID.LOCATION) 

	 * @param id id del sitio al que pertenence el public name

	 * @param publicName el public name del que se quiere sacar el location

	 * @return la parte correspondiente al location dentro del public name dado

	 * @Author lailoken

	 */

	static public String getLocation (String id, String publicName){

		int beginIndex = publicName.indexOf(id);

		if (beginIndex != -1) return publicName.substring(id.length()+2);

		else return publicName;

	}



	// --------------------------------------------------------------------------------------------------

	// ------------------ METODOS PARA SACAR VALORES DE UNA URL -----------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**

	 * Analiza una Iurl y devuelve el flowserver y la sesion en un array (primer campo flow, segundo sesion)

	 * @param url URL a analizar.

	 * @return el flowserver y la sesion en un array (primer campo flow, segundo sesion)

	 * @Author lailoken

	 */

	static public String[] getUrlData (String url){

		String[] result = {"",""};

		String[] url_total_parts;

		String[] url_parts;

		String flowPlusSession = "";

		url_total_parts = url.split("//"); // con esto separamos el protocolo (isabel://) del resto de la url

		if (url_total_parts.length == 2) {

			flowPlusSession = url_total_parts[1]; // La url contiene el protocolo: isabel://

		} else {

			if (url_total_parts.length == 1) {

				flowPlusSession = url_total_parts[0]; // La url no contiene el protocolo: isabel://      

			} else return result; // la url tiene mas de 1 // por lo que esta mal

		}

		url_parts = flowPlusSession.split("/"); 

		if (url_parts.length == 2) {result[0] = url_parts[0].trim(); result[1] = url_parts[1].trim(); return result;} //url bien formada

		else 

			if (url_parts.length == 1) {

				// Vemos si lo que nos han puesto es el flow o la sesion

				if (flowPlusSession.indexOf("/")!=0) { result[0] = url_parts[0].trim(); return result;}

				else { result[1] = url_parts[0].trim(); return result;}

			} else return result; // mal formada

	}



	/**

	 * Analiza una Iurl y devuelve el flowserver

	 * @param url URL a analizar.

	 * @return el flowserver contenido en la URL.

	 * @Author lailoken

	 */

	static public String getFlowFromUrl (String url){

		if (getUrlData(url) != null) return getUrlData(url)[0];

		else return null;

	}



	/**

	 * Analiza una Iurl y devuelve la sesion

	 * @param url URL a analizar.

	 * @return la sesion contenida en la URL.

	 * @Author lailoken

	 */

	static public String getSessionFromUrl (String url){

		if (getUrlData(url) != null) return getUrlData(url)[1];

		else return null;

	}





	// --------------------------------------------------------------------------------------------------

	// ------------------ GET SOURCE ADDRESS ------------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**

	 * Calcula la direcci�n origen que se utiliza para conectar con una direcci�n dada.

	 * @param address Direcci�n con la que se quiere conectar.

	 * @return Direcci�n utilizada para realizar la conexi�n; null, si no se pudo descubrir la direcci�n.

	 * @Author lailoken

	 */

	static public String getSourceAddress (String address){

		try {

			DatagramSocket s = new DatagramSocket();

			InetAddress localAddress;

			s.connect(InetAddress.getByName(address),53019);

			localAddress=s.getLocalAddress();

			return localAddress.toString().substring(1,localAddress.toString().length());

		} catch (Exception e) {

			return null;

		}

	}




	/**

	 * Calcula la ip a partir del nombre

	 * @param name nombre a resolver

	 * @return ip

	 * @Author enrique

	 */

	static public String getIpByName (String name){
		//Cambio la direccion por la direccion literal y la pongo
		//entre corchetes si es IPv6, si tiene varias me quedo con la ipv4

		InetAddress[] address;
		try {
			address = InetAddress.getAllByName(name);
		} catch (UnknownHostException e) {
			e.printStackTrace();
			return null;
		}

		int i = 0;
		String ip = "";
		if(address.length > 1) {
			for (i = 0; i < address.length; i++) {
				if (address[i] instanceof Inet4Address) {
					ip = address[i].getHostAddress();
					break;
				}                
			}
		}
		else {
			ip = address[0].getHostAddress();
		}

		if (address[i] instanceof Inet6Address)
			ip = "[" + ip + "]";

		return ip;
	}


	// --------------------------------------------------------------------------------------------------

	// ------------------ DELETE DIRECTORY ------------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**
	 * Borra todos los ficheros contenidos en un directorio.
	 * @param f El directorio a borrar.
	 */
	public static void deleteDirectory(File f) {
		// Listo los ficheros del directorio
		File[] contents = f.listFiles();

		// Borro cada fichero contenido
		// utilizando deleteDirectory si es un directorio.
		for (int i = 0; i < contents.length; i++) {
			if(contents[i].isDirectory())
				deleteDirectory(contents[i]);
			contents[i].delete();
		}
		// Finalmente no borro el propio directorio
		//f.delete();
	}


	// --------------------------------------------------------------------------------------------------

	// ------------------ SAVE PARAM TO FILE ------------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**

	 * A�ade el parametro-valor en el fichero dado. Si no existe el fichero, lo crea

	 * @param fileName fichero donde se almacenan las tuplas parametro-valor

	 * @param param parametro a guardar

	 * @param value valor asignado al parametro

	 * @return el antiguo valor para ese parametro almacenado en el fichero

	 * @throws FileNotFoundException, IOException si hubo algun problema.

	 * @Author lailoken

	 */

	static public String saveParamToFile (String fileName, String param, String value) throws FileNotFoundException, IOException{

		return saveParamToFile(fileName,param,value,new File(fileName).getName().toString() + " PARAMS");

	}



	/**

	 * A�ade el parametro-valor en el fichero dado. Si no existe el fichero, lo crea

	 * @param fileName fichero donde se almacenan las tuplas parametro-valor

	 * @param param parametro a guardar

	 * @param value valor asignado al parametro

	 * @param fileTitle comentario q es escribe al comienzo del fichero

	 * @return el antiguo valor para ese parametro almacenado en el fichero

	 * @throws FileNotFoundException, IOException si hubo algun problema.

	 * @Author lailoken

	 */

	static public String saveParamToFile (String fileName, String param, String value, String fileTitle)  throws FileNotFoundException, IOException{

		// cargamos las properties con los antiguos valores

		Properties p = new Properties();

		File file = new File(fileName);

		if (file.exists()) {

			p.load(new FileInputStream(fileName));

		}

		Object object = p.setProperty(param, value);

		String oldValue = "";

		if (object!= null) oldValue = object.toString();

		p.store(new FileOutputStream(fileName), fileTitle);

		return oldValue;

	}



	/**

	 * guarda las tuplas parametro-valor en el fichero dado. Antes de guardarlas, elimina el contenido del fichero, si existia.

	 * @param fileName fichero donde se almacenan las tuplas parametro-valor

	 * @param params tuplas parametro-valor a guardar

	 * @throws FileNotFoundException, IOException si hubo algun problema.

	 * @Author lailoken

	 */

	static public void saveParamsToFile (String fileName, String[] params)  throws FileNotFoundException, IOException{

		saveParamsToFile(fileName,params,new File(fileName).getName().toString() + " PARAMS");

	}

	/**

	 * guarda las tuplas parametro-valor en el fichero dado

	 * @param fileName fichero donde se almacenan las tuplas parametro-valor

	 * @param params tuplas parametro-valor a guardar

	 * @param fileTitle comentario q es escribe al comienzo del fichero

	 * @throws FileNotFoundException, IOException si hubo algun problema.

	 * @Author lailoken

	 */

	static public void saveParamsToFile (String fileName, String[] params, String fileTitle)  throws FileNotFoundException, IOException{

		Properties p = new Properties();

		File file = new File(fileName);

		if (file.exists()) {

			p.load(new FileInputStream(fileName));

		}

		for (int i=0; i<params.length; i=i+2) {

			//System.out.println("param: " + params[i] + ", value: " + params[i+1]);

			p.setProperty(params[i], params[i+1]);

		}

		p.store(new FileOutputStream(fileName), fileTitle);

	}



	// --------------------------------------------------------------------------------------------------

	// ------------------ LOAD PARAM FROM FILE ------------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**

	 * devuelve el valor correspondiente al parametro en el fichero dado

	 * @param fileName nombre del fichero de parametros

	 * @param param parametro del que queremos averiguar su valor.

	 * @param defValue valor que devuelve si no encontro el parametro

	 * @return el valor asignado al parametro almacenado en el fichero. Si no se encuentra, devuelve defValue.

	 * @throws FileNotFoundException, IOException si hubo algun problema.

	 * @Author lailoken

	 */

	static public String loadParamFromFile (String fileName, String param, String defValue)  throws FileNotFoundException, IOException{

		Properties p = new Properties();

		p.load(new FileInputStream(fileName));

		return p.getProperty(param,defValue);

	}



	/**

	 * devuelve el valor correspondiente al parametro en el fichero dado

	 * @param fileName nombre del fichero de parametros

	 * @param param parametro del que queremos averiguar su valor.

	 * @return el valor asignado al parametro almacenado en el fichero. Si no se encuentra, devuelve null.

	 * @throws FileNotFoundException, IOException si hubo algun problema.

	 * @Author lailoken

	 */

	static public String loadParamFromFile (String fileName, String param)  throws FileNotFoundException, IOException{

		String defValue = null;

		return loadParamFromFile(fileName,param,defValue);

	}





	// --------------------------------------------------------------------------------------------------

	// ------------------ REMOVE PARAM FROM FILE---------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**

	 * Elimina un parametro de un fichero de parametros

	 * @param fileName el fichero de parametros

	 * @param param parametro a eliminar

	 * @returns true si se consiguio eliminar el parametro

	 */

	static public boolean removeParamFromFile (String fileName, String param)  {

		try {

			String defValue = null;

			Properties in = new Properties();

			in.load(new FileInputStream(fileName));

			Properties out = new Properties();

			for (Enumeration list = in.propertyNames(); list.hasMoreElements() ;) {

				String readParam = list.nextElement().toString();

				if (!readParam.equals(param)) {

					//Escribimos el parametro-valor en la salida

					out.setProperty(readParam, in.getProperty(readParam,""));

				}

			}

			//Borramos el fichero

			new File(fileName).delete();

			//escribimos la salida en el fichero

			out.store(new FileOutputStream(fileName),"");

			return true;

		} catch (Exception e) {

			return false;

		}

	}



	// --------------------------------------------------------------------------------------------------

	// ------------------ GET URL for Running Session ------------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**throws IOException, InterruptedException
	 * @return url de la ultima sesion isabel que corrió o de la actual
	 * @Author Enrique
	 */
	static public String getURLRemoteRunningSession() {
		String path = Constants.ISABEL_SESSIONS_DIR+Constants.FILE_SEPARATOR + "remote";
		//FileReader file_reader = new FileReader (file);
		//BufferedReader buf_reader = new BufferedReader (file_reader);
		File sesiones = new File(path);
		String sesion_dir = "";
		String[] children = sesiones.list();
		if(children.length==0)
			return "";
		else if(children.length>1)
		{
			//hay más de un directorio no debería pasar nunca
			try {
				if(getRunningSession()!=null)
					sesion_dir = getRunningSession();
				else
					sesion_dir = children[0];
			} catch (IOException e) {			
				e.printStackTrace();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		else
			sesion_dir = children[0];

		path += Constants.FILE_SEPARATOR + sesion_dir + Constants.FILE_SEPARATOR + "url";	  
		FileReader file_reader=null;
		try {
			file_reader = new FileReader(path);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
		BufferedReader buf_reader = new BufferedReader (file_reader);
		String line=null;
		try {
			line = buf_reader.readLine();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return line;
	}

	// --------------------------------------------------------------------------------------------------

	// ------------------ GET Running Session ------------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**

	 * Devuelve el nombre de la sesion de ISABEL que esta corriendo en la maquina.

	 * @return Nombre de la sesion ISABEL.

	 * @throws Exception si hubo algun problema al averiguar el nombre de la sesion.

	 * @Author lailoken

	 */

	static public String getRunningSession() throws IOException, InterruptedException{

		// Para averiguar que sesion esta corriendo miramos en el fichero isabel_is_running

		// Comprobar si existe el fichero:

		if (!new File(Constants.ISABEL_IS_RUNNING_FILE).exists()) {

			return null;

		}

		// Primero comprobamos si isabel esta corriendo...

		if (isIsabelRunning()) {

			// Carga el fichero:

			Properties pro = new Properties();

			pro.load(new FileInputStream(Constants.ISABEL_IS_RUNNING_FILE));

			// Coger el valor de session:

			String session = pro.getProperty("session");

			return session;

		} else {

			return null;

		}

	}





	// --------------------------------------------------------------------------------------------------

	// ------------------ GET Remote Running Session ------------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	



	


	




	// --------------------------------------------------------------------------------------------------

	// ------------------ GET LOCAL VERSION ------------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**

	 * Devuelve la version del programa especificado (solo si el sistema es suse o red hat, porque usa rpm -qa)

	 * @param program Programa del que se quiere saber la version

	 * @return La version del programa especificado

	 * @throws IOException si hubo algun problema

	 * @Author lailoken

	 */

	static public String getVersion(String program) throws IOException{

		String result = "";

		try {

			//System.out.println (" Ejecutando....: "+RPM_COMMAND + program);

			String[] argsToProc = new String[]{"sh", "-c", Constants.RPM_COMMAND + program + " > " + Constants.RPM_OUT_FILE + " 2>&1"};

			Process extAppl = Runtime.getRuntime().exec(argsToProc);

			extAppl.waitFor();

			// Y ahora comprobamos si ese fichero es un xerl

			LineNumberReader procOutLine = new LineNumberReader(new FileReader(Constants.RPM_OUT_FILE));

			String newLine = "";

			while ((newLine = procOutLine.readLine()) != null) {

				//System.out.println("Leido: "+ newLine);

				result = result + newLine + "\r\n";

			}

		} catch (Exception e) {

			System.out.println (" Error al hacer getVersion...");

		}

		return result;

	}


	/**

	 * Devuelve la version del isabel

	 * @return La version del programa especificado

	 * @Author ebarra

	 */

	static public String getIsabelVersionClear(){

		LineNumberReader procOutLine;
		String linea = "";
		try {
			procOutLine = new LineNumberReader(new FileReader(Constants.ISABEL_DIR + Constants.FILE_SEPARATOR + "lib/version.Isabel"));
			linea = procOutLine.readLine();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} 
		catch (IOException e) {
			e.printStackTrace();
		}    

		if(linea==null)
			return "";

		return linea.substring(8);
	}


	/**

	 * Devuelve la release del isabel

	 * @return La version del programa especificado

	 * @Author ebarra

	 */

	static public String getIsabelRelease() {

		LineNumberReader procOutLine = null;
		String linea =  "";
		try {
			procOutLine = new LineNumberReader(new FileReader(Constants.ISABEL_DIR + Constants.FILE_SEPARATOR + "lib/version.Isabel"));
			linea = procOutLine.readLine();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
		catch (IOException e) {
			e.printStackTrace();
		}    

		if(linea==null)
			return "";

		//la release viene en la segunda linea del fichero
		try {
			linea = procOutLine.readLine();
		} catch (IOException e) {
			e.printStackTrace();
		}
		if(linea==null)
			return "";

		return linea.substring(8);
	}

	/**

	 * Devuelve la version del isabel

	 * @return La version del programa especificado

	 * @throws IOException si hubo algun problema

	 * @Author ebarra

	 */

	static public String getIsabelVersion() throws IOException{

		LineNumberReader procOutLine = new LineNumberReader(new FileReader(Constants.ISABEL_DIR + Constants.FILE_SEPARATOR + "lib/version.Isabel"));

		String linea = procOutLine.readLine();
		if(linea==null)
			return "";

		return "Isabel-"+linea.substring(8);
	}


	/**

	 * Devuelve la version del programa especificado, la saca de /usr/local/isabel/lib/version.Isabel version.IsabelExtras

	 * @param program Programa del que se quiere saber la version ("Isabel" o "IsabelExtras")

	 * @return La version del programa especificado

	 * @throws IOException si hubo algun problema

	 * @Author Enrique

	 */

	static public String getVersionFromLib(String program) throws IOException{

		if(!program.equals("Isabel") && !program.equals("IsabelExtras"))
			return "Can't know the version installed";	  

		String result = "";
		try {
			if(program.equals("Isabel"))
			{
				//parseado del fichero /usr/local/isabel/lib/version.Isabel
				FileReader file_reader = new FileReader (Constants.ISABEL_DIR + Constants.FILE_SEPARATOR + "lib/version.Isabel");
				BufferedReader buf_reader = new BufferedReader (file_reader);
				int num_lines = 1;
				while(true)
				{
					String line = buf_reader.readLine();
					if(line==null)
						break;
					if (line.startsWith("VERSION"))
					{
						result = "Isabel-"+line.substring(8);
					}
					if (line.startsWith("RELEASE"))
					{
						result = result+"-"+line.substring(8);
					}

				}
			}
			else
			{
				//    		parseado del fichero /usr/local/isabel/lib/version.Isabel
				FileReader file_reader = new FileReader (Constants.ISABEL_DIR + Constants.FILE_SEPARATOR + "lib/version.IsabelExtras");
				BufferedReader buf_reader = new BufferedReader (file_reader);
				int num_lines = 1;
				while(true)
				{
					String line = buf_reader.readLine();
					if(line==null)
						break;
					if (line.startsWith("VERSION"))
					{
						result = "IsabelExtras-"+line.substring(8);
					}
					if (line.startsWith("RELEASE"))
					{
						result = result+"-"+line.substring(8);
					}

				}
			}
		}
		catch (Exception e) {
			System.out.println (" Error al hacer getVersion...");
		}
		return result;
	}

	// --------------------------------------------------------------------------------------------------













	// --------------------------------------------------------------------------------------------------

	// ------------------ GET REMOTE VERSION ------------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**

	 * Devuelve la version de un programa instalado en una maquina determinada.

	 * @param program Programa del que queremos saber la version

	 * @terminal Direccion de la maquina donde queremos ver que version esta instalada

	 * @return La version instalada del programa.

	 * @throws Exception si hubo algun problema al averiguar la version.

	 */

	static public String getRemoteVersion(String program, String terminal) throws IOException,InterruptedException,RuntimeException{

		//Creamos un fichero donde ponemos el parametro...

		PrintWriter fileParam = new PrintWriter (new FileOutputStream (Constants.REMOTE_VERSION_FILE),true);

		fileParam.println (program);

		fileParam.close();



		// Hacemos log de la salida estandar:

		//LogStdOut logStdOut = new LogStdOut(OUTPUT_VERSION_FILE);

		//logStdOut.start();

		// Llamamos a XLIM con los parametros adecuados:

		//String[] params = {"-server",terminal,"-command","aboutserver","-file",REMOTE_VERSION_FILE};

		//Client.main(params);

		//logStdOut.stop();

		//Lanzamos el proceso

		String process = Constants.ISABEL_LIM_CLIENT + " -server " + terminal + " -command aboutserver -file " + Constants.REMOTE_VERSION_FILE;

		execProcess(process,Constants.OUTPUT_VERSION_FILE,Constants.XERL_IDENTIFIER,true);

		if (checkExprInFile (Constants.OUTPUT_VERSION_FILE,Constants.XERL_IDENTIFIER)) {

			// Hubo error

			throw new RuntimeException("Problems running XLIM client");

		} else {

			// Todo oka    

			// Leemos el resultado

			String result = "";

			String newLine = "";

			LineNumberReader procOutLine = new LineNumberReader(new FileReader(Constants.OUTPUT_VERSION_FILE));

			while ((newLine = procOutLine.readLine()) != null) {

				//System.out.println("Leido: "+ newLine);

				result = result + newLine + "\r\n";

			}



			// Borramos el fichero de parametros...

			File aboutParams = new File (Constants.REMOTE_VERSION_FILE);

			aboutParams.delete();



			// Devolvemos la salida:

			return result;

		}

	}

	// --------------------------------------------------------------------------------------------------

















	// --------------------------------------------------------------------------------------------------

	// ------------------ IS ISABEL RUNNING ------------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**

	 * Indica si hay una sesion de ISABEL corriendo

	 * @return true si ya hay una sesion corriendo.

	 * @throws IOException si hubo problemas.

	 */

	static public boolean isIsabelRunning() throws IOException, InterruptedException{

		boolean isabelRunning = false;

		try {

			// PARA ISABEL 4.7

			// Donde mandamos los mensajes de trazas: 

			PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (Constants.XLIM_LOGS_WORK_DIR+Constants.FILE_SEPARATOR+"isIsabelRunning.log")),true);

			outTraceFile.println("Para version 4.7");



			// Para averiguar si hay alguna sesion corriendo y cual es miramos el resultado de fp sabel_ y el .isabel/tmp/current.isa

			Process extAppl = Runtime.getRuntime().exec(Constants.ISABEL_FP);

			// Check irouter devuelve 0 si esta corriendo

			//DataInputStream extApplIn = new DataInputStream(extAppl.getInputStream());

			BufferedReader extApplIn = new BufferedReader (new InputStreamReader(extAppl.getInputStream()));

			DataOutputStream extApplOut = new DataOutputStream(extAppl.getOutputStream());

			String extApplStr="";

			String extApplStrTot="";

			while ((extApplStr = extApplIn.readLine()) != null) {

				//extApplStrTot = extApplStrTot + extApplStr;

				if (extApplStr.indexOf("0") != -1){

					//System.out.println ("Isabel running...");

					//System.out.println("La salida del ps de sabel_irouter: " + extApplStr);

					isabelRunning = true;

					return isabelRunning;

				}

			}

			return isabelRunning;

		} catch (Exception e) {

			// Para ISABEL 4.8



			// Donde mandamos los mensajes de trazas: 

			PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (Constants.XLIM_LOGS_WORK_DIR+Constants.FILE_SEPARATOR+"isIsabelRunning.log")),true);

			outTraceFile.println("Version 4.8");



			// En .isabel/tmp/ hay un fichero isabel_is_running. Si NO existe estamos seguros de que isabel no esta corriendo

			File isabelIsRunning = new File (Constants.ISABEL_IS_RUNNING_FILE);

			outTraceFile.println("Checking if file exists..: " + isabelIsRunning.getAbsolutePath());

			if (!isabelIsRunning.exists()) {

				isabelRunning = false;

				outTraceFile.println("it does not exist... ISABEL STOPPED.");

			} else {

				// Si existe el fichero en principio isabel esta corriendo...

				// ...pero puede que se rebotase la maquina y no se borrase ese fichero. 

				// Dentro del fichero hay una fecha



				long timeInFile;

				String pid;

				try {

					// Carga el fichero:

					outTraceFile.println("Loading file properties...");

					Properties pro = new Properties();

					pro.load(new FileInputStream(Constants.ISABEL_IS_RUNNING_FILE));

					// Coger el valor de la hora:

					outTraceFile.println("Date: |" + pro.getProperty("date") + "|" + ". Date : |" +pro.getProperty("date").trim() +"|");

					timeInFile = new Long(pro.getProperty("date").trim()).longValue();

					outTraceFile.println(" Time in isabel_is_running: " + timeInFile);

					pid = pro.getProperty("pid").trim();

					outTraceFile.println(" PID: " + pid);

				} catch (FileNotFoundException fnfe) {

					timeInFile = new Long("0").longValue();

					pid = "";

					fnfe.printStackTrace(outTraceFile);

					return false;

				} catch (NumberFormatException nfe) {

					timeInFile = new Long("0").longValue();

					pid = "";

					nfe.printStackTrace(outTraceFile);

					return false;

				} catch (Exception le) {

					timeInFile = new Long("0").longValue();

					pid = "";

					le.printStackTrace(outTraceFile);

					return false;

				}

				// Primero comprobamos si existe el proceso:

				outTraceFile.println(" Checking process " + pid);

				String[] argsToProc = new String[]{"sh", "-c", Constants.CHECK_PROCESS_PROGRAM+" "+pid+" > "+Constants.PROCESS_OUT_FILE+" 2>&1"};

				Process extAppl = Runtime.getRuntime().exec(argsToProc);

				extAppl.waitFor();

				outTraceFile.println(" Process " + pid+ " checked.");



				if (extAppl.exitValue() == 0) {

					// Ahora necesitamos la fecha en la que se reinicio el sistema:

					LineNumberReader timeLine = null;

					try {

						timeLine = new LineNumberReader(new FileReader(Constants.PROC_UPTIME_FILE));

						outTraceFile.println(" Boot Date: " + timeLine);

					} catch (FileNotFoundException fnfe) {

						isabelRunning = false;

					}

					// Lo que leemos tiene la pinta A.B C.D y solo nos interesa A -> por primero cortamos por " " y luego por "."

					// Nos podria valer con hacer solo un split(".") y coger el [0] pero por si acaso no pone . cuando es A.00

					long timeSinceReboot = new Long(timeLine.readLine().split(" ")[0].split("\\.")[0]).longValue();

					outTraceFile.println("timeSinceReboot: " + timeSinceReboot);

					long currentDate = System.currentTimeMillis()/1000;

					outTraceFile.println("currentDate" + currentDate);

					long timeInReboot = currentDate - timeSinceReboot;

					outTraceFile.println("timeInReboot" + timeInReboot);

					// Comparamos las dos fechas: si la del fichero 

					if (timeInFile>timeInReboot) {

						isabelRunning = true;

					} else {

						isabelRunning = false;

					}

				} else {

					outTraceFile.println(" Process " + pid + " not running..");

					isabelRunning = false;

				}

			}

			return isabelRunning;

		}

	}

	// --------------------------------------------------------------------------------------------------







	// --------------------------------------------------------------------------------------------------

	// ------------------ Is Localhost ------------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**

	 * Indica si la direccion IP/host que se pasa como parametro pertenece a la maquina local o no
	 * @return true si es de la maquina local.
	 */

	public static boolean isLocalHost(String address){

		if ((address == null) || (address.equals(""))) {
			return false;
		}

		try {
			InetAddress ia = InetAddress.getByName(address);

			if (ia.isLoopbackAddress()) {
				return true;
			}

			NetworkInterface ni = NetworkInterface.getByInetAddress(ia);

			return ni != null;
		} catch (SocketException se) {
			return false;
		} catch (UnknownHostException uhe) {
			return false;
		}
	}    


	// --------------------------------------------------------------------------------------------------




	// --------------------------------------------------------------------------------------------------

	// ------------------ Launch Isabel Local------------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**

	 * Lanza ISABEL remotamente en funcion de los parametros dados

	 * @param session nombre de la sesion que se quiere lanzar

	 * @param id nombre del identificador del sitio que queremos lanzar

	 * @param xedl nombre del fichero XEDL que contiene la descripcion del sitio

	 * @throws IOException si hubo problemas.

	 */
	static public Process launchLocalFromXedl(String session, String id, String xedl) throws IOException, XEDLException{
		return launchLocalFromXedl(session, NetworkManagerConstants.CLIENT_PORT, id, xedl, false);
	}
	
	static public Process launchLocalFromXedl(String session, String id, String xedl, boolean hack) throws IOException, XEDLException{
		return launchLocalFromXedl(session, NetworkManagerConstants.CLIENT_PORT, id, xedl, hack);
	}

	static public Process launchLocalFromXedl(String session, int port, String id, String xedl, boolean hack) throws IOException, XEDLException{

		// Donde mandamos los mensajes de trazas: 

		String logFile = Constants.XLIM_LOGS_WORK_DIR+Constants.FILE_SEPARATOR+"launchLocalFromXedl.log";

		PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (logFile)),true);

		// XLIM Notifications

		XlimNotification xlimNotif = new XlimNotification("Isabel Launcher");

		xlimNotif.println("Starting local launch...");

		if (session == null) throw new IOException ("Bad session value (null).");

		if (session.equals("")) throw new IOException ("Bad session value (empty value).");

		if (id == null) throw new IOException ("Bad id value (null).");

		if (id.equals("")) throw new IOException ("Bad id value (empty value).");

		if (xedl == null) throw new IOException ("Bad xedl name value (null).");

		if (xedl.equals("")) throw new IOException ("Bad xedl name value (empty value).");

		XPathParser parser = new XPathParser();

		parser.setFile(xedl);

		String role = parser.getFirstSiteRole();
		String urladdress = "";

		// Lanzamos isabel

		//-------------------------------------------------------------------------//

		//-------------- chequeamos las conexiones ISDN -------------------//

		//-------------------------------------------------------------------------//

		try {

			ConnectionsManager.startISDNIfNeeded(xedl,id); 

		} catch (XEDLException xedle) {

			xlimNotif.println(" ISDN checking process Failed");

			xedle.printStackTrace(outTraceFile);

			throw new IOException  ("Problems checking ISDN. Please, check your ISDN configuration in Options.");

		} catch (IOException ioe) {

			xlimNotif.println(" ISDN checking process Failed");

			ioe.printStackTrace(outTraceFile);

			throw new IOException  ("Problems checking ISDN. Please, check your ISDN configuration in Options.");

		} catch (Exception e) {

			xlimNotif.println(" ISDN checking process Failed");

			e.printStackTrace(outTraceFile);

			throw new IOException  ("Problems checking ISDN. Please, check your ISDN configuration in Options.");

		}


		xlimNotif.println("Running xedlsplitter with id " + id + " xedl " + xedl);

		outTraceFile.println("Running xedlsplitter with id " + id + " xedl " + xedl);

		// Llamamos primero al splitter de XEDL a variables de ISABEL

		LogStdErr logStdErr = new LogStdErr(logFile, false);

		outTraceFile.println("Logging std err to " + logFile);

		logStdErr.start();

		XEDL rXEDL = new XEDL (xedl); 	 
		SiteList lista = new SiteList(rXEDL);
		String master = lista.getRealRootSite();
		String private_ip = "";
		if (!master.equals(id) || hack) {
			outTraceFile.println("No somos el master");
			//si no soy el master tengo que poner private_ip para que use el tunel
			if(lista.getSite(id).getMCUAddress()!=null)
			{
				urladdress = (String)lista.getSite(id).getMCUAddress().getFirst();
			}
			else if(lista.getSite(id).getCONNECTIONMODE().getMulticast()!=null)
			{
				urladdress = (String)lista.getSite(id).getCONNECTIONMODE().getMulticast().getMulticastParameters().getMCASTROOT(); 			
			}
			
			//let s see if it is IPv6
			int veces = new StringTokenizer(urladdress, ":").countTokens()-1;
			if( veces > 0)
			{
				private_ip = "";
			}
			else
			{
				outTraceFile.println("Let's create the tunnel to " + urladdress + " and port " + port);
				Map<String, String> data_hash = Tunnelator.getTunnelator().createTunnel(urladdress, port, "run isabel"); 	 
				if(data_hash!= null && !data_hash.isEmpty())
				{					
					private_ip = data_hash.get("SERVER_PRIVATE_IP");
					outTraceFile.println("Tunnel created, private ip: " + private_ip);
				}
				else
				{
					//try {
					outTraceFile.println("Tunnel NOT created, private ip blank ");
						private_ip = "";					
						//writeErrorMessage ("localhost","Starting Tunnel","Error trying to start an open vpn tunnel to the url " + urladdress,"Error trying to start an open vpn tunnel to the url " + urladdress + ".\n Contact your network administrator and check that there is no firewall blocking this connection",true,Constants.CONNECT_TO_XERL);
					//} catch (Exception e) {					
					//	e.printStackTrace();
					//}	
					//return null;
				}					
			}			
		}
		else
		{
			outTraceFile.println("Somos el master");
			//si en MCAST_ROOT pone 0.0.0.0 lo quito y lo dejo vacío porque si no Santi no arranca Isabel
			if(lista.getSite(id).getCONNECTIONMODE()!=null && lista.getSite(id).getCONNECTIONMODE().getMulticast()!=null)
			{
				outTraceFile.println("Es conexion multicast, quito 0.0.0.0 si lo había");
				lista.getSite(id).getCONNECTIONMODE().getMulticast().getMulticastParameters().setMCASTROOT("");	
				rXEDL.save();
			}
		}
		String[] transformParams = {id,xedl,private_ip};

		XEDLsplitter.main(transformParams);

		logStdErr.stop();

		xlimNotif.println("Running isabel_launch with sesion " + session + " and id " + id);

		outTraceFile.println("Running isabel_launch with sesion " + session + " and id " + id);

		// Si no esta creada en la agenda la creo
		Agenda agenda = new Agenda(Constants.ISABEL_USER_DIR + Constants.FILE_SEPARATOR);
		// Si es una sesion creada al lanzar el sitio delde el editor la borro
		if (agenda.isLocalSession(session) || agenda.isRemoteSession(session)) {
			File autoFile = agenda.getAutomaticSessionFile(session);
			if (autoFile.exists())
				agenda.deleteSession(session);
		}
		// Si no esta creada en la agenda la creo            
		if (!agenda.isLocalSession(session) && !agenda.isRemoteSession(session)) {   	 
			// Si soy el master
			if (master.equals(id)) {
				agenda.createSession(session, true);
				agenda.getAutomaticSessionFile(session).createNewFile();
				rXEDL.save( agenda.getOriginalXedl(session).getAbsolutePath()); 
				XEDL local = new XEDL();
				SiteList listilla = new SiteList(local);
				listilla.addSite(lista.getSite(id));
				local.save( agenda.getLocalXedl(session).getAbsolutePath());
			}
			// Si no soy el master
			else {
				agenda.createSession(session, false);
				agenda.getAutomaticSessionFile(session).createNewFile();
				rXEDL.save( agenda.getFullXedl(session).getAbsolutePath());
				XEDL local = new XEDL();
				SiteList listilla = new SiteList(local);           
				listilla.addSite(lista.getSite(id));
				local.save(agenda.getLocalXedl(session).getAbsolutePath());  
				PrintWriter urlFile = new PrintWriter(new FileWriter(agenda.getURLFile(session)));           
				urlFile.println("isabel://"+urladdress+"/"+session);
				urlFile.flush();
				urlFile.close();
			}
		}   
		// Finalmente llamamos a ISABEL
		System.out.println("Execute isabel_launch command: " + "isabel_launch "+session+" "+id+" > "+Constants.ISA_LAUNCH_LOG+" 2>&1");
		String[] argsToProc = new String[]{"sh", "-c", "isabel_launch "+session+" "+id+" > "+Constants.ISA_LAUNCH_LOG+" 2>&1"};
		Process extAppl = Runtime.getRuntime().exec(argsToProc);
		/*
     try {
		extAppl.waitFor();
	  } catch (InterruptedException e) {
		e.printStackTrace(outTraceFile);
	  }
		 */
		xlimNotif.println("Local launch finished.");
		return extAppl;
	}







	// --------------------------------------------------------------------------------------------------

	// ------------------ Stop Isabel Local------------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**

	 * Indica si la direccion IP/host que se pasa como parametro pertenece a la maquina local o no

	 * @param session nombre de la sesion que se quiere parar

	 * @param id nombre del identificador del sitio que queremos parar

	 * @param xedl nombre del fichero XEDL que contiene la descripcion del sitio

	 * @throws IOException si hubo problemas.

	 */

	static public void stopLocalFromXedl(String session, String id, String xedl) throws IOException, XEDLException{

		// De momento llamamos a isabel -xedlconf

		XEDL rXEDL = new XEDL (xedl);

		SiteList rXEDLSiteList = new SiteList(rXEDL);

		SITE rlocalSite = rXEDLSiteList.getSite(id);

		String role = rlocalSite.getSITEROLE();

		// MODOS ACTUALES:

		if ((role.equalsIgnoreCase("interactive")) || (role.equalsIgnoreCase("mcu")) || (role.equalsIgnoreCase("sipgateway")) || (role.equalsIgnoreCase("watchpoint")) || (role.equalsIgnoreCase("mediaserver"))) {

			stopIsabel();

		}

		if (role.equalsIgnoreCase("recorder")) {

			// Paramos el Recorder

		}

		if (role.equalsIgnoreCase("antenna")) {

			// Paramos la antenna

		}    

	}

	// --------------------------------------------------------------------------------------------------





	static public void stopIsabel() throws IOException {

		// Hacemos ISABEL_CLEANUP

		//String[] argsToProc = new String[]{"sh", "-c", ISABEL_CLEAN_UP + " > /dev/null"};

		String[] argsToProc = new String[]{"sh", "-c", Constants.ISABEL_CLEAN_UP+" > $HOME/.xlim/tmp/stop_isabel.log 2>&1"};

		Process extAppl = Runtime.getRuntime().exec(argsToProc);

	}





	// --------------------------------------------------------------------------------------------------

	// ------------------ CHECK EXPRESSION IN FILE ------------------------------------------------------

	// --------------------------------------------------------------------------------------------------



	/**

	 * Comprueba si existen en el fichero especificado alguna de las expresiones

	 * @param fileOutName fichero donde queremos buscar las expresiones

	 * @param regExpr expresiones que queremos comprobar

	 * @return true si en la salida del proceso encuentra el string regExpr; en caso contrario devuelve false

	 * @Author lailoken

	 */

	static public boolean checkExprInFile (String fileOutName, String[] regExpr) throws IOException,FileNotFoundException {

		boolean result = false;

		LineNumberReader procOutLine = new LineNumberReader(new FileReader(fileOutName));

		String newLine = "";

		while ((newLine = procOutLine.readLine()) != null) {

			// Comprobamos si aparecen en el fichero recibido las expresiones contenidas en regExpr

			for (int i=0; i<regExpr.length;i++) {

				if ((newLine.indexOf(regExpr[i]))!=-1) {

					result = true;

				}

			}

		}

		// Terminamos

		return result;

	}

	// --------------------------------------------------------------------------------------------------







	// --------------------------------------------------------------------------------------------------

	// ------------------ EXEC PROCESS(antiguo GET RESULT PROCESS) ------------------------------------------------------------

	// --------------------------------------------------------------------------------------------------



	/**

	 * Guarda la salida del proceso process en el fichero fileName.

	 * @param Process Proceso que queremos ejecutar

	 * @param fileName  Nombre del fichero donde escribimos el resultado del proceso

	 * @param regExpr Expresion regular que queremos encontrar en la salida estandar

	 * @param delFile Indica si queremos borrar el fichero antes de escribir en el.

	 * @return true si en la salida del proceso encuentra el string regExpr; en caso contrario devuelve false

	 * @Author lailoken

	 */

	static public boolean execProcess (String process, String fileName, String[] regExpr, boolean delFile) throws IOException,InterruptedException,FileNotFoundException{

		if (process == null) {return true;}

		boolean result = false;

		//System.out.println (" Ejecutando....: "+RPM_COMMAND + program);

		String[] argsToProc;

		if (Constants.OS.indexOf("Windows")== -1) argsToProc = new String[]{"sh", "-c", process + " > " + fileName}; // Sistema Linux

		else argsToProc = new String[]{"cmd.exe", "/c", process + " > " + fileName}; // Sistema Windows

		Process extAppl = Runtime.getRuntime().exec(argsToProc);

		extAppl.waitFor();

		if (extAppl.exitValue()!=0) result = true;

		// Y ahora comprobamos si ese fichero es un xerl

		LineNumberReader procOutLine = new LineNumberReader(new FileReader(fileName));

		String newLine = "";

		while ((newLine = procOutLine.readLine()) != null) {

			// Comprobamos si aparecen en el fichero recibido las expresiones contenidas en regExpr

			for (int i=0; i<regExpr.length;i++) {

				if ((newLine.indexOf(regExpr[i]))!=-1) {

					result = true;

				}

			}

			//System.out.println("Leido: "+ newLine);

			//result = result + newLine + "\r\n";

		}

		return result;

	}





	/**

	 * Guarda la salida del proceso process en el fichero fileName.

	 * @param Process Proceso cuya salida estandar queremos escribir

	 * @param fileName Nombre del fichero donde escribimos el resultado del proceso

	 * @param regExpr Expresion regular que queremos encontrar en la salida estandar

	 * @param delFile Indica si queremos borrar el fichero antes de escribir en el.

	 * @return true si en la salida del proceso encuentra el string regExpr; en caso contrario devuelve false

	 * @deprecated This method is not valid in Windows systems. Use execProcess instead.

	 */

	static public boolean getResultProcess (Process process, String fileName, String[] regExpr, boolean delFile) throws Exception{

		if (process == null) {return true;}

		try {

			// Donde mandamos los mensajes de trazas: 

			PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (Constants.XLIM_LOGS_WORK_DIR+Constants.FILE_SEPARATOR+"getResulProcess.log")),true);

			//DataInputStream extApplIn = new DataInputStream(process.getInputStream());

			BufferedReader extApplIn = new BufferedReader (new InputStreamReader(process.getInputStream()));

			//DataOutputStream extApplOut = new DataOutputStream(process.getOutputStream());

			//DataInputStream extApplErr = new DataInputStream(process.getErrorStream());

			BufferedReader extApplErr = new BufferedReader (new InputStreamReader(process.getErrorStream()));

			String extApplStr="";

			String extApplStrErr="";

			PrintWriter outFileWriter = new PrintWriter (new BufferedWriter (new FileWriter (fileName,!delFile)),true);

			boolean result = false;

			outTraceFile.println("Old getResultProcess....");

			while (((extApplStr = extApplIn.readLine()) != null) || ((extApplStrErr = extApplErr.readLine()) != null)) {

				//outFileWriter.println (extApplStr);

				//outTraceFile.println("Esto es lo que nos da el proceso: "+extApplStr);

				//outTraceFile.println(extApplStr);

				//outTraceFile.println("Esto es lo que nos da la salida de error del proceso: "+extApplStrErr);

				if (extApplStr != null) outFileWriter.println (extApplStr);

				//if ((extApplStrErr != null) && (extApplStrErr != "")) outFileWriter.println (extApplStrErr);

				for (int i=0; i<regExpr.length;i++) {

					if ((extApplStr.indexOf(regExpr[i]))!=-1) {

						result = true;

					}

				}

			}

			return result;

		} catch (Exception e) {

			try {

				// Donde mandamos los mensajes de trazas: 

				PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (Constants.XLIM_LOGS_WORK_DIR+Constants.FILE_SEPARATOR+"getResulProcess.log")),true);

				outTraceFile.println("Captured Exception: "+e);

			} catch (Exception ee) {

				System.out.println ("No nos sale bien ni un misero printwriter!!!! :'(");

			}

			return true;

		}

	}



	/**

	 * Guarda la salida del proceso process en el fichero asociado al Printwriter fileWriter.

	 * @param process Proceso cuya salida estandar queremos escribir

	 * @param fileWriter writer del fichero donde escribimos el resultado del proceso

	 * @param regExpr Expresion regular que queremos encontrar en la salida estandar

	 * @return true si en la salida del proceso encuentra el string regExpr; en caso contrario devuelve false

	 * @deprecated This method is not valid in Windows systems. Use execProcess instead.

	 */

	static public boolean getResultProcess (Process process, PrintWriter fileWriter, String[] regExpr) throws Exception{

		if (process == null) {return true;}

		try {

			// Donde mandamos los mensajes de trazas: 

			PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (Constants.XLIM_LOGS_WORK_DIR+Constants.FILE_SEPARATOR+"getResulProcess.log")),true);

			//DataInputStream extApplIn = new DataInputStream(process.getInputStream());

			BufferedReader extApplIn = new BufferedReader (new InputStreamReader(process.getInputStream()));

			DataOutputStream extApplOut = new DataOutputStream(process.getOutputStream());

			DataInputStream extApplErr = new DataInputStream(process.getErrorStream());

			String extApplStr="";

			String extApplStrErr="";

			boolean result = false;

			while ((extApplStr = extApplIn.readLine()) != null) {

				//fileWriter.println (extApplStr);

				//outTraceFile.println("Esto es lo que nos da la salida estandar del proceso: "+extApplStr);

				//outTraceFile.println("Esto es lo que nos da la salida de error del proceso: "+extApplStrErr);

				if (extApplStr != null) fileWriter.println (extApplStr);

				if ((extApplStrErr != null) && (extApplStrErr != "")) fileWriter.print (extApplStrErr);

				for (int i=0; i<regExpr.length;i++) {

					if ((extApplStr.indexOf(regExpr[i]))!=-1) {

						result = true;

					}

				}

				//        if ((extApplStr.indexOf(regExpr))!=-1) {

				//          result = true;

				//        }

			}

			return result;

		} catch (Exception e) {

			try {

				// Donde mandamos los mensajes de trazas: 

				PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (Constants.XLIM_LOGS_WORK_DIR+Constants.FILE_SEPARATOR+"getResulProcess.log")),true);

				outTraceFile.println("Captured Exception: "+e);

			} catch (Exception ee) {

				System.out.println ("No nos sale bien ni un misero printwriter!!!! :'(");

			}

			return true;

		}

	}



	// --------------------------------------------------------------------------------------------------






	// --------------------------------------------------------------------------------------------------

	// ------------------ MANEJAR EL HISTORIAL DEL CONNECT TO Y STAR SESSION SERVER --------------------

	// --------------------------------------------------------------------------------------------------


	/**
	 * this method adds a new entry to the history file given,
	 * the history file has NUMBER_OF_ENTRIES entries
	 * @param url the url to add
	 * @param file the file to add
	 * @return true if it wa all ok or false if there were a problem
	 * @Author  Enrique
	 */
	static public boolean add_history_entry(String url, String file)
	{
		try {
			File the_file = new File(file);
			if(!the_file.exists())
				the_file.createNewFile();
			FileReader file_reader = new FileReader (file);
			BufferedReader buf_reader = new BufferedReader (file_reader);
			//fichero nuevo en el que voy añadiendo las líneas según las leo, hasta 10
			String file_temp = file + ".tmp";	      
			FileOutputStream out = new FileOutputStream(file_temp);
			PrintStream print_out = new PrintStream(out);
			/*ahora las nuevas las añado al principio, así que siempre modifico el fichero
	      if(!is_new_entry(url, file))
	      {
	    	  //si la entrada es antigua no la añadimos porque ya la tenemos, simplemente salimos
	    	  buf_reader.close ();
		      print_out.close();
		      File temporal = new File (file_temp);
		      temporal.delete();
		      return true;
	      }
			 */
			//la primera será la url nueva
			print_out.println(url);
			int num_lines = 1;
			while(true)
			{
				String line = buf_reader.readLine();
				if (line == null || num_lines >= Constants.NUMBER_OF_ENTRIES) break;
				num_lines++;
				if(num_lines <= Constants.NUMBER_OF_ENTRIES && !line.equals(url))
				{	        	  
					print_out.println(line);
				}
			}
			copyFile(file_temp, file);

			buf_reader.close ();
			print_out.close();
			File temporal = new File (file_temp);
			temporal.delete();
		}
		catch (IOException e)
		{
			System.out.println("Excepcion..." + e.toString() + " " + e.getMessage());
			return false;
		} 	  
		return true;
	}



	private static boolean is_new_entry(String url, String file)
	{
		FileReader file_reader;
		try {
			file_reader = new FileReader (file);
			BufferedReader buf_reader = new BufferedReader (file_reader);
			String line = null;
			while((line=buf_reader.readLine())!=null)
			{
				if(line.equals(url))
				{
					buf_reader.close();			
					return false;
				}
			}
			buf_reader.close();			
			return true;
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			return false;
		}catch (IOException e) {
			e.printStackTrace();	
			return false;
		}	      
	}



	/**
	 * this method adds a new entry to the history file given,
	 * the history file has NUMBER_OF_ENTRIES entries
	 * @param name the name to add
	 * @param service the service to add
	 * @param quality the quality to add
	 * @param file the file to add
	 * @return true if it wa all ok or false if there were a problem
	 * @Author  Enrique
	 */
	static public boolean add_history_entry_sss(String name, String service, String quality, String file)
	{
		try {
			//fichero nuevo en el que voy añadiendo las líneas
			String file_temp = file + ".tmp";	      
			FileOutputStream out = new FileOutputStream(file_temp);
			PrintStream print_out = new PrintStream(out);  	      
			//la primera será el nombre, despues service y luego quality 
			print_out.println(name);
			print_out.println(service);
			print_out.println(quality);  	      
			copyFile(file_temp, file);

			print_out.close();
			File temporal = new File (file_temp);
			temporal.delete();
		}
		catch (IOException e)
		{
			System.out.println("Excepcion..." + e.toString() + " " + e.getMessage());
			return false;
		} 	  
		return true;
	}

	/**
	 * method to get the name of the last session from the history
	 * @return the session name
	 */
	static public String get_name_from_history(){
		try {
			String history_file = Constants.ISABEL_SESSIONS_DIR + Constants.FILE_SEPARATOR + Constants.FICHERO_HISTORIAL_SSS;
			File the_file = new File(history_file);
			if(!the_file.exists())
				return Constants.NOMBRE_SESION_POR_DEFECTO;
			FileReader file_reader = new FileReader (history_file);
			BufferedReader buf_reader = new BufferedReader (file_reader);
			String line = buf_reader.readLine();
			return line;
		}
		catch (IOException e)
		{
			System.out.println("Excepcion..." + e.toString() + " " + e.getMessage());
			return Constants.NOMBRE_SESION_POR_DEFECTO;
		}
	}

	/**
	 * method to get the service of the last session from the history
	 * @return the session name
	 */
	static public String get_service_from_history(){
		try {
			String history_file = Constants.ISABEL_SESSIONS_DIR + Constants.FILE_SEPARATOR + Constants.FICHERO_HISTORIAL_SSS;
			File the_file = new File(history_file);
			if(!the_file.exists())
				return Constants.SERVICIO_SESION_POR_DEFECTO;
			FileReader file_reader = new FileReader (history_file);
			BufferedReader buf_reader = new BufferedReader (file_reader);
			buf_reader.readLine();
			//es la segunda linea
			String line = buf_reader.readLine();
			return line;
		}
		catch (IOException e)
		{
			System.out.println("Excepcion..." + e.toString() + " " + e.getMessage());
			return Constants.SERVICIO_SESION_POR_DEFECTO;
		}
	}


	/**
	 * method to get the quality of the last session from the history
	 * @return the session name
	 */
	static public String get_quality_from_history(){
		try {
			String history_file = Constants.ISABEL_SESSIONS_DIR + Constants.FILE_SEPARATOR + Constants.FICHERO_HISTORIAL_SSS;
			File the_file = new File(history_file);
			if(!the_file.exists())
				return Constants.CALIDAD_SESION_POR_DEFECTO;
			FileReader file_reader = new FileReader (history_file);
			BufferedReader buf_reader = new BufferedReader (file_reader);
			buf_reader.readLine();
			buf_reader.readLine();
			//es la tercera linea
			String line = buf_reader.readLine();
			return line;
		}
		catch (IOException e)
		{
			System.out.println("Excepcion..." + e.toString() + " " + e.getMessage());
			return Constants.CALIDAD_SESION_POR_DEFECTO;
		}
	}

	// --------------------------------------------------------------------------------------------------

	// ------------------ COPIAR FICHEROS ---------------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**

	 * Copia el fichero origen en el fichero destino.

	 * @param fileInName Path del fichero origen.

	 * @param fileOutName Path del fichero destino.

	 * @return True, si la copia tuvo �xito; False, en caso contrario

	 * @Author lailoken

	 */

	static public boolean copyFile (String fileInName, String fileOutName){

		try {

			FileInputStream in = new FileInputStream(fileInName);

			FileOutputStream out = new FileOutputStream(fileOutName);

			byte buffer[] = new byte[16];

			int n;

			while ((n=in.read(buffer))>-1)

				out.write(buffer,0,n);

			out.close();

			in.close();

			return true;

		} catch (Exception e) {

			return false;

		}

	}



	/**

	 * Copia el fichero origen en el fichero destino.

	 * @param fileInName Path del fichero origen.

	 * @param out Outputstream del fichero destino.

	 * @return True, si la copia tuvo �xito; False, en caso contrario

	 * @Author lailoken

	 */

	static public boolean copyFile (String fileInName, OutputStream out){

		try {

			FileInputStream in = new FileInputStream(fileInName);

			byte buffer[] = new byte[16];

			int n;

			while ((n=in.read(buffer))>-1)

				out.write(buffer,0,n);

			out.close();

			in.close();

			return true;

		} catch (Exception e) {

			return false;

		}

	}

	// --------------------------------------------------------------------------------------------------













	// --------------------------------------------------------------------------------------------------

	// ------------------ COMPRIMIR / DESCOMPRIMIR FICHEROS ---------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**

	 * Introduce un fichero en el stream de zip.

	 * @param file Fichero que queremos introducir en el stream

	 * @param out stream de zip donde metemos el fichero que queremos comprimir

	 * @Author lailoken

	 */

	/*  static private void zipOneFile (File file, ZipOutputStream out) throws IOException, FileNotFoundException{

    // Donde mandamos los mensajes de trazas: 

    PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (XLIM_LOGS_WORK_DIR+Servutilities.FILE_SEPARATOR+"zip.log",true)),true);

    if (file.isDirectory()) {

      outTraceFile.println ("Zipping..." + file.getAbsolutePath());

       //Create an array with all of the files and subdirectories

       //of the current directory.

       String[] fileNamesInDir = file.list();

       if (fileNamesInDir != null) {

          //Recursively add each array entry to make sure that we get 

          //subdirectories as well as normal files in the directory.

          for (int i=0; i<fileNamesInDir.length; i++)  {

             zipOneFile(new File(file, fileNamesInDir[i]),out);

          }

       }

    }

    //Otherwise, a file so add it as an entry to the Zip file.

    else {

      //----- For each file --

      outTraceFile.println ("Zipping..." + file.getAbsolutePath());

      byte[] buf = new byte[1024];

      FileInputStream in = new FileInputStream(file);

      // Add ZIP entry to output stream.

      out.putNextEntry(new ZipEntry(file.toString()));

      // Transfer bytes from the file to the ZIP file

      int len;

      while ((len = in.read(buf)) > 0) {

        out.write(buf, 0, len);

      }

      // Complete the entry

      out.closeEntry();

      in.close();

      //----------------------

    }

  }





  /**

	 * Comprime los ficheros especificados en un unico fichero.

	 * @param outFilename Path del fichero comprimido resultante.

	 * @param filenames Path de los ficheros que se quieren comprimir

	 * @return true si la compresi�n tuvo �xito; false, en caso contrario.

	 * @Author lailoken

	 */

	static public boolean zipFile (String outFilename,String[] filenames){

		try {

			ZipUtils.zip(outFilename, filenames,true);

			return true;

		} catch (Exception e) {

			return false;

		}

	}

	/*  static public boolean zipFile (String outFilename,String[] filenames){

    // Create a buffer for reading the files

    // Donde mandamos los mensajes de trazas: 

    PrintWriter outTraceFile = null;

    try {

      outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (XLIM_LOGS_WORK_DIR+Servutilities.FILE_SEPARATOR+"zipfile.log")),true);

      outTraceFile.println("outFilename: "+ outFilename);

      // Create the ZIP file

      ZipOutputStream out = new ZipOutputStream(new FileOutputStream(outFilename));

      // Compress the files

      for (int i=0; i<filenames.length; i++) {

        // File asociated to this file...

        outTraceFile.println("filenames[" +i+"]: "+ filenames[i]);

        File file = new File(filenames[i]);

        zipOneFile (file,out);

      }

      // Complete the ZIP file

      out.close();

      return true;

    } catch (IOException e) {

      outTraceFile.println("Problems zipping: " + e);

      return false;

    }

  }





  /**

	 * Descomprime un fichero zip.

	 * @param zipFileName Nombre del fichero ZIP a descomprimir.

	 * @param unzipDir Directorio donde queremos guardar los ficheros descomprimidos. 

	 * Los ficheros se descomprimen en el directorio sin paths absolutos.

	 * @return Path de los ficheros descomprimidos.

	 * @Author lailoken

	 */

	static public String[] unzipFile (String zipFileName,String unzipDir){

		try {

			return ZipUtils.unzip(zipFileName, unzipDir, false);

		} catch (Exception e) {

			return null;

		}

	}

	/*  static public String[] unzipFile (String zipFileName,String unzipDir){

    try {

      // Si no existe el directorio donde hay que descomprimir, lo creamos...

      File unzipDirFile = new File (unzipDir);

      unzipDirFile.mkdirs();



      ZipFile zipFile = new ZipFile(zipFileName);

      //for (Enumeration e = zipFile.entries() ; e.hasMoreElements() ;) {

      //  System.out.println("VAlor de las enumeraciones: " + e.nextElement());

      //}

      String[] fileNames = new String[zipFile.size()];

      //System.out.println("Numero de entries: " + zipFile.size());



      // Open the ZIP file

      ZipInputStream in = new ZipInputStream(new FileInputStream(zipFileName));



      for (int i=0;i<zipFile.size();i++) {

        // Get the next entry

        ZipEntry entry = in.getNextEntry();

        //System.out.println("Nombre de la entry: " + entry.getName());

        String entryName = entry.getName().substring(entry.getName().lastIndexOf(Servutilities.FILE_SEPARATOR)+1,entry.getName().length());

        // Metemos el nombre del fichero descomprimido en el array que vamosa devolver

        fileNames[i] = entryName;

        //System.out.println("Donde copiamos el fichero descomprimido: "+entryName);

        // Open the output file

        String outFilename = unzipDir+Servutilities.FILE_SEPARATOR+entryName;

        OutputStream out = new FileOutputStream(outFilename);

        // Transfer bytes from the ZIP file to the output file

        byte[] buf = new byte[1024];

        int len;

        while ((len = in.read(buf)) > 0) {

          out.write(buf, 0, len);

        }

        // Close the streams

        out.close();

      }

      in.close();

      return fileNames;

    } catch (IOException e) {

      System.out.println("ZIP Exception: "+e);

      return null;

    }

  }





  /**

	 * Descomprime un fichero zip.

	 * @param zipFileName Nombre del fichero ZIP a descomprimir.

	 * @param unzipDir Directorio donde queremos guardar los ficheros descomprimidos. 

	 * @param includePaths: indica si queremos que se mantengan los paths de los ficheros comprimidos

	 * @return Path de los ficheros descomprimidos.

	 * @Author lailoken

	 */

	static public String[] unzipFile (String zipFileName,String unzipDir,boolean includePaths){

		try {

			return ZipUtils.unzip(zipFileName, unzipDir, includePaths);

		} catch (Exception e) {

			return null;

		}

	}

	/*  static public String[] unzipFile (String zipFileName,String unzipDir,boolean includePaths){

    try {

      // Si no existe el directorio donde hay que descomprimir, lo creamos...

      File unzipDirFile = new File (unzipDir);

      unzipDirFile.mkdirs();



System.out.println("abriendo .zip");

      ZipFile zipFile = new ZipFile(zipFileName);

System.out.println("abierto .zip");

      //for (Enumeration e = zipFile.entries() ; e.hasMoreElements() ;) {

      //  System.out.println("VAlor de las enumeraciones: " + e.nextElement());

      //}

      String[] fileNames = new String[zipFile.size()];

      //System.out.println("Numero de entries: " + zipFile.size());



      // Open the ZIP file

      ZipInputStream in = new ZipInputStream(new FileInputStream(zipFileName));



      for (int i=0;i<zipFile.size();i++) {

        // Get the next entry

        ZipEntry entry = in.getNextEntry();

        //System.out.println("Nombre de la entry: " + entry.getName());

        String entryName;

        if (!includePaths) {

          entryName = entry.getName().substring(entry.getName().lastIndexOf(Servutilities.FILE_SEPARATOR)+1,entry.getName().length());

        } else {

          // Nos aseguramos de que existen los directorios absolutos

          try {

            String dirName = unzipDir+Servutilities.FILE_SEPARATOR+entry.getName().substring(0,entry.getName().lastIndexOf(Servutilities.FILE_SEPARATOR));

            System.out.println("Directorio a comprobar: " + dirName);

            File dirNameFile = new File(dirName);

            dirNameFile.mkdirs();

          } catch (Exception e) {

          }

          entryName = entry.getName();

        }

        // Metemos el nombre del fichero descomprimido en el array que vamosa devolver

        System.out.println("Fichero descomprimido. " + entryName);

        fileNames[i] = entryName;

        //System.out.println("Donde copiamos el fichero descomprimido: "+entryName);

        // Open the output file

        String outFilename = unzipDir+Servutilities.FILE_SEPARATOR+entryName;

        OutputStream out = new FileOutputStream(outFilename);

        // Transfer bytes from the ZIP file to the output file

        byte[] buf = new byte[1024];

        int len;

        while ((len = in.read(buf)) > 0) {

          out.write(buf, 0, len);

        }

        // Close the streams

        out.close();

      }

      in.close();

      return fileNames;

    } catch (IOException e) {

      System.out.println("ZIP Exception: "+e);

      return null;

    }

  }





  /**

	 * Descomprime un fichero zip.

	 * @param zipFileName Nombre del fichero ZIP a descomprimir.

	 * @param unzipDir Directorio donde queremos guardar los ficheros descomprimidos. 

	 * @param relativeDir: Directorio relativo que queremos conservar de los ficheros comprimidos 

	 * @return Path de los ficheros descomprimidos.

	 * @Author lailoken

	 */

	/*  static public String[] unzipFile (String zipFileName,String unzipDir,String relativeDir){

    try {

            // Donde mandamos los mensajes de trazas: 

      PrintWriter outTraceFile = new PrintWriter (new BufferedWriter (new FileWriter (XLIM_LOGS_WORK_DIR+Servutilities.FILE_SEPARATOR+"unzip.log")),true);



      // Si no existe el directorio donde hay que descomprimir, lo creamos...

      File unzipDirFile = new File (unzipDir);

      unzipDirFile.mkdirs();



      ZipFile zipFile = new ZipFile(zipFileName);

      //for (Enumeration e = zipFile.entries() ; e.hasMoreElements() ;) {

      //  System.out.println("VAlor de las enumeraciones: " + e.nextElement());

      //}

      String[] fileNames = new String[zipFile.size()];

      //System.out.println("Numero de entries: " + zipFile.size());



      // Open the ZIP file

      ZipInputStream in = new ZipInputStream(new FileInputStream(zipFileName));



      for (int i=0;i<zipFile.size();i++) {

        // Get the next entry

        ZipEntry entry = in.getNextEntry();

        //System.out.println("Nombre de la entry: " + entry.getName());

        String entryName;



        // Quitamos parte del path para manejar paths relativos (en lugar de absolutos, que es lo que se guarda en el ZIP)

        int startCont = entry.getName().indexOf(relativeDir);

        String relativeName;

        boolean relativeDirs = false;

        if (startCont != -1) {

          relativeName = entry.getName().substring(startCont,entry.getName().length());

          outTraceFile.println("Valor de relativeName: " + relativeName);

          relativeDirs = true;

        } else {

          relativeName = entry.getName().substring(entry.getName().lastIndexOf(Servutilities.FILE_SEPARATOR)+1,entry.getName().length());

          outTraceFile.println("Valor de relativeName: " + relativeName);

        }



        // Nos aseguramos de que existen los directorios absolutos

        try {

          if (relativeDirs) {

            String dirName = unzipDir + Servutilities.FILE_SEPARATOR + relativeName.substring(0,relativeName.lastIndexOf(Servutilities.FILE_SEPARATOR));

            System.out.println("Directorio a comprobar: " + dirName);

            File dirNameFile = new File(dirName);

            dirNameFile.mkdirs();

          }

        } catch (Exception e) {

        }

        entryName = relativeName;



        // Metemos el nombre del fichero descomprimido en el array que vamosa devolver

        System.out.println("Fichero descomprimido. " + entryName);

        fileNames[i] = entryName;

        //System.out.println("Donde copiamos el fichero descomprimido: "+entryName);

        // Open the output file

        String outFilename = unzipDir + Servutilities.FILE_SEPARATOR + entryName;

        OutputStream out = new FileOutputStream(outFilename);

        // Transfer bytes from the ZIP file to the output file

        byte[] buf = new byte[1024];

        int len;

        while ((len = in.read(buf)) > 0) {

          out.write(buf, 0, len);

        }

        // Close the streams

        out.close();

      }

      in.close();

      return fileNames;

    } catch (IOException e) {

      System.out.println("ZIP Exception: "+e);

      return null;

    }

  }

  // --------------------------------------------------------------------------------------------------

	 */  



	//--------------------------------------------------------------------------------------------------

	// ------------------ TRABAJAR CON EL PASSWORD -------------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**
	 * método para chequear un passwd que nos llega
	 * @param serverId id del server o master
	 * @param serverPasswd passwd encriptada del server
	 * @param siteId id del site que queremos comprobar
	 * @param sitePasswd passwd encriptado del site que queremos comprobar
	 * @return si el passwd es correcto o no
	 */
	public static boolean checkPasswd(String serverId, String serverPasswd, String siteId, String sitePasswd) throws Exception
	{
		// ----------------> COMPROBACIOIN AUTENTICACION SOLICITUD (PASSWD)
		try {
			boolean autenticationOK = false;
			if(serverPasswd == null ) return true;

			//si el passwd recibido es null (no hay) y llegamos aquí (si hay passwd de sesion) es que esta mal autenticado
			if(sitePasswd == null)  return false;

			String cipher_key_server = Constants.KEY_CODE + serverId;
			DesEncrypter encrypter_server = new DesEncrypter(cipher_key_server);
			String passwd_session = encrypter_server.decrypt(serverPasswd);

			String cipher_key_site = Constants.KEY_CODE + siteId;
			DesEncrypter encrypter_site = new DesEncrypter(cipher_key_site);
			String passwd_site = encrypter_site.decrypt(sitePasswd);

			if (passwd_site.equals(passwd_session)) autenticationOK = true;

			return autenticationOK;	      
		} catch (Exception e) {
			e.printStackTrace();
			throw new Exception(e.getMessage());
		}	
	}



	/**
	 * método para chequear un passwd que introduce un usuario en claro
	 * @param serverId id del server o master
	 * @param serverPasswd passwd encriptada del server
	 * @param siteId id del site que queremos comprobar
	 * @param sitePasswd passwd encriptado del site que queremos comprobar
	 * @return si el passwd es correcto o no
	 */
	public static boolean checkClearPasswd(String serverId, String serverPasswd, String clearPasswd) 
	{
		// ----------------> COMPROBACIOIN AUTENTICACION SOLICITUD (PASSWD)  
		boolean autenticationOK = false;
		if(serverPasswd == null ) return true;

		//si el passwd recibido es null (no hay) y llegamos aquí (si hay passwd de sesion) es que esta mal autenticado
		if(clearPasswd == null)  return false;

		String cipher_key_server = Constants.KEY_CODE + serverId;
		DesEncrypter encrypter_server = new DesEncrypter(cipher_key_server);
		String passwd_session = encrypter_server.decrypt(serverPasswd);

		String pass =  "";
		if(!passwd_session.equals(""))
		{
			pass = passwd_session.substring(0,passwd_session.length()-1);    
		}
		if (clearPasswd.equals(passwd_session) || clearPasswd.equals(pass)) autenticationOK = true;

		return autenticationOK;	      

	}

	/**
	 * metodo para encriptar el passwd
	 * @param clearPasswd passwd en claro
	 * @param siteID nombre del site para el que se encripta
	 * @return passwd encriptado, null si clearPasswd era null
	 */
	public static String encryptPasswd(String clearPasswd, String siteId)
	{
		if(clearPasswd == null)
			return null;
		String cipher_key_server = Constants.KEY_CODE + siteId;
		DesEncrypter encrypter = new DesEncrypter(cipher_key_server);
		String passwd = encrypter.encrypt(clearPasswd);;
		return passwd;	    
	}

	/**
	 * metodo para desencriptar el passwd
	 * @param encryptedPasswd passwd encriptado
	 * @param siteID nombre del sitio para el que se desencripta
	 * @return passwd en claro, null si encryptedPasswd era null
	 */
	public static String decryptPasswd(String encryptedPasswd, String siteId)
	{
		if(encryptedPasswd == null)
			return null;
		String cipher_key_server = Constants.KEY_CODE + siteId;
		DesEncrypter encrypter = new DesEncrypter(cipher_key_server);
		String clear_passwd = encrypter.decrypt(encryptedPasswd);;
		return clear_passwd;	
	}

	// --------------------------------------------------------------------------------------------------

	// ------------------ GET LOCAL ADDRESS -------------------------------------------------------------

	// This method returns 127.0.X.X, not the ip

	// --------------------------------------------------------------------------------------------------

	static public InetAddress[] getLocalAddrs () throws UnknownHostException {

		// Primero sacamos el nombre de la maquina

		String host = InetAddress.getLocalHost().getCanonicalHostName();

		// Ahora cogemos todas las ips

		return InetAddress.getAllByName(host);

	}




	//--------------------------------------------------------------------------------------------------

	// ------------------ GET REAL IP ADDRESS-------------------------------------------------------------

	//this method returns the real ip not the local one (127.0.X.X), return null if some problem or no ip

	// --------------------------------------------------------------------------------------------------

	public static InetAddress getRealIPAdrress()
	{
		Enumeration interfaces = null;
		try {
			interfaces = NetworkInterface.getNetworkInterfaces();
		} catch (SocketException e) {		
			e.printStackTrace();
		}
		if (interfaces == null)
			return null;

		while (interfaces.hasMoreElements()) {
			NetworkInterface card = (NetworkInterface) interfaces.nextElement();
			Enumeration addresses = card.getInetAddresses();
			if (addresses == null)
				continue;

			while (addresses.hasMoreElements()) {
				InetAddress address = (InetAddress) addresses.nextElement();

				String temp = address.getHostAddress();
				if(!address.isLoopbackAddress() && !temp.startsWith("10.") && !temp.contains(":"))
				{
					//si no empieza por "127.0" o "10." devuelvo la ip, o si no tiene ":" que indica que es ipv6
					return address;
				}
			}
		}
		return null;
	}


	/**

	 * Calcula la direcci�n IP local

	 * @return Direcci�n IP local

	 * @Author lailoken

	 */

	static public String getLocalAddresses () throws UnknownHostException {

		//String host = InetAddress.getLocalHost().getHostName();

		String host = InetAddress.getLocalHost().getCanonicalHostName();

		//String host = InetAddress.getLocalHost().getHostAddress();

		//System.out.println("host: " + host);

		String address = host;

		String localAddress = "";

		//localAddress = InetAddress.getLocalHost().toString().split("/")[1];

		InetAddress inet = getRealIPAdrress();
		if(inet!=null)
		{
			localAddress = inet.toString();
		}
		else
		{
			localAddress = InetAddress.getLocalHost().getHostAddress();
		}

		address = address + "/" + localAddress;

		return address;

	}

	// --------------------------------------------------------------------------------------------------











	// --------------------------------------------------------------------------------------------------

	// ------------------ WRITE MESSAGE ERROR -----------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/**

	 * Presenta un mensaje de error por consola o en una ventana y genera un XERL

	 * @param msg Mensaje q queremos presentar

	 * @para error Error XERL asociado a un fichero

	 * @param errorService Nombre del servicio que genera el error

	 * @param IsGraphical Indica si queremos presentarlo en una ventana o en consola.

	 */

	static public void writeMsgError (String msg, String errorService, boolean IsGraphical) throws IOException, ParserConfigurationException, TransformerException, UnknownHostException, XERLException {

		// Creamos el error	
		XERL error = new XERL(getLocalAddresses(),errorService);



		// El fichero XERL se guarda en .isabel/tmp/xerl/sesion.xerl

		// Aqui comprobamos que existe el directorio xerl; si no existe, lo creamos

		File xerldir = new File(Constants.ISABEL_XERLDIR);

		xerldir.mkdir();

		error.setInfo(msg);

		if (IsGraphical) {

			// Sacamos un mensaje en una ventanita...

			JFrame parentFrame = new JFrame();

			JOptionPane.showMessageDialog(parentFrame, msg, errorService, JOptionPane.ERROR_MESSAGE);

			parentFrame.dispose();

		} else {

			error.writeError(System.err);

		}

		//System.out.println ("Aqui escribimos el error: "+ISABEL_XERLDIR +"/"+ ISABEL_XERLFILE);

		//String stringError = error.toString();

		//System.out.println("Antes de escribir el error y salir.");

		error.writeError(Constants.ISABEL_XERLDIR +Constants.FILE_SEPARATOR+ Constants.ISABEL_XERLFILE);

		System.exit(0);

	}



	/**

	 * Presenta un mensaje de error por consola o en una ventana y genera un XERL

	 * @param msg Mensaje q queremos presentar

	 * @param error Error XERL asociado a un fichero

	 * @param ISABEL_XERLDIR Directorio donde se almacena el XERL

	 * @param ISABEL_XERLFILE Nombre del fichero XERL generado

	 * @param errorService Nombre del servicio que genera el error

	 * @param IsGraphical Indica si queremos presentarlo en una ventana o en consola.

	 */

	static public void writeMsgError (String msg,String XERLDIR, String ISABEL_XERLFILE, String errorService, boolean IsGraphical) throws IOException, ParserConfigurationException, TransformerException, UnknownHostException, XERLException {

		// Creamos el error

		XERL error = new XERL(getLocalAddresses(), errorService);



		// Aqui comprobamos que existe el directorio xerl; si no existe, lo creamos

		File xerldir = new File(XERLDIR);

		xerldir.mkdir();

		error.setInfo(msg);

		if (IsGraphical) {

			// Sacamos un mensaje en una ventanita...

			JFrame parentFrame = new JFrame();

			JOptionPane.showMessageDialog(parentFrame, msg, errorService, JOptionPane.ERROR_MESSAGE);

			parentFrame.dispose();

		} else {

			error.writeError(System.err);

		}

		//String stringError = error.toString();

		//System.out.println(" Esto es lo que sacamos con la funcion toString: " + stringError);

		error.writeError(XERLDIR + Constants.FILE_SEPARATOR+ISABEL_XERLFILE);

		System.exit(0);

	}



	/**

	 * Presenta un mensaje de error por consola o en una ventana y genera un XERL

	 * @param msg Mensaje q queremos presentar

	 * @param error Error XERL asociado a un fichero

	 * @param XERLFILE Path completo del fichero XERL generado

	 * @param errorService Nombre del servicio que genera el error

	 * @param IsGraphical Indica si queremos presentarlo en una ventana o en consola.

	 */

	static public void writeMsgError (String msg, String XERLFILE, String errorService, boolean IsGraphical) throws IOException, ParserConfigurationException, TransformerException, UnknownHostException, XERLException  {

		//	 Creamos el error

		XERL error = new XERL(getLocalAddresses(),errorService);



		error.setInfo(msg);

		if (IsGraphical) {

			// Sacamos un mensaje en una ventanita...

			JFrame parentFrame = new JFrame();

			JOptionPane.showMessageDialog(parentFrame, msg, errorService, JOptionPane.ERROR_MESSAGE);

			parentFrame.dispose();

		} else {

			error.writeError(System.err);

		}

		//String stringError = error.toString();

		//System.out.println(" Esto es lo que sacamos con la funcion toString: " + stringError);

		error.writeError(XERLFILE);

		System.exit(0);

	}



	/**

	 * Presenta un mensaje de error por consola o en una ventana y genera un XERL

	 * @param errorServer Servidor del que hemos recibido el error

	 * @param errorService Servicio que produjo el error

	 * @param msg Mensaje q queremos presentar

	 * @param IsGraphical Indica si queremos presentarlo en una ventana o en consola.

	 * @param XERLFILE Path completo del fichero XERL generado

	 */

	static public void writeMsgError (String errorServer, String errorService, String msg, boolean IsGraphical, String XERLFILE) throws IOException, ParserConfigurationException, TransformerException, UnknownHostException, XERLException  {

		// Creamos el error

		//XERL error = new XERL(getLocalAddresses(),errorService);

		XERL error = new XERL(errorServer,errorService);



		error.setInfo(msg);

		if (IsGraphical) { 

			// Sacamos un mensaje en una ventanita...

			ErrorPanel errorPanel = new ErrorPanel();

			errorPanel.setServerContentLabel(errorServer);

			errorPanel.setServiceContentLabel(errorService);

			errorPanel.setErrorTextArea(msg);

			Object[] message = {errorPanel};



			//int resultPanel = JOptionPane.showConfirmDialog(null, message, errorService,JOptionPane.OK_CANCEL_OPTION,JOptionPane.QUESTION_MESSAGE);



			JFrame parentFrame = new JFrame();

			JOptionPane.showMessageDialog(parentFrame, message, errorService, JOptionPane.ERROR_MESSAGE);

			parentFrame.dispose();

		} else {

			error.writeError(System.err);

		}

		//String stringError = error.toString();

		//System.out.println(" Esto es lo que sacamos con la funcion toString: " + stringError);

		if (XERLFILE==null) {

			// Si es null, ponemos el XERL por defecto..

			XERLFILE = Constants.ISABEL_XERLDIR + Constants.FILE_SEPARATOR + errorService.trim() + Constants.FILE_SEPARATOR + Constants.ISABEL_XERLFILE;

		}

		error.writeError(XERLFILE);

		System.exit(0);

	}



	/**

	 * Presenta un mensaje de error por consola o en una ventana y genera un XERL

	 * @param msg Mensaje q queremos presentar

	 * @param XERLFILE Path completo del fichero XERL generado

	 * @param errorService Nombre del servicio que genera el error

	 * @param IsGraphical Indica si queremos presentarlo en una ventana o en consola.

	 */

	static public void writeSimpleErrorMessage (String msg, String XERLFILE, String errorService, boolean IsGraphical) throws IOException, ParserConfigurationException, TransformerException, UnknownHostException, XERLException  {

		// Creamos el error

		XERL error = new XERL(getLocalAddresses(),errorService);

		error.setInfo(msg);

		if (IsGraphical) {

			// Sacamos un mensaje en una ventanita...

			JFrame parentFrame = new JFrame();

			JOptionPane.showMessageDialog(parentFrame, msg, errorService, JOptionPane.ERROR_MESSAGE);

			parentFrame.dispose();

		} else {

			error.writeError(System.err);

		}

		//String stringError = error.toString();

		//System.out.println(" Esto es lo que sacamos con la funcion toString: " + stringError);

		error.writeError(XERLFILE);

		//System.exit(0);

	}





	/**

	 * Presenta un mensaje de error por consola o en una ventana y genera un XERL

	 * @param errorService Servicio que produjo el error.

	 * @param msg Mensaje q queremos presentar.

	 * @param errorDetails Detalles del mensaje que se presenta.

	 * @param IsGraphical Indica si queremos presentarlo en una ventana o en consola.

	 * @param XERLFILE Path completo del fichero XERL generado

	 */

	static public void writeErrorMessage (String errorService, String msg,String errorDetails, boolean IsGraphical, String XERLFILE) throws IOException, ParserConfigurationException, TransformerException, UnknownHostException, XERLException  {

		//	 Creamos el error

		writeErrorMessage(getLocalAddresses(),errorService,msg,errorDetails,IsGraphical, XERLFILE);

	}





	/**

	 * Presenta un mensaje de error por consola o en una ventana y genera un XERL

	 * @param errorServer Servidor del que hemos recibido el error.

	 * @param errorService Servicio que produjo el error.

	 * @param msg Mensaje q queremos presentar.

	 * @param errorDetails Detalles del mensaje que se presenta.

	 * @param IsGraphical Indica si queremos presentarlo en una ventana o en consola.

	 * @param XERLFILE Path completo del fichero XERL generado

	 */
	static public void writeErrorMessage (String errorServer, String errorService, String msg,String errorDetails, boolean IsGraphical, String XERLFILE) throws IOException, ParserConfigurationException, TransformerException, UnknownHostException, XERLException  {

		// Creamos el error

		//XERL error = new XERL(getLocalAddresses(),errorService);

		XERL error = new XERL(errorServer,errorService);



		if (errorDetails == null) error.setInfo(msg);

		else error.setInfo(msg + " - " + errorDetails);

		if (IsGraphical) { 

			// Sacamos un mensaje en una ventanita...

			final ErrorPanel errorPanel = new ErrorPanel();

			if (errorServer == null) errorPanel.setServerContentLabel("Localhost");

			else errorPanel.setServerContentLabel(errorServer);

			errorPanel.setServiceContentLabel(errorService);

			errorPanel.setErrorContentLabel(msg);

			errorPanel.setTextAreaDimension();

			//errorPanel.setBackground(Color.BLUE);
			//System.out.println("Valor de detalles: " +errorDetails);

			if (errorDetails == null) errorPanel.setErrorTextArea("No details found.");

			else errorPanel.setErrorTextArea(errorDetails);

			Object[] message = {errorPanel};


			Frame f = null;
			//Enrique pongo el jdialog modal para que se quede bloqueado esperando a que el usuario acepte
			//esto antes no hacía falta pero ahora si el passwd es malo sacamos de nuevo la ventana de connect to
			//y si no es esta modal se pone encima y sale todo mal
			final JDialog dialogo = new JDialog(f,errorService,true);
			JOptionPane miOption = new JOptionPane(message,JOptionPane.WARNING_MESSAGE);  
			miOption.addPropertyChangeListener("value",new PropertyChangeListener(){
				public void propertyChange(PropertyChangeEvent evt) {
					dialogo.setVisible(false);					
				}            	  
			});
			dialogo.getContentPane().add(miOption);
			dialogo.pack();
			//muestro la ventana en el centro de la pantalla
			dialogo.setLocationRelativeTo(null);   
			dialogo.setVisible(true);
			//JOptionPane.showMessageDialog(parentFrame, message, errorService, JOptionPane.ERROR_MESSAGE);

		} else {

			error.writeError(System.err);

		}

		//String stringError = error.toString();

		//System.out.println(" Esto es lo que sacamos con la funcion toString: " + stringError);

		if (XERLFILE==null) {

			// Si es null, ponemos el XERL por defecto..

			XERLFILE = Constants.ISABEL_XERLDIR + Constants.FILE_SEPARATOR + errorService.trim() + "_" + Constants.ISABEL_XERLFILE;

		}

		error.writeError(XERLFILE);

		//System.exit(0);

	}


	/**

	 * Presenta un mensaje de error por consola o en una ventana y genera un XERL

	 * @param errorServer Servidor del que hemos recibido el error.

	 * @param errorService Servicio que produjo el error.

	 * @param msg Mensaje q queremos presentar.

	 * @param errorDetails Detalles del mensaje que se presenta.

	 * @param IsGraphical Indica si queremos presentarlo en una ventana o en consola.

	 * @param XERLFILE Path completo del fichero XERL generado
	 * 
	 * @param ismir indicates if the mir is the application calling

	 */
	static public void writeErrorMessage (String errorServer, String errorService, String msg,String errorDetails, boolean IsGraphical, String XERLFILE, boolean ismir) throws IOException, ParserConfigurationException, TransformerException, UnknownHostException, XERLException  {

		// Creamos el error
		XERL error = new XERL(errorServer,errorService);

		if (errorDetails == null) error.setInfo(msg);
		else error.setInfo(msg + " - " + errorDetails);

		if (ismir) 
		{
			//Trazas
			String MIR_TRACES = Constants.XLIM_LOGS_WORK_DIR + Constants.FILE_SEPARATOR +"mir.log";    
			//Donde mandamos los mensajes de trazas: 
			PrintStream outTraceFile = new PrintStream (new FileOutputStream(MIR_TRACES, true),true);

			Date mi_date = new Date();
			outTraceFile.print(mi_date.toString() + " - ");
			error.writeError(outTraceFile);

			if (XERLFILE==null) {
				// Si es null, ponemos el XERL por defecto..
				XERLFILE = Constants.ISABEL_XERLDIR + Constants.FILE_SEPARATOR + errorService.trim() + "_" + Constants.ISABEL_XERLFILE;
			}
			error.writeError(XERLFILE);
			System.out.println("Ya no hacemos System.exit en writeErrorMessage, peligro");
			return;
		}
		if (IsGraphical) { 

			// Sacamos un mensaje en una ventanita...

			ErrorPanel errorPanel = new ErrorPanel();

			if (errorServer == null) errorPanel.setServerContentLabel("Localhost");

			else errorPanel.setServerContentLabel(errorServer);

			errorPanel.setServiceContentLabel(errorService);

			errorPanel.setErrorContentLabel(msg);

			errorPanel.setTextAreaDimension();

			//System.out.println("Valor de detalles: " +errorDetails);

			if (errorDetails == null) errorPanel.setErrorTextArea("No details found.");

			else errorPanel.setErrorTextArea(errorDetails);

			Object[] message = {errorPanel};


			Frame f = null;
			//Enrique pongo el jdialog modal para que se quede bloqueado esperando a que el usuario acepte
			//esto antes no hacía falta pero ahora si el passwd es malo sacamos de nuevo la ventana de connect to
			//y si no es esta modal se pone encima y sale todo mal
			final JDialog dialogo = new JDialog(f,errorService,false);
			JOptionPane miOption = new JOptionPane(message,JOptionPane.WARNING_MESSAGE);  
			miOption.addPropertyChangeListener("value",new PropertyChangeListener(){
				public void propertyChange(PropertyChangeEvent evt) {
					dialogo.setVisible(false);					
				}            	  
			});
			dialogo.getContentPane().add(miOption);
			dialogo.pack();
			//muestro la ventana en el centro de la pantalla
			dialogo.setLocationRelativeTo(null);   
			dialogo.setVisible(true);
			//      JOptionPane.showMessageDialog(parentFrame, message, errorService, JOptionPane.ERROR_MESSAGE);

		} else {

			error.writeError(System.err);

		}

		//String stringError = error.toString();

		//System.out.println(" Esto es lo que sacamos con la funcion toString: " + stringError);

		if (XERLFILE==null) {

			// Si es null, ponemos el XERL por defecto..

			XERLFILE = Constants.ISABEL_XERLDIR + Constants.FILE_SEPARATOR + errorService.trim() + "_" + Constants.ISABEL_XERLFILE;

		}

		error.writeError(XERLFILE);


	}

	/**

	 * Genera un XERL para devolverselo al Sir, es usado en el servicio ConditionalLauncher para informar de que ha ocurrido

	 * @param msg Mensaje q queremos presentar

	 * @param XERLFILE Path completo del fichero XERL generado

	 * @param Service Nombre del servicio que genera el error
	 * @throws XERLException 
	 * @throws IOException 
	 */

	static public void writeSirXerl (String msg, String XERLFILE, String Service) throws XERLException, IOException {

		//Creamos el error
		XERL error = new XERL(getLocalAddresses(),Service);
		error.setInfo(msg);
		error.writeError(XERLFILE);   

	}


	/**

	 * Escribe un mensaje de status por salida estandar o en una ventana, dependiendo del valor de IsGraphical

	 * @param msg El mensaje a imprimir.

	 * @param statusPane Objeto que contiene la ventana donde se escribira el texto

	 * @param outMsgFile Fichero donde se escribe en caso de que IsGraphical sea false.

	 * @param IsGraphical True-> escribe en ventana; false-> escribe en fichero (o en salida estandar)

	 */

	static public void writeStatusMsg (String msg, int progress, StatusWindow statusWin, PrintWriter outMsgFile, boolean IsGraphical) {

		if (IsGraphical) {

			statusWin.setStatusMsg(msg,progress);

		} else {

			outMsgFile.println(msg);

		}

	}



	/**

	 * Escribe un mensaje de status por salida estandar o en una ventana, dependiendo del valor de IsGraphical

	 * @param msg El mensaje a imprimir.

	 * @param statusPane Objeto que contiene la ventana donde se escribira el texto

	 * @param outMsgFile Fichero donde se escribe en caso de que IsGraphical sea false.

	 * @param IsGraphical True-> escribe en ventana; false-> escribe en fichero (o en salida estandar)

	 */

	static public void writeStatusMsg (String msg, int progress, StatusFrame statusWin, PrintWriter outMsgFile, boolean IsGraphical) {

		if (IsGraphical) {

			statusWin.setMsg(msg,progress);

		} else {

			outMsgFile.println(msg);

		}

	}

	// --------------------------------------------------------------------------------------------------







	// --------------------------------------------------------------------------------------------------

	// ------------------ EXTENSIONES DE FICHEROS -------------------------------------------------------

	// --------------------------------------------------------------------------------------------------

	/*

	 * Get the extension of a file.

	 */

	public static String getExtension(File f) {

		String ext = null;

		String s = f.getName();

		int i = s.lastIndexOf('.');



		if (i > 0 &&  i < s.length() - 1) {

			ext = s.substring(i+1).toLowerCase();

		}

		return ext;

	}



	/** Returns an ImageIcon, or null if the path was invalid. */

	/*    protected static ImageIcon createImageIcon(String path) {

        java.net.URL imgURL = Utils.class.getResource(path);

        if (imgURL != null) {

            return new ImageIcon(imgURL);

        } else {

            System.err.println("Couldn't find file: " + path);

            return null;

        }

    }

	 */

	//ENRIQUE, a partir de aquí hay metodos añadidos por enrique para las nuevas funcionalidades
	//y modificaciones
	/**
	 * Método para saber si un caracter está permitido en nick
	 * @param caracter caracter que queremos saber si está permitido
	 * @return
	 */
	public static boolean estapermitido_nick(char caracter)
	{
		if(caracter>='0' && caracter<='9')
			return true;
		if(caracter>='a' && caracter<='z')
			return true;
		if(caracter>='A' && caracter<='Z')
			return true;
		if(caracter=='-' || caracter=='_')
			return true;   

		return false;
	}

	/**
	 * Método para saber si un caracter está permitido en location
	 * @param caracter caracter que queremos saber si está permitido
	 * @return
	 */
	public static boolean estapermitido_location(char caracter)
	{
		if(caracter>='0' && caracter<='9')
			return true;
		if(caracter>='a' && caracter<='z')
			return true;
		if(caracter>='A' && caracter<='Z')
			return true;
		if(caracter=='-' || caracter=='_' || caracter=='(' || caracter==')' 
			|| caracter=='.' || caracter==':' || caracter==' ')
			return true;   

		return false;
	}



	public static void main (String args[]) {

		if (args[0].equalsIgnoreCase("--add_entry")) {
			//probamos el método de añadir entradas al historial
			add_history_entry(args[1],args[2]);
			return;
		}


		if (args[0].equalsIgnoreCase("--help")) {

			System.out.println("Servutilities running on " + System.getProperty("os.name") + "with default temp folder " +System.getProperty("java.io.tmpdir") );

			System.out.println("Usage: Servutilities function [paramsForThisFunction]");

			System.out.println("  - Available functions:");

			System.out.println("     * getremotesession ip: gives the session running in this ip address.");

			System.out.println("     * writeerrormsg server service msg details xerl_file: writes a pretty error msg in a window..");

			System.out.println("     * isabelisrunning:.");

			System.out.println("     * islocalhost host: tells if the given host is local or not.");

			System.out.println("     * crypt seed passwd: crypts the given passwd using this seed.");

			System.out.println("     * getremoteversion program ip: gives the installed version of the program in the given ip address.");

			System.out.println("     * getversion program: gives the installed version of the given program.");

			System.out.println("     * getrunningsession: gives the actual session running in the machine.");

			System.out.println("     * saveparamtofile: save a param-value into a file given. Usage: file param value");

			System.out.println("     * saveparamstofile: save n params-values into a file given. Usage: Servutilities saveparamstofile fileName param1,value1,param2,value2...");

			System.out.println("     * loadparamfromfile: returns the value of the given param in the given file. Usage: Servutilities loadparamfromfile fileName param [defValue]");

			System.out.println("     * removeparamfromfile: removes the param in the given file. Usage: Servutilities removeparamfromfile fileName param");

			System.out.println("     * geturldata: returns the value of flow and session form the given url. Usage: Servutilities geturldata url");

		}



		if (args[0].equalsIgnoreCase("geturldata")) {

			// Prueba del saveparamtofile

			if (args.length==2) {

				try {

					System.out.println("Flow value: " + getFlowFromUrl(args[1]));

					System.out.println("Session value: " + getSessionFromUrl(args[1]));

				} catch (Exception e) {

					System.out.println("Excepcion..." + e.toString() + " " + e.getMessage());

				}

			}

		}



		if (args[0].equalsIgnoreCase("removeparamfromfile")) {

			// Prueba del removeparamtofile

			if (args.length==3) {

				try {

					removeParamFromFile(args[1],args[2]);

				} catch (Exception e) {

					System.out.println("Excepcion..." + e.toString() + " " + e.getMessage());

				}

			} else {

				System.out.println("Usage: Servutilities loadparamfromfile fileName param [defValue]");

			}

		}



		if (args[0].equalsIgnoreCase("loadparamfromfile")) {

			// Prueba del saveparamtofile

			if (args.length==3) {

				try {

					System.out.println("Returned value: " + loadParamFromFile(args[1],args[2]));

				} catch (Exception e) {

					System.out.println("Excepcion..." + e.toString() + " " + e.getMessage());

				}

			} else {

				if (args.length==4) {

					try {

						System.out.println("Returned value: " + loadParamFromFile(args[1],args[2],args[3]));

					} catch (Exception e) {

						System.out.println("Excepcion..." + e.toString() + " " + e.getMessage());

					}

				} else {

					System.out.println("Usage: Servutilities loadparamfromfile fileName param [defValue]");

				}

			}

		}



		if (args[0].equalsIgnoreCase("saveparamtofile")) {

			// Prueba del saveparamtofile

			if (args.length==4) {

				try {

					System.out.println("Returned value: " + saveParamToFile(args[1],args[2],args[3]));

				} catch (Exception e) {

					System.out.println("Excepcion..." + e.toString() + " " + e.getMessage());

				}

			} else {

				System.out.println("Usage: Servutilities saveparamtofile fileName param value");

			}

		}



		if (args[0].equalsIgnoreCase("saveparamstofile")) {

			// Prueba del saveparamstofile

			if (args.length==3) {

				try {

					String[] params = args[2].split(",");

					saveParamsToFile(args[1], params);

				} catch (Exception e) {

					System.out.println("Excepcion..." + e);

				}

			} else {

				System.out.println("Usage: Servutilities saveparamstofile fileName param1,value1,param2,value2...");

			}

		}



		if (args[0].equalsIgnoreCase("xlimnotif")) {

			// Prueba del xlimlogs

			if (args.length==4) {

				try {

					System.out.println("Logging to " + args[1]);

					XlimNotification xlimNotif = new XlimNotification(args[1],args[2]);

					xlimNotif.println(args[3]);

				} catch (Exception e) {

					System.out.println("Excepcion..." + e);

				}

			} else {

				System.out.println("Usage: Servutilities xlimlog fileName service msg");

			}

		}



		if (args[0].equalsIgnoreCase("getlocaladdresses")) {

			// Prueba del nuevo writeErrorMsg

			if (args.length==1) {

				try {



					System.out.println("ipeses: " + getLocalAddresses());

				} catch (Exception e) {

					System.out.println("Excepcion..." + e);

				}

			} else {

				System.out.println("Usage: Servutilities getlocaladdresses");

			}

		}



		if (args[0].toLowerCase().equals("writeerrormsg")) {

			// Prueba del nuevo writeErrorMsg

			if (args.length==6) {

				try {

					writeErrorMessage (args[1], args[2], args[3],args[4], true, args[5]);

				} catch (Exception e) {

					System.out.println("Excepcion..." + e);

				}

			} else {

				System.out.println("Usage: server service msg details fichero_xerl");

			}

		}



		// Prueba de checkErrInFile

		/*    try {

      String[] expr = {args[1],args[2]};

      if (checkExprInFile(args[0],expr)) {

        System.out.println("Encontrada expresion en fichero");

      } else {

        System.out.println("NO encontrada expresion en fichero");

      }

    } catch (Exception e) {

      System.out.println("Excepcion: " + e);

    }

		 */



		if (args[0].toLowerCase().equals("isabelisrunning")) {

			// Prueba de isIsabelRunning

			try {

				if (isIsabelRunning()) {

					System.out.println("ISABEL corriendo...");

				} else {

					System.out.println("ISABEL parada...");

				}

			} catch (Exception e) {

				System.out.println("Excepcion..." + e);

			}

		}





		/*    // prueba de callXlim

    String[] expr = {"<XERL","</XERL"};

    try {

      System.out.println("Empezamos la fiesta...");

      if (callXlim (args [0], "copy", args[1], "kkout.txt",true, expr)) {

        System.out.println("Encontrado un XERL");

      } else { 

        System.out.println("NO Encontrado un XERL");

      }

      System.out.println("Terminamos la fiesta...");

    } catch (Exception e) {

    }

		 */  





		if (args[0].toLowerCase().equals("islocalhost")) {

			// Prueba de isLocalhost

			try {

				if (isLocalHost (args[1])) {

					System.out.println("Es local host");

				} else {

					System.out.println("NO Es local host");

				}

			} catch (Exception e) {



			}

		}



		if (args[0].toLowerCase().equals("crypt")) {

			// Prueba de Crypt.crypt(salt,passwd)

			String result = Crypt.crypt(args[1],args[2]);

			System.out.println("El classpath: " + Constants.CLASSPATH);

			System.out.println("El resultado es: " + result);

		}





		/*

    // Prueba de Zip y un zip

    // args[0] es el fichero zip y el resto son los que quieres comprimir.

    if (args.length>=2) {

      System.out.println("Probando el ZIP");

      String zipName = args[0];

      String[] fileNames = new String[args.length -3];

      System.out.println("Generando el listado de ficheros a comprimir");

      for (int i=3;i<args.length;i++){

        fileNames[i-3] = args[i];

      }

      System.out.println("Zipeando...");

      zipFile (args[0],fileNames);

      System.out.println("Unzipping zape...");

      // args[0]: fichero zip

      // args[1]: directorio donde descomprimimos

      // args[2]: directorio relativo

      String[] unzippedFiles = unzipFile (args[0],args[1],args[2]);

      for (int i=0;i<unzippedFiles.length;i++) {



      }

    } 

		 */





		if (args[0].toLowerCase().equals("getremoteversion")) {

			// Prueba del getRemoteVersion

			if (args.length==3) {

				try {

					System.out.println("Probando el getRemoteVersion");

					System.out.println("Resultado: " + getRemoteVersion(args[1], args[2]));

				} catch (Exception e) {

				}

			}

		}

		// Prueba el getVersion

		if (args[0].toLowerCase().equals("getversion")) {

			if (args.length==2) {

				try {

					System.out.println("Probando el getVersion");

					System.out.println("Resultado: " + getVersion(args[1]));

				} catch (Exception e) {

				}

			}

		}



		// Prueba el getRunningSession

		if (args[0].toLowerCase().equals("getrunningsession")) {

			if (args.length==1) {

				try {

					System.out.println("Probando el getRunningSession");

					System.out.println("Resultado: " + getRunningSession());

				} catch (Exception e) {

				}

			}

		}



	}

}



