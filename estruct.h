/*
 *  $Id: estruct.h,v 1.2 1994/06/24 17:22:21 sev Exp $
 *
 * ---------------------------------------------------------- 
 *
 * $Log: estruct.h,v $
 * Revision 1.2  1994/06/24 17:22:21  sev
 * Added ttputs into tcap structure
 *
 * Revision 1.1  1994/06/24  14:17:12  sev
 * Initial revision
 *
 *
 */
 
/*	ESTRUCT:	Structure and preprocesser defined for
			MicroEMACS 3.10

			written by Daniel Lawrence
			based on code by Dave G. Conroy,
				Steve Wilhite and George Jones
*/


/*	Program Identification.....

	PROGNAME should always be MicroEMACS for a distribution
	unmodified version. People using MicroEMACS as a shell
	for other products should change this to reflect their
	product. Macros can query this via the $progname variable
*/

#define PROGNAME	"MicroEMACS"
#define	VERSION		"3.10"


/*	Configuration options	*/

#define TYPEAH	1	/* type ahead causes update to be skipped	*/
#define	CLEAN	0	/* de-alloc memory on exit			*/
#define	CALLED	0	/* is emacs a called subroutine? or stand alone */

/* handle constant and voids properly */

#define CONST
#define NOSHARE

/*	System dependant library redefinitions, structures and includes */

/*	the following define allows me to initialize unions...
	otherwise we make them structures (like the keybinding table)  */

#define ETYPE	struct

#define NEAR
#define	DNEAR
#define	PASCAL
#define	CDECL

/*	Emacs global flag bit definitions (for gflags)	*/

#define GFREAD	1

/*	internal constants	*/

#define NBINDS	300			/* max # of bound keys		*/
#define	NFILEN	256
#define NBUFN	32			  /* # of bytes, buffer name	  */
#define NLINE	256			/* # of bytes, input line	*/
#define	NSTRING	128			/* # of bytes, string buffers	*/
#define NKBDM	256			/* # of strokes, keyboard macro */
#define NPAT	128			/* # of bytes, pattern		*/
#define HUGE	1000			/* Huge number			*/
#define	KBLOCK	250			/* sizeof kill buffer chunks	*/
#define	NBLOCK	16			/* line block chunk size	*/
#define NMARKS	10			/* number of marks		*/

#define CTRL	0x0100		/* Control flag, or'ed in		*/
#define META	0x0200		/* Meta flag, or'ed in			*/
#define CTLX	0x0400		/* ^X flag, or'ed in			*/
#define SPEC	0x0800		/* special key (function keys)		*/
#define MOUS	0x1000		/* alternative input device (mouse)	*/
#define	SHFT	0x2000		/* shifted (for function keys)		*/
#define	ALTD	0x4000		/* ALT key...				*/

#define BINDNUL 0		/* not bount to anything		*/
#define	BINDFNC	1		/* key bound to a function		*/

#ifdef	FALSE
#undef	FALSE
#endif
#ifdef	TRUE
#undef	TRUE
#endif

#define FALSE	0			/* False, no, bad, etc. 	*/
#define TRUE	1			/* True, yes, good, etc.	*/
#define ABORT	2			/* Death, ^G, abort, etc.	*/

#define STOP	0			/* keyboard macro not in use	*/
#define	PLAY	1			/*		  playing	*/
#define	RECORD	2			/*		  recording	*/

/*
 * PTBEG, PTEND, FORWARD, and REVERSE are all toggle-able values for
 * the scan routines.
 */
#define PTBEG	0	/* Leave the point at the beginning on search	*/
#define	PTEND	1	/* Leave the point at the end on search		*/
#define	FORWARD	0			/* forward direction		*/
#define REVERSE	1			/* backwards direction		*/

#define FIOSUC	0			/* File I/O, success.		*/
#define FIOFNF	1			/* File I/O, file not found.	*/
#define FIOEOF	2			/* File I/O, end of file.	*/
#define FIOERR	3			/* File I/O, error.		*/
#define	FIOMEM	4			/* File I/O, out of memory	*/
#define	FIODEL	6			/* Can't delete/rename file	*/

#define CFCPCN	0x0001			/* Last command was C-P, C-N	*/
#define CFKILL	0x0002			/* Last command was a kill	*/

#define BELL	0x07			/* a bell character		*/

#define INTWIDTH	sizeof(int) * 3

/* DIFCASE represents the integer difference between upper
   and lower case letters.  It is an xor-able value, which is
   fortunate, since the relative positions of upper to lower
   case letters is the opposite of ascii in ebcdic.
*/

#ifdef	islower
#undef	islower
#endif
#ifdef	isupper
#undef	isupper
#endif

#define DIFCASE 	0x20
#define	CHCASE(c)	chcase(c)     /* Toggle extended letter case.*/

