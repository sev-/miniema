/*
 *  $Id: main.c,v 1.1 1994/06/24 14:17:12 sev Exp $
 *
 * ---------------------------------------------------------- 
 *
 * $Log: main.c,v $
 * Revision 1.1  1994/06/24 14:17:12  sev
 * Initial revision
 *
 *
 */
 
/*
 *	MicroEMACS 3.10
 *		written by Daniel M. Lawrence
 *		based on code by Dave G. Conroy.
 *
 *	(C)opyright 1988,1989 by Daniel M. Lawrence
 *	MicroEMACS 3.10 can be copied and distributed freely for any
 *	non-commercial purposes. MicroEMACS 3.10 can only be incorporated
 *	into commercial software with the permission of the current author.
 *
 * This file contains the main driving routine, and some keyboard processing
 * code, for the MicroEMACS screen editor.
 *
 */

#include	<stdio.h>

/* make global definitions not external */
#define maindef

#include	"estruct.h"	/* global structures and defines */
#include	"etype.h"	/* variable prototype definitions */
#include	"edef.h"	/* global definitions */
#include	"english.h"	/* human language definitions */
#include	"ebind.h"	/* default key bindings */

/* for many different systems, increase the default stack space */

#ifndef GOOD
#define GOOD	0
#endif

/*
	This is the primary entry point that is used by command line
	invocation, and by applications that link with microemacs in
	such a way that each invocation of Emacs is a fresh environment.

	There is another entry point in VMS.C that is used when
	microemacs is "linked" (In quotes, because it is a run-time link
	rather than a link-time link.) with applications that wish Emacs
	to preserve it's context across invocations.  (For example,
	EMACS.RC is only executed once per invocation of the
	application, instead of once per invocation of Emacs.)

	Note that re-entering an Emacs that is saved in a kept
	subprocess would require a similar entrypoint.
*/

#if	CALLED
emacs(argc, argv)
#else
main(argc, argv)
#endif

int argc;	/* # of arguments */
char *argv[];	/* argument strings */

{
	register int status;

	/* Initialize the editor */
	eexitflag = FALSE;
	vtinit();	     /* Terminal */
	if (eexitflag)
		goto abortrun;
	edinit("main");        /* Buffers, windows */
	initchars();		/* character set definitions */

	/* Process the command line and let the user edit */
	dcline(argc, argv);
	status = editloop();
abortrun:
	vttidy();
#if	CLEAN
	clean();
#endif
#if	CALLED
	return(status);
#else
	exit(status);
#endif
}

#if	CLEAN
/*
	On some primitive operation systems, and when emacs is used as
	a subprogram to a larger project, emacs needs to de-alloc its
	own used memory, otherwise we just exit.
*/

PASCAL NEAR clean()
{
	register BUFFER *bp;	/* buffer list pointer */
	register WINDOW *wp;	/* window list pointer */
	register WINDOW *tp;	/* temporary window pointer */

	/* first clean up the windows */
	wp = wheadp;
	while (wp) {
		tp = wp->w_wndp;
		free(wp);
		wp = tp;
	}
	wheadp = NULL;

	/* then the buffers */
	bp = bheadp;
	while (bp) {
		bp->b_nwnd = 0;
		bp->b_flag = 0; /* don't say anything about a changed buffer! */
		zotbuf(bp);
		bp = bheadp;
	}

	/* and the kill buffer */
	kdelete();

	/* clear some search variables */
	if (patmatch != NULL) {
		free(patmatch);
		patmatch = NULL;
	}

	/* and the video buffers */
	vtfree();
}
#endif

/*	Process a command line.   May be called any time.	*/

PASCAL NEAR dcline(argc, argv)

int argc;
char *argv[];

