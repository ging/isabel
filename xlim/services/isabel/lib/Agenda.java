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
 * Agenda.java
 *
 * Created on 2 de septiembre de 2003, 10:48
 */

package services.isabel.lib;

import java.io.*;

/**
 * Esta clase contiene m�todos para acceder, crear y borrar el sistema de ficheros
 * que forma la agenda.
 * Contiene metodos para acceder a los distintos archivos donde se guardan los
 * datos de una session. Todos estos archivos devuelven un objeto tipo File.
 * Una vez obtenido este objeto podra comprobarse si el fichero existe, crearlo,
 * borrarlo, abrirlo, etc... con los m�todos definidos en esa clase.
 * @author Fernando Escribano
 */
public class Agenda {
    
        
    /**
     * Nombre del directorio donde se almacenan las sesiones locales.
     */
    private static final String LOCAL_DIRECTORY = "master";
    
    /**
     * Nombre del directorio donde se almacenan las sesiones remotas.
     */
    private static final String REMOTE_DIRECTORY = "remote";

    /**
     * Nombre del directorio donde se almacenan los ficheros temporales.
     */
    private static final String TEMP_DIRECTORY = "tmp";
        
    /**
     * Nombre del fichero de options.
     */
    private static final String OPTIONS_XEDL_FILE = "options";
    
    /**
     * Nombre del fichero donde se guardan los datos de la sesion por defecto.
     */
    private static final String DEFAULT_FILE = "default.session";
    
    /**
     * Nombre del fichero con el URL de la sesion.
     */
    private static final String URL_FILE = "url";
    
    /**
     * Nombre del fichero que marca una sesion como automatica.
     */
    private static final String AUTOMATIC_SESSION_FILE = "auto";
        
    
    /**
     * Nombre del fichero que guarda el ultimo servidor al que se conecto.
     */
    private static final String LAST_SERVER_FILE = "last-server";
    
    /**
     * Apendice del fichero XEDL que se empleo para lanzar la sesion por ultima vez.
     */
    private static final String FULL_XEDL_FILE_APPENDIX = "_TMP";
        
    /**
     * Nombre del fichero que contiene la descripcion parcial
     * del sitio local.
     */
    private static final String LOCAL_XEDL_FILE = "local";   
    
    /**
     * Directorio a partir del cual se encuentra la agenda.
     */
    private File rootDir;
    
    /**
     * Directorio con las sesiones locales.
     */
    public File localDir;
    
    /**
     * Directorio con las sesiones remotas.
     */
    public File remoteDir;
    
    /**
     * Directorios con los ficheros temporales.
     */
    private File tmpDir;
    
    /**
     * Directorios donde esta el fichero de opciones.
     */
    private File optionsDir;
    
        
    /**
     * Crea un objeto tipo Agenda. En caso de que no existan los ficheros y
     * directorios necesarios para guardar los datos de la agenda son creados.
     * La agenda se crea a partir del directorio $HOME/.isabel/.
     * @throws IOException Si surge algun problema al acceder a los ficheros o al crearlos.
     */
    public Agenda() throws IOException {
        this(getDefaultAgendaRoot());
    }
    
    /**
     * Crea un objeto tipo Agenda. En caso de que no existan los ficheros y
     * directorios necesarios para guardar los datos de la agenda son creados.
     * @param root Path a partir del cual se encuentra el sistema de ficheros
     * que forma la agenda. En general este parametro debe ser ~/.isabel/
     * @throws IOException Si surge algun problema al acceder a los ficheros o al crearlos.
     */
    public Agenda(String root) throws IOException {
        
        // Compruebo que la raiz que me han pasado en un directorio
        rootDir = new File(root);
        if (!rootDir.isDirectory())
            throw new java.io.IOException(root + " is not a directory");
        
        // Creo todos los directorios necesarios si no existian previamente.
        File sessions = new File(Constants.ISABEL_SESSIONS_DIR);
        sessions.mkdir();
        localDir = new File(sessions, LOCAL_DIRECTORY);
        localDir.mkdir();
        remoteDir = new File(sessions, REMOTE_DIRECTORY);
        remoteDir.mkdir();
        tmpDir = new File(rootDir, TEMP_DIRECTORY);
        tmpDir.mkdir();
        optionsDir = new File(Constants.ISABEL_PROFILES_DIR);
        optionsDir.mkdir();
    }

