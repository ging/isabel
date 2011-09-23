#include "image.h"
#include <string.h>

using namespace libvnc;

CImage::CImage(u32 w, u32 h)
{
    m_Updated = false;
    this->m_W = w;
    this->m_H = h;
    m_Image = new u32*[m_H];
    for (u32 y = 0; y < m_H; ++y)
    {
        m_Image[y] = new u32[m_W];
        memset(m_Image[y],0,m_W*sizeof(u32));
    }
}

CImage::CImage(CImage& other)
{
    this->m_W = other.GetW();
    this->m_H = other.GetH();
    m_Image = new u32*[m_H];
    for (u32 y = 0; y < m_H; ++y)
    {
        m_Image[y] = new u32[m_W];
        memcpy(m_Image[y],other.m_Image[y],m_W*sizeof(u32));
    }
}

CImage::~CImage(void)
{
    for (u32 y = 0; y < m_H; ++y)
    {
        delete[] m_Image[y];
    }
    delete[] m_Image;
}

u32 
CImage::GetPixel(u8* buf,u32 x, u32 y, u32 stride, EImageFormat format)
{
    int bpp;
    switch (format)
    {
    case RGB565:         
        {
            bpp = 2;
            u8 * pixel = &buf[y*stride*bpp+x*bpp];
            return (u32)(*(u16*)(pixel));
        }
        break;
    case RGB24:
        {
            bpp = 3;
            u8 * pixel = &buf[y*stride*bpp+x*bpp];
            return (*(u32*)(pixel))>>8;
        }
        break;
    case RGBA:
        {
            bpp = 4;
            u8 * pixel = &buf[y*stride*bpp+x*bpp];
            return (*(u32*)(pixel));
        }
    break;
    }
    return 0;
}

u32 
CImage::GetR(u32 pixel, EImageFormat format)
{
    switch (format)
    {
    case RGB565: 
        return ((pixel & 0x0000f800)>>8);
    break;
    case RGB24:
        return ((pixel & 0x000000ff)>>8);
    break;
    case RGBA:
        return ((pixel & 0x000000ff)>>8);
    break;
    }
    return 0;
}

u32 
CImage::GetG(u32 pixel, EImageFormat format)
{
    switch (format)
    {
    case RGB565: 
        return ((pixel & 0x000007E0)>>3);
    break;
    case RGB24:
        return ((pixel & 0x0000ff00)>>8);
    break;
    case RGBA:
        return ((pixel & 0x0000ff00)>>8);
    break;
    }
    return 0;
}

u32 
CImage::GetB(u32 pixel, EImageFormat format)
{
    switch (format)
    {
    case RGB565: 
        return ((pixel & 0x0000001f)<<3);        
    break;
    case RGB24:
        return ((pixel & 0x00ff0000)>>16);
    break;
    case RGBA:
        return ((pixel & 0x00ff0000)>>16);
    break;
    }
    return 0;
}
    
u32 
CImage::GetA(u32 pixel, EImageFormat format)
{
    switch (format)
    {
    case RGB565: 
        return 0xff;
    break;
    case RGB24:
        return 0xff;
    break;
    case RGBA:
        return (pixel & 0xff000000)>>24;
    break;
    }
    return 0;
}

void 
CImage::PutImage(u8 * buf, u32 x, u32 y, u32 w,u32 h, EImageFormat format)
{
    CLocker lock(m_Mutex);    
    if (x >= m_W || y >= m_H)
    {
        return;
    }
    m_Updated = true;
    //check edges
    int imageW = w;
    
    if (y + h > m_H) h = m_H - y;
    if (x + w > m_W) w = m_W - x;
    for (u32 row = 0; row < h; ++row)
    {
        u32 * line = m_Image[row+y] + x;        
        for (u32 col = 0; col < w; ++col)
        {
            u32 R = GetR(GetPixel(buf,col,row,imageW,format),format);
            u32 G = GetG(GetPixel(buf,col,row,imageW,format),format);
            u32 B = GetB(GetPixel(buf,col,row,imageW,format),format);
            u32 A = GetA(GetPixel(buf,col,row,imageW,format),format);
            *line = GetRGB32Pixel(R,G,B,A); line++;
        }
    }
}

