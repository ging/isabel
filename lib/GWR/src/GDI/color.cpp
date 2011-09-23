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

#include "color.h"
#include "math.h"

#define COLOR_24_TO_16(r,g,b) ((unsigned short)((r>>3)<<11 | (g>>2)<<5| (b>>3)))
#define COLOR_24_TO_32(r,g,b) ((unsigned long)(0xff << 24 | r << 16 | g << 8 | b))


typedef struct colorName_t
{
    char * name;
    u32    color;
} colorName_t;

colorName_t colorNames[] = {
    {    "AliceBlue" ,    0xFFF0F8FF} ,
    {    "AntiqueWhite" ,    0xFFFAEBD7} ,
    {    "Aqua" ,    0xFF00FFFF} ,
    {    "Aquamarine" ,    0xFF7FFFD4} ,
    {    "Azure" ,    0xFFF0FFFF} ,
    {    "Beige" ,    0xFFF5F5DC} ,
    {    "Bisque" ,    0xFFFFE4C4} ,
    {    "Black" ,    0xFF000000} ,
    {    "BlanchedAlmond" ,    0xFFFFEBCD} ,
    {    "Blue" ,    0xFF0000FF} ,
    {    "BlueViolet" ,    0xFF8A2BE2} ,
    {    "Brown" ,    0xFFA52A2A} ,
    {    "BurlyWood" ,    0xFFDEB887} ,
    {    "CadetBlue" ,    0xFF5F9EA0} ,
    {    "Chartreuse" ,    0xFF7FFF00} ,
    {    "Chocolate" ,    0xFFD2691E} ,
    {    "Coral" ,    0xFFFF7F50} ,
    {    "CornflowerBlue" ,    0xFF6495ED} ,
    {    "Cornsilk" ,    0xFFFFF8DC} ,
    {    "Crimson" ,    0xFFDC143C} ,
    {    "Cyan" ,    0xFF00FFFF} ,
    {    "DarkBlue" ,    0xFF00008B} ,
    {    "DarkCyan" ,    0xFF008B8B} ,
    {    "DarkGoldenrod" ,    0xFFB8860B} ,
    {    "DarkGray" ,    0xFFA9A9A9} ,
    {    "DarkGreen" ,    0xFF006400} ,
    {    "DarkKhaki" ,    0xFFBDB76B} ,
    {    "DarkMagenta" ,    0xFF8B008B} ,
    {    "DarkOliveGreen" ,    0xFF556B2F} ,
    {    "DarkOrange" ,    0xFFFF8C00} ,
    {    "DarkOrchid" ,    0xFF9932CC} ,
    {    "DarkRed" ,    0xFF8B0000} ,
    {    "DarkSalmon" ,    0xFFE9967A} ,
    {    "DarkSeaGreen" ,    0xFF8FBC8B} ,
    {    "DarkSlateBlue" ,    0xFF483D8B} ,
    {    "DarkSlateGray" ,    0xFF2F4F4F} ,
    {    "DarkTurquoise" ,    0xFF00CED1} ,
    {    "DarkViolet" ,    0xFF9400D3} ,
    {    "DeepPink" ,    0xFFFF1493} ,
    {    "DeepSkyBlue" ,    0xFF00BFFF} ,
    {    "DimGray" ,    0xFF696969} ,
    {    "DodgerBlue" ,    0xFF1E90FF} ,
    {    "Firebrick" ,    0xFFB22222} ,
    {    "FloralWhite" ,    0xFFFFFAF0} ,
    {    "ForestGreen" ,    0xFF228B22} ,
    {    "Fuchsia" ,    0xFFFF00FF} ,
    {    "Gainsboro" ,    0xFFDCDCDC} ,
    {    "GhostWhite" ,    0xFFF8F8FF} ,
    {    "Gold" ,    0xFFFFD700} ,
    {    "Goldenrod" ,    0xFFDAA520} ,
    {    "Gray" ,    0xFF808080} ,
    {    "Green" ,    0xFF008000} ,
    {    "GreenYellow" ,    0xFFADFF2F} ,
    {    "Honeydew" ,    0xFFF0FFF0} ,
    {    "HotPink" ,    0xFFFF69B4} ,
    {    "IndianRed" ,    0xFFCD5C5C} ,
    {    "Indigo" ,    0xFF4B0082} ,
    {    "Ivory" ,    0xFFFFFFF0} ,
    {    "Khaki" ,    0xFFF0E68C} ,
    {    "Lavender" ,    0xFFE6E6FA} ,
    {    "LavenderBlush" ,    0xFFFFF0F5} ,
    {    "LawnGreen" ,    0xFF7CFC00} ,
    {    "LemonChiffon" ,    0xFFFFFACD} ,
    {    "LightBlue" ,    0xFFADD8E6} ,
    {    "light blue" , 0xFFADD8E6} ,
    {    "LightCoral" ,    0xFFF08080} ,
    {    "LightCyan" ,    0xFFE0FFFF} ,
    {    "LightGoldenrodYellow" ,    0xFFFAFAD2} ,
    {    "LightGray" ,    0xFFD3D3D3} ,
    {    "LightGreen" ,    0xFF90EE90} ,
    {    "LightPink" ,    0xFFFFB6C1} ,
    {    "LightSalmon" ,    0xFFFFA07A} ,
    {    "LightSeaGreen" ,    0xFF20B2AA} ,
    {    "LightSkyBlue" ,    0xFF87CEFA} ,
    {    "LightSlateGray" ,    0xFF778899} ,
    {    "LightSteelBlue" ,    0xFFB0C4DE} ,
    {    "LightYellow" ,    0xFFFFFFE0} ,
    {    "Lime" ,    0xFF00FF00} ,
    {    "LimeGreen" ,    0xFF32CD32} ,
    {    "Linen" ,    0xFFFAF0E6} ,
    {    "Magenta" ,    0xFFFF00FF} ,
    {    "Maroon" ,    0xFF800000} ,
    {    "MediumAquamarine" ,    0xFF66CDAA} ,
    {    "MediumBlue" ,    0xFF0000CD} ,
    {    "MediumOrchid" ,    0xFFBA55D3} ,
    {    "MediumPurple" ,    0xFF9370DB} ,
    {    "MediumSeaGreen" ,    0xFF3CB371} ,
    {    "MediumSlateBlue" ,    0xFF7B68EE} ,
    {    "MediumSpringGreen" ,    0xFF00FA9A} ,
    {    "MediumTurquoise" ,    0xFF48D1CC} ,
    {    "MediumVioletRed" ,    0xFFC71585} ,
    {    "MidnightBlue" ,    0xFF191970} ,
    {    "MintCream" ,    0xFFF5FFFA} ,
    {    "MistyRose" ,    0xFFFFE4E1} ,
    {    "Moccasin" ,    0xFFFFE4B5} ,
    {    "NavajoWhite" ,    0xFFFFDEAD} ,
    {    "Navy" ,    0xFF000080} ,
    {    "OldLace" ,    0xFFFDF5E6} ,
    {    "Olive" ,    0xFF808000} ,
    {    "OliveDrab" ,    0xFF6B8E23} ,
    {    "Orange" ,    0xFFFFA500} ,
    {    "OrangeRed" ,    0xFFFF4500} ,
    {    "Orchid" ,    0xFFDA70D6} ,
    {    "PaleGoldenrod" ,    0xFFEEE8AA} ,
    {    "PaleGreen" ,    0xFF98FB98} ,
    {    "PaleTurquoise" ,    0xFFAFEEEE} ,
    {    "PaleVioletRed" ,    0xFFDB7093} ,
    {    "PapayaWhip" ,    0xFFFFEFD5} ,
    {    "PeachPuff" ,    0xFFFFDAB9} ,
    {    "Peru" ,    0xFFCD853F} ,
    {    "Pink" ,    0xFFFFC0CB} ,
    {    "Plum" ,    0xFFDDA0DD} ,
    {    "PowderBlue" ,    0xFFB0E0E6} ,
    {    "Purple" ,    0xFF800080} ,
    {    "Red" ,    0xFFFF0000} ,
    {    "RosyBrown" ,    0xFFBC8F8F} ,
    {    "RoyalBlue" ,    0xFF4169E1} ,
    {    "SaddleBrown" ,    0xFF8B4513} ,
    {    "Salmon" ,    0xFFFA8072} ,
    {    "SandyBrown" ,    0xFFF4A460} ,
    {    "SeaGreen" ,    0xFF2E8B57} ,
    {    "SeaShell" ,    0xFFFFF5EE} ,
    {    "Sienna" ,    0xFFA0522D} ,
    {    "Silver" ,    0xFFC0C0C0} ,
    {    "SkyBlue" ,    0xFF87CEEB} ,
    {    "SlateBlue" ,    0xFF6A5ACD} ,
    {    "SlateGray" ,    0xFF708090} ,
    {    "Snow" ,    0xFFFFFAFA} ,
    {    "SpringGreen" ,    0xFF00FF7F} ,
    {    "SteelBlue" ,    0xFF4682B4} ,
    {    "Tan" ,    0xFFD2B48C} ,
    {    "Teal" ,    0xFF008080} ,
    {    "Thistle" ,    0xFFD8BFD8} ,
    {    "Tomato" ,    0xFFFF6347} ,
    {    "Transparent" ,    0x00FFFFFF} ,
    {    "Turquoise" ,    0xFF40E0D0} ,
    {    "Violet" ,    0xFFEE82EE} ,
    {    "Wheat" ,    0xFFF5DEB3} ,
    {    "White" ,    0xFFFFFFFF} ,
    {    "white" ,     0xFFFFFFFF} ,
    {    "WhiteSmoke" ,    0xFFF5F5F5} ,
    {    "Yellow" ,    0xFFFFFF00} ,
    {    "YellowGreen" ,   0xFF9ACD32} ,  
    {      NULL                    ,   0x00000000} ,
    };