    /**
     * Crea los ficheros y directorios necesarios para guardar en la egenda
     * los datos de una sesion. Los directorios son creados vacios.
     * @param name El nombre de la sesion.
     * @param local Boolean que indica si la session debe ser creada en la zona
     * de sessiones master o de sesiones remotas.
     * @return True Si la sesion fue creada con exito y false en caso contrario.
     * (por ejemplo si la sesion ya existia)
     */
    public boolean createSession(String name, boolean local) {
        //ENRIQUE, ahora se borra el directorio de la sesion antes de crearlo
    	//ya no se usan los xedl de sessiones antiguas
    	deleteSession(name);    	
    	// Dependiendo de si la sesion es local o no el directorio donde crear
        // el directorio de la sesion varia.
        File dir;
        if(local)
            dir = localDir;
        else
            dir = remoteDir;
        
        File session = new File(dir, name);
        
        if (session.exists())
            return false;

        return session.mkdir();
    }
    
    /**
     * Borra de la agenda una sesion. Si no existe no hace nada.
     * @param name El nombre de la sesion.
     */
    public void deleteSession(String name) {
        // Obtengo el directorio donde se guardan los datos de la sesion.
        File dir;
        if(isLocalSession(name) && isRemoteSession(name))
        {
        	dir = localDir;
        	File session = new File(dir, name);
            deleteDirectory(session);
            dir = remoteDir;
            session = new File(dir, name);
            deleteDirectory(session);
        }
        if(isLocalSession(name))
            dir = localDir;
        else if (isRemoteSession(name))
            dir = remoteDir;
        else
            return;
        
        File session = new File(dir, name);
        deleteDirectory(session);
    }
    
    /**
     * Deletes all master sessions
     *
     */
    public void deleteMasterSessions(){
    	deleteDirectoryContent(localDir);
    }
    
    /**
     * Deletes all remote sessions
     *
     */
    public void deleteRemoteSessions(){
    	deleteDirectoryContent(remoteDir);
    }
    
    /**
     * Devuelve una lista de nombres de sesiones locales definidas en la agenda
     * @return Un String [] con los nombres de la sesiones definidas.
     */
    public String[] getLocalSessions() {
        return localDir.list();
    }
    
    /**
     * Devuelve una lista de nombres de sesiones remotas definidas en la agenda.
     * @return Un String [] con los nombres de la sesiones definidas.
     */
    public String[] getRemoteSessions() {
        return remoteDir.list();
    }
    
    /**
     * Dice si la sesion cuyo nombre se pasa esta definida en la agenda como
     * una sesion local. Solo asegura que existe el directorio no el XEDL.
     * @param name Nomnbre de la sesion.
     * @return True si la sesion esta definida como local false en caso contrario.
     */
    public boolean isLocalSession(String name) {
        String[] sessions = getLocalSessions();
        
        // Si el nombre coincide con alguno de los de las sesiones locales
        // devuelvo true
        for (int i = 0; i < sessions.length; i++) {
            if (sessions[i].equals(name))
                return true;
        }
        
        // Si no devuelvo false
        return false;
    }
    
    /**
     * Dice si la sesion cuyo nombre se pasa esta definida en la agenda como
     * una sesion remota. Solo asegura que existe el directorio no el XEDL.
     * @param name Nomnbre de la sesion.
     * @return True si la sesion esta definida como local false en caso contrario.
     */    
    public boolean isRemoteSession(String name) {
        String[] sessions = getRemoteSessions();
        
        // Si el nombre coincide con alguno de los de las sesiones remotas
        // devuelvo true
        for (int i = 0; i < sessions.length; i++) {
            if (sessions[i].equals(name))
                return true;
        }
        
        // Si no devuelvo false
        return false;
    }
    
    /**
     * Devuelve el fichero original de la session, esto es el fichero que se crea
     * mediante la aplicacion de Accept Connections.
     * @param name El nombre de la sesion.
     * @return Objeto File del fihcero XEDL "original" de la sesion.
     * throws IOException Si la sesion no esta definida
     */
    public File getOriginalXedl(String name) throws IOException {
        // Obtengo el directorio donde se guardan los datos de la sesion.
        if(!isLocalSession(name))
            throw new IOException("Session not defined in this agenda");
        
        File session = new File(localDir, name);
        
        // Devuelvo el File del fichero <nombre_session>.xml.
        String filename = name + "." + Constants.XEDL_EXTENSION;
        return new File(session, filename);
    }
    
