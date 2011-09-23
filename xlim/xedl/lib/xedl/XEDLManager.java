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
 * XEDLManager.java
 *
 * Created on 18 de marzo de 2004, 11:33
 */

package xedl.lib.xedl;

import java.io.*;
import java.util.*;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;


import services.isabel.lib.Constants;
import services.isabel.lib.Servutilities;
import services.isabel.services.ac.ServiceNames;
import xedl.lib.jaxb.Edl;
import xedl.lib.jaxb.MulticastParameters;
import xedl.lib.jaxb.SESSION;
import xedl.lib.jaxb.SITE;

/**
 * @author  ebarra
 */

public class XEDLManager  {
    
    private int state;
    private String xedlFile;
    private SiteList siteList;
    private List<String> listaIDs=new LinkedList();
    private int indiceIDs=0;
    
    public static String[] SESSION_QUALTIY_VALUES=new String[] {"128K", "128/256K","256K", "300K/1M", "384K", "512K", "1M", "2M", "10M"};
    public static String[] SESSION_SERVICES_VALUES=new String[] {"meeting.act", "class.act", "conference.act"};
    public static String CONNECTION_MODE_MCU="mcu";
    public static String CONNECTION_MODE_MULTICAST="multicast";
    public static String CONNECTION_MODE_UNICAST="unicast";
    
    
    
    //estados del manejador de ficheros xedl:
    
    private static final int E_WITHOUT_XEDL=0; //SI FICHERO XEDL
    private static final int E_XEDL_LOADED=1;  //fichero xedl abierto
    private static final int E_XEDL_CREATED=2;  //fichero xedl sin path
    
    public String currentSessionName="";
    
    private XEDL xedl;
    /** Creates a new instance of XEDLManager */
    
    public PrintWriter outTraceFile= null;
    
   
    //Espera el path completo del fichero.
    
    public XEDLManager(){
    	outTraceFile = Constants.inicializa_trazas_jaxb();
    	outTraceFile.println("Constructor XEDLManager");
    	changeState(E_WITHOUT_XEDL);
    }
    
    
    public XEDLManager(String file) {
    	
    	outTraceFile = Constants.inicializa_trazas_jaxb();
    	outTraceFile.println("Constructor XEDLManager con file: " + file);
    	xedlFile=file;
    	try {
			xedl = new XEDL(file);
			Edl miedl = xedl.getEdl();
            listaIDs = miedl.getSiteDescription().getSITEListIds();
            goFirstID();
            siteList = new SiteList(xedl);
            changeState(E_XEDL_LOADED);
		} catch (XEDLException e) {			
			e.printStackTrace();
			e.printStackTrace(outTraceFile);
		}
    }
    
    
    public XEDLManager(XEDL xedl) {
    	
    	outTraceFile = Constants.inicializa_trazas_jaxb();
    	if(xedl==null){
            changeState(E_WITHOUT_XEDL);
        }
        else{
        	outTraceFile.println("Constructor XEDLManager con xedl: " + xedl.getPath());
            this.xedl=xedl;
            Edl miedl = xedl.getEdl();
            listaIDs = miedl.getSiteDescription().getSITEListIds();
            goFirstID();
            siteList = new SiteList(xedl);
            changeState(E_XEDL_LOADED);
        }
    }
    
    
    public void reload(){
    	outTraceFile.println("Metodo reload de XEDLManager");
        if(state!=E_XEDL_LOADED) return;
        Edl miedl = xedl.getEdl();
        listaIDs = miedl.getSiteDescription().getSITEListIds();
        siteList = new SiteList(xedl);
        goFirstID();
    }
    
    
    public XEDL getXEDLObject(){
        return xedl;
    }
    
    
    public  void loadFile() throws XEDLException,IOException{
    	outTraceFile.println("Metodo loadFile de XEDLManager");
    	xedl=new XEDL(xedlFile);
        Edl miedl = xedl.getEdl();
        listaIDs = miedl.getSiteDescription().getSITEListIds();
        siteList = new SiteList(xedl);
        goFirstID();
        changeState(E_XEDL_LOADED);
    }
    
    /**
     * method to pass to the next id of the list
     */
    public String NextID(){
    	outTraceFile.println("Metodo NextID de XEDLManager");
        if(indiceIDs<listaIDs.size()){
            String ID= String.valueOf(listaIDs.get(indiceIDs));
            indiceIDs++;
            return ID;
        }
        return null;
    }
    
    
    public void goFirstID(){
        indiceIDs=0;
    }
    
    
    public String getIDRaiz(){
    	outTraceFile.println("Metodo getIDRaiz de XEDLManager");
        String IDRaiz= siteList.getRealRootSite();
        outTraceFile.println("idRaiz: " + IDRaiz);
        if(IDRaiz==null){
            return null;
        }
        return IDRaiz;
    }
    
    
    public LinkedList getHijos(String idSite){
    	outTraceFile.println("Metodo getHijos de XEDLManager");
        SITE sitioPadre=null;
        sitioPadre=siteList.getSite(idSite);
        if(sitioPadre==null) return null;
        int ndirecciones=sitioPadre.getSiteAddress().size();
        LinkedList direccionesPadre=sitioPadre.getSiteAddress();
        //Un sitio sin direcci�n no puede tener hijos
        if(ndirecciones<=0) return null;
        LinkedList lstHijos=siteList.getChildrenSites(idSite);        
        return lstHijos;
    }
    
