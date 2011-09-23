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
package xedl.lib.xedl;



   import java.io.*;

   import java.lang.*;
   import java.net.*;
   import java.util.*;



import services.isabel.lib.Constants;

import xedl.lib.jaxb.CONNECTIONMODE;
import xedl.lib.jaxb.Edl;
import xedl.lib.jaxb.SITE;
import xedl.lib.jaxb.SiteDescription;


import java.text.SimpleDateFormat;

/**
	Class that represents the list of sites defined in an XEDL file
*/
    public class SiteList {
   
    	/**
    	 * EDL that contains all the definition of the session
    	 */
    	private Edl the_edl;
    	
    	/**
    	 * Para las trazas
    	 */
        public PrintWriter outTraceFile= null;
      
	   /**
	    Creates an EventInfo object.Needs the DOM document in which it is going to be inserted.
	   */
       public SiteList(XEDL xedl){
    	   outTraceFile = Constants.inicializa_trazas_jaxb();
    	   outTraceFile.println("Constructor SiteList");
    	   the_edl = xedl.getEdl(); 
       }
    
       
	    /**
	    * Returns a site parent node identifier. 
	    * WARNING:This function searches in CONNECTION-MODE fields keeping this preference order:
	    * MCU_ADDRESS->PEER_ADDRESS->MCAST_ROOT.
	    * The identifier returned will correspond to the first ip address found.
	    **/
       public String getParentSiteId(String id){
    	   outTraceFile.println("Metodo getParentSiteId de SiteList para la id: " + id);
      	   SITE site = this.getSite(id);
      	   String result = null;
      	   SITE auxSite = null;
      	   String parentAddress = site.getParentSiteAddress();
      	   if (parentAddress != null){
             LinkedList siteList = this.getSiteIds();
             int i=0;
             while ((i<siteList.size()) && (result==null)){
            	 String name_aux = (String) siteList.get(i);
            	 auxSite = this.getSite(name_aux);
                 LinkedList auxSiteAddresses = auxSite.getSiteAddress();
                 int j = 0;
                 while (j<auxSiteAddresses.size()){
                     if (parentAddress.equals(String.valueOf(auxSiteAddresses.get(j)))){
                          result = auxSite.getID();
                          break;
                     }
                     j++;
                 }
                 if (result != null){
                     break;
                 }
                 i++;
             }
      	   }
      	   return result;
       }
       
       
       
     /**
      *	Returns a site, identified by "id", unicast and mcu children sites.
      * Returns empty list not found
      */
          public LinkedList getChildrenSites(String id){                    
        	outTraceFile.println("Metodo getChildrenSites de SiteList para la id: " + id);
        	if (id==null){
               return null;
            }
            SITE site = this.getSite(id);            
            LinkedList addressList=site.getSiteAddress();
            LinkedList tempMCUAddressList=new LinkedList();
            LinkedList tempPeerAddressList=new LinkedList();
            LinkedList sitesList=this.getSiteIds();
            LinkedList children=new LinkedList();
            CONNECTIONMODE connectionMode = null;
            //For all sites
            int i=0;		
            while (i<sitesList.size()){
               SITE tempSite = this.getSite((String)sitesList.get(i));
               tempMCUAddressList=tempSite.getMCUAddress();
               tempPeerAddressList=tempSite.getPeerAddress();
               //ENRIQUE, los sitios multicast no los añade como hijos, es porque no tienen mcu ni peer
               //intento detectar si son multicast y si estan unidos al padre
               connectionMode = tempSite.getCONNECTIONMODE();
               if(connectionMode!=null && connectionMode.getMulticast()!=null)
               {
               	//es sitio multicast
               	//veo si el padre es del que estamos calculando los hijos
               	int j=0;
               	while (j<addressList.size())
               	{
               		if(tempSite.getParentSiteAddress()!=null && !tempSite.getParentSiteAddress().equals("") && tempSite.getParentSiteAddress().equals(addressList.get(j)))
               		{
               			outTraceFile.println("Hijo multicast encontrado " + (String) sitesList.get(i));
               			children.add(sitesList.get(i));
               		}
               		j++;
               	}
               }
               else
               {
               //For all addresses
               int j=0;
               while (j<addressList.size()){
               		//is addressList.get(i) contained in new Site(this.doc,sitesList(j)).getMCUAddress())?
               		if(tempMCUAddressList!=null && tempMCUAddressList.contains(addressList.get(j))){
               	   		//If contained, add to children
               			outTraceFile.println("Hijo con MCUAddress a mi " + (String) sitesList.get(i));
               			children.add(sitesList.get(i));
                  	}
                  	if(tempPeerAddressList!=null && tempPeerAddressList.contains(addressList.get(j))){
               	   		//If contained, add to children
                  		outTraceFile.println("Hijo con peeraddress a mi " + (String) sitesList.get(i));
                  		children.add(sitesList.get(i));
                  	}
                  	j++;
               	}
               }
               i++;
            }
            return children;  
              
         }
       
       
          
      public LinkedList getMcastChildrenSites(String id){
    	  outTraceFile.println("Metodo getMcastChildrenSites de SiteList para la id: " + id);
    	  if (id==null){
             return null;
          }
          SITE site = this.getSite(id);
          LinkedList addressList = site.getSiteAddress();
          LinkedList sitesList=this.getSiteIds();
          LinkedList children=new LinkedList();
          //For all sites
          int i=0;		
          while (i<sitesList.size()){
        	 SITE tempSite = this.getSite((String)sitesList.get(i));        	 
             if(tempSite.getCONNECTIONMODE()!=null && tempSite.getCONNECTIONMODE().getMulticast()!=null){
            	 String rootNode = tempSite.getCONNECTIONMODE().getMulticast().getMulticastParameters().getMCASTROOT();
            	 if (rootNode!=null){
                     //For all addresses
                     if (addressList.contains(rootNode)){
                    	 outTraceFile.println("Hijo multicast encontrado " + (String) sitesList.get(i));
                    	 children.add(sitesList.get(i));
                     }                                  
                 }//if  
             }            
             i++;
          }
          return children;
       } 
          
          
       
      public LinkedList getAllChildrenSites(String id){
          return this.getChildrenSites(id);
      }
      
      
      

     /**
      *	Returns sites path from site "src" to site "dst" supposing "dst" site is a child node of "src".
      **/
          public LinkedList getPathToChild(String src,String dst,LinkedList path){
        	outTraceFile.println("Metodo getPathToChild de SiteList para src: " + src + " y dst: " + dst);
        	if (src.equals(dst)){
               LinkedList same=new LinkedList();
               same.add(src);
               return same;
            }//if
         //Getting all childen nodes
            LinkedList list=this.getAllChildrenSites(src);
            int i=0;
         //For each child node
            while (i<list.size()){
               if(String.valueOf(list.get(i)).equals(dst)){
               //Node found: adding to path
                  path.add(list.get(i));
                  break;
               }
               else{
                  if(path.size()==0){
                  //No sites added to path so recursive search must be done
                     XEDLTools.concat(path,this.getPathToChild(String.valueOf(list.get(i)),dst,path));
                  }//if
               }//if/else
               i++;
            }//while
         //There are children sites added to path, so this site id must be addend to path
            if(path.size()>0){
               path.add(src);
            }//if
            return path;
         }
      
       
          
	   /**
	    *	Returns sites path from site "src" to site "dst" 
	    *   supposing one node has just one parent node and "src" site is a child node of "dst".
	    **/
		public LinkedList getPathToParent(String src,String dst,LinkedList path){
			outTraceFile.println("Metodo getPathToParent de SiteList para src: " + src + " y dst: " + dst);
			if (src.equals(dst)){
				LinkedList same=path;
				same.addLast(src);
				return same;
			}//if
			//Creating site			
			SITE srcSite=this.getSite(src);
			if (srcSite==null){
				return null;
			}else{
				CONNECTIONMODE connMode=srcSite.getCONNECTIONMODE();
				LinkedList list=new LinkedList();				
				if(connMode.getMcu()!=null){
					list=XEDLTools.concat(list,srcSite.getMCUAddress());
				}else{
					if(connMode.getUnicast()!=null){
						list=XEDLTools.concat(list,srcSite.getPeerAddress());
					}else{
						if(connMode.getMulticast()!=null){
							list.add(connMode.getMulticast().getMulticastParameters().getMCASTROOT());
						}//if
					}//if/else
				}//if/else
				LinkedList allNodes=this.getSiteIds();
				String parentNodeIp=String.valueOf(list.getFirst());
				String parentNodeId=null;
				//Getting ID for parent IP
				int j=0;
				SITE parentSite=null;
				LinkedList parentSiteIps=null;
				while (j<allNodes.size()){
					parentSite=this.getSite(String.valueOf(allNodes.get(j)));
					parentSiteIps=parentSite.getSiteAddress();
					if (parentSiteIps.contains(parentNodeIp)){
						parentNodeId=parentSite.getID();
						break;
					}
					j++;
				}//while
				path.addLast(src);
				path=this.getPathToParent(parentNodeId,dst,path);;
				return path;
			}//if/else
			
		}
          
		
		

		   /**
		   *	Returns sites path from site "src" to site "dst". 
		   **/
	       public LinkedList getPath(String src,String dst){
	    	 outTraceFile.println("Metodo getPath de SiteList para src: " + src + " y dst: " + dst);
	    	 String root=this.getRootSite();
	         LinkedList pathA=new LinkedList();
	         LinkedList pathB=new LinkedList();
	         //Getting paths to both sites
	         SimpleDateFormat timeFormatter = new SimpleDateFormat("HH:mm:ss.SSS");
	         LinkedList listA=this.getPathToChild(root,src,pathA);
	         LinkedList listB=this.getPathToChild(root,dst,pathB);
	         //System.out.println("listA= "+listA);
	         //System.out.println("listB= "+listB);		
	         //Getting intersection
	         int i=0;
	         String last=null;
	         LinkedList listAux=new LinkedList();
	         listA = XEDLTools.flip(listA);//Put root node first
	         listB = XEDLTools.flip(listB);//Put root node first
	         //System.out.println("listA= "+listA);
	         //System.out.println("listB= "+listB);
	         while((i<listA.size()) && (i<listB.size())){
	         //if both elements are equal they are eliminated
	            if (listA.get(i).equals(listB.get(i))){
	               last=String.valueOf(listA.get(i));
	               listA.removeFirst();
	               listB.removeFirst();
	            }//if
	            i++;
	         }//while
	         listAux=listA;
	         listAux.addFirst(last);//Inserting coincident node
	         //System.out.println("listAux= "+listAux);
	         //System.out.println("listA= "+listA);
	         //System.out.println("listB= "+listB);
	         listB=XEDLTools.flip(listB);//Put coincident node last
	         listAux=XEDLTools.concat(listB,listA);
	         return XEDLTools.flip(listAux);
	      }
		
		
	       
	       
	       
	       /**
	        *	Returns sites path from site "src" to site "dst".Supposes each node has just one parent node.
	        **/
        public LinkedList getPathTree(String src,String dst){
          outTraceFile.println("Metodo getPathTree de SiteList para src: " + src + " y dst: " + dst);
	      String root=this.getRootSite();
          LinkedList pathA=new LinkedList();
          LinkedList pathB=new LinkedList();
          //Getting paths to both sites
          LinkedList listA=this.getPathToParent(src,root,pathA);
          LinkedList listB=this.getPathToParent(dst,root,pathB);
          //Getting intersection
          int i=0;
          String last=null;
          LinkedList listAux=new LinkedList();
          listA=XEDLTools.flip(listA);//Put root node first
          listB=XEDLTools.flip(listB);//Put root node first
          while((i<listA.size()) && (i<listB.size())){
          //if both elements are equal they are eliminated
             if (listA.get(i).equals(listB.get(i))){
                last=String.valueOf(listA.get(i));
                listA.removeFirst();
                listB.removeFirst();
             }//if
             i++;
          }//while
          listAux=listA;
          listAux.addFirst(last);//Inserting coincident node
          listB=XEDLTools.flip(listB);//Put coincident node last
          listAux=XEDLTools.concat(listB,listA);
          return XEDLTools.flip(listAux);
       }	  
   
        
        
        
        
      /**
       * Adds site element from other XEDL.Returns Site object added.
       */
       public SITE addSite(SITE site){
    	 outTraceFile.println("Metodo addSite de SiteList para site: " + site.getID());
    	 if (site==null){
            return null;
         }
         String id = site.getID();
         int i=0;
         if(the_edl.getSiteDescription()==null)
         {
        	 SiteDescription sitedes = new SiteDescription();
        	 the_edl.setSiteDescription(sitedes);
         }
         List<SITE> lista = the_edl.getSiteDescription().getSITE();
         if(lista.contains(site))
         {
        	 //existe el sitio, hay que quitarlo de la lista
        	 lista.remove(site);
         }
         //ya no está o no estaba, lo añado
         lista.add(site);
         return site;
      }
           
     
     /**
      *	Deletes site element.Returns true if ok.
      */
      public boolean deleteSite(SITE site){
    	outTraceFile.println("Metodo deleteSite de SiteList para site: " + site.getID()); 
    	boolean result=false;
        if (site==null){
           return false;
        }
        List<SITE> lista = the_edl.getSiteDescription().getSITE();
        if(lista.contains(site))
        {
       	 //existe el sitio, hay que quitarlo de la lista
       	 lista.remove(site);
       	 return true;
        }
        return false;
     }	
       
      

     /**
      *	Returns the number of sites on this XEDL.
      */
      public int getLength(){
    	  outTraceFile.println("Metodo getLength de SiteList");
    	  return this.getSiteIds().size();
      }
      
          
      /**
       * Returns root site
       * this time the good one
       */
      public String getRealRootSite(){
    	   outTraceFile.println("Metodo getRealRootSite de SiteList");
	   	   //lista de nodos con connection mode
	   	   if(the_edl.getSiteDescription()==null)
	   		   the_edl.setSiteDescription(new SiteDescription());
    	   List<SITE> lista = the_edl.getSiteDescription().getSITE();
	   	   //recorro la lista de sites
	   	   //hash que contiene la direcc del sitio y el número de links al sitio
	   	   HashMap links_to_site = new HashMap(lista.size());
	   	   //array con todas las direcc
	   	   String all_addresses[] = new String[lista.size()*10];
	   	   //array con los sitios de los que salen enlaces para ver que no pueden ser master
	   	   String connected_sites[] = new String[lista.size()*10];
	   	   int index = 0;
	   	   SITE temp = null;
	   	   for (int i=0;i<lista.size();i++)
	   	   {
	   		   temp = lista.get(i);
	   		   //miro a quien está conectado ese nodo ya sea por multicast o unicast
	   		   String conected_to = getConnectedTo(temp);    		   
	   		   LinkedList address = temp.getSiteAddress();
	   		   //String address[] = getNodeAddress(site_node);
	   		   int j = 0;
	   		   while (!address.isEmpty())
	   		   {    			   
	   			   all_addresses[index] = (String) address.get(j);
	   			   //si el nodo no está conectado a nada lo añado como posible master
	   			   //si si está lo quito porque ya no puede ser master
	   			   if(conected_to==null || conected_to.equals(""))
	   			   {
	   				   Integer ocurrences =(Integer) links_to_site.remove(address);	
	   				   if(ocurrences==null)
	   					   ocurrences = new Integer(0);
	   				   links_to_site.put(address,ocurrences);
	   			   }
	   			   else
	   			   {
	   				   //si esta conectado a algo no es master, lo guardo en sisitos conectados    			   
	   				   connected_sites[i] = (String) address.get(j);
	   				   //si ya estaba cojo el numero de links que le apuntan y le sumo uno	    			   
	   				   if(links_to_site.containsKey(conected_to))
	   				   {
	   					   Integer ocurrences =(Integer) links_to_site.remove(conected_to);
	   					   links_to_site.put(conected_to,ocurrences + 1);
	   				   }
	   				   else
	   					   links_to_site.put(conected_to,new Integer(1));
	   			   }
	   			   address.remove(j);
	   			   index++;
	   		   }//fin del while
	   	   }    
	   	   //quito de la hash los sitios con conexión saliente porque no pueden ser master
	   	   for(int i =0;i<connected_sites.length;i++){
	   		   links_to_site.remove(connected_sites[i]);
	   	   }
	   	   String master_address = getMasterAddress(links_to_site, all_addresses);
	   	   String real_master = getIdFromAddress(master_address);
	   	   return real_master;
      } 
      
      
      /**
       * Returns the site to which this site is connected to
       */
      public String getConnectedTo(SITE site)
      {
	       outTraceFile.println("Metodo getConnectedTo de SiteList para site: " + site.getID());
	   	   if(site.getCONNECTIONMODE()!=null && site.getCONNECTIONMODE().getMulticast()!=null)
	   	   {
	   		   return site.getCONNECTIONMODE().getMulticast().getMulticastParameters().getMCASTROOT();
	   	   }
	   	   else if(site.getCONNECTIONMODE()!=null && site.getCONNECTIONMODE().getUnicast()!=null)
	   	   {
	   		   return site.getCONNECTIONMODE().getUnicast().getPEERADDRESS().get(0);
	   	   }
	   	   else if(site.getCONNECTIONMODE()!=null && site.getCONNECTIONMODE().getMcu()!=null)
	   	   {
	   		   return site.getCONNECTIONMODE().getMcu().getMCUADDRESS().get(0);
	   	   }
	   	   else
	   	   {
	   		   return "";
	   	   }
      }
      
      
      /**
       * Method to get the id of a site from the address of that site
       */
      public String getIdFromAddress(String address)
      {
    	    outTraceFile.println("Metodo getIdFromAddress de SiteList para la direcc: " + address);
	   	    if(address==null)
	   		    return null;
		   	String result = null;
	   	    SITE auxSite = null;
		   	LinkedList siteList = this.getSiteIds();
	        int i=0;
	        while ((i<siteList.size()) && (result==null)){
	       	    String name_aux = (String) siteList.get(i);
	       	    auxSite = this.getSite(name_aux);
	            LinkedList auxSiteAddresses = auxSite.getSiteAddress();
	            int j = 0;
	            while (j<auxSiteAddresses.size()){
	                if (address.equals(String.valueOf(auxSiteAddresses.get(j)))){
	                     result = auxSite.getID();
	                     break;
	                }
	                j++;
	            }
	            if (result != null){
	                break;
	            }
	            i++;
	        }	 	   
	 	   return result;
      }
      
      
      public String getMasterAddress(HashMap links_to_site, String[] all_addresses)
      {
    	   outTraceFile.println("Metodo getMasterAddress de SiteList");
	   	   //devuelve el key con el valor mayor
	   	   if(all_addresses.length==0)
	   		   return null;
	   	   Collection valores = links_to_site.values();
	   	   Integer maximo = (Integer) Collections.max(valores);
	   	   for(int i =0; i<all_addresses.length;i++)
	   	   {
	   		   Integer actual = (Integer) links_to_site.get(all_addresses[i]);
	   		   if(actual==maximo)
	   			   return all_addresses[i];
	   	   }
	   	   return all_addresses[0];
      }
      
      
     /**
      * Returns root site ("master site") ID field
      */
      public String getRootSite(){
        //getMASTER es un objeto de tipo nodo, tenemos que obtener el value
    	//the_edl.getSESSION().getSessionInfo().getMASTER();
    	  
    	String root=null;
    	SITE temp = null;
        boolean isRoot=false;
        LinkedList ids = this.getSiteIds();
        while(!ids.isEmpty() && !isRoot)
        {
        	temp = this.getSite((String)ids.poll());
        	if(temp.getCONNECTIONMODE()==null)
        	{
        		isRoot = true;
        		root = temp.getID();
        	}
        		
        }
        return root;
     }	
	       
	       
	       
      /**
      	Returns a LinkedList with all site ID field
      */
          public LinkedList getSiteIds(){
        	outTraceFile.println("Metodo getSiteIds de SiteList");
        	SiteDescription sitedes = the_edl.getSiteDescription();
        	if(sitedes==null)
        		sitedes = new SiteDescription();
            LinkedList ids= new LinkedList();
            //Searching for SITE nodes
            List<SITE> list = sitedes.getSITE();
            for(int i=0; i<list.size();i++)
            {
            	ids.add(i,list.get(i).getID());
            }
            return ids;
         }
          
        
          
     /**
     	Returns site whose ID field equals "id" or null if not found.
     */
        public SITE getSite(String id){
           outTraceFile.println("Metodo getSite de SiteList para la id: " + id);
           SITE site = null;
           if (id==null){
              return null;
           }
      	   if(the_edl.getSiteDescription()!=null)
      	   {
      		 site = the_edl.getSiteDescription().getSITE(id);
      	   }
           return site;
        } 
       
       
            
 	   /**
 	   	Creates a new Site object identified by "id". If a site identified by "id" already exists, it is returned.
 	   */
       public SITE createSite(String id){
     	 outTraceFile.println("Metodo createSite de SiteList para la id: " + id);
      	 if (id==null){
             return null;
          }
      	 if(getSite(id)!=null)
      		 return getSite(id);
      	 else
      	 {
      		 SITE site = new SITE();
      		 site.setID(id);
      		 return site;
      	 }
       }
      
       
      
      public static void main(String args[]){
    	XEDL elxedl = null;
    	try {
            elxedl = new XEDL("/home/enrique/trabajo/rama/isabel/xlim/xedl/lib/xedl/prueba_completa.xedl");
		} catch (XEDLException e) {
			e.printStackTrace();
		}
		SiteList lalista = new SiteList(elxedl);
		System.out.println("getParentSiteId de moriarty: " + lalista.getParentSiteId("moriarty"));
		System.out.println("ChildrenSites para azul: " + lalista.getChildrenSites("azul"));
		System.out.println("MCASTChildrenSites para moriarty: " + lalista.getMcastChildrenSites("moriarty"));
		LinkedList path = new LinkedList();
		System.out.println("getPathToChild desde azul a fin: " + lalista.getPathToChild("azul", "fin", path));
		LinkedList path2 = new LinkedList();		
		System.out.println("getPathToParent desde fin a azul: " + lalista.getPathToParent("fin", "azul", path2));
		System.out.println("getRootSite devuelve: " + lalista.getRootSite());
		System.out.println("getPath de fin a mcu: " + lalista.getPath("fin", "mcu"));
		System.out.println("getPath de multi2 a final: " + lalista.getPath("multi2", "final"));
		System.out.println("getPath de azul a triton: " + lalista.getPath("azul", "triton"));
		System.out.println("getPathTree de fin a mcu: " + lalista.getPathTree("fin", "mcu"));
		System.out.println("getPathTree de multi2 a final: " + lalista.getPathTree("multi2", "final"));
		System.out.println("getPathTree de azul a triton: " + lalista.getPathTree("azul", "triton"));
		System.out.println("getRealRootSite devuelve: " + lalista.getRealRootSite());
		SITE site2 = lalista.getSite("moriarty");
		lalista.addSite(site2);
		lalista.deleteSite(site2);
		lalista.addSite(site2);
		System.out.println("Ha ido bien addSite y deleteSite");
		
      }
      
   
   }