{
	register BUFFER *bp;		/* temp buffer pointer */
	register int	firstfile;	/* first file flag */
	register int	carg;		/* current arg to scan */
	BUFFER *firstbp = (BUFFER *)NULL;/* ptr to first buffer in cmd line */
	int viewflag;			/* are we starting in view mode? */
	int gotoflag;			/* do we need to goto a line at start? */
	int searchflag; 		/* Do we need to search at start? */
	char bname[NBUFN];		/* buffer name of file to read */

	viewflag = FALSE;	/* view mode defaults off in command line */
	gotoflag = FALSE;	/* set to off to begin with */
	searchflag = FALSE;	/* set to off to begin with */
	firstfile = TRUE;	/* no file to edit yet */
	/* Parse a command line */
	for (carg = 1; carg < argc; ++carg) {

		/* Process Switches */
		if (argv[carg][0] == '-') {
			switch (argv[carg][1]) {
				/* Process Startup macroes */
				case 'c':	/* -c for changable file */
				case 'C':
					viewflag = FALSE;
					break;
				case 'r':   /* -r restrictive use */
				case 'R':
					restflag = TRUE;
					break;
				case 's':   /* -s for initial search string */
				case 'S':
					searchflag = TRUE;
					bytecopy(pat,&argv[carg][2],NPAT);
					setjtable(pat);
					break;
				case 'v':   /* -v for View File */
				case 'V':
					viewflag = TRUE;
					break;
				default:    /* unknown switch */
					/* ignore this for now */
					break;
			}

		} else if (argv[carg][0]== '@') {

		} else {

			/* Process an input file */

			/* set up a buffer for this file */
			makename(bname, argv[carg]);
			unqname(bname);

			/* set this to inactive */
			bp = bfind(bname, TRUE, 0);
			strcpy(bp->b_fname, argv[carg]);
			bp->b_active = FALSE;
			if (firstfile) {
				firstbp = bp;
				firstfile = FALSE;
			}

			/* set the modes appropriatly */
			if (viewflag)
				bp->b_mode |= MDVIEW;
		}
	}

	/* if there are any files to read, read the first one! */
	bp = bfind("main", FALSE, 0);
	if (firstfile == FALSE && (gflags & GFREAD)) {
		swbuffer(firstbp);
		curbp->b_mode |= gmode;
		update(TRUE);
		mlwrite(lastmesg);
		zotbuf(bp);
	} else
	       bp->b_mode |= gmode;

	/* Deal with startup gotos and searches */
	if (gotoflag && searchflag) {
		update(FALSE);
		mlwrite(TEXT101);
/*			"[Can not search and goto at the same time!]" */
	}
	else if (searchflag) {
		if (forwhunt(FALSE, 0) == FALSE)
			update(FALSE);
	}

}

/*
	This is called to let the user edit something.	Note that if you
	arrange to be able to call this from a macro, you will have
	invented the "recursive-edit" function.
*/

PASCAL NEAR editloop()

