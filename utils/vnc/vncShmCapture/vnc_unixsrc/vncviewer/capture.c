/*
 * capture.c: capture windows and memory share to allow
 * other programs to get the VNC image
 */

/* header files */

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/types.h>
#include <X11/Xlib.h>
#include <vncviewer.h>

#define DEBUG 0

/* types definition */

union swapun
{
    unsigned int l;                        /* 32 bits */
    unsigned short s;                      /* 16 bits */
    unsigned char b[sizeof(unsigned int)]; /* 32 bits */
};


struct grabInfo_t
{
    int w;
    int h;
    int format;
    int vncCapture;
};

struct ximageInfo_t
{
    int format;
    int depth;
    int BPP;
    int bytes_per_pixel;
    unsigned long redmask;
    unsigned long greenmask;
    unsigned long bluemask;
    int redshift;
    int greenshift;
    int blueshift;
    int flipbytes;
    int bit_shift;
};


/* variable definition */
Bool shMemoryOK= False;   /* if shared memory capture is in use or not */

/* image info from X server */
struct ximageInfo_t  myxinfo;     /* XImage info, for color conversion */
struct grabInfo_t   *grabInfo;    /* grab image shared info */

/* Shared Memory for the Image Raster */
int infoShmId;                  /* info Shared Memory ID. */

/* Shared memory for Image Info */

unsigned char *mem_pixmap;      /* Pointer to the Data Pixmap Shared Memory */
int imageShmId;                 /* image Shared Memory Id */

void
copyline_RGB24(unsigned char *dst,
               unsigned char *src,
               int dst_x,
               int src_x,
               int width
              )
{
    union swapun sw;
    int i, j;
    unsigned int r, g, b;
    unsigned char tmpbyte;
    unsigned char  pixel_8;
    unsigned short pixel_16;
    unsigned int   pixel_32;

    switch (myxinfo.bytes_per_pixel)
    {
    case 1: // FALTA COMPROBAR
        //fprintf(stderr, "8bpp conversion.\n");
        for (i= 0; i < width; i++)
        {
          pixel_8= *src;
          src++;

          r= (pixel_16 & myxinfo.redmask)   >> myxinfo.redshift;
          g= (pixel_16 & myxinfo.greenmask) >> myxinfo.greenshift;
          b= (pixel_16 & myxinfo.bluemask)  >> myxinfo.blueshift;
          *dst++ = r;
          *dst++ = g;
          *dst++ = b;
        }
        break;
    case 2:  //2 bytes per pixel SIN HACER
        //fprintf(stderr, "16bpp conversion.\n");
        for (i= 0; i < width; i++)
        {
          pixel_16= *(unsigned short *)src;
          src += 2;

          if (myxinfo.flipbytes)
          {   /* swap CARD16 */
              sw.s = pixel_16;
              tmpbyte = sw.b[0];
              sw.b[0] = sw.b[1];
              sw.b[1] = tmpbyte;
              pixel_16 = sw.s;
          }
          r= (pixel_16 & myxinfo.redmask)   >> myxinfo.redshift;
          g= (pixel_16 & myxinfo.greenmask) >> myxinfo.greenshift;
          b= (pixel_16 & myxinfo.bluemask)  >> myxinfo.blueshift;
          *dst++ = r;
          *dst++ = g;
          *dst++ = b;
        }

        break;
    case 4: //32bpp
        //fprintf(stderr, "32bpp conversion.\n");
        for (i= 0; i < width; i++)
        {
          pixel_32= *(unsigned int *)src;
          src += 4;

          if (myxinfo.flipbytes)   // swap CARD32
          {
              sw.l = pixel_32;
              tmpbyte = sw.b[0];
              sw.b[0] = sw.b[3];
              sw.b[3] = tmpbyte;
              tmpbyte = sw.b[1];
              sw.b[1] = sw.b[2];
              sw.b[2] = tmpbyte;
              pixel_32 = sw.l;
          }
          pixel_32 = ( pixel_32 >> myxinfo.bit_shift) & 0xffffffff;

          r= (pixel_32 & myxinfo.redmask)   >> myxinfo.redshift;
          g= (pixel_32 & myxinfo.greenmask) >> myxinfo.greenshift;
          b= (pixel_32 & myxinfo.bluemask)  >> myxinfo.blueshift;
          *dst++ = r;
          *dst++ = g;
          *dst++ = b;
        }
        break;
    default:
        fprintf(stderr,
                "copyline_RGB24: %d not implemented\n",
                myxinfo.bytes_per_pixel
               );
    }
}

