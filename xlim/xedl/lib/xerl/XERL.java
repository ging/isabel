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
package xedl.lib.xerl;
//package xerl;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException; 

import org.xml.sax.SAXException;

import java.io.*;

import java.util.*;

import org.w3c.dom.Document;

import org.w3c.dom.*;

import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerConfigurationException;

import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

//import xedl.*;

public class XERL {
  /**
  * Location of XERL file represented by this XERL object
  */  
  private String path;
  
  
  /**
  * DOM document used by this XERL object
  */
  private Document doc;
  private Element rootElement;
  
  private String Status = "";
  private String Service = "";
  private String Address = "";
  private String Info = "";


  /**
   * Creates an XERL object that represents an XERL file named "path".If "path" file does not exist, an exception is thrown.
   */
  public XERL(String path) throws IOException,XERLException{
    DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
    //If validating towards an schema is desired, uncomment next line
    //factory.setValidating(true);
    //System.out.println("Validating?: "+factory.isValidating());
    try {
      DocumentBuilder builder = factory.newDocumentBuilder();
      Document auxDocument = builder.parse(new File(path));
      this.path=path;
      this.doc=auxDocument;
    }catch (SAXException sxe) { // Error generated during parsing  
      throw new XERLException(String.valueOf(sxe));
      /*Exception x = sxe;
      if (sxe.getException() != null) x = sxe.getException(); 
        x.printStackTrace();*/
    }catch (ParserConfigurationException pce) { // Parser with specified options can't be built 
      throw new XERLException(String.valueOf(pce));
      /*pce.printStackTrace();*/
    }
  }
  
  /**
   * Creates an XERL object that represents an XERL file
   */
  public XERL()throws XERLException{
    DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
    try {
      DocumentBuilder builder = factory.newDocumentBuilder();
      Document auxDocument = builder.newDocument();
      this.path="";
      rootElement = (Element) auxDocument.createElement("XERL");
//      rootElement = (Element) auxDocument.createElement("error");
      //rootElement.setAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
      auxDocument.appendChild(rootElement);
      //Node versionNode = edlNode.appendChild(auxDocument.createElement("VERSION"));
      //versionNode.appendChild(auxDocument.createTextNode("1.0"));
      this.doc=auxDocument;
    }catch (ParserConfigurationException pce) { // Parser with specified options can't be built 
      throw new XERLException(String.valueOf(pce));
      /*pce.printStackTrace();*/
    }
  }
   

  /** Constructor de la clase. Crea un objeto de tipo XERL asigna los valores stat, addr, serv e inf a los diferentes campos del XMLerror (XERL) (status,source_address,source_service,info).*/
  public XERL (String stat, String addr, String serv, String inf)throws XERLException{
    // call to the simpliest constructor XERL();
    this();
    setStatus(stat);
    setAddress(addr);
    setService(serv);
    setInfo(inf);
    //Status = stat; Address = addr; Service = serv; Info = inf;
  }

  /** Constructor. Crea un objeto de tipo XERL asigna los valores addr y serv a los campos source_address y source_service del XMLerror (XERL).*/
  public XERL (String addr, String serv)throws XERLException{
    // call to the simpliest constructor XERL();
    this();
    setAddress(addr);
    setService(serv);
    //Address = addr; Service = serv; 
  }

  /**
   * Saves XERL object to file.
   */
  public boolean save() throws IOException{
    boolean ok=true;
    System.out.println("path: "+this.path);
    if (this.path.equals("")){
      ok=false;
    }else{
      ok=this.save(this.path);
    }
    return ok;
  }
   
  /**
   * Saves XERL object to file named "path"
   */
  public boolean save(String path) throws IOException{
    boolean ok=true;
    if (path==null){
      return false;
    }
    try{
      TransformerFactory tFactory = TransformerFactory.newInstance();
      Transformer transformer = tFactory.newTransformer();
      DOMSource source = new DOMSource(this.doc);
      PrintWriter out=new PrintWriter(new FileOutputStream(path));
      StreamResult result = new StreamResult(out);
      
      Properties prop=new Properties();
      prop.setProperty("method","xml");
      prop.setProperty("indent","yes");     
      transformer.setOutputProperties(prop);

      transformer.transform(source, result);
      //System.out.println("Antes de cerrar");
      out.close();
      //System.out.println("Despues de cerrar");
      //XEDLTools.indentXEDL(path);
      }catch (TransformerConfigurationException tce) {
        ok=false;
      }catch (TransformerException te) {
        ok=false;
    }
    return ok;
  }


