/*
 *  $Id: edef.h,v 1.1 1994/06/24 14:17:12 sev Exp $
 *
 * ---------------------------------------------------------- 
 *
 * $Log: edef.h,v $
 * Revision 1.1  1994/06/24 14:17:12  sev
 * Initial revision
 *
 *
 */
 
/*	EDEF:		Global variable definitions for
			MicroEMACS 3.9

			written by Daniel Lawrence
			based on code by Dave G. Conroy,
				Steve Wilhite and George Jones
*/

#ifdef	maindef

/* for MAIN.C */

/* initialized global definitions */

NOSHARE short kbdm[NKBDM];		/* Macro			*/
NOSHARE int DNEAR eolexist = TRUE;	/* does clear to EOL exist?	*/
NOSHARE int DNEAR revexist = FALSE;	/* does reverse video exist?	*/
CONST char *modename[] = {		/* name of modes		*/
	"WRAP", "CMODE", "SPELL", "EXACT", "VIEW", "OVER",
	"MAGIC", "CRYPT", "ASAVE" };
NOSHARE int DNEAR gmode = 0;	   /* global editor mode	   */
NOSHARE int DNEAR gflags = GFREAD;	/* global control flag	      */
NOSHARE int DNEAR gasave = 256; 	/* global ASAVE size		*/
NOSHARE int DNEAR gacount = 256;	/* count until next ASAVE	*/
NOSHARE int DNEAR sgarbf = TRUE;	/* TRUE if screen is garbage	*/
NOSHARE int DNEAR mpresf = FALSE;	/* TRUE if message in last line */
NOSHARE int DNEAR discmd = TRUE;	/* display command flag 	*/
NOSHARE int DNEAR modeflag = TRUE;	/* display modelines flag    */
NOSHARE int DNEAR sscroll = FALSE;	/* smooth scrolling enabled flag*/
NOSHARE int DNEAR hscroll = TRUE;	/* horizontal scrolling flag	*/
NOSHARE int DNEAR hjump = 1;		/* horizontal jump size 	*/
NOSHARE int DNEAR ssave = TRUE; 	/* safe save flag		*/
NOSHARE int DNEAR vtrow = 0;		/* Row location of SW cursor	*/
NOSHARE int DNEAR vtcol = 0;		/* Column location of SW cursor */
NOSHARE int DNEAR ttrow = HUGE; 	/* Row location of HW cursor	*/
NOSHARE int DNEAR ttcol = HUGE; 	/* Column location of HW cursor */
NOSHARE int DNEAR lbound = 0;		/* leftmost column of current line
					   being displayed		*/
NOSHARE int DNEAR taboff = 0;		/* tab offset for display	*/
NOSHARE int DNEAR tabsize = 8;		/* current hard tab size	*/
NOSHARE int DNEAR stabsize = 0;		/* current soft tab size (0: use hard tabs)  */
NOSHARE int DNEAR reptc = CTRL | 'U';	/* current universal repeat char*/
NOSHARE int DNEAR abortc = CTRL | 'G';	/* current abort command char	*/
NOSHARE int DNEAR quotec = 0x11;	/* quote char during getstring() */
NOSHARE int DNEAR sterm = CTRL | '[';	/* search terminating character	*/

NOSHARE int DNEAR prefix = 0;		/* currently pending prefix bits */
NOSHARE int DNEAR prenum = 0;		/*     "       "     numeric arg */
NOSHARE int DNEAR predef = TRUE;	/*     "       "     default flag */

NOSHARE KILL *kbufp = (KILL *)NULL;	/* current kill buffer chunk pointer*/
NOSHARE KILL *kbufh = (KILL *)NULL;	/* kill buffer header pointer	*/
NOSHARE int kused = KBLOCK;		/* # of bytes used in kill buffer*/
NOSHARE short *kbdptr;			/* current position in keyboard buf */
NOSHARE short *kbdend = &kbdm[0];	/* ptr to end of the keyboard */
NOSHARE int DNEAR kbdmode = STOP;	/* current keyboard macro mode	*/
NOSHARE int DNEAR kbdrep = 0;		/* number of repetitions	*/
NOSHARE int DNEAR restflag = FALSE;	/* restricted use?		*/
NOSHARE int DNEAR lastkey = 0;		/* last keystoke		*/
NOSHARE int DNEAR seed = 0;		/* random number seed		*/
NOSHARE char lastmesg[NSTRING] = "";	/* last message posted		*/
NOSHARE char *lastptr = (char *)NULL;	/* ptr to lastmesg[]		*/
NOSHARE char *fline = (char *)NULL;	/* dynamic return line		*/
NOSHARE int DNEAR flen = 0;		/* current length of fline	*/
NOSHARE int DNEAR eexitflag = FALSE;	/* EMACS exit flag		*/
NOSHARE int DNEAR eexitval = 0; 	/* and the exit return value	*/
NOSHARE int xpos = 0;		/* current column mouse is positioned to*/
NOSHARE int ypos = 0;		/* current screen row	     		*/

