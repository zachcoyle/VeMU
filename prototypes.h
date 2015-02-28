


#define VEMU_VERSION "0.19"

#define WIDTH 48                  // LCD screen is technically 48x32
#define HEIGHT 40                 // make it 40 for the built-in icons at the bottom

#define SCALE 1             //Change here to scale window & content

#define BPP 4
#define DEPTH 32

#define BACKGROUND 0xaad5c300
#define FOREGROUND 0x08105200

/* Button defines */
#define VMU_PAD1_UP     0x0001
#define VMU_PAD1_DOWN   0x0002
#define VMU_PAD1_LEFT   0x0004
#define VMU_PAD1_RIGHT  0x0008
#define VMU_PAD1_A      0x0010
#define VMU_PAD1_B      0x0020
#define VMU_PAD1_MODE   0x0040
#define VMU_PAD1_SLEEP  0x0080

unsigned char *lcdbuffer[(WIDTH*SCALE)*(HEIGHT*SCALE)];

extern void waitforevents(struct timeval *);
extern void checkevents();
extern void redrawlcd();

extern void run_cpu();
extern void resetcpu();
extern int loadflash(char *filename);
extern int loadbios(char *filename);

extern void VeMU_ButtonPressed(int button);
extern void VeMU_ButtonReleased(int button);

extern void vmputpixel(int, int, int);
extern void keypress(int i);
extern void keyrelease(int i);
extern int do_vmsgame(char *filename, char *biosname);
extern int do_lcdimg(char *filename);
extern void sound(int freq);
//#ifdef BSD_STYLE_GETTIMEOFDAY
//#ifdef TIMEZONE_IS_VOID
extern int gettimeofday(struct timeval *, void *);
#define GETTIMEOFDAY(tp) gettimeofday(tp, NULL)
/*
#else
extern int gettimeofday(struct timeval *, struct timezone *);
#endif
#define GETTIMEOFDAY(tp) gettimeofday(tp, NULL)
#else
extern int gettimeofday(struct timeval *);
#define GETTIMEOFDAY(tp) gettimeofday(tp)
#endif
 */
extern void error_msg(char *fmt, ...);
