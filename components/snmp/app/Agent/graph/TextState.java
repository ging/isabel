package graph;

import java.awt.*;
import java.util.*;
import java.lang.*;
import java.awt.image.*;

/*
**************************************************************************
**
**    Class  TextState
**
**************************************************************************
**    Copyright (C) 1996 Leigh Brookshaw
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the Affero GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    Affero GNU General Public License for more details.
**
**    You should have received a copy of the Affero GNU General Public License
**    along with this program; if not, write to the Free Software
**    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**************************************************************************
**
**    This class is designed to bundle together all the information required
**    to draw short Strings
**
*************************************************************************/

/**
 * A structure class used exclusively with the TextLine class.
 * When the Text changes state (new font, new color, new offset)
 * then this class holds the information plus the substring
 * that the state pertains to.
 */
public class TextState extends Object { 
      Font f         = null;
      StringBuffer s = null;
      int x          = 0;
      int y          = 0;

      
      public TextState() {
              s = new StringBuffer();
	    }


      public TextState copyAll() {
             TextState tmp = copyState();
             if(s.length()==0) return tmp;
             for(int i=0; i<s.length(); i++) { tmp.s.append(s.charAt(i)); }
             return tmp;
	   }


      public TextState copyState() {
             TextState tmp = new TextState();
             tmp.f = f;
             tmp.x = x;
             tmp.y = y;
             return tmp;
	   }


      public String toString() {
             return s.toString();
	   }


      public boolean isEmpty() {
           return (s.length() == 0);
	 }

      public int getWidth(Graphics g) {

           if(g == null || f == null || s.length()==0 ) return 0;

           return g.getFontMetrics(f).stringWidth(s.toString());
      }

      public int getHeight(Graphics g) {
           if(g == null || f == null ) return 0;
           
           return g.getFontMetrics(f).getHeight();
	 }
      public int getAscent(Graphics g) {
           if(g == null || f == null ) return 0;
           
           return g.getFontMetrics(f).getAscent();
	 }
      public int getDescent(Graphics g) {
           if(g == null || f == null ) return 0;
           
           return g.getFontMetrics(f).getDescent();
	 }
      public int getMaxAscent(Graphics g) {
           if(g == null || f == null ) return 0;
           
           return g.getFontMetrics(f).getMaxAscent();
	 }
      public int getMaxDescent(Graphics g) {
           if(g == null || f == null ) return 0;
           
           return g.getFontMetrics(f).getMaxDescent();
	 }
      public int getLeading(Graphics g) {
           if(g == null || f == null ) return 0;
           
           return g.getFontMetrics(f).getLeading();
	 }
}