bool 
color_t::GetColorByName(const char * name,color_t * color)
{
    bool found = false;
    for (int i = 0;colorNames[i].name != NULL;i++)
    {
        if (strcmp(colorNames[i].name,name)==0)
        {
            found = true;
            color->a = (colorNames[i].color & 0xff000000)>>24;
            color->r = (colorNames[i].color & 0x00ff0000)>>16;
            color->g = (colorNames[i].color & 0x0000ff00)>>8;
            color->b = (colorNames[i].color & 0x000000ff);
            break;
        }
    }
    return found;
}

void 
color_t::Convert24To16(unsigned char * in,unsigned char ** out,unsigned int &w,unsigned int &h,float zoomx,float zoomy)
{
    if (w*h>0)
    {
        if ((float)((int)zoomx)==zoomx &&
            (float)((int)zoomy)==zoomy)
        {
            //Metodo de zoom rapido si el zoom es un entero
            int realw = w*(int)zoomx;
            int realh = h*(int)zoomy;
            zoomx = (float)(realw/w);
            zoomy = (float)(realh/h);
            realw = w*(int)zoomx;
            realh = h*(int)zoomy;
            *out = new unsigned char[realw*realh*2];
            int len  = realw*realh;
            unsigned char   * p8  = in;
            unsigned short  * p16 = (unsigned short*)*out;
            int in_line = w*3;
            for(int y = 0;y<realh/zoomy;y++)
            {
                p8+=in_line;
                for (int zy = 0;zy < zoomy;zy++)
                {
                    p8-=in_line;
                    for (int x = 0;x<realw/zoomx;x++)
                    {
                        for (int j = 0;j<zoomx;j++)
                        {
                            WORD BLUE   = p8[0]>>3;
                            WORD GREEN  = p8[1]>>2;
                            WORD RED    = p8[2]>>3;
                            *p16   = RED<<11 | GREEN<<5| BLUE;p16++;
                        }                
                        p8+=3;
                    }                
                }
            } 
        }else{
            //Metodo mas lento con comas flotantes
            float x1,y1;
            float w1 = (float)w;
            float h1 = (float)h;
            float w2 = (float)w*zoomx;
            float h2 = (float)h*zoomy;
            float maxh = (float)((int)h2);
            float maxw = (float)((int)w2);
            *out = new unsigned char[(int)(maxh*maxw*2.0)];
            unsigned short * data_out = (unsigned short * )(*out);

            for (float y2 = 0.0;y2<maxh;y2++)
            {
                y1 = y2*h1/h2;
                for(float x2 = 0.0;x2<maxw;x2++)
                {
                    x1 = x2*w1/w2;                    
                    *data_out = (unsigned short)get_pixel(in,x1,y1,w,h,24,16);
                    data_out++;
                }
            }        
            w = (int)maxw;
            h = (int)maxh;
        }
    }else{
        *out = NULL;
    }    
}        