    /**
     * M�todo para obtener la lista de acronimos de los sitios de los que hay
     * ficheros con definiciones parciales.
     * @param name El nombre de la sesion.
     * @return String [] con los acronimos de los sitios cuyas definiciones parciales
     * se encuantran en la agenda.
     * throws IOException Si la sesion no esta definida.
     */
    public String[] getSitesNames(final String name) throws IOException {
        // Obtengo el directorio donde se guardan los datos de la sesion.
        if(!isLocalSession(name))
            throw new IOException("Session not defined in this agenda");
        
        File session = new File(localDir, name);

        // En este directorio obtengo una lista de los ficheros
        String[] contents = session.list(new FilenameFilter() {
            public boolean accept(File f, String s) {
                if (s.startsWith(name + "-") && s.endsWith("." + Constants.XEDL_EXTENSION))
                    return true;
                else
                    return false;
            }
        }
        );
        for (int i = 0; i < contents.length; i++) {
            contents[i] = contents[i].substring(name.length() + 1, contents[i].length() - Constants.XEDL_EXTENSION.length() -1);
        }
        return contents;
    }
    
    /**
     * Devuelve el fichero que contiene la definicions parcial del sitio indicado.
     * @param name Nombre de la sesion.
     * @param acronymous Acronimo del sitio cuya definicion parcial se quiere obtener.
     * @return Objeto File del fichero XEDL con la definicion parcial del sitio.
     * Puede corresponder a un fichero que no existe.
     * @throws IOException Si la session no esta definida.
     */
    public File getSiteXedl(String name, String acronymous) throws IOException {
        // Obtengo el directorio donde se guardan los datos de la sesion.
        if(!isLocalSession(name))
            throw new IOException("Session not defined in this agenda");
        
        File session = new File(localDir, name);
        
        String filename = name + "-" + acronymous + "." + Constants.XEDL_EXTENSION;
        return new File(session, filename);
    }
    
    /**
     * Devuelve el fichero que contiene la definici�n parcial del sitio local.
     * @param name Nombre de la sesion.
     * @return Objeto File del fichero XEDL con la definicion parcial del sitio.
     * @throws IOException Si la session no esta definida.
     */
    public File getLocalXedl(String name) throws IOException {
        // Obtengo el directorio donde se guardan los datos de la sesion.
        File dir;
        if(isLocalSession(name))
            dir = localDir;
        else if (isRemoteSession(name))
            dir = remoteDir;
        else
            throw new IOException("Session not defined in this agenda");
        File session = new File(dir, name);
        
        String filename = LOCAL_XEDL_FILE + "." + Constants.XEDL_EXTENSION;
        return new File(session, filename);
    }
    
    /**
     * Devuelve el fichero que contiene el XEDL completo con el que se arranco
     * la sesion por ultima vez.
     * @param name Nombre de la sesion.
     * @return Objeto File del fichero XEDL completo de la sesion tal y como se
     * arranco la ultima vez.
     * @throws IOException Si la session no esta definida.
     */
    public File getFullXedl(String name) throws IOException {
        // Obtengo el directorio donde se guardan los datos de la sesion.
        File dir;
        if(isLocalSession(name))
            dir = localDir;
        else if (isRemoteSession(name))
            dir = remoteDir;
        else
            throw new IOException("Session not defined in this agenda");
        File session = new File(dir, name);
        String filename1 = name + FULL_XEDL_FILE_APPENDIX + "." + Constants.XEDL_EXTENSION;
        String filename2 = name + "." + Constants.XEDL_EXTENSION;
        File temp =  new File(session, filename1);
        if(temp.exists())
        	return temp;
        else
        {
        	File temp2 = new File(session, filename2);
        	if(temp2.exists())
        	{
        		//el fichero que existe como total es el sesion.xedl no sesion_TMP.xedl lo copio y lo devuelvo
        		Servutilities.copyFile(temp2.getAbsolutePath(), temp.getAbsolutePath());
        	}
        	return temp;
        }
    }
    
