#include	<stdio.h>
#include	"estruct.h"
#include	"etype.h"
#include	"edef.h"
#include	"english.h"

#include	<sys/types.h>
#include	<signal.h>
#include	<termio.h>
#include	<fcntl.h>
#include	<sys/ndir.h>
#include	<sys/stat.h>
int kbdflgs;			  /* saved keyboard fd flags	  */
int kbdpoll;			  /* in O_NDELAY mode		     */
int kbdqp;			  /* there is a char in kbdq	 */
char kbdq;			  /* char we've already read	 */
struct termio otermio;		  /* original terminal characteristics */
struct termio ntermio;		  /* charactoristics to use inside */

#include	<signal.h>
extern int vttidy();

/*
 * This function is called once to set up the terminal device streams. On
 * VMS, it translates TT until it finds the terminal, then assigns a channel
 * to it and sets it raw. On CPM it is a no-op.
 */
ttopen()
{
  ioctl(0, TCGETA, &otermio);	  /* save old settings */
  ntermio.c_iflag = otermio.c_iflag & ~(INLCR | ICRNL | IGNCR);
  ntermio.c_oflag = otermio.c_oflag;
  ntermio.c_cflag = otermio.c_cflag;
  ntermio.c_lflag = otermio.c_lflag & ~(ICANON | ISIG | ECHO);
  ntermio.c_line = otermio.c_line;
  ntermio.c_cc[VMIN] = 1;
  ntermio.c_cc[VTIME] = 0;
  ioctl(0, TCSETAW, &ntermio);	  /* and activate them */
  kbdflgs = fcntl(0, F_GETFL, 0);
  kbdpoll = FALSE;

  /* on all screens we are not sure of the initial position of the cursor					 */
  ttrow = 999;
  ttcol = 999;
}

/*
 * This function gets called just before we go back home to the command
 * interpreter.
 */
ttclose()
{
  ioctl(0, TCSETA, &otermio);	  /* restore terminal settings */
  fcntl(0, F_SETFL, kbdflgs);
}

ttputs(char *s)
{
  char *p = s;

  while (*p)
    ttputc(*p++);
}

ttputc(int c)
{
  fputc(c, stdout);
}

/*
 * Flush terminal buffer. Does real work where the terminal output is
 * buffered up. A no-operation on systems where byte at a time terminal I/O
 * is done.
 */
ttflush()
{
  fflush(stdout);
}

/*
 * TTGETC: Read a character from the terminal, performing no editing and
 * doing no echo at all. More complex in VMS that almost anyplace else, which
 * figures. Very simple on CPM, because the system can do exactly what you
 * want.
 */
ttgetc()
{
  if (kbdqp)
    kbdqp = FALSE;
  else
  {

    /*
     * we desperatly seek a character so we turn off the NO_DELAY flag and
     * simply wait for the bastard
     */
    if (fcntl(0, F_SETFL, kbdflgs) < 0 && kbdpoll)
      return (FALSE);
    kbdpoll = FALSE;		  /* no polling */
    read(0, &kbdq, 1);		  /* wait until we get a character */
  }
  return (kbdq & 255);
}

#if	TYPEAH
/*
 * typahead:	Check to see if any characters are already in the keyboard
 * buffer
 */
typahead()
{
  if (!kbdqp)
  {
    /* set O_NDELAY */
    if (fcntl(0, F_SETFL, kbdflgs | O_NDELAY) < 0 && kbdpoll)
      return (FALSE);
    kbdpoll = TRUE;
    kbdqp = (1 == read(0, &kbdq, 1));
  }
  return (kbdqp);
}
#endif

rename1(char *old, char *new)	  /* change the name of a file */
{
  link(old, new);
  unlink(old);
}
