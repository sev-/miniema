/*
 * $Id: file.c,v 1.2 1994/08/15 20:42:11 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: file.c,v $
 * Revision 1.2  1994/08/15 20:42:11  sev
 * Indented
 * Revision 1.1  1994/06/24  14:17:12  sev Initial revision
 * 
 * 
 */

/*
 * FILE.C:   for MicroEMACS
 * 
 * The routines in this file handle the reading, writing and lookup of disk
 * files.  All of details about the reading and writing of the disk are in
 * "fileio.c".
 * 
 */

#include	<stdio.h>
#include	"estruct.h"
#include	"etype.h"
#include	"edef.h"
#include	"english.h"

/*
 * Read file "fname" into the current buffer, blowing away any text found
 * there.  Called by both the read and find commands.  Return the final
 * status of the read.  Also called by the mainline, to read in a file
 * specified on the command line as an argument. The command in $readhook is
 * called after the buffer is set up and before it is read.
 */

readin(fname, lockfl)

char fname[];			       /* name of file to read */
int lockfl;			       /* check for file locks? */

{
  register LINE *lp1;
  register LINE *lp2;
  register int i;
  register WINDOW *wp;
  register BUFFER *bp;
  register int s;
  register int nbytes;
  register int nline;
  register int cmark;		       /* current mark */
  char mesg[NSTRING];

  bp = curbp;			       /* Cheap.		 */
  if ((s = bclear(bp)) != TRUE)	       /* Might be old.	 */
    return (s);
  bp->b_flag &= ~(BFINVS | BFCHG);
  strcpy(bp->b_fname, fname);

  /* turn off ALL keyboard translation in case we get a dos error */
  TTkclose();

  if ((s = ffropen(fname)) == FIOERR)  /* Hard file open.	 */
    goto out;

  if (s == FIOFNF)
  {				       /* File not found.	 */
    mlwrite(TEXT138);
    /* "[New file]" */
    goto out;
  }

  /* read the file in */
  mlwrite(TEXT139);
  /* "[Reading file]" */
  nline = 0;
  while ((s = ffgetline()) == FIOSUC)
  {
    nbytes = strlen(fline);
    if ((lp1 = lalloc(nbytes)) == (LINE *) NULL)
    {
      s = FIOMEM;		       /* Keep message on the	 */
      break;			       /* display.		 */
    }
    lp2 = lback(curbp->b_linep);
    lp2->l_fp = lp1;
    lp1->l_fp = curbp->b_linep;
    lp1->l_bp = lp2;
    curbp->b_linep->l_bp = lp1;
    for (i = 0; i < nbytes; ++i)
      lputc(lp1, i, fline[i]);
    ++nline;
  }
  ffclose();			       /* Ignore errors.       */
  strcpy(mesg, "[");
  if (s == FIOERR)
  {
    strcat(mesg, TEXT141);
    /* "I/O ERROR, " */
    curbp->b_flag |= BFTRUNC;
  }
  if (s == FIOMEM)
  {
    strcat(mesg, TEXT142);
    /* "OUT OF MEMORY, " */
    curbp->b_flag |= BFTRUNC;
  }
  strcat(mesg, TEXT140);
  /* "Read " */
  strcat(mesg, int_asc(nline));
  strcat(mesg, TEXT143);
  /* " line" */
  if (nline > 1)
    strcat(mesg, "s");
  strcat(mesg, "]");
  mlwrite(mesg);

out:
  TTkopen();			       /* open the keyboard again */
  for (wp = wheadp; wp != (WINDOW *) NULL; wp = wp->w_wndp)
  {
    if (wp->w_bufp == curbp)
    {
      wp->w_linep = lforw(curbp->b_linep);
      wp->w_dotp = lforw(curbp->b_linep);
      wp->w_doto = 0;
      for (cmark = 0; cmark < NMARKS; cmark++)
      {
	wp->w_markp[cmark] = (LINE *) NULL;
	wp->w_marko[cmark] = 0;
      }
      wp->w_flag |= WFMODE | WFHARD;
    }
  }
  if (s == FIOERR || s == FIOFNF)      /* False if error.      */
    return (FALSE);
  return (TRUE);
}

/*
 * Take a file name, and from it fabricate a buffer name. This routine knows
 * about the syntax of file names on the target system. I suppose that this
 * information could be put in a better place than a line of code. Returns a
 * pointer into fname indicating the end of the file path; i.e., 1 character
 * BEYOND the path name.
 */
char *makename(bname, fname)
char bname[];
char fname[];
{
  register char *cp1;
  register char *cp2;
  register char *pathp;

  cp1 = &fname[0];
  while (*cp1 != 0)
    ++cp1;

  while (cp1 != &fname[0] && cp1[-1] != '/')
    --cp1;
  /* cp1 is pointing to the first real filename char */
  pathp = cp1;

  cp2 = &bname[0];
  while (cp2 != &bname[NBUFN - 1] && *cp1 != 0 && *cp1 != ';')
    *cp2++ = *cp1++;
  *cp2 = 0;

  return (pathp);
}

unqname(name)			       /* make sure a buffer name is unique */