{
	register int c; 		/* command character */
	register int f; 	/* default flag */
	register int n; 	/* numeric repeat count */
	register int mflag;	/* negative flag on repeat */
	register int basec;	/* c stripped of meta character */
	register int oldflag;	/* old last flag value */

	/* setup to process commands */
	lastflag = 0;		/* Fake last flags.	*/

loop:
	/* if we were called as a subroutine and want to leave, do so */
	if (eexitflag)
		return(eexitval);

	/* execute the "command" macro...normally null */
	oldflag = lastflag;	/* preserve lastflag through this */
	execkey(&cmdhook, FALSE, 1);
	lastflag = oldflag;

	/* Fix up the screen	*/
	update(FALSE);

	/* get the next command from the keyboard */
	c = getkey();

	/* if there is something on the command line, clear it */
	if (mpresf != FALSE) {
		mlerase();
		update(FALSE);
	}

	/* override the arguments if prefixed */
	if (prefix) {
		if (islower(c & 255))
			c = (c & ~255) | upperc(c & 255);
		c |= prefix;
		f = predef;
		n = prenum;
		prefix = 0;
	} else {
		f = FALSE;
		n = 1;
	}

	/* do META-# processing if needed */

	basec = c & ~META;		/* strip meta char off if there */
	if ((c & META) && ((basec >= '0' && basec <= '9') || basec == '-') &&
	    (getbind(c) == (KEYTAB *)NULL)) {
		f = TRUE;	    /* there is a # arg */
		n = 0;			/* start with a zero default */
		mflag = 1;		/* current minus flag */
		c = basec;		/* strip the META */
		while ((c >= '0' && c <= '9') || (c == '-')) {
			if (c == '-') {
				/* already hit a minus or digit? */
				if ((mflag == -1) || (n != 0))
					break;
				mflag = -1;
			} else {
				n = n * 10 + (c - '0');
			}
			if ((n == 0) && (mflag == -1))	     /* lonely - */
				mlwrite("Arg:");
			else
			     mlwrite("Arg: %d",n * mflag);

			c = getkey();	/* get the next key */
		}
		n = n * mflag;	      /* figure in the sign */
	}

	/* do ^U repeat argument processing */

	if (c == reptc) {		   /* ^U, start argument   */
		f = TRUE;
		n = 4;			       /* with argument of 4 */
		mflag = 0;			/* that can be discarded. */
		mlwrite("Arg: 4");
		while ((c=getkey()) >='0' && c<='9' || c==reptc || c=='-') {
			if (c == reptc)
				if ((n > 0) == ((n*4) > 0))
					n = n*4;
				else
				    n = 1;
			/*
			 * If dash, and start of argument string, set arg.
			 * to -1.  Otherwise, insert it.
			 */
			else if (c == '-') {
				if (mflag)
					break;
				n = 0;
				mflag = -1;
			}
			/*
			 * If first digit entered, replace previous argument
			 * with digit and set sign.  Otherwise, append to arg.
			 */
			else {
			     if (!mflag) {
					n = 0;
					mflag = 1;
				}
				n = 10*n + c - '0';
			}
			mlwrite("Arg: %d", (mflag >=0) ? n : (n ? -n : -1));
		}
		/*
		 * Make arguments preceded by a minus sign negative and change
		 * the special argument "^U -" to an effective "^U -1".
		 */
		if (mflag == -1) {
			if (n == 0)
				n++;
			n = -n;
		}
	}

	/* and execute the command */
	execute(c, f, n);
	goto loop;
}

/*
 * Initialize all of the buffers and windows. The buffer name is passed down
 * as an argument, because the main routine may have been told to read in a
 * file by default, and we want the buffer name to be right.
 */

PASCAL NEAR edinit(bname)

char bname[];	/* name of buffer to initialize */

{
	register BUFFER *bp;
	register WINDOW *wp;
	int cmark;	    /* current mark */

	/* initialize some important globals */

	readhook.k_ptr.fp = nullproc;	/* set internal hooks to OFF */
	readhook.k_type = BINDFNC;
	wraphook.k_type = BINDFNC;
	cmdhook.k_ptr.fp = nullproc;
	cmdhook.k_type = BINDFNC;
	writehook.k_ptr.fp = nullproc;
	writehook.k_type = BINDFNC;
	bufhook.k_ptr.fp = nullproc;
	bufhook.k_type = BINDFNC;
	exbhook.k_ptr.fp = nullproc;
	exbhook.k_type = BINDFNC;

	bp = bfind(bname, TRUE, 0);		/* First buffer 	*/
	blistp = bfind("[List]", TRUE, BFINVS); /* Buffer list buffer	*/
	wp = (WINDOW *) malloc(sizeof(WINDOW)); /* First window 	*/
	if(bp==(BUFFER *)NULL || wp==(WINDOW *)NULL || blistp==(BUFFER *)NULL)
		meexit(1);
	curbp  = bp;			  /* Make this current	  */
	wheadp = wp;
	curwp  = wp;
	wp->w_wndp  = (WINDOW *)NULL;		/* Initialize window    */
	wp->w_bufp  = bp;
	bp->b_nwnd  = 1;			/* Displayed.	       */
	wp->w_linep = bp->b_linep;
	wp->w_dotp  = bp->b_linep;
	wp->w_doto  = 0;
	for (cmark = 0; cmark < NMARKS; cmark++) {
		wp->w_markp[cmark] = (LINE *)NULL;
		wp->w_marko[cmark] = 0;
	}
	wp->w_toprow = 0;
	wp->w_fcol = 0;
	wp->w_ntrows = term.t_nrow-1;	       /* "-1" for mode line.  */
	wp->w_force = 0;
	wp->w_flag  = WFMODE|WFHARD;	       /* Full. 	       */
}

