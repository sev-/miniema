/*
 * $Id: tcap.c,v 1.4 1994/08/15 21:27:30 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: tcap.c,v $
 * Revision 1.4  1994/08/15 21:27:30  sev
 * i'm sorry, but this indent IMHO more better ;-)
 * Revision 1.3  1994/08/15  20:42:11  sev Indented Revision
 * 1.2  1994/06/24  17:22:21  sev Patched ^Q ^S bug and added ttputs function
 * 
 * Revision 1.1  1994/06/24  14:17:12  sev Initial revision
 * 
 * 
 */

/*
 * tcap:	Unix V5, SUN OS, SCO XENIX, V7 and BS4.2 Termcap video driver
 * for MicroEMACS 3.10
 * 
 * o The following lines define the new termcap entry for the ansi kind of
 * terminal: it permits the use of functions keys F1 .. F10 and keys
 * HOME,END,PgUp,PgDn on the IBM PC keyboard (the last 3 lines of the
 * definition have been added):
 * 
 * li|ansi|Ansi standard
 * :al=\E[L:am:bs:cd=\E[J:ce=\E[K:cl=\E[2J\E[H:cm=\E[%i%d;%dH:co
 * :dc=\E[P:dl=\E[M:do=\E[B:bt=\E[Z:ei=:ho=\E[H:ic=\E[@:im=:li
 * :nd=\E[C:pt:so=\E[7m:se=\E[m:us=\E[4m:ue=\E[m:up=\
 * :kb=^h:ku=\E[A:kd=\E[B:kl=\E[D:kr=\E[C:eo:sf=\E[S:sr=\
 * :GS=\E[12m:GE=\E[10m:GV=\63:G
 * :GC=E:GL=\64:GR=C:RT=^J:G1=?:G2=Z:G3=@:G4=Y:GU=A:G
 * :CW=\E[M:NU=\E[N:RF=\E[O:RC=\	:WL=\E[S:WR=\E[T:CL=\E[U:CR=\
 * :HM=\E[H:EN=\E[F:PU=\E[I:PD=\	:k1=\E[M:k2=\E[N:k3=\E[O:k4=\E[P:k5=\
 * :k6=\E[R:k7=\E[S:k8=\E[T:k9=\E[U:k0=\
 * :kh=\E[H:kH=\E[F:kA=\E[L:kN=\E[G:kP=\E[I:
 * 
 */

#define termdef 1		  /* don't define "term" external */

#include <stdio.h>
#include	"estruct.h"
#include	"etype.h"
#include	"edef.h"
#include	"english.h"

#include	<time.h>
#include	<sys/types.h>
#include	<sys/time.h>

#define MARGIN	8
#define SCRSIZ	64
#define NPAUSE	10		  /* # times thru update to pause */
#define BEL	0x07

/* Termcap Sequence definitions	 */

typedef struct TBIND
{
  char p_name[4];		  /* sequence name */
  short p_code;			  /* resulting keycode of sequence */
  char p_seq[10];		  /* terminal escape sequence */
} TBIND;

TBIND ttable[] =
{
  "bt", SHFT | CTRL | 'i', "",	  /* backtab */
  "k1", SPEC | '1', "",		  /* function key 1 */
  "k2", SPEC | '2', "",		  /* function key 2 */
  "k3", SPEC | '3', "",		  /* function key 3 */
  "k4", SPEC | '4', "",		  /* function key 4 */
  "k5", SPEC | '5', "",		  /* function key 5 */
  "k6", SPEC | '6', "",		  /* function key 6 */
  "k7", SPEC | '7', "",		  /* function key 7 */
  "k8", SPEC | '8', "",		  /* function key 8 */
  "k9", SPEC | '9', "",		  /* function key 9 */
  "k0", SPEC | '0', "",		  /* function key 10 */
  "kA", CTRL | 'O', "",		  /* insert line */
  "kb", CTRL | 'H', "",		  /* backspace */
  "kC", CTRL | 'L', "",		  /* clear screen */
  "kD", SPEC | 'D', "",		  /* delete character */
  "kd", SPEC | 'N', "",		  /* down cursor */
  "kE", CTRL | 'K', "",		  /* clear to end of line */
  "kF", CTRL | 'V', "",		  /* scroll down */
  "kH", SPEC | '>', "",		  /* home down [END?] key */
  "kh", SPEC | '<', "",		  /* home */
  "kI", SPEC | 'C', "",		  /* insert character */
  "kL", CTRL | 'K', "",		  /* delete line */
  "kl", SPEC | 'B', "",		  /* left cursor */
  "kN", SPEC | 'V', "",		  /* next page */
  "kP", SPEC | 'Z', "",		  /* previous page */
  "kR", CTRL | 'Z', "",		  /* scroll down */
  "kr", SPEC | 'F', "",		  /* right cursor */
  "ku", SPEC | 'P', "",		  /* up cursor */
};