    /**
     * method to get all the id of the sites
     */
    public LinkedList getAllIDSites(){
    	outTraceFile.println("Metodo getAllIDSites de XEDLManager");
    	LinkedList lstAll=new LinkedList();
        lstAll= siteList.getSiteIds(); //recupera todos los ID
        return lstAll;
    }
    
    
    /**
     * Method to delete the connection (MCU connection, not multicast) between 2 sites
     * @param idi child
     * @param idf parent
     */
    public void deleteConection(String idi,String idf){
    	outTraceFile.println("Metodo deleteConection de XEDLManager");
        SITE site=siteList.getSite(idi);
        SITE sitef=siteList.getSite(idf);
        outTraceFile.println("Borrar conexion entre idi" + idi + " y " + idf);
        if(site==null && sitef==null) return;
        LinkedList listaMCUs = site.getMCUAddress();
        LinkedList listaIpPadre = sitef.getSiteAddress();
        int sizeIPpadre = listaIpPadre.size();
        
        //buscamos la lista de mcus del nodo hijo y vemos si existe una
        //entrada en las direcciones del padre. si es asi, se borra la mcu
        for(int i=0; i< sizeIPpadre; i++){
            if(listaMCUs.contains((String) listaIpPadre.get(i))){
                site.deleteMCUAddress((String) listaIpPadre.get(i));
                break;
            }
        }
    }
    
    
    /**
     * method to delete the connection
     * @param idSite
     */
    public void deleteParentConection(String idSite){
    	outTraceFile.println("Metodo deleteParentConection de XEDLManager");
    	SITE site=siteList.getSite(idSite);
        if(site==null) return;
        site.setCONNECTIONMODE(null);
    }
    
    
    public LinkedList getChildByLinkedList(LinkedList ls){
    	outTraceFile.println("Metodo getChildByLinkedList de XEDLManager");
        LinkedList fullLinkedList=new LinkedList();
        SmallXEDLTree tree=null;
        LinkedList hijos=new LinkedList();
        int size=ls.size();
        if(size<=0) return null;
        for(int i=0;i<size;i++){
            //Busca los hijos de cada uno de los sitios pasados en el array.
            hijos=siteList.getChildrenSites((String) ls.get(i));
            if(hijos.size()>0){
                tree=new SmallXEDLTree(this);
                tree.setIDPadre((String) ls.get(i));
                tree.setArrayHijos(hijos);
                fullLinkedList.add(tree);
            }
        }
        return fullLinkedList;
    }
    
    
    public String getRole(String id){
    	outTraceFile.println("Metodo getRole de XEDLManager");
    	SITE site=siteList.getSite(id);
        if(site==null) return "";
        return site.getSITEROLE();
    }
    
    
    public SITE getSite(String id){
    	outTraceFile.println("Metodo getSite de XEDLManager");
    	return siteList.getSite(id);
    }
    
    
    public String getMCU(String id){
    	outTraceFile.println("Metodo getMCU de XEDLManager");
    	SITE site=siteList.getSite(id);
        if(site==null) return null;
        if(site.getMCUAddress()==null) return null;
        if(site.getMCUAddress().size()>0){
            return (String) site.getMCUAddress().get(0);
        }
        return null;
    }
    
    
    public String getParentSiteAddress(String idSite){
    	outTraceFile.println("Metodo getParentSiteAddress de XEDLManager");
        SITE site=(SITE)siteList.getSite(idSite);
        if(site!=null){
            return site.getParentSiteAddress();
        }
        return null;
    }
    
    
    /**
     * metodo para ver si una direcc está repetida
     * @param address direcc a comprobar
     * @return id del otro sitio que la tiene, null si no hubo coincidencia
     */
    public String isAddressRepeated(String address)
    {
    	outTraceFile.println("Metodo isAddressRepeated de XEDLManager con address " + address);
    	LinkedList all_ids = getAllIDSites();
    	for(int index=0;index<all_ids.size();index++)
    	{
    		//cojo el sitio
    		String id = (String)all_ids.get(index);
    		//cojo su lista de direcciones
    		LinkedList direcc = getAddressList(id);
    		for(int a=0;a<direcc.size();a++)
    		{
    			if( ((String)direcc.get(a)).equals(address) )
    					return id;
    		}
    	}
    	return null;
    }
    
    
    public LinkedList getAddressList(String id){
    	outTraceFile.println("Metodo getAddressList de XEDLManager con id " + id);
        return siteList.getSite(id).getSiteAddress();
    }
    
    
    /**
     * 
     * @return la passwd encriptada del sitio raiz, con el id del sitio raiz se puede desencriptar, devuelve null si no hay passwd
     */
    public String getAccessControl()
    {
    	outTraceFile.println("Metodo getAccessControl de XEDLManager");
    	return xedl.getEdl().getSESSION().getACCESSCONTROL();
    }
    
    