/*
 * share_image_info
 * This function shared the info of the image
 */
void
share_image_info(XImage *theimage)
{
  /* Check for flip bytes */
  union swapun sw;
  int isLsbMachine;

  /* Initializating the shared info of the grabbed image */
  grabInfo->w=          si.framebufferWidth;
  grabInfo->h=          si.framebufferHeight;
  grabInfo->vncCapture= 237855; /* communication control */

  /* Initializating the ximage info for color converision */
  myxinfo.bytes_per_pixel=  theimage->bits_per_pixel / 8;
  myxinfo.redmask=          theimage->red_mask;
  myxinfo.greenmask=        theimage->green_mask;
  myxinfo.bluemask=         theimage->blue_mask;
  myxinfo.redshift=         myFormat.redShift;
  myxinfo.greenshift=       myFormat.greenShift;
  myxinfo.blueshift=        myFormat.blueShift;
  myxinfo.depth=            theimage->depth;
  myxinfo.BPP=              theimage->bytes_per_line / si.framebufferWidth;

  /* byte order */
  sw.l = 1;
  isLsbMachine = (sw.b[0]) ? 1 : 0;
  myxinfo.flipbytes=
      ( isLsbMachine && theimage->byte_order != LSBFirst) ||
      (!isLsbMachine && theimage->byte_order == LSBFirst);

  if (theimage->bitmap_bit_order == MSBFirst)
      myxinfo.bit_shift = theimage->bitmap_unit - theimage->bits_per_pixel;
  else
      myxinfo.bit_shift = 0;

#if DEBUG
  fprintf(stderr,
          "BPP= %d, bits_per_pixel= %d, bitmapunit=%d\n",
          myxinfo.BPP,
          theimage->bits_per_pixel,
          theimage->bitmap_unit
         );
#endif
}

Bool
CreateShmMemory()
{
  if ( ! shMemoryOK)
  {
    fprintf(stderr, "CreateShmMemory(): NO USING SHARED MEMORY\n");
    return False;
  }

  /*
   * SHARED MEMORY for image info
   */
  infoShmId = shmget((key_t)appData.idShMemory,
                     sizeof(struct grabInfo_t) + appData.sizeShMemory,
                     IPC_CREAT | 0777
                    );
  if (infoShmId < 0)
  {
    fprintf(stderr, "shmget FAILED: %d %s\n", errno, strerror(errno));
    exit(1);
  }

  grabInfo = (struct grabInfo_t*) shmat(infoShmId, 0, 0);
  if (grabInfo == (struct grabInfo_t *) (-1))
  {
    fprintf(stderr,"shmat FAILED: %d %s\n", errno, strerror(errno));
    shmctl(infoShmId, IPC_RMID, 0);
    exit(2);
  }

  mem_pixmap= (unsigned char*)grabInfo + sizeof(struct grabInfo_t);

  /*  Sharing the Data Memory */
  fprintf(stderr, "Using shared memory for capturing image\n");

  return True;
}


void
vnc_capture_init(XImage *theimage)
{
  if (appData.shmCapture)
  {
    if (appData.idShMemory > 0 && appData.sizeShMemory > 0)
    {
      fprintf(stderr, "vncCapture: TRYING SHARED MEMORY\n");
      shMemoryOK= True;
    }
    else
    {
      fprintf(stderr,"Bad Usage: either idShMemory <= 0 or sizeShMemory <=0\n");
      shMemoryOK= False;
      usage();
      exit(1);
    }
  }
  else
  {
    fprintf(stderr, "vncCapture: NO SHARED MEMORY, common vncviewer\n");
    shMemoryOK= False;
  }

  if (shMemoryOK)
  {
    if ( ! CreateShmMemory())
    {
      fprintf(stderr, "vncCapture: CANNOT CREATE SHARED MEMORY\n");
      exit(1);
    }
    share_image_info(theimage);
  }
}

