#include	<stdio.h>

/* make global definitions not external */
#define maindef

#include	"estruct.h"	  /* global structures and defines */
#include	"etype.h"	  /* variable prototype definitions */
#include	"edef.h"	  /* global definitions */
#include	"english.h"	  /* human language definitions */
#include	"ebind.h"	  /* default key bindings */

/* for many different systems, increase the default stack space */

#ifndef GOOD
#define GOOD	0
#endif

#if	CALLED
emacs(int argc, char *argv[])
#else
main(int argc, char *argv[])
#endif

{
  register int status;

  /* Initialize the editor */
  vtinit();			  /* Terminal */
  edinit("main");		  /* Buffers, windows */

  if (argc > 1)
  {
    strcpy(curbp->b_fname, argv[1]);
    readin(curbp->b_fname);
    curbp->b_dotp = lforw(curbp->b_linep);
    curbp->b_doto = 0;
    curbp->b_active = TRUE;
  }

  status = editloop();

  vttidy();
#if	CALLED
  return (status);
#else
  exit(status);
#endif
}

/*
 * This is called to let the user edit something.	Note that if you
 * arrange to be able to call this from a macro, you will have invented the
 * "recursive-edit" function.
 */
editloop()
{
  register int c;		  /* command character */
  register int oldflag;		  /* old last flag value */

  /* setup to process commands */
  lastflag = 0;			  /* Fake last flags.	 */

loop:
  /* if we were called as a subroutine and want to leave, do so */
  if (eexitflag)
    return (eexitval);

  /* execute the "command" macro...normally null */
  oldflag = lastflag;		  /* preserve lastflag through this */
  lastflag = oldflag;

  /* Fix up the screen	 */
  update(FALSE);

  /* get the next command from the keyboard */
  c = getkey();

  /* if there is something on the command line, clear it */
  if (mpresf != FALSE)
  {
    mlerase();
    update(FALSE);
  }

  /* override the arguments if prefixed */
  if (prefix)
  {
    if (islower(c & 255))
      c = (c & ~255) | toupper(c & 255);
    c |= prefix;
    prefix = 0;
  }
  /* and execute the command */
  execute(c);
  goto loop;
}

/*
 * Initialize all of the buffers and windows. The buffer name is passed down
 * as an argument, because the main routine may have been told to read in a
 * file by default, and we want the buffer name to be right.
 */
edinit(char bname[])
{
  register BUFFER *bp;
  register WINDOW *wp;
  int cmark;			  /* current mark */

  /* initialize some important globals */

  bp = bfind(bname, TRUE);	  /* First buffer 	 */
  wp = (WINDOW *) malloc(sizeof(WINDOW));	/* First window 	 */
  if (bp == (BUFFER *) NULL || wp == (WINDOW *) NULL)
    meexit(1);
  curbp = bp;			  /* Make this current	  */
  wheadp = wp;
  curwp = wp;
  wp->w_wndp = (WINDOW *) NULL;	  /* Initialize window    */
  wp->w_bufp = bp;
  bp->b_nwnd = 1;		  /* Displayed.	       */
  wp->w_linep = bp->b_linep;
  wp->w_dotp = bp->b_linep;
  wp->w_doto = 0;
  for (cmark = 0; cmark < NMARKS; cmark++)
  {
    wp->w_markp[cmark] = (LINE *) NULL;
    wp->w_marko[cmark] = 0;
  }
  wp->w_toprow = 0;
  wp->w_fcol = 0;
  wp->w_ntrows = term.t_nrow - 1; /* "-1" for mode line.  */
  wp->w_force = 0;
  wp->w_flag = WFMODE | WFHARD;	  /* Full. 	       */
}

/*
 * This is the general command execution routine. It handles the fake binding
 * of all the keys to "self-insert". It also clears out the "thisflag" word,
 * and arranges to move it to the "lastflag", so that the next command can
 * look at it. Return the status of command.
 */
execute(int c)
{
  register int status;
  KEYTAB *key;			  /* key entry to execute */

  /* if the keystroke is a bound function...do it */
  key = getbind(c);
  if (key != (KEYTAB *) NULL)
  {
    thisflag = 0;
    status = execkey(key);
    lastflag = thisflag;
    return (status);
  }

  /*
   * If a space was typed, fill column is defined, the argument is non-
   * negative, wrap mode is enabled, and we are now past fill column, and we
   * are not read-only, perform word wrap.
   */
  if ((c >= 0x20 && c <= 0xFF))
  {				  /* Self inserting.      */
    thisflag = 0;		  /* For the future.      */

    /*
     * if we are in overwrite mode, not at eol, and next char is not a tab or
     * we are at a tab stop, delete a char forword
     */
    if (curwp->w_bufp->b_mode & MDOVER &&
	curwp->w_doto < curwp->w_dotp->l_used &&
	(lgetc(curwp->w_dotp, curwp->w_doto) != '\t' ||
	 (curwp->w_doto) % tabsize == (tabsize - 1)))
      ldelete(1L);

    status = linsert(1, c);


    /* check auto-save mode */
    if (curbp->b_mode & MDASAVE)
      if (--gacount == 0)
      {
	/* and save the file if needed */
	upscreen();
	filesave();
	gacount = gasave;
      }

    lastflag = thisflag;
    return (status);
  }
  TTbeep();
  mlwrite(TEXT19);		  /* complain	       */
  /* "[Key not bound]" */
  lastflag = 0;			  /* Fake last flags.	 */
  return (FALSE);
}

/*
 * Quit command. If an argument, always quit. Otherwise confirm if a buffer
 * has been changed and not written out. Normally bound to "C-X C-C".
 */
quit()
{
  register int status;		  /* return status */

  if ((curbp->b_flag & BFCHG) == 0 || (status = mlyesno(TEXT104)) == TRUE)
                                   /* "Modified buffers exist. Leave anyway" */
      status = meexit(GOOD);

  mlerase();
  return (status);
}

meexit(int status)
{
  eexitflag = TRUE;		  /* flag a program exit */
  eexitval = status;

  /* and now.. we leave and let the main loop kill us */
  return (TRUE);
}

/*
 * Abort. Beep the beeper. Kill off any keyboard macro, etc., that is in
 * progress. Sometimes called as a routine, to do general aborting of stuff.
 */
ctrlg()
{
  TTbeep();
  mlwrite(TEXT8);
  /* "[Aborted]" */
  return (ABORT);
}

/*
 * tell the user that this command is illegal while we are in VIEW
 * (read-only) mode
 */
rdonly()
{
  TTbeep();
  mlwrite(TEXT109);
  /* "[Key illegal in VIEW mode]" */
  return (FALSE);
}

meta()			  /* set META prefixing pending */
{
  prefix |= META;
  return (TRUE);
}

cex()			  /* set ^X prefixing pending */
{
  prefix |= CTLX;
  return (TRUE);
}

/*
 * bytecopy:	copy a string...with length restrictions ALWAYS null
 * terminate
 */
char *bytecopy(char *dst, char *src, int maxlen)
{
  char *dptr;			  /* ptr into dst */

  dptr = dst;
  while (*src && (maxlen-- > 0))
    *dptr++ = *src++;
  *dptr = 0;
  return (dst);
}