    public String getParentSiteID(String id){
    	outTraceFile.println("Metodo getParentSiteID de XEDLManager para la id " + id);
        String parent=siteList.getSite(id).getParentSiteAddress();
        if(parent != null){
            return parent;
        }
        return null;
    }
    
    
    public String getParentIDbyMCU(String mcu){
    	outTraceFile.println("Metodo getParentIDbyMCU de XEDLManager");
        int size=listaIDs.size();
        String id;
        //Por cada sitio, busca sus direcciones y la compara con la mcu
        for(int i=0;i<size;i++){
            id=(String) listaIDs.get(i);
            SITE s=(SITE)siteList.getSite(id);
            int n=s.getSiteAddress().size();
            //mira el n�mero de direcciones que tiene el sitio.
            for(int y=0;(y<n && n>0);y++){
                if(mcu.equals(s.getSiteAddress().get(y))) return id;
            }
        }
        return null;
    }
    
    
    public String getConnectionMode(String id){
    	outTraceFile.println("Metodo getConnectionMode de XEDLManager para la id " + id);
        if(siteList.getSite(id).getCONNECTIONMODE()!=null){
            if(siteList.getSite(id).getCONNECTIONMODE().getMcu()!=null)
            	return CONNECTION_MODE_MCU;
            if(siteList.getSite(id).getCONNECTIONMODE().getUnicast()!=null)
            	return CONNECTION_MODE_UNICAST;
            if(siteList.getSite(id).getCONNECTIONMODE().getMulticast()!=null)
            	return CONNECTION_MODE_MULTICAST;
        }
        return null;
    }
    
    
    public String getSiteSumary(String id){
    	outTraceFile.println("Metodo getSiteSumary de XEDLManager para la id " + id);
        String cadena;
        SITE s = siteList.getSite(id);
        String resumen="<FONT face='Verdana' color='#330099' size='2'><b>Site ID:&nbsp;&nbsp;</b>"+s.getID();
        cadena = s.getPUBLICNAME();
        resumen+="<br><b>Public Name:&nbsp;&nbsp;</b>"+ cadena;
        resumen+="<br><b>Role:</b>"+s.getSITEROLE();
        int n=s.getSiteAddress().size();
        for(int i=0;(i<n && n>0);i++){
            resumen+="<br><b>Address (" + i +")</b>&nbsp;&nbsp;"+s.getSiteAddress().get(i);
        }
        cadena=getConnectionMode(id);
        if(cadena==null) cadena="";
        resumen+="<br><b>Conection Mode:&nbsp;&nbsp;</b>"+ cadena;
        if(s.getMCUAddress()!=null && s.getMCUAddress().size()>0)
            resumen+="<br><b>MCU:&nbsp;&nbsp;</b>" + (String) s.getMCUAddress().getFirst();
        //modo de conexi�n
        String mcast="False";
        if(this.isEnableMulticast(id)) mcast="True";
        resumen+="<br><b>Multicast Enabled:&nbsp;</b>" + mcast;
        String contact="";
        String email="";
        if(s.getAdminInfoEmails().size()>0){
            email=(String)s.getAdminInfoEmails().getFirst();
            contact=(String) s.getAdminContact(email).getNAME();
        }
        resumen+="<br><b>Contact:&nbsp;&nbsp;</b>"+contact;
        resumen+="<br><b>Email:&nbsp;&nbsp;</b>"+email;
        
        resumen+="</FONT>";
        return resumen;
    }
    
        
    public boolean isOpenSession(){
    	outTraceFile.println("Metodo isOpenSession de XEDLManager");
        return xedl.getEdl().getSESSION().getSessionInfo().isOPENSESSION();
    }
    
    
    public boolean isEnableMulticast(String idSite){
    	outTraceFile.println("Metodo isEnableMulticast de XEDLManager para el site " + idSite);
        SITE site = siteList.getSite(idSite);
        if(site!=null){
             if(site.getCONNECTIONMODE()!=null && site.getCONNECTIONMODE().getMulticast()!=null)
            	 return true;            
        }
        return false;
    }
    
        
    public void setSessionData(String sName,String sDesc,String sQuality,String sService,boolean sIsOpen){
    	outTraceFile.println("Metodo setSessionData de XEDLManager con sName: " + sName + " sDesc: " + sDesc + " ... ");
        currentSessionName = sName;
        SESSION mises = xedl.getEdl().getSESSION();
        if(mises==null) return;
        mises.setID(sName);
        if(sDesc!=null && sDesc.length() > 0){
        	mises.setCOMMENT(sDesc);
        	mises.setDESCRIPTION(sDesc);
        }
        mises.setSERVICEQUALITY(sQuality); 
        mises.setSERVICENAME(sService);
        mises.setOPENSESSION(sIsOpen);
        mises.setDELIVERYPLATFORM("Isabel " + Servutilities.getIsabelVersionClear() + " " + Servutilities.getIsabelRelease());
        
        
    }
    
    
    public String getSessionName(){
    	outTraceFile.println("Metodo getSessionName de XEDLManager");
        if(currentSessionName.equals("")){
        	SESSION sess = xedl.getEdl().getSESSION();
            currentSessionName = sess.getID();
        }
        return currentSessionName;
    }
    
    
    public String getSessionQuality(){
    	outTraceFile.println("Metodo getSessionQuality de XEDLManager");
        return xedl.getEdl().getSESSION().getSERVICEQUALITY();
    }
    
    
    public String getSessionService(){
    	outTraceFile.println("Metodo getSessionService de XEDLManager");
        return xedl.getEdl().getSESSION().getSERVICENAME();
    }
    
    
    public String getSessionDescription(){
    	outTraceFile.println("Metodo getSessionDescription de XEDLManager");
        return xedl.getEdl().getSESSION().getDESCRIPTION();
    }
    
    
    public boolean saveXEDL(){
    	outTraceFile.println("Metodo saveXEDL de XEDLManager");
        if(state!=E_XEDL_LOADED) return false;
        try{
            xedl.save();
            return true;
        }
        catch(XEDLException e){
        	e.printStackTrace(outTraceFile);
            return false;
        }
    }
    
    
    /**
     * Metodo para conseguir mi id del xedl local
     * @return String con el id
     */
    public String getMyIDFromLocalXEDL(){
    	List<SITE> lista = xedl.getEdl().getSiteDescription().getSITE();
    	return lista.get(0).getSiteIdentification().getID();    	
    }
    