/*
 * There is a window structure allocated for every active display window. The
 * windows are kept in a big list, in top to bottom screen order, with the
 * listhead at "wheadp". Each window contains its own values of dot and mark.
 * The flag field contains some bits that are set by commands to guide
 * redisplay. Although this is a bit of a compromise in terms of decoupling,
 * the full blown redisplay is just too expensive to run for every input
 * character.
 */
typedef struct	WINDOW {
	struct	WINDOW *w_wndp; 	/* Next window			*/
	struct	BUFFER *w_bufp; 	/* Buffer displayed in window	*/
	struct	LINE *w_linep;		/* Top line in the window	*/
	struct	LINE *w_dotp;		/* Line containing "."		*/
	short	w_doto; 		/* Byte offset for "."		*/
	struct	LINE *w_markp[NMARKS];	/* Line containing "mark"	*/
	short	w_marko[NMARKS];	/* Byte offset for "mark"	*/
	char	w_toprow;		/* Origin 0 top row of window	*/
	char	w_ntrows;		/* # of rows of text in window	*/
	char	w_force;		/* If NZ, forcing row.		*/
	char	w_flag; 		/* Flags.			*/
	int	w_fcol; 		/* first column displayed	*/
}	WINDOW;

#define WFFORCE 0x01			/* Window needs forced reframe	*/
#define WFMOVE	0x02			/* Movement from line to line	*/
#define WFEDIT	0x04			/* Editing within a line	*/
#define WFHARD	0x08			/* Better to a full display	*/
#define WFMODE	0x10			/* Update mode line.		*/

/*
 * Text is kept in buffers. A buffer header, described below, exists for every
 * buffer in the system. The buffers are kept in a big list, so that commands
 * that search for a buffer by name can find the buffer header. There is a
 * safe store for the dot and mark in the header, but this is only valid if
 * the buffer is not being displayed (that is, if "b_nwnd" is 0). The text for
 * the buffer is kept in a circularly linked list of lines, with a pointer to
 * the header line in "b_linep"	Buffers may be "Inactive" which means the files associated with them
 * have not been read in yet. These get read in at "use buffer" time.
 */
typedef struct	BUFFER {
	struct	BUFFER *b_bufp; 	/* Link to next BUFFER		*/
	struct	LINE *b_dotp;		/* Link to "." LINE structure	*/
	short	b_doto; 		/* Offset of "." in above LINE	*/
	struct	LINE *b_markp[NMARKS];	/* The same as the above two,	*/
	short	b_marko[NMARKS];	/* but for the "mark"		*/
	int	b_fcol;			/* first col to display		*/
	struct	LINE *b_linep;		/* Link to the header LINE	*/
	struct	LINE *b_topline;	/* Link to narrowed top text	*/
	struct	LINE *b_botline;	/* Link to narrowed bottom text	*/
	char	b_active;		/* window activated flag	*/
	char	b_nwnd; 		/* Count of windows on buffer	*/
	char	b_flag; 		/* Flags			*/
	int	b_mode;			/* editor mode of this buffer	*/
	char	b_fname[NFILEN];	/* File name			*/
	char	b_bname[NBUFN]; 	/* Buffer name			*/
}	BUFFER;

#define BFINVS	0x01			/* Internal invisable buffer	*/
#define BFCHG	0x02			/* Changed since last write	*/
#define	BFTRUNC	0x04			/* buffer was truncated when read */
#define	BFNAROW	0x08			/* buffer has been narrowed	*/

/*	mode flags	*/
#define	NUMMODES	9	      /* # of defined modes	      */

#define MDCMOD	0x0002			/* C indentation and fence match*/
#define	MDEXACT	0x0008			/* Exact matching for searches	*/
#define	MDVIEW	0x0010			/* read-only buffer		*/
#define MDOVER	0x0020			/* overwrite mode		*/
#define	MDASAVE	0x0100			/* auto-save mode		*/

/*
 * The starting position of a region, and the size of the region in
 * characters, is kept in a region structure.  Used by the region commands.
 */
typedef struct	{
	struct	LINE *r_linep;		/* Origin LINE address. 	*/
	short	r_offset;		/* Origin LINE offset.		*/
	long	r_size; 		/* Length in characters.	*/
}	REGION;

/*
 * All text is kept in circularly linked lists of "LINE" structures. These
 * begin at the header line (which is the blank line beyond the end of the
 * buffer). This line is pointed to by the "BUFFER". Each line contains a the
 * number of bytes in the line (the "used" size), the size of the text array,
 * and the text. The end of line is not stored as a byte; it's implied.
 */
typedef struct	LINE {
	struct	LINE *l_fp;		/* Link to the next line	*/
	struct	LINE *l_bp;		/* Link to the previous line	*/
	short	l_size; 		/* Allocated size		*/
	short	l_used; 		/* Used size			*/
	char	l_text[1];		/* A bunch of characters.	*/
}	LINE;