    /**
     * Devuelve el fichero donde se guardan los datos de la session por defecto.
     * @return Objeto File del fichero con los datos por defecto.
     */
    public File getDefaultSessionFile() {
        File sessions = new File(Constants.ISABEL_SESSIONS_DIR);
        return new File(sessions, DEFAULT_FILE);
    }
    
    /**
     * Devuelve el fichero donde se guarda el URL que el usuario ha
     * introducido al hacer connect to.
     * @param name Nombre de la sesion.
     * @return Objeto File del fichero.
     * @throws IOException Si la sesion no esta definida.
     */
    public File getURLFile(String name) throws IOException {
        // La sesion tiene que ser remota luego compruebo que existe
        if (!isRemoteSession(name))
            throw new IOException("Session not defined in this agenda");
        
        File session = new File(remoteDir, name);
        return new File(session, URL_FILE);
    }
    
    /**
     * Devuelve el fichero que marca la sesion como de creacion automatica
     * al lanzarse.
     * @param name Nombre de la sesion.
     * @return Objeto File del fichero.
     * @throws IOException Si la sesion no esta definida.
     */
    public File getAutomaticSessionFile(String name) throws IOException {
        // Obtengo el directorio donde se guardan los datos de la sesion.
        File dir;
        if(isLocalSession(name))
            dir = localDir;
        else if (isRemoteSession(name))
            dir = remoteDir;
        else
            throw new IOException("Session not defined in this agenda");
        File session = new File(dir, name);
        String filename = AUTOMATIC_SESSION_FILE;
        return new File(session, filename);
    }
    
    
    /**
     * Devuelve el fichero donde se guarda el 
     * servidor de connect_to al que se conect� por �ltima vez
     * @param name Nombre de la sesion.
     * @return Objeto File del fichero.
     * @throws IOException Si la sesion no esta definida.
     */
    public File getLastServerFile(String name) throws IOException {
        // La sesion tiene que ser remota luego compruebo que existe
        if (!isRemoteSession(name))
            throw new IOException("Session not defined in this agenda");
        
        File session = new File(remoteDir, name);
        return new File(session, LAST_SERVER_FILE);
    }
    
    /**
     * Devuelve el fichero donde se guardan las opciones del sitio local.
     * @return Objeto File del fichero.
     */
    public File getOptionsFile() {
        return new File(optionsDir, OPTIONS_XEDL_FILE + "_" + "Default" + ".xml");
    }

    /**
     * Devuelve el fichero donde se guardan las opciones del sitio local para el perfil
     * dado.
     * @param profile Nombre del profile cuyo fichero de opciones se quiere obtener. Si es null
     * se devuelve el por defecto.
     * @return Objeto File del fichero.
     */
    public File getOptionsFile(String profile) {
        if (profile == null)
            return getOptionsFile();
        else
            return new File(optionsDir, OPTIONS_XEDL_FILE +"_"+ profile + ".xml");
    }
    
    /**
     * Devuelve una lista de nombres de profiles definidos.
     * @para def Dice si se incluye en la lista el fichero por defecto o
     * si no se incluye.
     * @return Un String [] con los nombres de los profiles.
     */
    public String[] getProfileNames(final boolean def) {
        String[] files = optionsDir.list(new FilenameFilter() {
            public boolean accept(File f, String s) {
            	boolean defi = def;
                if (!def && s.equals(OPTIONS_XEDL_FILE + "_" + "Default" + ".xml"))
                    return false;
                
                if (s.startsWith(OPTIONS_XEDL_FILE + "_") && s.endsWith(".xml"))
                    return true;
                else
                    return false;
            }
        });
        for (int i = 0; i < files.length; i++)
            files[i] = files[i].substring(OPTIONS_XEDL_FILE.length() + 1,
                                         files[i].length() - ".xml".length());
        return files;
    }
    
    /***************************************************************************
     *                            METODOS PRIVADOS                             *
     **************************************************************************/
    