#define NTBINDS sizeof(ttable)/sizeof(TBIND)

extern int ttopen();
extern int ttgetc();
extern int ttputc();
extern int tgetnum();
extern int ttflush();
extern int ttclose();
extern int tcapkopen();
extern int tcapkclose();
extern int tcapgetc();
extern int tcapmove();
extern int tcapeeol();
extern int tcapeeop();
extern int tcapbeep();
extern int tcaprev();
extern int tcapcres();
extern int tcapopen();
extern int tcapclose();
extern int tput();
extern char *tgoto();
extern int ttputs();

#define TCAPSLEN 1024
char tcapbuf[TCAPSLEN];
char *UP, PC, *CM, *CE, *CL, *SO, *SE, *IS, *KS, *KE;

TERM term =
{
  0, 0, 0, 0,			  /* these four values are set dynamically at
				   * open time */
  MARGIN,
  SCRSIZ,
  NPAUSE,
  tcapopen,
  tcapclose,
  tcapkopen,
  tcapkclose,
  tcapgetc,
  ttputc,
  ttflush,
  tcapmove,
  tcapeeol,
  tcapeeop,
  tcapbeep,
  tcaprev,
  tcapcres,
  ttputs
};

/* input buffers and pointers	 */

#define IBUFSIZE	64	  /* this must be a power of 2 */

unsigned char in_buf[IBUFSIZE];	  /* input character buffer */
int in_next = 0;		  /* pos to retrieve next input character */
int in_last = 0;		  /* pos to place most recent input character */

in_init()			  /* initialize the input buffer */

{
  in_next = in_last = 0;
}

in_check()			  /* is the input buffer non-empty? */

{
  if (in_next == in_last)
    return (FALSE);
  else
    return (TRUE);
}

in_put(event)

int event;			  /* event to enter into the input buffer */

{
  in_buf[in_last++] = event;
  in_last &= (IBUFSIZE - 1);
}

int in_get()			  /* get an event from the input buffer */

{
  register int event;		  /* event to return */

  event = in_buf[in_next++];
  in_next &= (IBUFSIZE - 1);
  return (event);
}

/*
 * Open the terminal put it in RA mode learn about the screen size read
 * TERMCAP strings for function keys
 */

tcapopen()

{
  register int index;		  /* general index */
  char *t, *p;
  char tcbuf[1024];
  char *tv_stype;
  char err_str[72];
  char *getenv();
  char *tgetstr();

  if ((tv_stype = getenv("TERM")) == (char *) NULL)
  {
    puts(TEXT182);
    /* "Environment variable TERM not defined!" */
    meexit(1);
  }

  if ((tgetent(tcbuf, tv_stype)) != 1)
  {
    sprintf(err_str, TEXT183, tv_stype);
    /* "Unknown terminal type %s!" */
    puts(err_str);
    meexit(1);
  }


  if ((term.t_nrow = (short) tgetnum("li") - 2) == -1)
  {
    puts(TEXT184);
    /* "termcap entry incomplete (lines)" */
    meexit(1);
  }
  term.t_mrow = term.t_nrow;

  if ((term.t_ncol = (short) tgetnum("co")) == -1)
  {
    puts(TEXT185);
    /* "Termcap entry incomplete (columns)" */
    meexit(1);
  }
  term.t_mcol = term.t_ncol;

  p = tcapbuf;
  t = tgetstr("pc", &p);
  if (t)
    PC = *t;

  CL = tgetstr("cl", &p);
  CM = tgetstr("cm", &p);
  CE = tgetstr("ce", &p);
  UP = tgetstr("up", &p);
  SE = tgetstr("se", &p);
  SO = tgetstr("so", &p);
  if (SO != (char *) NULL)
    revexist = TRUE;

  if (CL == (char *) NULL || CM == (char *) NULL || UP == (char *) NULL)
  {
    puts(TEXT186);
    /* "Incomplete termcap entry\n" */
    meexit(1);
  }

  if (CE == (char *) NULL)	  /* will we be able to use clear to EOL? */
    eolexist = FALSE;

  IS = tgetstr("is", &p);	  /* extract init string */
  KS = tgetstr("ks", &p);	  /* extract keypad transmit string */
  KE = tgetstr("ke", &p);	  /* extract keypad transmit end string */

  /* read definitions of various function keys into ttable */
  for (index = 0; index < NTBINDS; index++)
  {
    strcpy(ttable[index].p_seq,
	   fixnull(tgetstr(ttable[index].p_name, &p)));
  }

  /* tell unix we are goint to use the terminal */
  ttopen();

  /* make sure we don't over run the buffer (TOO LATE I THINK) */
  if (p >= &tcapbuf[TCAPSLEN])
  {
    puts(TEXT187);
    /* "Terminal description too big!\n" */
    meexit(1);
  }

  /* send init strings if defined */
  if (IS != (char *) NULL)
    putpad(IS);

  if (KS != (char *) NULL)
    putpad(KS);

  /* initialize the input buffer */
  in_init();
}