#define lforw(lp)	((lp)->l_fp)
#define lback(lp)	((lp)->l_bp)
#define lgetc(lp, n)	((lp)->l_text[(n)]&0xFF)
#define lputc(lp, n, c) ((lp)->l_text[(n)]=(c))
#define llength(lp)	((lp)->l_used)

/*
 * The editor communicates with the display using a high level interface. A
 * "TERM" structure holds useful variables, and indirect pointers to routines
 * that do useful operations. The low level get and put routines are here too.
 * This lets a terminal, in addition to having non standard commands, have
 * funny get and put character code too. The calls might get changed to
 * "termp->t_field" style in the future, to make it possible to run more than
 * one terminal type.
 */
typedef struct	{
	short	t_mrow; 	  /* max number of rows allowable */
	short	t_nrow; 		/* current number of rows used */
	short	t_mcol; 		/* max Number of columns.	*/
	short	t_ncol; 		/* current Number of columns.	*/
	short	t_margin;		/* min margin for extended lines*/
	short	t_scrsiz;		/* size of scroll region "	*/
	int	t_pause;		/* # times thru update to pause */
	int (PASCAL NEAR *t_open)();	/* Open terminal at the start.	*/
	int (PASCAL NEAR *t_close)();	/* Close terminal at end.	*/
	int (PASCAL NEAR *t_kopen)();	/* Open keyboard		*/
	int (PASCAL NEAR *t_kclose)();	/* close keyboard		*/
	int (PASCAL NEAR *t_getchar)(); /* Get character from keyboard. */
	int (PASCAL NEAR *t_putchar)(); /* Put character to display.	*/
	int (PASCAL NEAR *t_flush)();	/* Flush output buffers.	*/
	int (PASCAL NEAR *t_move)();	/* Move the cursor, origin 0.	*/
	int (PASCAL NEAR *t_eeol)();	/* Erase to end of line.	*/
	int (PASCAL NEAR *t_eeop)();	/* Erase to end of page.	*/
	int (PASCAL NEAR *t_beep)();	/* Beep.			*/
	int (PASCAL NEAR *t_rev)();	/* set reverse video state	*/
	int (PASCAL NEAR *t_rez)();	/* change screen resolution	*/
	int (PASCAL NEAR *t_puts)();	/* put string			*/
}	TERM;

/*	TEMPORARY macros for terminal I/O  (to be placed in a machine
					    dependant place later) */

#define TTopen		(*term.t_open)
#define	TTclose		(*term.t_close)
#define	TTkopen		(*term.t_kopen)
#define	TTkclose	(*term.t_kclose)
#define	TTgetc		(*term.t_getchar)
#define	TTputc		(*term.t_putchar)
#define	TTflush		(*term.t_flush)
#define	TTmove		(*term.t_move)
#define	TTeeol		(*term.t_eeol)
#define	TTbeep		(*term.t_beep)
#define	TTrev		(*term.t_rev)
#define	TTputs		(*term.t_puts)

/*	Structure for the table of current key bindings 	*/

ETYPE EPOINTER {
	int (PASCAL NEAR *fp)();       /* C routine to invoke */
	BUFFER *buf;			/* buffer to execute */
};

typedef struct	{
	short k_code;	       /* Key code		       */
	short k_type;		/* binding type (C function or EMACS buffer) */
	ETYPE EPOINTER k_ptr;	/* ptr to thing to execute */
}	KEYTAB;

/*	structure for the name binding table		*/

typedef struct {
	char *n_name;		       /* name of function key */
	int (PASCAL NEAR *n_func)();	/* function name is bound to */
}	NBIND;

/*	The editor holds deleted text chunks in the KILL buffer. The
	kill buffer is logically a stream of ascii characters, however
	due to its unpredicatable size, it gets implemented as a linked
	list of chunks. (The d_ prefix is for "deleted" text, as k_
	was taken up by the keycode structure)
*/

typedef struct KILL {
	struct KILL *d_next;   /* link to next chunk, NULL if last */
	char d_chunk[KBLOCK];	/* deleted text */
} KILL;

/*	When emacs' command interpetor needs to get a variable's name,
	rather than it's value, it is passed back as a VDESC variable
	description structure. The v_num field is a index into the
	appropriate variable table.
*/

typedef struct VDESC {
	int v_type;    /* type of variable */
	int v_num;	/* ordinal pointer to variable in list */
} VDESC;

/* HICHAR - 1 is the largest character we will deal with.
 */
#define HICHAR		256

/*
	This is the message which should be added to any "About MicroEMACS"
	boxes on any of the machines with window managers.


	------------------------------------------
	|					 |
	|	 MicroEMACS v3.xx		 |
	|		for the ............	 |
	|					 |
	|    Text Editor and Corrector		 |
	|					 |
	|    written by Daniel M. Lawrence	 |
	|    [based on code by Dave Conroy]	 |
	|					 |
	|    Send inquiries and donations to:	 |
	|    617 New York St			 |
	|    Lafayette, IN 47901		 |
	|					 |
	------------------------------------------
*/
