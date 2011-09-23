/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
// Copyright (C) 2009 Agora System S.A.
// 
// This file is part of Isabel.
// 
// Isabel is free software: you can redistribute it and/or modify
// it under the terms of the Affero GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Isabel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details.
// 
// You should have received a copy of the Affero GNU General Public License
// along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////

#include "XVumeter.h"
#include "noise.xbm"
#include "silence.xbm"

XVumeter::XVumeter(Display *dpy, Window w, int vuID) {
  this -> vuID = vuID;
  XWindowAttributes	winAttr;
  int	parentWidth= 0;
  int	parentHeight= 0;
  //  int	width= 0;
  //  int	height= 0;
  int	depth= CopyFromParent;
  int	visual= CopyFromParent;
  int winAttrMask= 0;
  XSetWindowAttributes	setWinAttr;
  
  Colormap	colormap;
  XColor	greenColor, greenDB;
  XColor	yellowColor, yellowDB;
  XColor	redColor, redDB;
  XColor	greyColor, greyDB;
  
  this -> display = dpy;
  this -> parent = w;

  
 
  XSetErrorHandler((int (*)(Display *, XErrorEvent *))XVumeter::Handler);

  if(!XGetWindowAttributes(
			   display,
			   parent,
			   &winAttr
			   )) {
    perror("XVumeter::XVumeter : XGetWindowAttributes");
    exit(1);
  }
  parentWidth= winAttr.width;
  parentHeight= winAttr.height;
  
  XSetErrorHandler((int (*)(Display *, XErrorEvent *))XVumeter::Handler);
  
  width= parentWidth;
  height= parentHeight;
  /*  
  winAttrMask= CWBackPixmap | CWWinGravity;
  setWinAttr.background_pixmap= ParentRelative;
  setWinAttr.win_gravity=SouthGravity;
  frame= XCreateWindow(
		       display,
		       parent,
		       0, parentHeight-height,
		       width, height,
		       0,
		       depth,
		       InputOutput,
		       (Visual *)visual,
		       winAttrMask,
		       &setWinAttr
		       );
  
  {
    XWindowAttributes xwa;
    
    XGetWindowAttributes(
			 display,
			 frame,
			 &xwa
			 );
    
    depth= xwa.depth;
  }
*/
  {
    XWindowAttributes xwa;
    
    XGetWindowAttributes(
			 display,
			 parent,
			 &xwa
			 );
    
    depth= xwa.depth;
  }
  colormap= XDefaultColormap(display, 0);
  black= XBlackPixel(display, 0);
  white= XWhitePixel(display, 0);
  XAllocNamedColor(
		   display,
		   colormap,
		   "darkgrey",
		   &greyColor,
		   &greyDB
		   );
  grey= greyColor.pixel;
  
  XAllocNamedColor(
		   display,
		   colormap,
		   "green",
		   &greenColor,
		   &greenDB
		   );
  green= greenColor.pixel;
  
  XAllocNamedColor(
		   display,
		   colormap,
		   "yellow",
		   &yellowColor,
		   &yellowDB
		   );
  yellow= yellowColor.pixel;
  
  XAllocNamedColor(
		   display,
		   colormap,
		   "red",
		   &redColor,
		   &redDB
		   );
  red= redColor.pixel;
  
  /*  
  black_noise= XCreatePixmapFromBitmapData(
					      display,
					      frame,
					      (char *)noise_bits,
					      noise_width,
					      noise_height,
					      black,
					      white,
					      depth
					      );
  
  green_noise= XCreatePixmapFromBitmapData(
					      display,
					      frame,
					      (char *)noise_bits,
					      noise_width,
					      noise_height,
					      green,
					      white,
					      depth
					      );
  
  yellow_noise= XCreatePixmapFromBitmapData(
					       display,
					       frame,
					       (char *)noise_bits,
					       noise_width,
					       noise_height,
					       yellow,
					       white,
					       depth
					       );
  
  red_noise= XCreatePixmapFromBitmapData(
					    display,
					    frame,
					    (char *)noise_bits,
					    noise_width,
					    noise_height,
					    red,
					    white,
					    depth
					    );
  
  
  
  if(width> noise_width)
    noise_pos= (width-noise_width)/2;
  else
    noise_pos= 0;
  */
  winAttrMask= CWBackPixmap | CWWinGravity;
  setWinAttr.background_pixmap= None;
  setWinAttr.win_gravity=SouthGravity;
  /*
  for(i= 0; i< NUM_LEDS; i++) {
    leds[i]= XCreateWindow(
			   display,
			   frame,
			   noise_pos, height-(i+1)*noise_height,
			   noise_width, noise_height,
			   0,
			   depth,
			   InputOutput,
			   (Visual *)visual,
			   winAttrMask,
			   &setWinAttr
			   );
  }
  XMapWindow(display, frame);
  for(i= 0; i< NUM_LEDS; i++)
    XMapWindow(display, leds[i]);
*/
  myframe = XCreateWindow(display, parent, 0, 0, width, height, 0, depth, InputOutput, (Visual *)visual, winAttrMask, &setWinAttr);
  XMapWindow(display, myframe);
  gc = XCreateGC(display, myframe, 0, NULL);

  power= 0;
  maxPower= 0;
  maxPowerLife= 0;
  
 XSync(display, False);
}

