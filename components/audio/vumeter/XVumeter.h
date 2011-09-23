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
// Basado en el trabajo de Manuel Petit.

#ifndef __VUMETER_H__
#define __VUMETER_H__



#include <X11/X.h>
#include <X11/Xlib.h>

#include <stdio.h>
#include <stdlib.h>

#define	NUM_LEDS	16
#define	RED_LED		12
#define	YELLOW_LED	8

class XVumeter {
  Display	*display;
  Window	parent;
  
  unsigned long	white;
  unsigned long	black;
  unsigned long	green;
  unsigned long	yellow;
  unsigned long	red;
  unsigned long grey;
  
  Pixmap	black_noise;
  Pixmap	green_noise;
  Pixmap	yellow_noise;
  Pixmap	red_noise;
  
  Window	frame;
  Window	leds[NUM_LEDS];
  Window myframe;
  GC gc;
  int width, height;
  
  int	power;
  
  int maxPower;
  int maxPowerLife;

  int vuID;
 public:
  XVumeter(Display *dpy, Window w, int vuID);
  virtual ~XVumeter();
  static XVumeter *Create(Display *dpy, Window w, int vuID);
  void SetPower(int power);
  void SetSize(int w, int h);
  void Decay();
  int GetID();
 private:
  static int Handler(Display *dpy, XErrorEvent *err);
};

#endif