    public boolean saveXEDL(String file){
    	outTraceFile.println("Metodo saveXEDL de XEDLManager");
        try{
            //System.out.println("vi");
        	xedl.setPath(file);
            xedl.save(file);
            xedlFile=file;
            state=E_XEDL_LOADED;
            return true;
        }
        catch(XEDLException e){
        	e.printStackTrace(outTraceFile);
            return false;
        }
    }
    
    
    public void setXEDLFile(String File){
    	outTraceFile.println("Metodo setXEDLFile de XEDLManager con file: " + File);
        xedlFile=File;
    }

    
    public boolean deleteNode(String id){
    	outTraceFile.println("Metodo deleteNode de XEDLManager");
        if(siteList.deleteSite(siteList.getSite(id))){
            listaIDs=siteList.getSiteIds();
            return true;
            }
        else {
            outTraceFile.println("No borr� ningun nodo...");
            return false;
        }
    }
        
    
    public String getIPbyID(String id){
    	outTraceFile.println("Metodo getIPbyID de XEDLManager para la id " + id);
        SITE site=siteList.getSite(id);
        LinkedList address = site.getSiteAddress();
        int n = address.size();
        if(n>0){
            return (String) address.getFirst();
        }
        else{
            return null;
        }
    }
    
    //metodo para fijar un nuevo password, se le llama desde isabelXEDLViewer
    public void changePassword(String local_id, String encrypted_passwd){
    	outTraceFile.println("Metodo changePassword de XEDLManager");
    	siteList.getSite(local_id).setAccessControl(encrypted_passwd);
    }
    