XVumeter::~XVumeter() {
  XDestroySubwindows(display, frame);
  XDestroyWindow(display, frame);
  
  XSync(display, False);
}

void XVumeter::SetPower(int power) {
  int h, i;

  power = (int)((power/100.0)*(power/100.0)*100.0);
  h = power*height/100;
  if (h < 0) h = 0;

  int step = ((height)/10);
  h = power/10;

  XSetForeground(display, gc, grey);
  XFillRectangle(display, myframe, gc, 0, 0, width, height);

   // Esto es para diodo
  if ( (width <= 32) && (height <= 32 ) ) {
    XSetForeground(display, gc, black);
    XFillRectangle(display, myframe, gc, 0, 0, width, height);
    XSetForeground(display, gc, power> 80?red:power>60?yellow:power>0?green:black);
    XFillRectangle(display, myframe, gc, (width/2)-(((width-2)*power/100)/2), (height/2)-(((height-2)*power/100)/2), (width-2)*power/100, (height-2)*power/100);
    XFlush(display);
    this -> power = power;
    return;
  } else {
    for ( i = 0 ; i < 10 ; i++ ) {
      
      if ( i >= h) {
	XSetForeground(display, gc, grey);
	XFillRectangle(display, myframe, gc, 0, height-((i+1)*step-1), width, step);
	
	XSetForeground(display, gc, black);
	XFillRectangle(display, myframe, gc, 2, height-((i+1)*step-1), width-4, step-2);
      } else {
	XSetForeground(display, gc, grey);
	XFillRectangle(display, myframe, gc, 0, height-((i+1)*step-1), width, step);
	
	XSetForeground(display, gc, i > 7 ?red: i > 4?yellow: i >= 0 ?green:black);
	XFillRectangle(display, myframe, gc, 2, height-((i+1)*step-1), width-4, step-2);
      }
    }
    XFlush(display);
    this -> power = power;
    return;
  }  /*
       for ( i = 0 ; i <= height ; i++ ) {
       int percent;
       if ( i > h ) {
       if (i%2) {
       XSetForeground(display, gc, grey);
       XDrawLine(display, myframe, gc, 0, height-i, width, height-i);
       } else {
	XSetForeground(display, gc, black);
	XDrawLine(display, myframe, gc, 0, height-i, width, height-i);
      } 
      //      XSetForeground(display, gc, grey);
      //      XDrawLine(display, myframe, gc, 0, height-i, width, height-i);
    } else {
      if (i%2) {
	percent = i*100/height;
	XSetForeground(display, gc, percent > 80?red: percent > 60?yellow: percent > 0 ?green:black);
	XDrawLine(display, myframe, gc, 0, height-i, width, height-i);
      } else {
	XSetForeground(display, gc, black);
	XDrawLine(display, myframe, gc, 0, height-i, width, height-i);
      }
    }
  }
  XFlush(display);
  this -> power = power;
  return;
  */
  // Esto es para diodo
  XSetForeground(display, gc, black);
  XFillRectangle(display, myframe, gc, 0, 0, width, height);
  XSetForeground(display, gc, power> 80?red:power>60?yellow:power>0?green:black);
  XFillRectangle(display, myframe, gc, (width/2)-(((width-2)*power/100)/2), (height/2)-(((height-2)*power/100)/2), (width-2)*power/100, (height-2)*power/100);
  XFlush(display);
  this -> power = power;
  return;

  h = power*NUM_LEDS/100;

  for ( i = 0 ; i < NUM_LEDS ; i++ ) {
    if ( i < h ) {
      XSetWindowBackgroundPixmap(
				 display,
				 leds[i],
				 i> RED_LED?red_noise:i>YELLOW_LED?yellow_noise:green_noise
				 );
      XClearWindow(display, leds[i]);
    } else {
    XSetWindowBackgroundPixmap(
			       display,
			       leds[i],
			       black_noise
			       );
    XClearWindow(display, leds[i]);
    }
  }
  if ( power > maxPower ) {
    maxPower = power;
    maxPowerLife = 10;
  } else {
    maxPowerLife--;
    if (maxPowerLife <= 0) {
      maxPowerLife = 10;
      maxPower = 0;
      if (maxPower <= 0 ) maxPower = 0;
    }
  }

  h = (maxPower*NUM_LEDS/100) - 1;
  if (h >= 0) {
    XSetWindowBackgroundPixmap(
			       display,
			       leds[h],
			       h > RED_LED?red_noise: h >YELLOW_LED?yellow_noise:green_noise
			       );
    XClearWindow(display, leds[h]);
  }
  this -> power = power;
  XSync(display, False);
  return;
}

void XVumeter::Decay() {
    SetPower((int)(power-1));
}

int XVumeter::Handler(Display *dpy, XErrorEvent *err) {
  return 0;
}

XVumeter *XVumeter::Create(Display *dpy, Window w, int vuID) {
  XWindowAttributes	winAttr;

  XSetErrorHandler((int (*)(Display *, XErrorEvent *))XVumeter::Handler);
  if(!XGetWindowAttributes(
			   dpy,
			   w,
			   &winAttr
			   )) {
    return NULL;
  }
  return new XVumeter(dpy, w, vuID);
}

int XVumeter::GetID() {
  return vuID;
}

void XVumeter::SetSize(int w, int h) {
  height = h;
  width = w;
}