void
vnc_capture_cleanup()
{
  struct shmid_ds cont;

  if ( ! shMemoryOK)
  {
#if DEBUG
    fprintf(stderr, "vnc_capture_cleanup(): NOT using shared memory\n");
#endif
    return;
  }

  fprintf(stderr, "VNC_CAPTURE: cleaning shared memory\n");
  shmdt(grabInfo);
  shmctl(infoShmId, IPC_STAT, &cont);
  if (cont.shm_nattch == 0) /* 0 attached segments, destroy shm */
  {
    shmctl(infoShmId, IPC_RMID, 0);
  }
}

void myxfillrec(Display *dpy,
                GC gc,
                int x,
                int y,
                unsigned int w,
                unsigned int h
               )
{
  int i, j, pos;
  XGCValues gcvalues;
  unsigned long foreground;

  if ( ! shMemoryOK)
  {
#if DEBUG
    fprintf(stderr, "myxfillrec(): NOT using shared memory\n");
#endif
    return;
  }

#if DEBUG
  fprintf(stderr, "myxfillrec: (%dx%d) +%d+%d\n", w, h, x, y);
#endif

  XGetGCValues(dpy, gc, GCForeground, &gcvalues);
  foreground= gcvalues.foreground;

#if 0
  for (i= x; i < x + w; i++)
    for (j= y; j < y + h; j++)
    {
      pos= 3*(j * si.framebufferWidth + i);
      mem_pixmap[pos + 0]= (foreground & myxinfo.redmask) >> myxinfo.redshift;
      mem_pixmap[pos + 1]= (foreground & myxinfo.greenmask) >> myxinfo.greenshift;
      mem_pixmap[pos + 2]= (foreground & myxinfo.bluemask) >> myxinfo.blueshift;
    }
#else
  fprintf(stderr, "myxfillrec: SIN HACER\n");
#endif
}


void myxcparea(Display *display,
               GC gc,
               int src_x,
               int src_y,
               int width,
               int height,
               int dest_x,
               int dest_y
              )
{
  int j;
  unsigned int possrc, posdst;

  if ( ! shMemoryOK)
  {
#if DEBUG
    fprintf(stderr,"myxcparea(): NOT using shared memory\n");
#endif
    return;
  }

#if DEBUG
  fprintf(stderr, "myxcparea: (%dx%d) +%d+%d -> +%d+%d\n",
          width,
          height,
          src_x,
          src_y,
          dest_x,
          dest_y
         );
#endif

#if 0
  possrc= myxinfo.BPP * (src_y  * si.framebufferWidth + src_x);
  posdst= myxinfo.BPP * (dest_y * si.framebufferWidth + dest_x);
  for (j= 0; j < height; j++)
  {
    memcpy(&(mem_pixmap[posdst]),
           &(mem_pixmap[possrc]),
           myxinfo.BPP * width
          );
    possrc += myxinfo.BPP * si.framebufferWidth;
    posdst += myxinfo.BPP * si.framebufferWidth;
  }
#else
  fprintf(stderr, "myxcparea: SIN HACER\n");
#endif
}


/* Para Manejo de Cursor - Guarda  */
void
myxcpareaTo(Display *display,
            Drawable dstDraw,
            GC gc,
            int src_x,
            int src_y,
            int width,
            int height,
            int dest_x,
            int dest_y
           )
{
  int j;
  unsigned int possrc, posdst;

  if ( ! shMemoryOK)
  {
#if DEBUG
    fprintf(stderr,"myxcpareaTo(): NOT using shared memory\n");
#endif
    return;
  }

#if DEBUG
  fprintf(stderr,
          "myxcpareaTo: (%dx%d) +%d+%d -> +%d+%d\n",
          width,
          height,
          src_x,
          src_y,
          dest_x,
          dest_y
         );
#endif

  fprintf(stderr, "myxcpareaTo: SIN HACER\n");
}