char *name;			       /* name to check on */

{
  register char *sp;

  /* check to see if it is in the buffer list */
  while (bfind(name, 0, FALSE) != (BUFFER *) NULL)
  {

    /* go to the end of the name */
    sp = name;
    while (*sp)
      ++sp;
    if (sp == name || (*(sp - 1) < '0' || *(sp - 1) > '8'))
    {
      *sp++ = '0';
      *sp = 0;
    }
    else
      *(--sp) += 1;
  }
}

/*
 * Save the contents of the current buffer in its associatd file. Do nothing
 * if nothing has changed (this may be a bug, not a feature). Error if there
 * is no remembered file name for the buffer. Bound to "C-X C-S". May get
 * called by "C-Z".
 */
filesave(f, n)
{
  register int s;

  if (curbp->b_mode & MDVIEW)	       /* don't allow this command if	 */
    return (rdonly());		       /* we are in read only mode	 */
  if ((curbp->b_flag & BFCHG) == 0)    /* Return, no changes.	 */
    return (TRUE);
  if (curbp->b_fname[0] == 0)
  {				       /* Must have a name.    */
    mlwrite(TEXT145);
    /* "No file name" */
    return (FALSE);
  }

  /* complain about truncated files */
  if ((curbp->b_flag & BFTRUNC) != 0)
  {
    if (mlyesno(TEXT146) == FALSE)
    {
      /* "Truncated file..write it out" */
      mlwrite(TEXT8);
      /* "[Aborted]" */
      return (FALSE);
    }
  }

  /* complain about narrowed buffers */
  if ((curbp->b_flag & BFNAROW) != 0)
  {
    if (mlyesno(TEXT147) == FALSE)
    {
      /* "Narrowed Buffer..write it out" */
      mlwrite(TEXT8);
      /* "[Aborted]" */
      return (FALSE);
    }
  }

  if ((s = writeout(curbp->b_fname)) == TRUE)
  {
    curbp->b_flag &= ~BFCHG;
    /* Update mode lines.	 */
    upmode();
  }
  return (s);
}

/*
 * This function performs the details of file writing. It uses the file
 * management routines in the "fileio.c" package. The number of lines written
 * is displayed. Several errors are posible, and cause writeout to return a
 * FALSE result. When $ssave is TRUE,  the buffer is written out to a
 * temporary file, and then the old file is unlinked and the temporary
 * renamed to the original name.  Before the file is written, a user
 * specifyable routine (in $writehook) can be run.
 */

writeout(fn)

char *fn;			       /* name of file to write current
				        * buffer to */

{
  register LINE *lp;		       /* line to scan while writing */
  register char *sp;		       /* temporary string pointer */
  register int nline;		       /* number of lines written */
  int status;			       /* return status */
  int sflag;			       /* are we safe saving? */
  char tname[NSTRING];		       /* temporary file name */
  char buf[NSTRING];		       /* message buffer */

  /* determine if we will use the save method */
  sflag = FALSE;
  if (ssave && fexist(fn))
    sflag = TRUE;

  /* turn off ALL keyboard translation in case we get a dos error */
  TTkclose();

  /* Perform Safe Save..... */
  if (sflag)
  {
    /* duplicate original file name, and find where to trunc it */
    sp = tname + (makename(tname, fn) - fn) + 1;
    strcpy(tname, fn);

    /* create a unique name, using random numbers */
    do
    {
      *sp = 0;
      strcat(tname, int_asc(ernd()));
    } while (fexist(tname));

    /* open the temporary file */
    status = ffwopen(tname);
  }
  else
    status = ffwopen(fn);

  /* if the open failed.. clean up and abort */
  if (status != FIOSUC)
  {
    TTkopen();
    return (FALSE);
  }

  /* write the current buffer's lines to the open disk file */
  mlwrite(TEXT148);		       /* tell us that we're writing */
  /* "[Writing...]" */
  lp = lforw(curbp->b_linep);	       /* start at the first line.     */
  nline = 0;			       /* track the Number of lines	 */
  while (lp != curbp->b_linep)
  {
    if ((status = ffputline(&lp->l_text[0], llength(lp))) != FIOSUC)
      break;
    ++nline;
    lp = lforw(lp);
  }


  /* report on status of file write */
  *buf = 0;
  status |= ffclose();
  if (status == FIOSUC)
  {
    /* report on success (or lack therof) */
    strcpy(buf, TEXT149);
    /* "[Wrote " */
    strcat(buf, int_asc(nline));
    strcat(buf, TEXT143);
    /* " line" */
    if (nline > 1)
      strcat(buf, "s");

    if (sflag)
    {
      /* erase original file */
      /* rename temporary file to original name */
      if (unlink(fn) == 0 && rename1(tname, fn) == 0)
	;
      else
      {
	strcat(buf, TEXT150);
	/* ", saved as " */
	strcat(buf, tname);
	status = FIODEL;	       /* failed */
      }
    }
    strcat(buf, "]");
    mlwrite(buf);
  }

  /* reopen the keyboard, and return our status */
  TTkopen();
  return (status == FIOSUC);
}