/* uninitialized global definitions */

NOSHARE int DNEAR currow;	/* Cursor row			*/
NOSHARE int DNEAR curcol;	/* Cursor column		*/
NOSHARE int DNEAR thisflag;	/* Flags, this command		*/
NOSHARE int DNEAR lastflag;	/* Flags, last command		*/
NOSHARE int DNEAR curgoal;	/* Goal for C-P, C-N		*/
NOSHARE WINDOW *curwp;		/* Current window		*/
NOSHARE BUFFER *curbp;		/* Current buffer		*/
NOSHARE WINDOW *wheadp;		/* Head of list of windows	*/
NOSHARE BUFFER *bheadp;		/* Head of list of buf	*/
NOSHARE BUFFER *blistp;		/* Buffer for C-X C-B	 */

NOSHARE char sres[NBUFN];	/* current screen resolution	*/

NOSHARE char lowcase[HICHAR];	/* lower casing map		*/
NOSHARE	char upcase[HICHAR];	/* upper casing map		*/

NOSHARE char pat[NPAT]; 	/* Search pattern		*/
NOSHARE char tap[NPAT];		/* Reversed pattern array.	*/
NOSHARE char rpat[NPAT];	/* replacement pattern		*/

/*	Various "Hook" execution variables	*/

NOSHARE KEYTAB readhook;	/* executed on all file reads */
NOSHARE KEYTAB wraphook;	/* executed when wrapping text */
NOSHARE KEYTAB cmdhook;		/* executed before looking for a command */
NOSHARE KEYTAB writehook;	/* executed on all file writes */
NOSHARE KEYTAB exbhook;		/* executed when exiting a buffer */
NOSHARE KEYTAB bufhook;		/* executed when entering a buffer */

/*	The variable matchlen holds the length of the matched string -
	used by the replace functions. The variable patmatch holds the
	string that satisfies the search command. The variables matchline
	and matchoff hold the line and offset position of the *start* of
	match.
*/

NOSHARE unsigned int DNEAR matchlen = 0;
NOSHARE unsigned int DNEAR mlenold  = 0;
NOSHARE char *patmatch = (char *)NULL;
NOSHARE LINE *matchline = (LINE *)NULL;
NOSHARE int DNEAR matchoff = 0;

#else

/* for all the other .C files */

/* initialized global external declarations */

NOSHARE extern short kbdm[];		/* Holds kayboard macro data	*/
NOSHARE extern int DNEAR eolexist;	/* does clear to EOL exist?	*/
NOSHARE extern int DNEAR revexist;	/* does reverse video exist?	*/
CONST extern char *modename[];		/* text names of modes		*/
NOSHARE extern KEYTAB keytab[];		/* key bind to functions table	*/
NOSHARE extern int DNEAR gmode;		/* global editor mode		*/
NOSHARE extern int DNEAR gflags;	/* global control flag		*/
NOSHARE extern int DNEAR gasave;	/* global ASAVE size		*/
NOSHARE extern int DNEAR gacount;	/* count until next ASAVE	*/
NOSHARE extern int DNEAR sgarbf;	/* State of screen unknown	*/
NOSHARE extern int DNEAR mpresf;	/* Stuff in message line	*/
NOSHARE extern int DNEAR discmd;	/* display command flag 	*/
NOSHARE extern int DNEAR modeflag;	/* display modelines flag	*/
NOSHARE extern int DNEAR sscroll;	/* smooth scrolling enabled flag*/
NOSHARE extern int DNEAR hscroll;	/* horizontal scrolling flag	*/
NOSHARE extern int DNEAR hjump;		/* horizontal jump size 	*/
NOSHARE extern int DNEAR ssave;		/* safe save flag		*/
NOSHARE extern int DNEAR vtrow;		/* Row location of SW cursor	*/
NOSHARE extern int DNEAR vtcol;		/* Column location of SW cursor */
NOSHARE extern int DNEAR ttrow;		/* Row location of HW cursor	*/
NOSHARE extern int DNEAR ttcol;		/* Column location of HW cursor */
NOSHARE extern int DNEAR lbound;	/* leftmost column of current line
					   being displayed		*/
