/*
 * capture:c: capture windows and memory share to allow
 * other programs to get the VNC image
 */

/* header files */

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/types.h>
#include <X11/Xlib.h>
#include <vncviewer.h>
#include <X11/extensions/XShm.h>

#define SHARED_PIXMAP    1   // if use shared memory pixmap
#define DEBUG 0

//KEY Define to Shared Memory Segments
#define KEY_STRUCT   ((key_t) (1250))           // structure key
#define KEY_DATA   ((key_t) (1260))             // key to shared memory data
#define KEY_DATA_IMAGE   ((key_t) (1270))       // key to image->data
#define KEY_IMAGE_DATA_PIXMAP ((key_t) (1280))  // key to myxpixmap data
          // Data Key Shared Memory  Key
#define SIZE_STRUCT  sizeof (struct MyXimage)   // Size of the structure


//Structure Shared to send the image structure

XShmSegmentInfo pixmap_shminfo;   // Shminfo to the Pixmap Shared Memory
unsigned char *myximage_data;     // Pointer to the Data Shared Memory

struct MyXimage{
    int w;
    int h;
    int xoffset;
    int format;
    int byteorder;
    int bitmapunit;
    int bitmap_bit_order;
    int bitmappad;
    int depth;
    int bytesperline;
    int bits_per_pixel;
    unsigned long redmask;
    unsigned long greenmask;
    unsigned long  bluemask;
    int redshift;
    int greenshift;
    int blueshift;
    int vncCapture;
};


/* variable definition */
Bool shMemoryOK= False;
Pixmap mypixmap; /* Pixmap used to store the image */
struct MyXimage *myximage_struct; /* Pointer to the Structure Shared Memory */

unsigned char *mem_pixmap;        /* Pointer to the Data Pixmap Shared Memory */
int idmem_struct,idmem_data;      /* Store the shared Memory ID. */

Bool Shmpixmap;       /* Set if the pixmap was created with shared memory */


/* shared memory control */
static Bool caughtShmError = False;
static Bool needShmCleanup = False;


/* Function Declaration */

void share_memory(XImage *theimage);  /* create or attach a shared memory to image structure and data. */
void create_pixmap(XImage *theimage); /* create a pixmap element. */
void myxfillrec(Display *dpy,GC gc,int x,int y,unsigned int w,unsigned int h); /* call XFillRectangle to the pixmap */
void myxcparea(Display *display,GC gc,int src_x,int src_y,int width,int height,int dest_x, int dest_y); /* call XCopyArea with the pixmap as parameter */

/* Function to handle the soft mouse */
void myxcpareaTo(Display *display,Drawable dstDraw,GC gc,int src_x,int src_y,int width,int height,int dest_x, int dest_y);
void myxcpareaFrom(Display *display,Drawable srcDraw,GC gc,int src_x,int src_y,int width,int height,int dest_x, int dest_y);

/*
 * share_memory()
 * This function shared the memory
 */
