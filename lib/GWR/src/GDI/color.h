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

#ifndef __color_h__
#define __color_h__

#include "general.h"
#include <GDIplus.h>
#include <icf2/general.h>

//Clase para manejo de colores
class color_t
{
private:
    static inline unsigned long get_pixel(unsigned char * in,float x,float y,int w,int h,int in_bits,int out_bits);
    
    //Metodos de interpolacion de colores
    static inline unsigned long get_color_method0(unsigned char * in,float x,float y,int a,int b,int c,int d,int bits);
    static inline unsigned long get_color_method1(unsigned char * in,float x,float y,int a,int b,int c,int d,int bits);
    static inline unsigned long get_color_method2(unsigned char * in,float x,float y,int a,int b,int c,int d,int bits);
    static inline unsigned long get_color_method3(unsigned char * in,float x,float y,int a,int b,int c,int d,int bits);
    static inline unsigned long get_color_method4(unsigned char * in,float x,float y,int a,int b,int c,int d,int bits);

public:
    u8 a;
    u8 r;
    u8 g;
    u8 b;
    color_t():a(0),r(0),g(0),b(0){};
    color_t(u8 red,u8 green,u8 blue):a(255),r(red),g(green),b(blue){};
    color_t(u8 alpha,u8 red,u8 green,u8 blue):a(alpha),r(red),g(green),b(blue){};    
    COLORREF GetColorRef(const color_t color){ return RGB(color.r,color.g,color.b); };
    HBRUSH   GetBrush(const color_t color){ return CreateSolidBrush(RGB(color.r,color.g,color.b));};
    
    static bool GetColorByName(const char * name,color_t * color);
    static void Convert24To32(unsigned char * in,unsigned char ** out,unsigned int &w,unsigned int &h,float zoomx,float zoomy);
    static void Convert24To16(unsigned char * in,unsigned char ** out,unsigned int &w,unsigned int &h,float zoomx,float zoomy);    
};

//Clase para correcion Gamma
class gamma_t
{
    bool  active;
    unsigned char  gamma_r[256];
    unsigned char  gamma_g[256];
    unsigned char  gamma_b[256];
    
public:
    
    gamma_t(void);
    void SetGamma(float gr,float gg2, float gb);
    void GammaTransform(unsigned char * in,unsigned char * out,int len);    
    inline bool IsActive(void){return active;}
};

#endif