NOSHARE extern int DNEAR taboff;	/* tab offset for display	*/
NOSHARE extern int DNEAR tabsize;	/* current hard tab size	*/
NOSHARE extern int DNEAR stabsize;	/* current soft tab size (0: use hard tabs)  */
NOSHARE extern int DNEAR reptc;		/* current universal repeat char*/
NOSHARE extern int DNEAR abortc;	/* current abort command char	*/
NOSHARE extern int DNEAR quotec;	/* quote char during getstring() */
NOSHARE extern int DNEAR sterm;		/* search terminating character	*/

NOSHARE extern int DNEAR prefix;	/* currently pending prefix bits */
NOSHARE extern int DNEAR prenum;	/*     "       "     numeric arg */
NOSHARE extern int DNEAR predef;	/*     "       "     default flag */

NOSHARE extern KILL *kbufp;		/* current kill buffer chunk pointer */
NOSHARE extern KILL *kbufh;		/* kill buffer header pointer	*/
NOSHARE extern int kused;		/* # of bytes used in kill buffer*/
NOSHARE extern short *kbdptr;		/* current position in keyboard buf */
NOSHARE extern short *kbdend;		/* ptr to end of the keyboard */
NOSHARE extern int kbdmode;		/* current keyboard macro mode	*/
NOSHARE extern int kbdrep;		/* number of repetitions	*/
NOSHARE extern int restflag;		/* restricted use?		*/
NOSHARE extern int lastkey;		/* last keystoke		*/
NOSHARE extern int seed;		/* random number seed		*/
NOSHARE extern char lastmesg[];		/* last message posted		*/
NOSHARE extern char *lastptr;		/* ptr to lastmesg[]		*/
NOSHARE extern char *fline;		/* dynamic return line */
NOSHARE extern int DNEAR flen;		/* current length of fline */
NOSHARE extern int DNEAR eexitflag;	/* EMACS exit flag */
NOSHARE extern int DNEAR eexitval;	/* and the exit return value */
NOSHARE extern int xpos;		/* current column mouse is positioned to */
NOSHARE extern int ypos;		/* current screen row	     "	 */

/* uninitialized global external declarations */

NOSHARE extern int DNEAR currow;	/* Cursor row			*/
NOSHARE extern int DNEAR curcol;	/* Cursor column		*/
NOSHARE extern int DNEAR thisflag;	/* Flags, this command		*/
NOSHARE extern int DNEAR lastflag;	/* Flags, last command		*/
NOSHARE extern int DNEAR curgoal;	/* Goal for C-P, C-N		*/
NOSHARE extern WINDOW *curwp;		/* Current window		*/
NOSHARE extern BUFFER *curbp;		/* Current buffer		*/
NOSHARE extern WINDOW *wheadp;		/* Head of list of windows	*/
NOSHARE extern BUFFER *bheadp;		/* Head of list of buffers	*/
NOSHARE extern BUFFER *blistp;		/* Buffer for C-X C-B		*/

NOSHARE extern char sres[NBUFN];	/* current screen resolution	*/

NOSHARE extern char lowcase[HICHAR];	/* lower casing map		*/
NOSHARE	extern char upcase[HICHAR];	/* upper casing map		*/

NOSHARE extern char pat[];		/* Search pattern		*/
NOSHARE extern char tap[];		/* Reversed pattern array.	*/
NOSHARE extern char rpat[];		/* replacement pattern		*/

/*	Various "Hook" execution variables	*/

NOSHARE extern KEYTAB readhook; 	/* executed on all file reads */
NOSHARE extern KEYTAB wraphook;		/* executed when wrapping text */
NOSHARE extern KEYTAB cmdhook;		/* executed before looking for a cmd */
NOSHARE extern KEYTAB writehook;	/* executed on all file writes */
NOSHARE extern KEYTAB exbhook;		/* executed when exiting a buffer */
NOSHARE extern KEYTAB bufhook;		/* executed when entering a buffer */

NOSHARE extern unsigned int matchlen;
NOSHARE extern unsigned int mlenold;
NOSHARE extern char *patmatch;
NOSHARE extern LINE *matchline;
NOSHARE extern int matchoff;

#endif

/* terminal table defined only in TERM.C */

#ifndef termdef
NOSHARE extern TERM	term;		/* Terminal information.	*/
#endif