void 
color_t::Convert24To32(unsigned char * in,unsigned char ** out,unsigned int &w,unsigned int &h,float zoomx,float zoomy)
{
    if (w*h>0)
    {    
        if ((float)((int)zoomx)==zoomx &&
            (float)((int)zoomy)==zoomy)
        {
            //Metodo de zoom rapido si el zoom es un entero
            int realw = w*(int)zoomx;
            int realh = h*(int)zoomy;
            *out = new unsigned char[(int)(realw*realh*4.0)];
            int len  = (int)realw*(int)realh;
            unsigned char   * in_tmp  = in;
            unsigned char   * out_tmp = *out;
            int in_line = w*3;
            for(int y = 0;y<realh/zoomy;y++)
            {
                in_tmp+=in_line;
                for (int zy = 0;zy < zoomy;zy++)
                {
                    in_tmp-=in_line;
                    for (int x = 0;x<realw/zoomx;x++)
                    {
                        for (int j = 0;j<zoomx;j++)
                        {
                            unsigned long * color = (unsigned long*)out_tmp;
                            *color = COLOR_24_TO_32(in_tmp[0],in_tmp[1],in_tmp[2]);
                            out_tmp += 4;
                        }                
                        in_tmp+=3;
                    }                
                }
            }        
            w = realw;
            h = realh;    
        }else{
            //Metodo mas lento con comas flotantes
            float x1,y1;
            float w1 = (float)w;
            float h1 = (float)h;
            float w2 = (float)w*zoomx;
            float h2 = (float)h*zoomy;
            float maxh = (float)((int)h2);
            float maxw = (float)((int)w2);
            *out = new unsigned char[(int)(maxh*maxw*4.0)];
            unsigned long * data_out = (unsigned long * )(*out);

            for (float y2 = 0.0;y2<maxh;y2++)
            {
                y1 = y2*h1/h2;
                for(float x2 = 0.0;x2<maxw;x2++)
                {
                    x1 = x2*w1/w2;                    
                    *data_out = get_pixel(in,x1,y1,w,h,24,32);
                    data_out++;
                }
            }        
            w = (int)maxw;
            h = (int)maxh;
        }
    }else{
        *out = NULL;
    }
}