    public void almacenarPosicionSitio(int x,int y,String id){
    	outTraceFile.println("Metodo almacenarPosicionSitio de XEDLManager para el sitio " + id);
        SITE site=siteList.getSite(id);
        if(site==null) 
        	return;
        List<Element> lista = site.getSITEADDITIONALPARAMS().getAny();
        boolean x_done=false;
        boolean y_done=false;
        if(lista.size()>0)
        {
        	for(int i=0; i<lista.size(); i++)
        	{
        		org.w3c.dom.Element elem = lista.get(i);
        		if(elem.getNodeName().equals("GUI_X"))
        		{
        			elem.setTextContent(String.valueOf(x));
        			x_done=true;
        		}
        		if(elem.getNodeName().equals("GUI_Y"))
        		{
        			elem.setTextContent(String.valueOf(y));
        			y_done=true;
        		}
        		if(x_done && y_done)
        			return;
        	}
        }
        //si llega aquí es que no había nodos gui_x y gui_y
        
        DocumentBuilder db = null;
		try {
			db = DocumentBuilderFactory.newInstance().newDocumentBuilder();
		} catch (ParserConfigurationException e) {			
			e.printStackTrace(outTraceFile);
		}
        Document doc = db.newDocument();
        outTraceFile.println("document creado, vamos a crear GUI_X y GUI_Y");
        org.w3c.dom.Element gui_x = doc.createElement("GUI_X");
        gui_x.setTextContent(String.valueOf(x));
        gui_x.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
        org.w3c.dom.Element gui_y = doc.createElement("GUI_Y");
        gui_y.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
        gui_y.setTextContent(String.valueOf(y));
        outTraceFile.println("almacenamos los Element en la lista");
        lista.add(gui_x);
        lista.add(gui_y);
    }
    
    
    public int[] getPosicionSitio(String id){
    	outTraceFile.println("Metodo getPosicionSitio de XEDLManager para la id " + id);
        int coordenadas[]=new int[2];
        coordenadas[0]=-1;
        coordenadas[1]=-1;
        SITE site=siteList.getSite(id);
        if(site==null) 
        	return coordenadas;
        
        List<Element> lista = site.getSITEADDITIONALPARAMS().getAny();
        int x = -1;
        int y = -1;
        if(lista.size()>0)
        {
        	for(int i=0; i<lista.size(); i++)
        	{
        		org.w3c.dom.Element elem = lista.get(i);
        		if(elem.getNodeName().equals("GUI_X"))
        		{
        			Integer I=new Integer(elem.getTextContent());
        	        x=I.intValue();
        		}
        		if(elem.getNodeName().equals("GUI_Y"))
        		{
        			Integer I=new Integer(elem.getTextContent());
        	        y=I.intValue();
        		}
        	}
        }
        coordenadas[0]=x;
        coordenadas[1]=y;
        return coordenadas;
    }
    
    
    public int getNoNodos(){
    	outTraceFile.println("Metodo getNoNodos de XEDLManager");
        return listaIDs.size();
    }
    
    
    public void setPadre(String padre,String hijo){
    	outTraceFile.println("Metodo setPadre de XEDLManager para padre " + padre + " e hijo " + hijo);
        String IPPadre = this.getIPbyID(padre);
        SITE siteHijo = siteList.getSite(hijo);
        siteHijo.deleteMCUConnectionMode();
        siteHijo.insertMCUAddress(IPPadre,true);
    }
    
    

    
    /*
     *borra una dirección IP de un site y elimina los modos de conexión de
     *de los hijos que tengan una referencia a esa IP.
     */
    public void deleteIPfromNode(String IP,String idSite){
    	outTraceFile.println("Metodo deleteIPfromNode de XEDLManager con IP " + IP + " y con id " + idSite);
        SITE sitex;
        String ipPadre="";
        int numSites = listaIDs.size();
        if(numSites<=0) {
            outTraceFile.println("no hay lista de ids");
            return;
        }
        //Borramos el conection mode de los que tiene como padre este sitio
        for(int i=0;i<numSites;i++){
            String id = (String) listaIDs.get(i);
            sitex=siteList.getSite(id);
            ipPadre=sitex.getParentSiteAddress();
            if(ipPadre!=null && ipPadre.equals(IP)){
                sitex.setCONNECTIONMODE(null);
            }
        }
        //borramos la IP del nodo
        sitex = siteList.getSite(idSite);
        sitex.deleteSiteAddress(IP);
    }
    
    