/* Para Manejo de Cursor - Copia */
void
myxcpareaFrom(Display *display,
              Drawable srcDraw,
              GC gc,
              int src_x,
              int src_y,
              int width,
              int height,
              int dest_x,
              int dest_y
             )
{
  int j;
  XImage *ximg;
  unsigned int possrc, posdst;
  unsigned char *dst, *src;

  if ( ! shMemoryOK)
  {
#if DEBUG
    fprintf(stderr, "myxcpareaFrom(): NOT using shared memory\n");
#endif
    return;
  }

#if DEBUG
  fprintf(stderr,
          "myxcpareaFrom: (%dx%d) +%d+%d -> +%d+%d\n",
          width,
          height,
          src_x,
          src_y,
          dest_x,
          dest_y
         );
#endif

#if 1
  ximg= XGetImage(dpy, srcDraw, src_x, src_y, width, height, AllPlanes, ZPixmap);

  possrc= myxinfo.BPP*(src_y  * width + src_x);
  posdst= 3*(dest_y * si.framebufferWidth + dest_x);

  for (j= 0; j < height; j++)
  {
    dst= &(mem_pixmap[posdst]);
    src= &(ximg->data[possrc]);
    copyline_RGB24(dst, src, dest_x, src_x, width);

    possrc += myxinfo.BPP * width;
    posdst += 3 * si.framebufferWidth;
  }

  XDestroyImage(ximg);
#else
  fprintf(stderr, "myxcpareaFrom: SIN HACER\n");
#endif
}

void
myxshmputimage(Display *display,
               GC gc,
               XImage *theimage,
               int src_x,
               int src_y,
               int dest_x,
               int dest_y,
               unsigned int width,
               unsigned int height
              )
{
  int j;
  unsigned int possrc, posdst;
  unsigned char *src, *dst;

  if ( ! shMemoryOK)
  {
#if DEBUG
    fprintf(stderr, "myxshmputimage(): NOT using shared memory\n");
#endif
    return;
  }

#if DEBUG
  fprintf(stderr,
          "myxshmputimage: (%dx%d) +%d+%d -> +%d+%d\n",
          width,
          height,
          src_x,
          src_y,
          dest_x,
          dest_y
         );
#endif

#if 1
  possrc= myxinfo.BPP*(src_y  * theimage->width + src_x);
  posdst= 3*(dest_y * si.framebufferWidth + dest_x);
  src= &theimage->data[possrc];
  dst= &mem_pixmap[posdst];

  for (j= 0; j < height; j++)
  {
    dst= &(mem_pixmap[posdst]);
    src= &(theimage->data[possrc]);
    copyline_RGB24(dst, src, dest_x, src_x, width);

    possrc += myxinfo.BPP * theimage->width;
    posdst += 3 * si.framebufferWidth;
  }
#else
  fprintf(stderr, "myxshmputimage: SIN HACER\n");
#endif
}

void
myxputimage(Display *display,
            GC gc,
            XImage *theimage,
            int src_x,
            int src_y,
            int dest_x,
            int dest_y,
            unsigned int width,
            unsigned int height
           )
{
  int j;
  unsigned int possrc, posdst;
  unsigned char *src, *dst;

  if ( ! shMemoryOK)
  {
#if DEBUG
    fprintf(stderr, "myxputimage(): using shared memory\n");
#endif
    return;
  }

#if DEBUG
  fprintf(stderr,
          "myxputimage: (%dx%d) +%d+%d -> +%d+%d\n",
          width,
          height,
          src_x,
          src_y,
          dest_x,
          dest_y
         );
#endif

#if 1
  possrc= myxinfo.BPP * (src_y  * theimage->width + src_x);
  posdst= 3 * (dest_y * si.framebufferWidth + dest_x);
  src= &theimage->data[possrc];
  dst= &mem_pixmap[posdst];

  for (j= 0; j < height; j++)
  {
    dst= &(mem_pixmap[posdst]);
    src= &(theimage->data[possrc]);
    copyline_RGB24(dst, src, dest_x, src_x, width);

    possrc += myxinfo.BPP * theimage->width;
    posdst += 3 * si.framebufferWidth;
  }
#else
  fprintf(stderr, "myxputimage: SIN HACER\n");
#endif
}


