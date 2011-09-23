/*-----------------------------------------
 *  test01.c
 *  Signal to Noise Ratio.
 *  Angel Fdez. Herrero. Mayo-1998.
 *----------------------------------------*/

#include <fcntl.h>
#include <unistd.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  char b1[2], b2[2];
  int fd1, fd2, code;
  double d1, d2, tmp, num=0, den=0;

  if(argc!=3)
  {
    fprintf(stderr, "uso: test01 <original> <nuevo>\n");
    exit(1);
  }
  if((fd1=open(argv[1], O_RDONLY))==-1)
  {
    fprintf(stderr, "test01: No puedo abrir %s para leer.\n", argv[1]);
    exit(1);
  }
  if((fd2=open(argv[2], O_RDONLY))==-1)
  {
    fprintf(stderr, "test01: No puedo abrir %s para leer.\n", argv[2]);
    exit(1);
  }
  while((code=read(fd1, b1, 2))==2)
  {
    code=read(fd2, b2, 2);
    if(code==-1)
    {
      fprintf(stderr, "test01: Problemas de lectura en %s.\n", argv[2]);
      exit(1);
    }
    else if(code==0)
    {
      fprintf(stderr, "test01: %s es mas largo.\n", argv[1]);
      exit(1);
    }
    else if(code!=2) /* code==1 */
    {
      fprintf(stderr, "test01: Fichero %s truncado.\n", argv[2]);
      exit(1);
    }
#ifdef ENDIAN
    { char t; t=b1[0]; b1[0]=b1[1]; b1[1]=t; }
    { char t; t=b2[0]; b2[0]=b2[1]; b2[1]=t; }
#endif
    /*if(*(short *)b1!=*(short *)b2)
    {
      fprintf(stderr, "test01: Ficheros diferentes.\n");
      exit(1);
    }*/
    d1=(double)(*(short *)b1);
    d2=(double)(*(short *)b2);
    tmp=d2-d1;
    num+=tmp*tmp;
    den+=d1*d1;
  }
  if(code==-1)
  {
    fprintf(stderr, "test01: Problemas de lectura en %s.\n", argv[1]);
    exit(1);
  }
  else if(code==1)
  {
    fprintf(stderr, "test01: Fichero %s truncado.\n", argv[1]);
    exit(1);
  }
  if(read(fd2, b2, 2)!=0)
  {
    fprintf(stderr, "test01: %s es mas largo.\n", argv[2]);
    exit(1);
  }
  /*fprintf(stderr, "test01: Ficheros iguales.\n");*/
  tmp=num/den;
  if(tmp==0.0)
    fprintf(stderr, "test01: Ficheros iguales.\n");
  else
    fprintf(stderr, "S/R=%.2f dB\n", 10*log10(tmp));
  close(fd2);
  close(fd1);
  return 0;
}