unsigned long 
color_t::get_pixel(unsigned char * in,float x,float y,int w,int h,int in_bits,int out_bits)
{
    int bytes = in_bits/8;
    int line  = w*bytes;
    int a = (int)y*line + (int)x*bytes;
    int b = a + bytes;
    int c = a + line;
    int d = c + bytes;
    if (((int)y)>=(h-1))
    {
        c = a;
        d = b;
    }
    return get_color_method4(in,x,y,a,b,c,d,out_bits);
}

unsigned long 
color_t::get_color_method0(unsigned char * in,float x,float y,int a,int b,int c,int d,int bits)
{
    switch (bits)
    {
    case 16:
        return COLOR_24_TO_16(*(in+a),*(in+a+1),*(in+a+2));        
    case 32:
        return COLOR_24_TO_32(*(in+a),*(in+a+1),*(in+a+2));        
    }
    return 0;
}

unsigned long 
color_t::get_color_method1(unsigned char * in,float x,float y,int a,int b,int c,int d,int bits)
{
    int color;    
    float dec_x = x - (float)((int)x);
    float dec_y = y - (float)((int)y);
    if (dec_x <= 0.5)
    {
        if (dec_y <= 0.5)
        {
            color = a;
        }else{
            color = c;
        }
    }else{
        if (dec_y <= 0.5)
        {
            color = b;
        }else{
            color = d;
        }
    }
    switch (bits)
    {
    case 16:
        return COLOR_24_TO_16(*(in+color),*(in+color+1),*(in+color+2));        
    case 32:
        return COLOR_24_TO_32(*(in+color),*(in+color+1),*(in+color+2));        
    }
    return 0;
}