/*
 * This is the general command execution routine. It handles the fake binding
 * of all the keys to "self-insert". It also clears out the "thisflag" word,
 * and arranges to move it to the "lastflag", so that the next command can
 * look at it. Return the status of command.
 */
PASCAL NEAR execute(c, f, n)

{
	register int status;
	KEYTAB *key;		  /* key entry to execute */

	/* if the keystroke is a bound function...do it */
	key = getbind(c);
	if (key != (KEYTAB *)NULL) {
		thisflag = 0;
		status = execkey(key, f, n);
		lastflag = thisflag;
		return(status);
	}

	/*
	 * If a space was typed, fill column is defined, the argument is non-
	 * negative, wrap mode is enabled, and we are now past fill column,
	 * and we are not read-only, perform word wrap.
	 */
	if ((c>=0x20 && c<=0xFF)) {    /* Self inserting.      */
		if (n <= 0) {			/* Fenceposts.	       */
			lastflag = 0;
			return(n<0 ? FALSE : TRUE);
		}
		thisflag = 0;		      /* For the future.      */

		/* if we are in overwrite mode, not at eol,
		   and next char is not a tab or we are at a tab stop,
		   delete a char forword			*/
		if (curwp->w_bufp->b_mode & MDOVER &&
		    curwp->w_doto < curwp->w_dotp->l_used &&
			(lgetc(curwp->w_dotp, curwp->w_doto) != '\t' ||
			 (curwp->w_doto) % tabsize == (tabsize - 1)))
				ldelete(1L, FALSE);

		status = linsert(n, c);


		/* check auto-save mode */
		if (curbp->b_mode & MDASAVE)
			if (--gacount == 0) {
				/* and save the file if needed */
				upscreen(FALSE, 0);
				filesave(FALSE, 0);
				gacount = gasave;
			}

		lastflag = thisflag;
		return(status);
	}
	TTbeep();
	mlwrite(TEXT19);	       /* complain	       */
/*		"[Key not bound]" */
	lastflag = 0;				/* Fake last flags.	*/
	return(FALSE);
}

/*
	Fancy quit command, as implemented by Norm. If the any buffer
has changed do a write on that buffer and exit emacs, otherwise simply
exit.
*/

PASCAL NEAR quickexit(f, n)

{
	register BUFFER *bp;	/* scanning pointer to buffers */
	register BUFFER *oldcb; /* original current buffer */
	register int status;

	oldcb = curbp;				/* save in case we fail */

	bp = bheadp;
	while (bp != (BUFFER *)NULL) {
		if ((bp->b_flag&BFCHG) != 0   /* Changed.	      */
		&& (bp->b_flag&BFINVS) == 0) {	/* Real.		*/
			curbp = bp;		/* make that buffer cur */
			mlwrite(TEXT103,bp->b_fname);
/*				"[Saving %s]" */
			mlwrite("\n");
			if ((status = filesave(f, n)) != TRUE) {
				curbp = oldcb;	/* restore curbp */
				return(status);
			}
		}
	bp = bp->b_bufp;		       /* on to the next buffer */
	}
	quit(f, n);			       /* conditionally quit   */
	return(TRUE);
}

/*
 * Quit command. If an argument, always quit. Otherwise confirm if a buffer
 * has been changed and not written out. Normally bound to "C-X C-C".
 */

PASCAL NEAR quit(f, n)