   /**
    * Returns XERL Status as an String
    */
   public String getStatus(){
    Node n=this.doc.getElementsByTagName("Status").item(0);
    if (n==null) {return null;}
    else { return n.getFirstChild().getNodeValue();}
   }

   /**
    * Returns XERL Source_address as an String
    */
   public String getAddress(){
    Node n=this.doc.getElementsByTagName("Source_address").item(0);
    if (n==null) {return null;}
    else { return n.getFirstChild().getNodeValue();}
   }

   /**
    * Returns XERL Source_service as an String
    */
   public String getService(){
    Node n=this.doc.getElementsByTagName("Source_service").item(0);
    if (n==null) {return null;}
    else { return n.getFirstChild().getNodeValue();}
   }

   /**
    * Returns XERL Info as an String
    */
   public String getInfo(){
    Node n=this.doc.getElementsByTagName("Info").item(0);
    if (n==null) {return null;}
    else { return n.getFirstChild().getNodeValue();}
   }


   /**
    * Returns XERL Status as an String
    */
   public boolean setStatus(String name){
    boolean ok=false;
    boolean replace=false;
    try{
      if (name==null){
        return false;
      }
      //Search for node
      //If found then replace
      if (this.getStatus()!=null){
        replace=true;
      }
      if (replace) {
        // Replace
        //System.out.println("Replacing..");
        Node node=this.doc.getElementsByTagName("Status").item(0);
        node.getFirstChild().setNodeValue(name);
      } else {
        // Create in correct possition
        //System.out.println("Creating..");
        Element statusNode = this.doc.createElement("Status");
        statusNode.appendChild(this.doc.createTextNode(name));
        // Append this element to root element
        if (this.getAddress()!=null){
          rootElement.insertBefore(statusNode,this.doc.getElementsByTagName("Source_address").item(0));
        } else {
          if (this.getService()!=null){
            rootElement.insertBefore(statusNode,this.doc.getElementsByTagName("Source_service").item(0));
          } else {
            if (this.getInfo()!=null){
              rootElement.insertBefore(statusNode,this.doc.getElementsByTagName("Info").item(0));
            } else {
              rootElement.appendChild(statusNode);
            }
          }
        }
      }
    }catch(Exception e){
        System.out.println("Exception in setStatus " +e);
        ok=false;
    }
    return ok;    
  }

   /**
    * Returns XERL Source_address as an String
    */
   public boolean setAddress(String name){
    boolean ok=false;
    boolean replace=false;
    try{
      if (name==null){
        return false;
      }
      //Search for node
      //If found then replace
      if (this.getAddress()!=null){
        replace=true;
      }
      if (replace) {
        // Replace
        //System.out.println("Replacing..");
        Node node=this.doc.getElementsByTagName("Source_address").item(0);
        node.getFirstChild().setNodeValue(name);
      } else {
        // Create
        //System.out.println("Creating..");
        Element sourceAddressNode = this.doc.createElement("Source_address");
        sourceAddressNode.appendChild(this.doc.createTextNode(name));
        // Append this element to root element
        if (this.getService()!=null){
          rootElement.insertBefore(sourceAddressNode,this.doc.getElementsByTagName("Source_service").item(0));
        } else {
          if (this.getInfo()!=null){
            rootElement.insertBefore(sourceAddressNode,this.doc.getElementsByTagName("Info").item(0));
          } else {
            rootElement.appendChild(sourceAddressNode);
          }
        }
      }
    }catch(Exception e){
        System.out.println("Excepcion en setSourceAddress " +e);
        ok=false;
    }
    return ok;    
  }

   /**
    * Returns XERL SourceService as an String
    */
   public boolean setService(String name){
    boolean ok=false;
    boolean replace=false;
    try{
      if (name==null){
        return false;
      }
      //Search for node
      //If found then replace
      if (this.getService()!=null){
        replace=true;
      }
      if (replace) {
        // Replace
        //System.out.println("Replacing..");
        Node node=this.doc.getElementsByTagName("Source_service").item(0);
        node.getFirstChild().setNodeValue(name);
      } else {
        // Create
        //System.out.println("Creating..");
        Element sourceServiceNode = this.doc.createElement("Source_service");
        sourceServiceNode.appendChild(this.doc.createTextNode(name));
        // Append this element to root element
        if (this.getInfo()!=null){
          rootElement.insertBefore(sourceServiceNode,this.doc.getElementsByTagName("Info").item(0));
        } else {
          rootElement.appendChild(sourceServiceNode);
        }
      }
    }catch(Exception e){
        System.out.println("Excepcion en setSourceService " +e);
        ok=false;
    }
    return ok;    
  }

