#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "locker.h"

namespace libvnc
{

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

class CImage
{
public:

    enum EImageFormat { RGB565, RGB24, RGBA }; 

private:

    bool m_Updated;
    u32** m_Image;
    u32 m_W,m_H;

    CPthreadMutex  m_Mutex;

    u32 GetRGB16Image(u8 ** image);
    u32 GetRGB24Image(u8 ** image);
    u32 GetRGBAImage(u8 ** image);

    inline u32 GetRGB32Pixel(u32 R,u32 G, u32 B, u32 A);
    inline u32 GetRGB24Pixel(u32 R,u32 G, u32 B);
    inline u16 GetRGB16Pixel(u32 R,u32 G, u32 B);

    inline u32 GetPixel(u8* buf,u32 x, u32 y, u32 stride, EImageFormat format);
    inline u32 GetR(u32 pixel, EImageFormat format);
    inline u32 GetG(u32 pixel, EImageFormat format);
    inline u32 GetB(u32 pixel, EImageFormat format);
    inline u32 GetA(u32 pixel, EImageFormat format);

public:

    CImage(u32 w, u32 h);
    CImage(CImage &);
    ~CImage(void);

    void PutImage(u8 * buf, u32 x, u32 y, u32 w,u32 h, EImageFormat format);
    void CopyRect(u32 orig_x, u32 orig_y, u32 dest_x, u32 dest_y, u32 w, u32 h);
    void DrawRect(u32 x, u32 y, u32 w, u32 h, u32 red, u32 green, u32 blue, u32 alfa = 0xff);
    u32  GetImage(u8 ** image, EImageFormat format);    
    
    inline u32 GetW(void) { return m_W; }
    inline u32 GetH(void) { return m_H; }
    inline bool IsUpdated(void) { return m_Updated; }
};

}

#endif