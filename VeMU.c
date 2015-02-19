
/* Simple program:  Create a blank window, wait for keypress, quit.

   Please see the SDL documentation for details on using the SDL API:
   /Developer/Documentation/SDL/docs.html
*/
   
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <unistd.h>

#include "prototypes.h"

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
extern void putpixel();




//start muted
static int mute = 1;

extern int optind;
extern char *optarg;
extern int getopt(int argc, char * const * argv, const char *optstring);



void VeMU_ButtonPressed(int button) {
  switch (button) {
    case VMU_PAD1_UP:
      keypress(0);
      break;
    case VMU_PAD1_DOWN:
      keypress(1);
      break;
    case VMU_PAD1_LEFT:
      keypress(2);
      break;
    case VMU_PAD1_RIGHT:
      keypress(3);
      break;
    case VMU_PAD1_A:
      keypress(4);
      break;
    case VMU_PAD1_B:
      keypress(5);
      break;
    case VMU_PAD1_SLEEP:
      keypress(6);
      break;
    case VMU_PAD1_MODE:
      keypress(7);
      break;
  }
}

void VeMU_ButtonReleased(int button) {
  switch (button) {
    case VMU_PAD1_UP:
      keyrelease(0);
      break;
    case VMU_PAD1_DOWN:
      keyrelease(1);
      break;
    case VMU_PAD1_LEFT:
      keyrelease(2);
      break;
    case VMU_PAD1_RIGHT:
      keyrelease(3);
      break;
    case VMU_PAD1_A:
      keyrelease(4);
      break;
    case VMU_PAD1_B:
      keyrelease(5);
      break;
    case VMU_PAD1_SLEEP:
      keyrelease(6);
      break;
    case VMU_PAD1_MODE:
      keyrelease(7);
      break;
  }
}





/* End Of VeMU Wrapper */





int sound_freq = -1;

static void sound_callback(void *userdata, Uint8 *stream, int len)
{
  if(sound_freq <= 0)
    memset(stream, 0, len);
  else {
    int i;
    static char v = 0x7f;
    static int f = 0;
    for(i=0; i<len; i++) {
      f += sound_freq;
      while(f >= 32768) {
        v ^= 0xff;
        f -= 32768;
      }
      *stream++ = v;
    }
  }
}

///* Main Function */
//int main(int argc, char *argv[]) {
//  int c;
//  char *bios = NULL;
//  
//  /* Get arguments */
//  while((c=getopt(argc, argv, "hVS:ab:"))!=EOF)
//    switch(c) {
//      case 'h':
//        fprintf(stderr, "Usage: %s [-h] [-V] [-S factor] [-a] [-b bios] game.vms ...\n",argv[0]);
//        break;
//      case 'V':
//        fprintf(stderr, "softvms " VEMU_VERSION " by Marcus Comstedt <marcus@idonex.se>\n");
//        break;
//   //   case 'S':
//   //     pixdbl = 1;
//   //     break;
//      case 'b':
//        bios = optarg;
//        break;
//      case 'a':
//        mute = 0;
//        break;
//    }
//  
//  
//
//
//  /* open vms file if submitted */
//  if(argc>optind) {
//    int fd = open(argv[optind], O_RDONLY);
//    int r;
//    char hdr[4];
//    if(fd<0 || (r=read(fd, hdr, 4)<4)) {
//      perror(argv[optind]);
//      if(fd>=0)
//        close(fd);
//      return 1;
//    }
//    if(hdr[0]=='L' && hdr[1]=='C' && hdr[2]=='D' && hdr[3]=='i')
//      do_lcdimg(argv[optind]);
//    else
//      do_vmsgame(argv[optind], bios);
//  } 
//  else if(bios) {
//    do_vmsgame(NULL, bios);
//  }
//  
//
//	return(0);
//}








void error_msg(char *fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  vfprintf(stderr, fmt, va);
  fputc('\n', stderr);
  va_end(va);
}

void vmputpixel(int x, int y, int p)
{
	int screen_x, screen_y;
	screen_x = (x<<1) + 36;
	screen_y = (y<<1) + 85;
	
	unsigned short color = p & 0x1 ? FOREGROUND : BACKGROUND;
    
	putpixel(screen_x, screen_y, color);
	putpixel(screen_x+1, screen_y, color);
	putpixel(screen_x, screen_y+1, color);
	putpixel(screen_x+1, screen_y+1, color);
}

void putpixel(int x, int y, int pixel)
{
	if(x > 479 || y > 271) return;
    int bpp = 4;
    /* Here p is the address to the pixel we want to set */
    int k = y * 96 + x * bpp;
    Uint8 *p = (Uint8 *)lcdbuffer + y * 24 + x * bpp;
    
    switch(bpp) {
        case 1:
            *p = pixel;
            break;
            
        case 2:
            *(Uint16 *)p = pixel;
            break;
            
        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;
            
        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}


//void vmputpixel(int x, int y, int p)
//{
//  
//  /* Multilpy for scale support */
//  int i,j;
//  int k=0;
//  int pixel = p&1;
//  x *= SCALE;
//  y *= SCALE;
//  
//  /* write scaled pixel (or not scaled) */
//  if(pixel) {
//    for (i=0;i<SCALE;i++) {
//      for (j=0;j<SCALE;j++) {
//        k =(y*WIDTH)+x+j+i;
//        lcdbuffer[k] = (unsigned char*)FOREGROUND;
//      }
//    }  
//  }
//  else {
//    for (i=0;i<SCALE;i++) {
//      for (j=0;j<SCALE;j++) {
//        k =(y*WIDTH)+x+j+i;
//        //k =(y*WIDTH)+x;
//        lcdbuffer[k] = (unsigned char*)BACKGROUND;
//      }
//    }  
//    
//  }
//  
//}

void redrawlcd()
{
}

void checkevents()
{
}


void waitforevents(struct timeval *t)
{
  sleep(t->tv_sec);
  usleep(t->tv_usec);
}

void sound(int freq)
{
  sound_freq = freq;
}