   /**
    * Returns XERL Info as an String
    */
   public boolean setInfo(String name){
    boolean ok=false;
    boolean replace=false;
    try{
      if (name==null){
        return false;
      }
      //Search for node
      //If found then replace
      if (this.getInfo()!=null){
        replace=true;
      }
      if (replace) {
        // Replace
        //System.out.println("Replacing..");
        Node node=this.doc.getElementsByTagName("Info").item(0);
        node.getFirstChild().setNodeValue(name);
      } else {
        // Create in the correct possition
        //System.out.println("Creating..");
        Element infoNode = this.doc.createElement("Info");
        infoNode.appendChild(this.doc.createTextNode(name));
        // Append this element to root element
        rootElement.appendChild(infoNode);
      }
    }catch(Exception e){
        System.out.println("Excepcion en setInfo " +e);
        ok=false;
    }
    return ok;    
  }

  /** Escribe en el fichero, cuyo nombre se pasa como parametro, el XERL. Debuelve true si todo fue bien y false si no pudo escribir.*/
  public boolean writeError (String filename) throws IOException{
    return save(filename);
  }

  /** Escribe el XERL.por el printStream dado*/
  public void writeError (PrintStream printStream) {
    printStream.println(" Error from server " + this.getAddress() + ", service " + this.getService() + ": " + this.getInfo());
  }

  /** Escribe el XERL.por la salida estandar*/
  public void writeError () throws ParserConfigurationException,TransformerException,TransformerConfigurationException{
    // Use a Transformer for output
    TransformerFactory tFactory = TransformerFactory.newInstance();
    Transformer transformer = tFactory.newTransformer();

    DOMSource source = new DOMSource(doc);
    StreamResult result = new StreamResult(System.out);
    transformer.transform(source, result);  
  }

  /** Escribe el XERL.en un String*/
  public String toString () {
    try {
      // Use a Transformer for output
      TransformerFactory tFactory = TransformerFactory.newInstance();
      Transformer transformer = tFactory.newTransformer();
  
      ByteArrayOutputStream stringStream = new ByteArrayOutputStream();
      DOMSource source = new DOMSource(doc);
      StreamResult result = new StreamResult(stringStream);
      transformer.transform(source, result);
      return stringStream.toString();
    } catch (Exception e) {
      return "";
    }
  }
  
/*  public boolean writeError (String filename) {
  public boolean writeError (String filename) {
    //Comprobamos que el fichero existe. Si no existe, lo creamos. Si existe, lo borramos.
    try {
      File fichero_out = new File(filename);
      if (fichero_out.exists()) {
        fichero_out.delete();
      } 
      fichero_out.createNewFile();
      FileOutputStream fileOut = new FileOutputStream(filename);
      Document ErrorfileDoc;
      DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
    
      DocumentBuilder builder = factory.newDocumentBuilder();
      ErrorfileDoc = builder.newDocument();
      // Creamos el root del XML
      Element error = (Element) ErrorfileDoc.createElement("error");
    
      //Creamos los elementos que contiene el root y les damos valores
      Element statusNode = ErrorfileDoc.createElement("Status");
      statusNode.appendChild(ErrorfileDoc.createTextNode(Status));
      Element addressNode = ErrorfileDoc.createElement("Source_address");
      addressNode.appendChild(ErrorfileDoc.createTextNode(Address));
      Element serviceNode = ErrorfileDoc.createElement("Source_service");
      serviceNode.appendChild(ErrorfileDoc.createTextNode(Service));
      Element infoNode = ErrorfileDoc.createElement("Info");
      infoNode.appendChild(ErrorfileDoc.createTextNode(Info));
    
      // Creamos la jerarquia, es decir, metemos status, service, address e info dentro de error:
      error.appendChild(statusNode);
      error.appendChild(addressNode);
      error.appendChild(serviceNode);
      error.appendChild(infoNode);
    
      // lo metemos todo dentro del Document
      ErrorfileDoc.appendChild(error);
        
      // Normalizamos el documento
      ErrorfileDoc.getDocumentElement().normalize();
    
      // Use a Transformer for output
      TransformerFactory tFactory =
      TransformerFactory.newInstance();
      Transformer transformer = tFactory.newTransformer();

      DOMSource source = new DOMSource(ErrorfileDoc);
      StreamResult result = new StreamResult(fileOut);
      transformer.transform(source, result);  
    } catch (FileNotFoundException e) {System.out.println("FILENOT FOUND Parametro: " + filename); return false; 
    } catch (IOException e) { System.out.println("IO EXCEP Parametro: " + filename);return false;
    } catch (ParserConfigurationException e) {return false;
    } catch (TransformerConfigurationException e) {return false;
    } catch (TransformerException e) {return false;
    }
    return true;
  }
*/