   /*
    * este método permite cambiar las conexiones a uno nodo si se conoce la
    * ip vieja y nueva.
    */
    public void changeIPfromNode(String newIP,String oldIP, String idSite){   
    	outTraceFile.println("Metodo changeIPfromNode de XEDLManager con newIP " + newIP + " para el site " + idSite);
        //Si son iguales no hay que hacer cambios o si la nueva IP es null        
        if(newIP.equals(oldIP) || newIP==null){
            return;
        }
        SITE sitex;
        String ipPadre="";
        int numSites=0;
        numSites = listaIDs.size();      
        if(numSites<=0) { //No hay sites           
            return;
        }
        //Borramos el conection mode de los que tiene como padre este sitio
        for(int i=0;i<numSites;i++){
            String id=(String) listaIDs.get(i); //recorremos la lista de IDs
            if(id.equals(idSite)){
                continue;
            }
            sitex = siteList.getSite(id);
            ipPadre = sitex.getParentSiteAddress();           
            if(ipPadre!=null && ipPadre.equals(oldIP)){
                String conMode=getConnectionMode(sitex.getID());
                boolean mcast=false;
                if(conMode.equals(CONNECTION_MODE_MULTICAST)){
                    mcast=true;
                }
                else if (conMode.equals(CONNECTION_MODE_MCU)){
                    mcast=false;
                }                
                setIPPadre(id, idSite, newIP, mcast);                
            }
            
        }
        sitex = siteList.getSite(idSite);
        sitex.deleteSiteAddress(oldIP);
        if(!newIP.equals(""))
        {
        	sitex.insertSiteAddress(newIP, true);
        }
    }
    
    
    public void setIPPadre(String idSite,String idPadre,String IPPadre,boolean mcast){
    	outTraceFile.println("Metodo setIPPadre de XEDLManager para el site " + idSite + " y el padre " + idPadre);
        SITE siteHijo=siteList.getSite(idSite);
        if(siteHijo!=null){            
            if(!mcast){
                siteHijo.deleteMCUConnectionMode();
                siteHijo.insertMCUAddress(IPPadre,true);
            }
            else{
                siteHijo.deleteMulticastConnectionMode();
                siteHijo.deleteMCUConnectionMode();
                MulticastParameters mparam=siteHijo.setConnectionModeMulticast();
                mparam.setMCASTROOT(IPPadre);
            }
        }
    }
    
    
    public void addIPAddressToNode(String ip,String idSite){
    	outTraceFile.println("Metodo addIPAddressToNode de XEDLManager");
        SITE site=siteList.getSite(idSite);
        if(!ip.equals(""))
        {
        	site.insertSiteAddress(ip, false);
        }
    }
    
    
    public void updateConectionMode(String idSite,boolean multicast,boolean peer){
    	outTraceFile.println("Metodo updateConectionMode de XEDLManager para el site " + idSite);
        try{
            SITE site=siteList.getSite(idSite);
            if(site==null) {
                outTraceFile.println("Sitio Nulo No se puede actualizar modo de conexi�n");
                return;
            }
            String connectionMode = getConnectionMode(idSite);
            String padre="";
            int cmode=0;  //modo de conexi�n actual 0-> No hay modo de conexi�n
            if(connectionMode!=null){
                if(connectionMode.equals(CONNECTION_MODE_MCU)){
                    padre=(String)site.getMCUAddress().getFirst();
                    cmode=1;
                }
                else if(connectionMode.equals(CONNECTION_MODE_MULTICAST)){
                    padre=site.getCONNECTIONMODE().getMulticast().getMulticastParameters().getMCASTROOT();
                    cmode=2;
                }
            }
            if(padre==null) padre="";
            outTraceFile.println("Padre del nodo a actualizar:" + padre);
            
            int cmodePedido=0; // Modo de conexi�n solicitado
            if(peer && multicast) cmodePedido=1;  //peer
            if(peer && !multicast) cmodePedido=1; //peer
            if(!peer && multicast) cmodePedido=2;  //Multicast
            if(!peer && !multicast) cmodePedido=0;  //Ninguno
            
            //De acuerdo al modo de conexi�n actual hacemos
            switch(cmode){
                case 0: //No hay modo de conexi�n.
                    switch(cmodePedido){
                        case 1:
                            break;
                        case 2:                             
                            break;
                        case 0:
                            break;
                    }
                    break;
                case 1://Modo actual es MCU
                    switch(cmodePedido){
                        case 1:
                            if(multicast){
                                site.setConnectionModeMulticast();
                            }
                            else{
                                //se borra por si acaso...
                                site.deleteMulticastConnectionMode();
                            }
                            break;
                        case 2: //cambiamos a multicast
                            site.deleteMCUConnectionMode();
                            MulticastParameters mparam=site.setConnectionModeMulticast();                            
                            if(!padre.equals("")){
                                mparam.setMCASTROOT(padre);
                            }                            
                            break;
                        case 0:                            
                           // site.deleteMCUConnectionMode();
                            break;
                    }                    
                    break;
                case 2:// Modo de conexi�n actual es Multicast                                        
                    switch(cmodePedido){
                        case 1: //peer
                        	site.deleteMulticastConnectionMode();
                            //ENRIQUE, no se inserta mcu address si es ""
                            if(!padre.equals(""))
                            {
                            	site.insertMCUAddress(padre,true);
                            }
                            break;
                        case 2: //multicast
                            
                            break;
                        case 0:
                            site.deleteMulticastConnectionMode();
                            break;
                    }                    
                    break;
                default :
                    System.out.println("este es un nodo especial.");
                    break;
            }
        }catch(NullPointerException e){
        	e.printStackTrace(outTraceFile);
            return;
        }
    }
    
        
    public void saveMcastParameters(String idSite,int TTL,String audioG,String videoG,String ftpG,String pointerG,String vumeterG,String appShG){
    	outTraceFile.println("Metodo saveMcastParameters de XEDLManager para el site " + idSite);
        SITE site = siteList.getSite(idSite);
        MulticastParameters mparam = site.setConnectionModeMulticast();
        if(mparam==null) {
            outTraceFile.println("retorno mparam=null");
            return;
        }
        Integer mio = Integer.valueOf(TTL);        
        mparam.setMCASTTTL(mio.byteValue());
        mparam.setMCASTAUDIOGROUP(audioG);
        mparam.setMCASTVIDEOGROUP(videoG);
        mparam.setMCASTFTPGROUP(ftpG);
        mparam.setMCASTPOINTERGROUP(pointerG);
        mparam.setMCASTVUMETERGROUP(vumeterG);
        mparam.setMCASTAPPSHGROUP(appShG);
    }
    
    
    public String getSessionInfo(){
    	outTraceFile.println("Metodo getSessionInfo de XEDLManager");
        String info="";
        String cadena;
        SESSION mises = xedl.getEdl().getSESSION();
        if(mises==null){
            return "void";
        }
        cadena=mises.getDELIVERYPLATFORM();
        if(cadena==null) cadena="";
        info="<b>Delivery Platform: </b>"+cadena;
        cadena= mises.getSERVICENAME();
        if(cadena==null) cadena="";
        info="<FONT face='Verdana' color='#330099' size='2'><b>Service:</b>"+ cadena;
        
        cadena=mises.getSessionInfo().getFistCOMMENT();
        if(cadena==null) cadena="";
        info+="<br>"+"<b>Description: </b>"+cadena;
        cadena=mises.getSERVICEQUALITY();
        info+="<br>"+"<b>Quality: </b>"+cadena;
        
        if(mises.getSessionInfo().isOPENSESSION()){
            cadena="Opened";
        }else{
            cadena="Closed";
        }
        info+="<br>"+"<b>Session Type: </b>"+cadena;
        info+="</font>";
        return info;
    }
    
    
    public float getAnchoBanda(String ID_Hijo){
    	outTraceFile.println("Metodo getAnchoBanda de XEDLManager id " + ID_Hijo);
        SITE siteHijo = siteList.getSite(ID_Hijo);
        int bandwidth = siteHijo.getUpBandwidth();
        if(bandwidth<=0) return 1.f;
        else{
            return (float) bandwidth/128;
        }
    }
        
    
    public void addNewSite(String idSite,String ip, String passwd_claro){
    	outTraceFile.println("Metodo addNewSite de XEDLManager con site " + idSite + " con ip " + ip);
        SITE newSite = siteList.createSite(idSite);
        newSite.setNetworkAccessEthernet();
        if(!ip.equals(""))
        {
        	newSite.insertSiteAddress(ip,true);
        }
        newSite.setSITEROLE("interactive");
        //adicionamos a la lista de identificadores locales
        listaIDs.add(idSite);
        //tenemos que añadirle el passwd de sesion si lo hay
        if(passwd_claro != null)
        {
        	String encrypted_passwd = Servutilities.encryptPasswd(passwd_claro, idSite);
        	newSite.getSiteIdentification().setACCESSCONTROL(encrypted_passwd);
        }
        siteList.addSite(newSite);
    }  
    
    
    public String[] getXEDLServicesValues(){
    	outTraceFile.println("Metodo getXEDLServicesValues de XEDLManager");
        ServiceNames services = new ServiceNames();
        String[] serviceNames = services.getServicesPublicNames();
        return serviceNames;
    }
    
    
    public boolean siteExits(String idSite){
    	outTraceFile.println("Metodo siteExits de XEDLManager con id " + idSite);
        if(this.listaIDs.contains(idSite)){
            return true;
        }
        return false;
    }
        
    
    public void newXEDL(String sname,String quality,String comment,String service,boolean open) throws XEDLException {
    	outTraceFile.println("Metodo newXEDL de XEDLManager con nombre " + sname);
        xedl=new XEDL();
        Edl miedl = xedl.getEdl();
        SESSION mises = miedl.getSESSION();
        if(mises==null)
        	mises = new SESSION();
        mises.setID(sname);
        mises.setSERVICEQUALITY(quality);
        mises.setSERVICENAME(service);
        mises.addCOMMENT(comment);
        mises.setDESCRIPTION(comment);
        mises.setOPENSESSION(open);
        mises.setDELIVERYPLATFORM("Isabel " + Servutilities.getIsabelVersionClear() + " " + Servutilities.getIsabelRelease());
        
        siteList=new SiteList(xedl);
        miedl.setSESSION(mises);
        changeState(E_XEDL_CREATED);
    }
    
    

    
    /*
     * ChangeSiteID. Cambia el id de un site definido
     */
    public void changeSiteID(String newID,String oldID,String pname){
    	outTraceFile.println("Metodo changeSiteID de XEDLManager con newID" + newID + " y old " + oldID);
        SITE s = siteList.getSite(oldID);        
        if(s!=null){
            s.setID(newID);
            s.setPUBLICNAME(pname);
        }
        int nids = listaIDs.size();
        for(int i=0;i<nids;i++){
            String id=(String) listaIDs.get(i);
            if(id.equals(oldID)){
                listaIDs.set(i,newID);
            }
        }
    }
        
        
    public String getFileName(){
    	outTraceFile.println("Metodo getFileName de XEDLManager");
        return this.xedlFile;
    }
    
    
    private void changeState(int new_state){
    	outTraceFile.println("Metodo changeState de XEDLManager");
        switch(new_state){
            case E_WITHOUT_XEDL:
                listaIDs.clear();
                xedlFile=null;
                state=new_state;
                break;
            case E_XEDL_LOADED:
                state=new_state;
                break;
            case E_XEDL_CREATED:
                listaIDs.clear();
                xedlFile=null;
                state=new_state;
                break;
        }
    }
    
    
    public static void main(String args[])
    {
    	XEDL elxedl = null;
    	try {
    		elxedl = new XEDL("/home/enrique/trabajo/rama/isabel/xlim/xedl/lib/xedl/prueba_completa.xedl");
		} catch (XEDLException e) {
			e.printStackTrace();
		}
		XEDLManager manager = new XEDLManager(elxedl);
		System.out.println("manager creado, comenzamos a probar");
		
		System.out.println("getIDRaiz " + manager.getIDRaiz());
		System.out.println("getHijos de azul " + manager.getHijos("azul"));
		System.out.println("getAllIDSites " + manager.getAllIDSites());
		System.out.println("getRole de azul " + manager.getRole("azul"));
		
		System.out.println("isAddressRepeated de 138.4.24.183 la tiene: " + manager.isAddressRepeated("138.4.24.183"));
		System.out.println("getConnectionMode para moriarty " + manager.getConnectionMode("moriarty"));
		System.out.println("isopensession: " + manager.isOpenSession());
		System.out.println("site summary de azul: " + manager.getSiteSumary("azul"));
		
		System.out.println("getSessionName: " + manager.getSessionName());
		System.out.println("getSessionQuality: " + manager.getSessionQuality());
		System.out.println("getSessionService: " + manager.getSessionService());
		System.out.println("getSessionDescription: " + manager.getSessionDescription());
		manager.setSessionData("nombre_nuevo", "nueva descrip", "10M", "telemeeting", false);
		System.out.println("ip de azul: " + manager.getIPbyID("azul"));
		System.out.println("añadimos ip a mcu ");
		manager.addIPAddressToNode("nueva_ip", "mcu");
		System.out.println("borramos sitio triton: " + manager.deleteNode("triton"));
		
		System.out.println("almacenarPosicionSitio 200 300 de azul");
		
		System.out.println("getPosicionSitio la x y la y de moriarty: " + manager.getPosicionSitio("moriarty")[0] + " " + manager.getPosicionSitio("moriarty")[1]);
		manager.almacenarPosicionSitio(200, 300, "azul");
		System.out.println("setpadre con padre azul e hijo final ");
		manager.setPadre("azul", "final");
		System.out.println("deleteipfromnode para multi");
		manager.deleteIPfromNode("multi.dit.upm.es", "multi");
		
		System.out.println("changeIPfromNode de mcu ");
		manager.changeIPfromNode("138.4.24.185","mcu1.dit.upm.es","mcu");
		//manager.deleteConection("fin", "multi");
		manager.deleteParentConection("multi");
		
		System.out.println("updateconnectionmode de moriarty a unicast");
		manager.updateConectionMode("moriarty", false, true);
		System.out.println("updateconnectionmode de mcu a multicast");
		manager.updateConectionMode("mcu", true, false);
		System.out.println("setmulticastparams para mcu ");
		manager.saveMcastParameters("mcu", 2, "audiog", "videog", "ftpg", "pointerg", "vumeterg", "appshg");
		System.out.println("getSessionInfo: " + manager.getSessionInfo());
		
		System.out.println("addNewSite ");
		manager.addNewSite("newone","ipnueva.dit.upm.es", "passwd");
		
		System.out.println("getXEDLServicesValues " + manager.getXEDLServicesValues()[0]);
		
		System.out.println("changeSiteID para multi2 por multi3");
		manager.changeSiteID("multi3", "multi2", "nombre_publico");
		manager.saveXEDL("/home/enrique/trabajo/rama/isabel/xlim/xedl/lib/xedl/salida.xedl");
		System.out.println("Terminado");
    }
}