void
share_memory(XImage *theimage)
{
  if ( ! shMemoryOK)
  {
    fprintf(stderr,"share_memory: should not use shared memory\n");
    return;
  }

#if DEBUG
  fprintf(stderr,"capture::share_memory()\n\r");
#endif

  /* Sharing the image information segment. */

  idmem_struct = shmget((key_t)appData.idShMemory,SIZE_STRUCT,IPC_CREAT | 0777);
  if (idmem_struct < 0)
  {
    fprintf(stderr, "shmget FAILED: %d %s\n", errno, strerror(errno));
    exit(1);
  }

  myximage_struct = (struct MyXimage*) shmat(idmem_struct,0,0);
  if (myximage_struct <= (struct MyXimage *) (0))
  {
    fprintf(stderr,"shmat FAILED: %d %s\n", errno, strerror(errno));
    exit(2);
  }

  /* Initializating the shared structure memory values */
  myximage_struct->bits_per_pixel=   theimage->bits_per_pixel;
  myximage_struct->redmask=          theimage->red_mask;
  myximage_struct->greenmask=        theimage->green_mask;
  myximage_struct->bluemask=         theimage->blue_mask;
  myximage_struct->redshift=         myFormat.redShift;
  myximage_struct->greenshift=       myFormat.greenShift;
  myximage_struct->blueshift=        myFormat.blueShift;
  myximage_struct->w=                si.framebufferWidth;
  myximage_struct->h=                si.framebufferHeight;
  myximage_struct->depth=            theimage->depth;
  myximage_struct->xoffset=          theimage->xoffset;
  myximage_struct->bitmappad=        theimage->bitmap_pad;
  myximage_struct->bytesperline=     theimage->bytes_per_line;
  myximage_struct->byteorder=        theimage->byte_order;
  myximage_struct->bitmapunit=       theimage->bitmap_unit;
  myximage_struct->bitmap_bit_order= theimage->bitmap_bit_order;
  myximage_struct->vncCapture=       237855; /* communication control */

  /*  Sharing the Data Memory */
  fprintf(stderr, "Usando Memoria Compartida\n\r");

#if 0
  /* Sharing the image data memory */
  idmem_data = shmget(KEY_DATA,
                      theimage->bytes_per_line * theimage->height,
                      IPC_CREAT | 0777
                     );
  if (idmem_data < 0)
  {
    fprintf(stderr, "shmget failed=%d %s\n", errno, strerror(errno));
    exit(1);
  }
  myximage_data = (unsigned char *) shmat(idmem_data, 0, 0);
  if (myximage_data <= (unsigned char *) (0))
  {
    fprintf(stderr, "shmat failed %d %s\n", errno, strerror(errno)));
    exit(2);
  }
#endif
}


void create_pixmap(XImage *theimage)
{
  Shmpixmap= False;

#if SHARED_PIXMAP
  if ( ! CreateShmXpixmap())
  {
    Shmpixmap= False;
    mypixmap= XCreatePixmap(dpy,
                            desktopWin,
                            si.framebufferWidth,
                            si.framebufferHeight,
                            myFormat.depth
                           );
    fprintf(stderr, "Created pixmap WITHOUT shared memory\n");
  }
  else
  {
    Shmpixmap= True;
    fprintf(stderr, "Created pixmap with shared memory\n");
  }
#else
  mypixmap= XCreatePixmap(dpy,
                          desktopWin,
                          si.framebufferWidth,
                          si.framebufferHeight,
                          myFormat.depth
                         );
  fprintf(stderr, "Create_pixmap WITHOUT shared memory\n\r");
#endif
}

void
vnc_capture_init(XImage *theimage)
{
  if (appData.shmCapture) {
    if (appData.idShMemory > 0 && appData.sizeShMemory > 0) {
      fprintf(stderr, "vncCapture: TRYING SHARED MEMORY\n");
      shMemoryOK= True;
    } else {
      fprintf(stderr,"Bad Usage: either idShMemory <= 0 or sizeShMemory <=0\n");
      shMemoryOK= False;
      usage();
      exit(1);
    }
  } else {
    fprintf(stderr, "vncCapture: NO SHARED MEMORY, common vncviewer\n");
    shMemoryOK= False;
  }

  if (shMemoryOK)
  {
    share_memory(theimage);
    create_pixmap(theimage);
  }
}

void
vnc_capture_cleanup()
{
  if ( ! shMemoryOK)
  {
#if DEBUG
    fprintf(stderr, "vnc_capture_cleanup(): NOT using shared memory\n");
#endif
    return;
  }

  shmdt(pixmap_shminfo.shmaddr);
  shmctl(pixmap_shminfo.shmid, IPC_RMID, 0);
}

void myxfillrec(Display *dpy, GC gc, int x, int y, unsigned int w, unsigned int h)
{
  if ( ! shMemoryOK)
  {
#if DEBUG
    fprintf(stderr, "myxfillrec(): NOT using shared memory\n");
#endif
    return;
  }

  XFillRectangle(dpy, mypixmap, gc, x, y,w,h);
}


