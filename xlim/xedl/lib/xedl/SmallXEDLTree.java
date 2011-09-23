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
 * SmallEDLTree.java
 *
 * Created on 23 de marzo de 2004, 16:13
 */

package xedl.lib.xedl;

import java.util.*;


/**
 * This class represents a branch with a root node.
 * @author Diego Andres Acosta
 * @since Apr 2004
 */
public class SmallXEDLTree {
    
    private String idPadre=null;
    private LinkedList listaHijos=null;
    private int nHijos=0;
    private XEDLManager xedlM=null;
    
    /** Creates a new instance of SmallEDLTree */
    public SmallXEDLTree(XEDLManager xedlObject) {
        xedlM=xedlObject;
        listaHijos=new LinkedList();
    }
    
    /**
     * Return the root ID
     * @return This method return a String with the root ID
     */    
    public String getIDPadre() {
        return idPadre;
    }
    
    /**
     * This method return a list of ID's depending of the root. This information is collected from the XEDL API
     * @return A LinkedList object. This linkedList could be emtpy
     */    
    public LinkedList getArrayIDsHijos() {
        return listaHijos;
    }
    
    /**
     * Set the root ID for this small XEDL tree object and get the children list.
     * @param id root ID
     */    
    public void setIDPadre(String id) {
        if(id!=null && id.length()>0){
        idPadre=id;
        listaHijos=xedlM.getHijos(idPadre);        
        }
    }
    
    /**
     * Set the children array list depending of the root node
     * @param l LinkedList with the childen IDs
     */    
    public void setArrayHijos(LinkedList l) {
        listaHijos=l;
    }
    
    /**
     * Return the number of childs having the root ID. This is the number of nodes in the linkedlist
     * @return 0 or N.
     */        
    public int getNHijos(){  
        if(listaHijos==null) return 0;
        return listaHijos.size();        
    }
    
    public String getIDHijo(int pos) {
        if(listaHijos!=null){
            if(pos<listaHijos.size()){
                return (String) listaHijos.get(pos);
            }            
        }
        return null;
    }
    
}
