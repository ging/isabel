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

//Source file: C:\\Documents and Settings\\acosta\\Mis documentos\\investigacion\\docpropios\\xedlAIP\\XEDL.java


import java.io.*;
import java.util.*;


import org.w3c.dom.*;

/**
	Contains generic methods for DOM node objects
*/

public class XEDLTools{

	/**
		Returns first-level child of Node "node" named "name".Returns null if not found
	*/	
	public static Node getChildByName(Node node, String name){
		Node result=null;
		//If node has childs:
		if (node!=null && node.hasChildNodes()){
			NodeList list=node.getChildNodes();
			int i=0;
			//While not end of NodeList:
			while ((i<node.getChildNodes().getLength())){
				//If node name is ok, end of loop
				if (node.getChildNodes().item(i).getNodeName().equals(name)){
					result = node.getChildNodes().item(i);
					break ;
				}
					//else: next element
				else{
					i++;
				}
			}	
		}
		return result;
	}
	
	/**
		* Introduces indentation on new XEDL files created with XEDL class.
		* Warning!!!use only in NEW files,NOT MODIFIED ones.Do NOT use twice on the same file.
	*/
	
	public static void indentXEDL(String path) throws IOException{
		XEDLTools.unIndentXEDL(path);//deja la salida en lmnopqrstuv
		FileReader fileIN = new FileReader(path);
		LineNumberReader input = new LineNumberReader(fileIN);
                File in=new File(path);
                File parentFile = in.getParentFile();
                String workDir = "";
                if (parentFile != null){
                    workDir=parentFile.getAbsolutePath()+File.separator;
                }
		FileWriter fileOUT = new FileWriter(workDir+"abcdefghijk");
		PrintWriter output = new PrintWriter(fileOUT,true);
		XEDLTools.indent(input,output,"");
		input.close();
		output.close();
		//falta cambiar el nombre del fichero temporal		
		in.delete();//deleting old file
		File out=new File(workDir+"abcdefghijk");
		out.renameTo(in);
		
	}
	
	public static void unIndentXEDL(String path) throws IOException{
		FileReader fileIN = new FileReader(path);
		LineNumberReader input = new LineNumberReader(fileIN);
                File in=new File(path);
                File parentFile = in.getParentFile();
                String workDir = "";
                if (parentFile != null){
                    workDir=parentFile.getAbsolutePath()+File.separator;
                }
		FileWriter fileOUT = new FileWriter(workDir+"lmnopqrstuv");
		PrintWriter output = new PrintWriter(fileOUT,true);
		XEDLTools.unIndent(input,output);
		input.close();
		output.close();
		//falta cambiar el nombre del fichero temporal		
		in.delete();//deleting old file
		File out=new File(workDir+"lmnopqrstuv");
		out.renameTo(in);
	}
	
	private static void unIndent (LineNumberReader input, PrintWriter output)throws IOException{
		String dataAux = "";
		String data=input.readLine();
		int index=0;
		while (data != null){
			index=data.indexOf("<");
			if(index>=0){
				//System.out.println(index);
				dataAux=data.substring(index);
				//System.out.println(dataAux);
				output.println(dataAux);
				output.flush();
			}else{
				if (!data.trim().equals("")){
					//System.out.println(dataAux.trim());
					output.println(dataAux.trim());
					output.flush();
				}
			}
			data=input.readLine();
		}
	}
	
	private static void indent (LineNumberReader input, PrintWriter output, String i)throws IOException{
		String indent = i;
		String indentIncrement = "   ";
		String data=" ";
		data=input.readLine();
		if (data!=null){
			data=data.trim();
		}
		while (data != null){
		
			if(data.indexOf("</") == 0){
				if (indent.length()>indentIncrement.length()){
					indent=indent.substring(0,indent.length()-indentIncrement.length());
				}else{
					indent="";
				}
				//System.out.print("*"+indent+"*");
				output.print(indent);
				output.flush();
				//System.out.println(data);
				output.println(data);				
			}else{
				if ((data.indexOf("/>") > 0) || (data.indexOf("?") > 0) || (data.indexOf("</") > 0) || (data.indexOf("-->") > 0)){
					//System.out.print("*"+indent+"*");
					output.print(indent);
					output.flush();
					//System.out.println(data);
					output.println(data);
				}else{
					//System.out.print("*"+indent+"*");
					output.print(indent);
					output.flush();
					//System.out.println(data);
					output.println(data);				
					XEDLTools.indent(input,output,indent+indentIncrement);
				}
			}
			data=input.readLine();
			if (data!=null){
				data=data.trim();
			}
		}
	}
	
	/**
	*	Concats 2 LinkedLists
	**/
	public static LinkedList concat(LinkedList listA,LinkedList listB){
		LinkedList auxList = new LinkedList();
		int i=0;
		while (i<listA.size()){
			auxList.addLast(listA.get(i));
		   i++;
		}//while
		i=0;
		while (i<listB.size()){
			auxList.addLast(listB.get(i));
		   i++;
		}//while
		return auxList;
	}
	
	/**
	*	Flips a LinkedList
	**/
	public static LinkedList flip(LinkedList list){
		LinkedList auxList = new LinkedList();
		int i=0;
		while (i<list.size()){
			auxList.add(list.get(list.size()-i-1));
		   i++;
		}//while		
		return auxList;
	}
	
	/*private static void indent (LineNumberReader input, PrintWriter output, String i)throws IOException{
		String indent = i;
		String indentIncrement = "   ";
		String data=" ";
		data=input.readLine();
		while (data != null){
		
			if(data.indexOf("</") == 0){
				if (indent.length()>indentIncrement.length()){
					indent=indent.substring(0,indent.length()-indentIncrement.length());
				}else{
					indent="";
				}
				//System.out.print(indent);
				output.print(indent);
				output.flush();
				//System.out.println(data);
				output.println(data);
			}else{
				if ((data.indexOf("/>") > 0) || (data.indexOf("?") > 0) || (data.indexOf("</") > 0) || (data.indexOf("-->") > 0)){
					//System.out.print(indent);
					output.print(indent);
					output.flush();
					//System.out.println(data);
					output.println(data);
				}else{
					//System.out.print(indent);
					output.print(indent);
					output.flush();
					//System.out.println(data);
					output.println(data);				
					XEDLTools.indent(input,output,indent+indentIncrement);
				}
			}
			//data=input.readLine();
		}
		
		
	}*/
	
	
	
}