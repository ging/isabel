/*
 * Copyright (C) 2005 Luca Veltri - University of Parma - Italy
 * 
 * This file is part of MjSip (http://www.mjsip.org)
 * 
 * MjSip is free software; you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * MjSip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with MjSip; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * Author(s):
 * Luca Veltri (luca.veltri@unipr.it)
 */

package org.zoolu.sip.call;


import org.zoolu.sdp.*;
import org.zoolu.tools.Parser;
import java.util.Enumeration;
import java.util.Vector;


/** Class SdpTools collects some static methods for managing SDP materials.
  */
public class SdpTools
{
   /** Costructs a new SessionDescriptor from a given SessionDescriptor
     * with olny media types and attribute values specified by a MediaDescriptor Vector.
     * <p> If no attribute is specified for a particular media, all present attributes are kept.
     * <br>If no attribute is present for a selected media, the media is kept (regardless any sepcified attributes).
     * @param sdp the given SessionDescriptor
     * @param m_descs Vector of MediaDescriptor with the selecting media types and attributes
     * @return this SessionDescriptor */
   public static SessionDescriptor sdpMediaProduct(SessionDescriptor sdp, Vector m_descs)
   {  Vector new_media=new Vector();
      if (m_descs!=null)
      {  for (Enumeration e=m_descs.elements(); e.hasMoreElements(); )
         {  
    	    MediaField media_field;
    	    MediaDescriptor spec_md=(MediaDescriptor)e.nextElement();
            System.out.print("DEBUG: SDP: sdp_select: "+spec_md.toString());
            MediaDescriptor prev_md=sdp.getMediaDescriptor(spec_md.getMedia().getMedia());
            System.out.print("DEBUG: SDP: sdp_origin: "+prev_md.toString());
            if (prev_md!=null)
            { 
               Vector<String> valid_formats = new Vector<String>();
               Vector spec_attributes=spec_md.getAttributes();
	           Vector prev_attributes=prev_md.getAttributes();
	           Vector new_attributes=new Vector();
               for (Enumeration i=spec_attributes.elements(); i.hasMoreElements(); )
               {  
        	     AttributeField spec_attr=(AttributeField)i.nextElement();
                 String spec_name=spec_attr.getAttributeName();
                 String spec_value=spec_attr.getAttributeValue();
                 for (Enumeration k=prev_attributes.elements(); k.hasMoreElements(); )
                 {  AttributeField prev_attr=(AttributeField)k.nextElement();
                    String prev_name=prev_attr.getAttributeName();
                    String prev_value=prev_attr.getAttributeValue();
                    if (prev_value != null && spec_value != null)
                    {
                        Parser localAttr = new Parser(prev_value);
                        Parser remoteAttr = new Parser(spec_value);
                        if (spec_name.equals("rtpmap")){
                        	char[] separator = new char[1]; 
                        	separator[0] = ' ';
                        	String local_codec = localAttr.getWord(separator);
                        	String remote_codec = remoteAttr.getWord(separator);
                        	String local_name = localAttr.getWord(separator);
                        	String remote_name = remoteAttr.getWord(separator);
                        	if (local_name.equals(remote_name)) //mismo nombre de codec
                        	{
                        	    new_attributes.addElement(prev_attr);
                        	    break;
                        	}
                        }else if (spec_name.equals("fmtp")){ //el formato lo copiamos del que nos pasan
	                        if (prev_name.equals(spec_name) && prev_value.equalsIgnoreCase(spec_value))
	                        {  new_attributes.addElement(prev_attr);
	                           break;
	                        }
                        }
                    }
                 }
              }
              // Primero copiamos los formatos conocidos que nosotros aceptamos
              //Segun  RFC 3551:
              // 0 - PCMU
              // 3 - GSM
              // 8 - PCMA
              // 34 - H263
              Vector<String> proposed_formats = prev_md.getMedia().getFormatList();              
              Vector<String> available_formats = spec_md.getMedia().getFormatList();
              for (String format : proposed_formats)
			  {
				    if (available_formats.contains(format))
					{
						//Si es conocido no esperamos a que coincida el atributo "rtpmap"
				    	valid_formats.add(format);
					}
			   }
               
               if (valid_formats.size() > 0)
               {
            	   MediaField new_md = new MediaField(prev_md.getMedia().getMedia(),prev_md.getMedia().getPort(),0,prev_md.getMedia().getTransport(),valid_formats);
            	   new_media.addElement(new MediaDescriptor(new_md,prev_md.getConnection(),new_attributes));	                          
               }
            }
         }
      }
      SessionDescriptor new_sdp=new SessionDescriptor(sdp);
      new_sdp.removeMediaDescriptors();
      new_sdp.addMediaDescriptors(new_media);
      return new_sdp;
   }
   
   /** Costructs a new SessionDescriptor from a given SessionDescriptor
     * with olny the first specified media attribute.
   /** Keeps only the fisrt attribute of the specified type for each media.
     * <p> If no attribute is present for a media, the media is dropped.
     * @param sdp the given SessionDescriptor
     * @param a_name the attribute name
     * @return this SessionDescriptor */
   public static SessionDescriptor sdpAttirbuteSelection(SessionDescriptor sdp, String a_name)
   {  Vector new_media=new Vector();
      for (Enumeration e=sdp.getMediaDescriptors().elements(); e.hasMoreElements(); )
      {  MediaDescriptor md=(MediaDescriptor)e.nextElement();
         AttributeField attr=md.getAttribute(a_name);
         if (attr!=null)
         { 
        	 new_media.addElement(new MediaDescriptor(md.getMedia(),md.getConnection(),attr));
         }
      }
      SessionDescriptor new_sdp=new SessionDescriptor(sdp);
      new_sdp.removeMediaDescriptors();
      new_sdp.addMediaDescriptors(new_media);
      return new_sdp;
   }

}
