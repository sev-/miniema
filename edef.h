/*
 * $Id: edef.h,v 1.3 1994/08/15 21:27:30 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: edef.h,v $
 * Revision 1.3  1994/08/15 21:27:30  sev
 * i'm sorry, but this indent IMHO more better ;-)
 * Revision 1.2  1994/08/15  20:42:11  sev Indented Revision
 * 1.1  1994/06/24  14:17:12  sev Initial revision
 * 
 * 
 */

/*
 * EDEF:		Global variable definitions for MicroEMACS 3.9
 * 
 * written by Daniel Lawrence based on code by Dave G. Conroy, Steve Wilhite and
 * George Jones
 */

#ifdef	maindef

/* for MAIN.C */

/* initialized global definitions */

short kbdm[NKBDM];		  /* Macro			 */
int eolexist = TRUE;		  /* does clear to EOL exist?	 */
int revexist = FALSE;		  /* does reverse video exist?	 */
CONST char *modename[] =
{				  /* name of modes		 */
  "WRAP", "CMODE", "SPELL", "EXACT", "VIEW", "OVER",
  "MAGIC", "CRYPT", "ASAVE"};
int gmode = 0;			  /* global editor mode	   */
int gflags = GFREAD;		  /* global control flag	      */
int gasave = 256;		  /* global ASAVE size		 */
int gacount = 256;		  /* count until next ASAVE	 */
int sgarbf = TRUE;		  /* TRUE if screen is garbage	 */
int mpresf = FALSE;		  /* TRUE if message in last line */
int discmd = TRUE;		  /* display command flag 	 */
int modeflag = TRUE;		  /* display modelines flag    */
int sscroll = FALSE;		  /* smooth scrolling enabled flag */
int hscroll = TRUE;		  /* horizontal scrolling flag	 */
int hjump = 1;			  /* horizontal jump size 	 */
int ssave = TRUE;		  /* safe save flag		 */
int vtrow = 0;			  /* Row location of SW cursor	 */
int vtcol = 0;			  /* Column location of SW cursor */
int ttrow = HUGE;		  /* Row location of HW cursor	 */
int ttcol = HUGE;		  /* Column location of HW cursor */
int lbound = 0;			  /* leftmost column of current line being
				   * displayed		 */
int taboff = 0;			  /* tab offset for display	 */
int tabsize = 8;		  /* current hard tab size	 */
int stabsize = 0;		  /* current soft tab size (0: use hard tabs)  */
int reptc = CTRL | 'U';		  /* current universal repeat char */
int abortc = CTRL | 'G';	  /* current abort command char	 */
int quotec = 0x11;		  /* quote char during getstring() */
int sterm = CTRL | '[';		  /* search terminating character	 */

int prefix = 0;			  /* currently pending prefix bits */
int prenum = 0;			  /* "       "     numeric arg */
int predef = TRUE;		  /* "       "     default flag */

KILL *kbufp = (KILL *) NULL;	  /* current kill buffer chunk pointer */
KILL *kbufh = (KILL *) NULL;	  /* kill buffer header pointer	 */
int kused = KBLOCK;		  /* # of bytes used in kill buffer */
short *kbdptr;			  /* current position in keyboard buf */
short *kbdend = &kbdm[0];	  /* ptr to end of the keyboard */
int kbdmode = STOP;		  /* current keyboard macro mode	 */
int kbdrep = 0;			  /* number of repetitions	 */
int restflag = FALSE;		  /* restricted use?		 */
int lastkey = 0;		  /* last keystoke		 */
int seed = 0;			  /* random number seed		 */
char lastmesg[NSTRING] = "";	  /* last message posted		 */
char *lastptr = (char *) NULL;	  /* ptr to lastmesg[]		 */
char *fline = (char *) NULL;	  /* dynamic return line		 */
int flen = 0;			  /* current length of fline	 */
int eexitflag = FALSE;		  /* EMACS exit flag		 */
int eexitval = 0;		  /* and the exit return value	 */
int xpos = 0;			  /* current column mouse is positioned to */
int ypos = 0;			  /* current screen row	     		 */

/* uninitialized global definitions */

int currow;			  /* Cursor row			 */
int curcol;			  /* Cursor column		 */
int thisflag;			  /* Flags, this command		 */
int lastflag;			  /* Flags, last command		 */
int curgoal;			  /* Goal for C-P, C-N		 */
WINDOW *curwp;			  /* Current window		 */
BUFFER *curbp;			  /* Current buffer		 */
WINDOW *wheadp;			  /* Head of list of windows	 */
BUFFER *bheadp;			  /* Head of list of buf	 */
BUFFER *blistp;			  /* Buffer for C-X C-B	 */

char sres[NBUFN];		  /* current screen resolution	 */

char lowcase[HICHAR];		  /* lower casing map		 */
char upcase[HICHAR];		  /* upper casing map		 */

char pat[NPAT];			  /* Search pattern		 */
char tap[NPAT];			  /* Reversed pattern array.	 */
char rpat[NPAT];		  /* replacement pattern		 */

/* Various "Hook" execution variables	 */

KEYTAB readhook;		  /* executed on all file reads */
KEYTAB wraphook;		  /* executed when wrapping text */
KEYTAB cmdhook;			  /* executed before looking for a command */
KEYTAB writehook;		  /* executed on all file writes */
KEYTAB exbhook;			  /* executed when exiting a buffer */
KEYTAB bufhook;			  /* executed when entering a buffer */