    /**
     * Borra todos los ficheros contenidos en un directorio.
     * @param f El directorio a borrar.
     */
    public void deleteDirectory(File f) {
        // Listo los ficheros del directorio
        File[] contents = f.listFiles();
        
        // Borro cada fichero contenido
        // utilizando deleteDirectory si es un directorio.
        for (int i = 0; i < contents.length; i++) {
            if(contents[i].isDirectory())
                deleteDirectory(contents[i]);
            contents[i].delete();
        }
        // Finalmente borro el propio directorio
        f.delete();
    }

    
    /**
     * Borra todos los ficheros contenidos en un directorio.Pero no el propio directorio
     * @param f El directorio a borrar.
     */
    public void deleteDirectoryContent(File f) {
        // Listo los ficheros del directorio
        File[] contents = f.listFiles();
        
        // Borro cada fichero contenido
        // utilizando deleteDirectory si es un directorio.
        for (int i = 0; i < contents.length; i++) {
            if(contents[i].isDirectory())
                deleteDirectory(contents[i]);
            contents[i].delete();
        }
    }
    
    
    /**
     * Este m�todo devuelve una cadena de texto que es el path al directorio
     * base de la agenda que se emplea normalmente.
     * Si el directorio no existe lo crea.
     */
    private static String getDefaultAgendaRoot() {
        String root = Constants.ISABEL_USER_DIR;
        new File(root).mkdir();
        return root;
    }
    

    
    /**
     * Metodo de prueba de la clase.
     * @param args Argumentos de la line de comando. No se usan.
     */
    public static void main (String[] args) throws Exception {        
        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
        System.out.println("Creando arbol de ficheros de la agenda");
        /**
        File root = new File("C:/agenda");
        root.mkdir();
        Agenda a = new Agenda(root.getAbsolutePath());
        */
        Agenda a = new Agenda();
        a.getDefaultSessionFile().createNewFile();
        System.out.println("Agenda situada en: " + a.rootDir.getAbsolutePath());
        System.out.println("Pulse Enter para continuar...");
        in.readLine();

        System.out.println("Creando session local LARALA");
        a.createSession("LARALA", true);
        System.out.println("Pulse Enter para continuar...");
        in.readLine();
        
        System.out.println("Creando session remota LERELE");
        a.createSession("LERELE", false);
        System.out.println("Pulse Enter para continuar...");
        in.readLine();
        
        System.out.println("Creando ficheros en sesion LARALA");
        String [] sites = {"FEC", "EDM", "DAA"};
        a.getOriginalXedl("LARALA").createNewFile();
        for (int i = 0; i < sites.length; i++)
            a.getSiteXedl("LARALA", sites[i]).createNewFile();
        a.getLocalXedl("LARALA").createNewFile();
        System.out.println("Pulse Enter para continuar...");
        in.readLine();
        
        System.out.println("Creando ficheros en sesion LERELE");
        a.getURLFile("LERELE").createNewFile();
        a.getLastServerFile("LERELE").createNewFile();
        a.getLocalXedl("LERELE").createNewFile();
        a.getFullXedl("LERELE").createNewFile();
        System.out.println("Pulse Enter para continuar...");
        in.readLine();
        
        System.out.println("Listando sesiones y sitios");
        System.out.println("LOCALES");
        String [] locals = a.getLocalSessions();
        for (int i = 0; i < locals.length; i++)
            System.out.println(locals[i]);
        System.out.println("REMOTAS");
        String [] remotes = a.getRemoteSessions();
        for (int i = 0; i < remotes.length; i++)
            System.out.println(remotes[i]);
        
        System.out.println("SITIOS DE LARALA");
        String [] ls = a.getSitesNames("LARALA");
        for (int i = 0; i < ls.length; i++)
            System.out.println(ls[i]);
        
        System.out.println("Pulse Enter para continuar...");
        in.readLine();
        
        System.out.println("Borrando sesion LARALA");
        a.deleteSession("LARALA");
        
        System.out.println("Pulse Enter para continuar...");
        in.readLine();
        System.out.println("Creando ficheros de perfiles Default, PEPE y PEPA: ");
        a.getOptionsFile().createNewFile();
        a.getOptionsFile("PEPE").createNewFile();
        a.getOptionsFile("PEPA").createNewFile();
        
        System.out.println("Pulse Enter para continuar...");
        in.readLine();
        System.out.println("Lista de perfiles(Default incluido)");
        String [] pn = a.getProfileNames(true);
        for (int i = 0; i < pn.length; i++)
            System.out.println(pn[i]);
    }
}