{
	register int status;	/* return status */

	if (f != FALSE		/* Argument forces it.	*/
	|| anycb() == FALSE	/* All buffers clean or user says it's OK. */
	|| (status = mlyesno(TEXT104)) == TRUE) {
/*			     "Modified buffers exist. Leave anyway" */
		if (f)
		      status = meexit(n);
		else
		      status = meexit(GOOD);
	}
	mlerase();
	return(status);
}

PASCAL NEAR meexit(status)
int status;	/* return status of emacs */
{
	eexitflag = TRUE;	/* flag a program exit */
	eexitval = status;

	/* and now.. we leave and let the main loop kill us */
	return(TRUE);
}

/*
 * Begin a keyboard macro.
 * Error if not at the top level in keyboard processing. Set up variables and
 * return.
 */

PASCAL NEAR ctlxlp(f, n)

{
	if (kbdmode != STOP) {
		mlwrite(TEXT105);
/*			"%%Macro already active" */
		return(FALSE);
	}
	mlwrite(TEXT106);
/*		"[Start macro]" */
	kbdptr = &kbdm[0];
	kbdend = kbdptr;
	kbdmode = RECORD;
	return(TRUE);
}

/*
 * End keyboard macro. Check for the same limit conditions as the above
 * routine. Set up the variables and return to the caller.
 */

PASCAL NEAR ctlxrp(f, n)

{
	if (kbdmode == STOP) {
		mlwrite(TEXT107);
/*			"%%Macro not active" */
		return(FALSE);
	}
	if (kbdmode == RECORD) {
		mlwrite(TEXT108);
/*			"[End macro]" */
		kbdmode = STOP;
	}
	return(TRUE);
}

/*
 * Execute a macro.
 * The command argument is the number of times to loop. Quit as soon as a
 * command gets an error. Return TRUE if all ok, else FALSE.
 */

PASCAL NEAR ctlxe(f, n)

{
	if (kbdmode != STOP) {
		mlwrite(TEXT105);
/*			"%%Macro already active" */
		return(FALSE);
	}
	if (n <= 0)
		return(TRUE);
	kbdrep = n;	   /* remember how many times to execute */
	kbdmode = PLAY; 	/* start us in play mode */
	kbdptr = &kbdm[0];	/*    at the beginning */
	return(TRUE);
}

/*
 * Abort.
 * Beep the beeper. Kill off any keyboard macro, etc., that is in progress.
 * Sometimes called as a routine, to do general aborting of stuff.
 */

PASCAL NEAR ctrlg(f, n)

{
	TTbeep();
	kbdmode = STOP;
	mlwrite(TEXT8);
/*		"[Aborted]" */
	return(ABORT);
}

/* tell the user that this command is illegal while we are in
   VIEW (read-only) mode				*/

PASCAL NEAR rdonly()

{
	TTbeep();
	mlwrite(TEXT109);
/*		"[Key illegal in VIEW mode]" */
	return(FALSE);
}

PASCAL NEAR resterr()

{
	TTbeep();
	mlwrite(TEXT110);
/*		"[That command is RESTRICTED]" */
	return(FALSE);
}

PASCAL NEAR nullproc(f, n)	/* user function that does NOTHING */

int n, f;	/* yes, these are default and never used.. but MUST be here */

{
}

PASCAL NEAR meta(f, n)	/* set META prefixing pending */

{
	prefix |= META;
	prenum = n;
	predef = f;
	return(TRUE);
}

PASCAL NEAR cex(f, n)	/* set ^X prefixing pending */

{
	prefix |= CTLX;
	prenum = n;
	predef = f;
	return(TRUE);
}

/*	bytecopy:	copy a string...with length restrictions
			ALWAYS null terminate
*/

char *PASCAL NEAR bytecopy(dst, src, maxlen)

char *dst;	/* destination of copied string */
char *src;	/* source */
int maxlen;	/* maximum length */

{
	char *dptr;	/* ptr into dst */

	dptr = dst;
	while (*src && (maxlen-- > 0))
		*dptr++ = *src++;
	*dptr = 0;
	return(dst);
}