unsigned long 
color_t::get_color_method2(unsigned char * in,float x,float y,int a,int b,int c,int d,int bits)
{
    int color[4];    
    memset(color,0,4);
    int n = 0;
    
    float dec_x = x - (float)((int)x);
    float dec_y = y - (float)((int)y);
    if (dec_x <= 0.33)
    {        
        if (dec_y <= 0.33)
        {
            color[0] = a;n=1;
        }else if (dec_y <= 0.66){
            color[0] = a;n = 2;
            color[1] = c;            
        }else{
            color[0] = c;n=1;            
        }    
    }else if (dec_x <= 0.66){
        if (dec_y <= 0.33)
        {
            color[0] = a;n=2;
            color[1] = b;
        }else if (dec_y <= 0.66){
            color[0] = a;n = 4;
            color[1] = b;
            color[2] = c;
            color[3] = d;            
        }else{
            color[0] = c;n=2;            
            color[1] = d;
        }    
    }else{
        if (dec_y <= 0.33)
        {
            color[0] = b;n=1;            
        }else if (dec_y <= 0.66){
            color[0] = b;n = 2;
            color[1] = d;            
        }else{
            color[0] = d;n=1;                        
        }    
    }
    int red=0,green=0,blue=0;    
    for (int i = 0;i<n;i++)
    {        
        red += in[color[i]];
        green += in[color[i]+1];
        blue += in[color[i]+2];
    }
    red /= n;
    green /= n;
    blue /= n;
    switch (bits)
    {
    case 16:
        return COLOR_24_TO_16(red,green,blue);        
    case 32:
        return COLOR_24_TO_32(red,green,blue);        
    }
    return 0;    
}

typedef struct pesos
{
    int pa;
    int pb;
    int pc;
    int pd;
}pesos;

pesos p[5][5] = 
{{    //dec_y<0.2
    {    40, 0, 0, 0 },
    {    30,10, 0, 0 },
    {    20,20, 0, 0 },
    {    10,30, 0, 0 },
    {    0 ,40, 0, 0 }
},
{    //dec_y>0.2 && dec_y<0.4
    {    30, 0,  10, 0 },
    {    20, 10, 10, 0 },
    {    15, 15,  5, 5 },
    {    10, 20,  0, 10 },
    {     0, 30,  0, 10 }
},
{    //dec_y>0.4 && dec_y<0.6
    {    20, 0,  20, 0 },
    {    15, 5,  15, 5 },
    {    10, 10, 10, 10 },
    {    5,  15,  5, 15 },
    {    0,  20,  0, 20 }
},
{    //dec_y>0.6 && dec_y<0.8
    {    10, 0, 30, 0 },
    {    10, 0, 20, 10 },
    {    5,  5, 15, 15 },
    {    0, 10, 10, 20 },
    {    0,  0, 10, 30 }
},
{    //dec_y>0.8
    {    0, 0, 40,  0 },
    {    0, 0, 30, 10 },
    {    0, 0, 20, 20 },
    {    0, 0, 10, 30 },
    {    0, 0,  0, 40 }
}};
unsigned long 
color_t::get_color_method3(unsigned char * in,float x,float y,int a,int b,int c,int d,int bits)
{
    int px = 0;
    int py = 0;
    float dec_x = x - (float)((int)x);
    float dec_y = y - (float)((int)y);
    if (dec_x <= 0.2)
    {        
        px = 0;
    }else if (dec_x <= 0.4){
        px = 1;
    }else if (dec_x <= 0.6){
        px = 2;
    }else if (dec_x <= 0.8){
        px = 3;
    }else{
        px = 4;    
    }
    if (dec_y <= 0.2)
    {
        py = 0;    
    }else if (dec_y <= 0.4){
        py = 1;
    }else if (dec_y <= 0.6){
        py = 2;
    }else if (dec_y <= 0.8){
        py = 3;
    }else{
        py = 4;
    }    
    int red,green,blue;    
    red   = in[a]*p[py][px].pa;
    green = in[a+1]*p[py][px].pa;
    blue  = in[a+2]*p[py][px].pa;
    red   += in[b]*p[py][px].pb;
    green += in[b+1]*p[py][px].pb;
    blue  += in[b+2]*p[py][px].pb;
    red   += in[c]*p[py][px].pc;
    green += in[c+1]*p[py][px].pc;
    blue  += in[c+2]*p[py][px].pc;
    red   += in[d]*p[py][px].pd;
    green += in[d+1]*p[py][px].pd;
    blue  += in[d+2]*p[py][px].pd;
    
    red /= 40;
    green /= 40;
    blue /= 40;
    switch (bits)
    {
    case 16:
        return COLOR_24_TO_16(red,green,blue);        
    case 32:
        return COLOR_24_TO_32(red,green,blue);        
    }
    return 0;    
}