void 
CImage::CopyRect(u32 orig_x, u32 orig_y, u32 dest_x, u32 dest_y, u32 w, u32 h)
{
    CLocker lock(m_Mutex);
    m_Updated = true;
    if (orig_x >= m_W  || dest_x >= m_W || orig_y >= m_H || dest_y >= m_H)
    {
        return;
    }
    //check edges
    if (orig_y + h > m_H) h = m_H - orig_y;
    if (orig_x + w > m_W) w = m_W - orig_x;
    if (dest_y + h > m_H) h = m_H - dest_y;
    if (dest_x + w > m_W) w = m_W - dest_x;
    
    //copiar imagen
    u32 ** tmp = new u32*[h];
    for (u32 row = orig_y,i = 0; row < orig_y + h; ++row,++i)
    {
        tmp[i] = new u32[w];
        memcpy(tmp[i],m_Image[row]+orig_x,w*sizeof(u32));
    }
    //pegar imagen
    for (u32 row = dest_y,i = 0; row < dest_y + h; ++row,++i)
    {
        memcpy(m_Image[row]+dest_x,tmp[i],w*sizeof(u32));
    }    
    for (u32 row = 0; row < h; ++row)
    {
        delete[] tmp[row];
    }
    delete[] tmp;
}

void 
CImage::DrawRect(u32 x, u32 y, u32 w, u32 h, u32 red, u32 green, u32 blue, u32 alfa)
{
    CLocker lock(m_Mutex);
    if (x >= m_W || y >= m_H)
    {
        return;
    }
    m_Updated = true;
    //check edges
    if (y + h > m_H) h = m_H - y;
    if (x + w > m_W) w = m_W - x;
    //crear rectangulo
    u32 color = GetRGB32Pixel(red,green,blue,alfa);
    for (u32 row = y; row < y + h; ++row)
    {
        u32* line = m_Image[row]+x;
        for (u32 col = 0; col < w; ++col)
        {
            *line = color;
            line++;
        }
    }     
}
u32  
CImage::GetImage(u8 ** image, EImageFormat format)
{    
    CLocker lock(m_Mutex);
    m_Updated = false;
    switch (format)
    {
    case RGB565: 
        return GetRGB16Image(image);
    break;
    case RGB24:
        return GetRGB24Image(image);
    break;
    case RGBA:
        return GetRGBAImage(image);
    break;
    }
    return 0;
}

u32 
CImage::GetRGB32Pixel(u32 R,u32 G, u32 B, u32 A)
{
    return (u32)(A << 24 | B << 16 | G << 8 | R);    
}

u32 
CImage::GetRGB24Pixel(u32 R,u32 G, u32 B)
{
    return (u32)(B << 16 | G << 8 | R);    
}

u16 
CImage::GetRGB16Pixel(u32 R,u32 G, u32 B)
{
     return (u16)((R>>3)<<11 | (G>>2)<<5| (B>>3));
}

u32 
CImage::GetRGB16Image(u8 ** image)
{
    int BPP = 2;
    u32 len = m_W*m_H*BPP;
    *image = new u8[len];
    u32 * pin =  (u32*)m_Image;
    u16 * pout = (u16*)*image;

    for (u32 y = 0; y < m_H; ++y)
    {
        for (u32 x = 0; x < m_W; ++x)
        {
            *pout = GetRGB16Pixel(GetR(*pin,RGBA),GetG(*pin,RGBA),GetB(*pin,RGBA));
             pin++;
             pout++;
        }
    }
    return len;
}

u32 
CImage::GetRGB24Image(u8 ** image)
{
    int BPP = 3;
    u32 len = m_W*m_H*BPP;
    *image = new u8[len];    
    u8 * pout = (u8*)*image;

    u8 ** line = (u8**)m_Image;
    for (u32 y = 0; y < m_H; ++y)
    {
        u8 * pin = (u8*)(*line++);
        for (u32 x = 0; x < m_W; ++x)
        {
            *pout++ = *pin++; 
            *pout++ = *pin++; 
            *pout++ = *pin++; 
             pin++;
        }
    }
    return len;
}

u32 
CImage::GetRGBAImage(u8 ** image)
{
    int BPP = 4;
    u32 len = m_W*m_H*BPP;
    *image = new u8[len];
    u8 * pout = (u8*)*image;
    int stride = m_W*BPP;
    for (u32 y = 0; y < m_H; ++y)
    {        
        memcpy(pout,m_Image[y],stride);
        pout+=stride;
    }
    return len;
}