/*
 * The variable matchlen holds the length of the matched string - used by the
 * replace functions. The variable patmatch holds the string that satisfies
 * the search command. The variables matchline and matchoff hold the line and
 * offset position of the *start* of match.
 */

unsigned int matchlen = 0;
unsigned int mlenold = 0;
char *patmatch = (char *) NULL;
LINE *matchline = (LINE *) NULL;
int matchoff = 0;

#else

/* for all the other .C files */

/* initialized global external declarations */

extern short kbdm[];		  /* Holds kayboard macro data	 */
extern int eolexist;		  /* does clear to EOL exist?	 */
extern int revexist;		  /* does reverse video exist?	 */
CONST extern char *modename[];	  /* text names of modes		 */
extern KEYTAB keytab[];		  /* key bind to functions table	 */
extern int gmode;		  /* global editor mode		 */
extern int gflags;		  /* global control flag		 */
extern int gasave;		  /* global ASAVE size		 */
extern int gacount;		  /* count until next ASAVE	 */
extern int sgarbf;		  /* State of screen unknown	 */
extern int mpresf;		  /* Stuff in message line	 */
extern int discmd;		  /* display command flag 	 */
extern int modeflag;		  /* display modelines flag	 */
extern int sscroll;		  /* smooth scrolling enabled flag */
extern int hscroll;		  /* horizontal scrolling flag	 */
extern int hjump;		  /* horizontal jump size 	 */
extern int ssave;		  /* safe save flag		 */
extern int vtrow;		  /* Row location of SW cursor	 */
extern int vtcol;		  /* Column location of SW cursor */
extern int ttrow;		  /* Row location of HW cursor	 */
extern int ttcol;		  /* Column location of HW cursor */
extern int lbound;		  /* leftmost column of current line being
				   * displayed		 */
extern int taboff;		  /* tab offset for display	 */
extern int tabsize;		  /* current hard tab size	 */
extern int stabsize;		  /* current soft tab size (0: use hard tabs)  */
extern int reptc;		  /* current universal repeat char */
extern int abortc;		  /* current abort command char	 */
extern int quotec;		  /* quote char during getstring() */
extern int sterm;		  /* search terminating character	 */

extern int prefix;		  /* currently pending prefix bits */
extern int prenum;		  /* "       "     numeric arg */
extern int predef;		  /* "       "     default flag */

extern KILL *kbufp;		  /* current kill buffer chunk pointer */
extern KILL *kbufh;		  /* kill buffer header pointer	 */
extern int kused;		  /* # of bytes used in kill buffer */
extern short *kbdptr;		  /* current position in keyboard buf */
extern short *kbdend;		  /* ptr to end of the keyboard */
extern int kbdmode;		  /* current keyboard macro mode	 */
extern int kbdrep;		  /* number of repetitions	 */
extern int restflag;		  /* restricted use?		 */
extern int lastkey;		  /* last keystoke		 */
extern int seed;		  /* random number seed		 */
extern char lastmesg[];		  /* last message posted		 */
extern char *lastptr;		  /* ptr to lastmesg[]		 */
extern char *fline;		  /* dynamic return line */
extern int flen;		  /* current length of fline */
extern int eexitflag;		  /* EMACS exit flag */
extern int eexitval;		  /* and the exit return value */
extern int xpos;		  /* current column mouse is positioned to */
extern int ypos;		  /* current screen row	     "	 */

/* uninitialized global external declarations */

extern int currow;		  /* Cursor row			 */
extern int curcol;		  /* Cursor column		 */
extern int thisflag;		  /* Flags, this command		 */
extern int lastflag;		  /* Flags, last command		 */
extern int curgoal;		  /* Goal for C-P, C-N		 */
extern WINDOW *curwp;		  /* Current window		 */
extern BUFFER *curbp;		  /* Current buffer		 */
extern WINDOW *wheadp;		  /* Head of list of windows	 */
extern BUFFER *bheadp;		  /* Head of list of buffers	 */
extern BUFFER *blistp;		  /* Buffer for C-X C-B		 */

extern char sres[NBUFN];	  /* current screen resolution	 */

extern char lowcase[HICHAR];	  /* lower casing map		 */
extern char upcase[HICHAR];	  /* upper casing map		 */

extern char pat[];		  /* Search pattern		 */
extern char tap[];		  /* Reversed pattern array.	 */
extern char rpat[];		  /* replacement pattern		 */

/* Various "Hook" execution variables	 */

extern KEYTAB readhook;		  /* executed on all file reads */
extern KEYTAB wraphook;		  /* executed when wrapping text */
extern KEYTAB cmdhook;		  /* executed before looking for a cmd */
extern KEYTAB writehook;	  /* executed on all file writes */
extern KEYTAB exbhook;		  /* executed when exiting a buffer */
extern KEYTAB bufhook;		  /* executed when entering a buffer */

extern unsigned int matchlen;
extern unsigned int mlenold;
extern char *patmatch;
extern LINE *matchline;
extern int matchoff;

#endif

/* terminal table defined only in TERM.C */

#ifndef termdef
extern TERM term;		  /* Terminal information.	 */

#endif