  /** Escribe el XERL.por la salida estandar*/
/*  public void writeError () throws ParserConfigurationException,TransformerException,TransformerConfigurationException{
    //Comprobamos que el fichero existe. Si no existe, lo creamos. Si existe, lo borramos.
    Document ErrorfileDoc;
    DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
    
    DocumentBuilder builder = factory.newDocumentBuilder();
    ErrorfileDoc = builder.newDocument();
    // Creamos el root del XML
    Element error = (Element) ErrorfileDoc.createElement("error");
    
    //Creamos los elementos que contiene el root y les damos valores
    Element statusNode = ErrorfileDoc.createElement("Status");
    statusNode.appendChild(ErrorfileDoc.createTextNode(Status));
    Element addressNode = ErrorfileDoc.createElement("Source_address");
    addressNode.appendChild(ErrorfileDoc.createTextNode(Address));
    Element serviceNode = ErrorfileDoc.createElement("Source_service");
    serviceNode.appendChild(ErrorfileDoc.createTextNode(Service));
    Element infoNode = ErrorfileDoc.createElement("Info");
    infoNode.appendChild(ErrorfileDoc.createTextNode(Info));
    
    // Creamos la jerarquia, es decir, metemos status, service, address e info dentro de error:
    error.appendChild(statusNode);
    error.appendChild(addressNode);
    error.appendChild(serviceNode);
    error.appendChild(infoNode);
    
    // lo metemos todo dentro del Document
    ErrorfileDoc.appendChild(error);
        
    // Normalizamos el documento
    ErrorfileDoc.getDocumentElement().normalize();
    
    // Use a Transformer for output
    TransformerFactory tFactory =
    TransformerFactory.newInstance();
    Transformer transformer = tFactory.newTransformer();

    DOMSource source = new DOMSource(ErrorfileDoc);
    StreamResult result = new StreamResult(System.out);
    transformer.transform(source, result);  
  }
  */

  public static void main(String args[]) throws Exception{
    XERL xerl = new XERL();
    xerl.setAddress("addresseses");
    xerl.setService("serviceseses");
    xerl.setStatus("statusossosos");
    xerl.setInfo("infolocos");
    xerl.save("prueba.xerl");
    
    XERL xerl2 = new XERL ("prueba.xerl");
    System.out.println("Status is: " +xerl2.getStatus());
    System.out.println("Service is: " +xerl2.getService());
    System.out.println("Address is: " +xerl2.getAddress());
    System.out.println("Info is: " +xerl2.getInfo());
    xerl2.setStatus("statusossosos2222");
    xerl2.setAddress("addresseses2222");
    xerl2.setService("serviceseses2222");
    xerl2.setInfo("infolocos2222");
    xerl2.save("prueba.xerl");

    XERL xerl3 = new XERL();
    xerl3.setInfo("infolocos");
    xerl3.setService("serviceseses");
    xerl3.setStatus("statusossosos");
    xerl3.setAddress("addresseses");
    xerl3.save("prueba2.xerl");
  }

  /** Establece un valor para el elemento Status del XERL.*/
//  public void setStatus (String stat){
//    Status = stat;  
//  }

  /** Establece un valor para el elemento Source_address del XERL.*/
//  public void setAddress (String addr){
//    Address = addr;
//  }

  /** Establece un valor para el elemento Source_service del XERL.*/
//  public void setService (String serv){
//    Service = serv;
//  }

  /** Establece un valor para el elemento Info del XERL.*/
//  public void setInfo (String inf){
//    Info = inf;
//  }

  /** Devuelve el valor del elemento Status del XERL.*/
//  public String getStatus (){
//    return Status;
//  }

  /** Devuelve el valor del elemento Source_address del XERL.*/
//  public String getAddress (){
//    return Address;
//  }

  /** Devuelve el valor del elemento Source_service del XERL.*/
//  public String getService (){
//    return Service;
//  }

  /** Devuelve el valor del elemento Info del XERL.*/
//  public String getInfo (){
//    return Info;
//  }

}