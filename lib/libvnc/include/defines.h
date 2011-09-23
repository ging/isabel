#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <string>
#include <vector>

namespace libvnc
{

#define MAX_ENCODINGS 10

#ifdef __BUILD_FOR_LINUX
typedef int HRESULT;
#define S_OK 0
#define E_FAIL -1
#define SUCCESS(x) ((x>=0)?true:false)
#define FAILED(x)  ((x<0)?true:false)
#endif

#ifdef WORDS_BIGENDIAN
#define Swap16IfLE(s) (s)
#define Swap32IfLE(l) (l)
#else
#define Swap16IfLE(s) ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff))
#define Swap32IfLE(l) ((((l) & 0xff000000) >> 24) | \
		       (((l) & 0x00ff0000) >> 8)  | \
 		       (((l) & 0x0000ff00) << 8)  | \
 		       (((l) & 0x000000ff) << 24))
#endif /* WORDS_BIGENDIAN */

#define RGB24_TO_PIXEL(bpp,r,g,b)						\
    (((CARD16)(r) & opt.client.redmax) << opt.client.redshift |	\
    ((CARD16)(g) & opt.client.greenmax) << opt.client.greenshift |	\
    ((CARD16)(b) & opt.client.bluemax) << opt.client.blueshift)

typedef struct _mousestate
{
   int x;
   int y;
   unsigned int buttonmask;
}MouseState;

typedef struct _serversettings
{
   std::string name;
   int width;
   int height;
   int bpp;
   int depth;
   int bigendian;
   int truecolour;
   int redmax;
   int greenmax;
   int bluemax;
   int redshift;
   int greenshift;
   int blueshift;
}ServerSettings;

typedef struct _clientsettings
{
   int bpp;
   int depth;
   int bigendian;
   int truecolour;
   int redmax;
   int greenmax;
   int bluemax;
   int redshift;
   int greenshift;
   int blueshift;
   int compresslevel;
   int quality;
}ClientSettings;

typedef struct VncOptions 
{
   std::string servername;
   int port;
   std::string password;
   std::vector<std::string> encodings;
   ServerSettings server;
   ClientSettings client;
   int shared;
   int stretch;   
   int poll_freq;
   /* not really options, but hey ;) */
   double h_ratio;
   double v_ratio;
}VncOptions;

}

#endif