unsigned long 
color_t::get_color_method4(unsigned char * in,float x,float y,int a,int b,int c,int d,int bits)
{
    int px = 0;
    int py = 0;
    float dx = x - (float)((int)x);
    float dy = y - (float)((int)y);
    float rx = (1-dx);
    float ry = (1-dy);
    float rxry = rx*ry;
    float dxry = dx*ry;
    float rxdy = rx*dy;
    float dxdy = dx*dy;
    
    unsigned char red,green,blue;
    unsigned char* in_a = in+a;
    unsigned char* in_b = in+b;
    unsigned char* in_c = in+c;
    unsigned char* in_d = in+d;
    red = (unsigned char)((float)*in_a*rxry + (float)*in_b*dxry + (float)*in_c*rxdy + (float)*in_d*dxdy);in_a++;in_b++;in_c++;in_d++;
    green = (unsigned char)((float)*in_a*rxry + (float)*in_b*dxry + (float)*in_c*rxdy + (float)*in_d*dxdy);in_a++;in_b++;in_c++;in_d++;
    blue = (unsigned char)((float)*in_a*rxry + (float)*in_b*dxry + (float)*in_c*rxdy + (float)*in_d*dxdy);
        
    switch (bits)
    {
    case 16:
        return COLOR_24_TO_16(red,green,blue);        
    case 32:
        return COLOR_24_TO_32(red,green,blue);        
    }
    return 0;    
}

gamma_t::gamma_t(void)
{
    SetGamma(1.0,1.0,1.0);
}

void 
gamma_t::SetGamma(float gr,float gg, float gb)
{
    if (gr==1.0 && gg==1.0 && gb==1.0)
    {
        active = false;
    }else{
        active = true;

        for (float i=0.0;i<=255.0;++i)
        {
            gamma_r[(int)i]= (unsigned char)(255.0*(float)pow(i/255.0,1.0/gr));
            gamma_g[(int)i]= (unsigned char)(255.0*(float)pow(i/255.0,1.0/gg));
            gamma_b[(int)i]= (unsigned char)(255.0*(float)pow(i/255.0,1.0/gb));
        }
    }    
}

//color' = 255*(color/255)^(1/gamma)
void 
gamma_t::GammaTransform(unsigned char * in,unsigned char * out,int len)
{
    if (active)
    {
        for (int i = 0;i<len;i+=3)
        {
            *out++ = gamma_b[*in++];
            *out++ = gamma_g[*in++];
            *out++ = gamma_r[*in++];
        }    
    }
}