tcapclose()
{
  /* send end-of-keypad-transmit string if defined */
  if (KE != (char *) NULL)
    putpad(KE);
  ttclose();
}

tcapkopen()

{
  strcpy(sres, "NORMAL");
}

tcapkclose()

{
}

/*
 * TCAPGETC:	Get on character.  Resolve and setup all the appropriate
 * keystroke escapes as defined in the comments at the beginning of input.c
 */

int tcapgetc()

{
  int c;			  /* current extended keystroke */

  /* if there are already keys waiting.... send them */
  if (in_check())
    return (in_get());

  /* otherwise... get the char for now */
  c = get1key();

  /* unfold the control bit back into the character */
  if (CTRL & c)
    c = (c & ~CTRL) - '@';

  /* fold the event type into the input stream as an escape seq */
  if ((c & ~255) != 0)
  {
    in_put(0);			  /* keyboard escape prefix */
    in_put(c >> 8);		  /* event type */
    in_put(c & 255);		  /* event code */
    return (tcapgetc());
  }

  return (c);
}

/*
 * GET1KEY:	Get one keystroke. The only prefixs legal here are the SPEC
 * and CTRL prefixes.
 * 
 * Note:
 * 
 * Escape sequences that are generated by terminal function and cursor keys
 * could be confused with the user typing the default META prefix followed by
 * other chars... ie
 * 
 * UPARROW  =  <ESC>A   on some terminals... apropos  =  M-A
 * 
 * The difference is determined by measuring the time between the input of the
 * first and second character... if an <ESC> is types, and is not followed by
 * another char in 1/30 of a second (think 300 baud) then it is a user input,
 * otherwise it was generated by an escape sequence and should be SPECed.
 */

int get1key()

{
  register int c;
  register int index;		  /* index into termcap binding table */
  char *sp;
  int fdset;
  struct timeval timeout;
  char cseq[10];		  /* current sequence being parsed */

  c = ttgetc();

  /* if it is not an escape character */
  if (c != 27)
    return (c);

  /* process a possible escape sequence */
  /* set up to check the keyboard for input */
  fdset = 1;
  timeout.tv_sec = 0;
  timeout.tv_usec = 35000L;

  /* check to see if things are pending soon */
  if (kbdmode != PLAY &&
      select(1, &fdset, (int *) NULL, (int *) NULL, &timeout) == 0)
    return (CTRL | '[');

  /* a key is pending within 1/30 of a sec... its an escape sequence */
  cseq[0] = 27;
  sp = &cseq[1];
  while (sp < &cseq[6])
  {
    c = ttgetc();
    *sp++ = c;
    *sp = 0;
    for (index = 0; index < NTBINDS; index++)
    {
      if (strcmp(cseq, ttable[index].p_seq) == 0)
	return (ttable[index].p_code);
    }
  }
  return (SPEC | 0);
}

tcapmove(row, col)
register int row, col;
{
  putpad(tgoto(CM, col, row + 1));
}

tcapeeol()
{
  putpad(CE);
}

tcapeeop()
{
  putpad(CL);
  tcapmove(0, 0);
}

tcaprev(state)			  /* change reverse video status */

int state;			  /* FALSE = normal video, TRUE = reverse
				   * video */

{
  /* static int revstate = FALSE; */

  if (state)
  {
    if (SO != (char *) NULL)
      putpad(SO);
  }
  else if (SE != (char *) NULL)
    putpad(SE);
}

tcapcres()			  /* change screen resolution */

{
  return (TRUE);
}

tcapbeep()
{
  ttputc(BEL);
}

putpad(str)
char *str;
{
  tputs(str, 1, ttputc);
}