void myxcparea(Display *display,GC gc,int src_x,int src_y,int width,int height,int dest_x, int dest_y)
{
  if ( ! shMemoryOK)
  {
#if DEBUG
    fprintf(stderr,"myxcparea(): NOT using shared memory\n");
#endif
    return;
  }

  XCopyArea(dpy, mypixmap, mypixmap,gc,src_x,src_y,width,height,dest_x,dest_y);
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
  if ( ! shMemoryOK)
  {
#if DEBUG
    fprintf(stderr,"myxcpareaTo(): NOT using shared memory\n");
#endif
    return;
  }

  XCopyArea(dpy, mypixmap,dstDraw,gc,src_x,src_y,width,height,dest_x,dest_y);
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
  if ( ! shMemoryOK)
  {
#if DEBUG
    fprintf(stderr, "myxcpareaFrom(): NOT using shared memory\n");
#endif
    return;
  }

  XCopyArea(dpy, srcDraw,mypixmap,gc,src_x,src_y,width,height,dest_x,dest_y);
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
  if ( ! shMemoryOK)
  {
#if DEBUG
    fprintf(stderr, "myxshmputimage(): NOT using shared memory\n");
#endif
    return;
  }

  XShmPutImage(display, mypixmap, gc, theimage, src_x, src_y, dest_x, dest_y, width, height, False);
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
  if ( ! shMemoryOK)
  {
#if DEBUG
    fprintf(stderr, "myxputimage(): NOT using shared memory\n");
#endif
    return;
  }

  XPutImage(display, mypixmap, gc, theimage, src_x, src_y, dest_x, dest_y, width, height);
}


static int ShmCreationXErrorHandler(Display *dpy, XErrorEvent *error)
{
  caughtShmError = True;
  return 0;
}

/***********************************************************************/

Bool
CreateShmXpixmap(XImage *theimage)
{
  if ( ! shMemoryOK)
  {
    fprintf(stderr, "CreateShmXpixmap(): NO USING SHARED MEMORY\n");
    return False;
  }

  Bool pix;
  int major,minor,format;
  int mem_size= theimage->bytes_per_line * theimage->height;

  // Pixmap *mipixmap;
  XErrorHandler oldXErrorHandler;

  if ( ! XShmQueryExtension(dpy))
    return False;

  if ( ! XShmQueryVersion(dpy,&major,&minor,&pix))
    return False;

  if ( ! pix)
  {
    fprintf(stderr, "Shared Memory Pixmap NOT SUPPORTED\n");
    return False;
  }

  format= XShmPixmapFormat(dpy);
  if (format != ZPixmap)
  {
    fprintf(stderr,"ZPIXMAP NOT SUPPORTED\n");
    return False;
  }

  //Creating Shared Memory for the pixmap

  pixmap_shminfo.shmid = shmget((key_t)(appData.idShMemory+30), appData.sizeShMemory, IPC_CREAT|0777);

  if (pixmap_shminfo.shmid < 0)
  {
    fprintf(stderr, "shmget failed %d %s\n", errno, strerror(errno));
    return False;
  }

  //fprintf(stderr, "Dir=%x\n", mem_pixmap);
  pixmap_shminfo.shmaddr = mem_pixmap = (unsigned char *)shmat(pixmap_shminfo.shmid,0, 0);
  //fprintf(stderr, "Dir=%x\n", mem_pixmap);

  if ( ! pixmap_shminfo.shmaddr)
  {
    shmctl(pixmap_shminfo.shmid, IPC_RMID, 0);
    fprintf(stderr, "shmat failed %d %s\n", errno, strerror(errno));
    return False;
  }

  if ( ! XShmAttach(dpy, &pixmap_shminfo))
  {
    fprintf(stderr, "XShmAttach FAILED\n");
    return False;
  }

  mypixmap = XShmCreatePixmap(dpy,
                              desktopWin,
                              mem_pixmap,
                              &pixmap_shminfo,
                              si.framebufferWidth,
                              si.framebufferHeight,
                              visdepth
                             );

  if (mypixmap <= 0)
  {
    fprintf(stderr, "XShmCreatePixmap FAILED\n");
    return False;
  }

  pixmap_shminfo.readOnly = False;
  needShmCleanup = True;

  oldXErrorHandler = XSetErrorHandler(ShmCreationXErrorHandler);
  XSync(dpy, False);
  XSetErrorHandler(oldXErrorHandler);

  if (caughtShmError)
  {
    shmdt(pixmap_shminfo.shmaddr);
    shmctl(pixmap_shminfo.shmid, IPC_RMID, 0);
    fprintf(stderr, "Saliendo :-( I ******\n");
    return False;
  }

  return True;
}

