#include	<stdio.h>
#include	"estruct.h"
#include	"etype.h"
#include	"edef.h"
#include	"english.h"

FILE *ffp;			  /* File pointer, all functions. */
static int eofflag;		  /* end-of-file flag */

/* Open a file for reading. */
ffropen(char *fn)
{
  if ((ffp = fopen(fn, "r")) == (FILE *) NULL)
    return (FIOFNF);
  eofflag = FALSE;
  return (FIOSUC);
}

/*
 * Open a file for writing. Return TRUE if all is well, and FALSE on error
 * (cannot create).
 */
ffwopen(char *fn)
{
  if ((ffp = fopen(fn, "w")) == (FILE *) NULL)
  {
    mlwrite(TEXT155);
    /* "Cannot open file for writing" */
    return (FIOERR);
  }
  return (FIOSUC);
}

/* Close a file. Should look at the status in all systems. */
ffclose()
{
  /* free this since we do not need it anymore */
  if (fline)
  {
    free(fline);
    fline = (char *) NULL;
  }

  if (fclose(ffp) != FALSE)
  {
    mlwrite(TEXT156);
    /* "Error closing file" */
    return (FIOERR);
  }
  return (FIOSUC);
}

/*
 * Write a line to the already opened file. The "buf" points to the buffer,
 * and the "nbuf" is its length, less the free newline. Return the status.
 * Check only at the newline.
 */
ffputline(char buf[], int nbuf)
{
  register int i;

  for (i = 0; i < nbuf; ++i)
    putc(buf[i], ffp);

  putc('\n', ffp);

  if (ferror(ffp))
  {
    mlwrite(TEXT157);
    /* "Write I/O error" */
    return (FIOERR);
  }

  return (FIOSUC);
}

/*
 * Read a line from a file, and store the bytes in the supplied buffer. The
 * "nbuf" is the length of the buffer. Complain about long lines and lines at
 * the end of the file that don't have a newline present. Check for I/O
 * errors too. Return status.
 */
ffgetline()
{
  register int c;		  /* current character read */
  register int i;		  /* current index into fline */
  register char *tmpline;	  /* temp storage for expanding line */

  /* if we are at the end...return it */
  if (eofflag)
    return (FIOEOF);

  /* dump fline if it ended up too big */
  if (flen > NSTRING && fline != (char *) NULL)
  {
    free(fline);
    fline = (char *) NULL;
  }

  /* if we don't have an fline, allocate one */
  if (fline == (char *) NULL)
    if ((fline = malloc(flen = NSTRING)) == (char *) NULL)
      return (FIOMEM);

  /* read the line in */
  i = 0;
  while ((c = getc(ffp)) != EOF && c != '\n')
  {
    fline[i++] = c;
    /* if it's longer, get more room */
    if (i >= flen)
    {
      if ((tmpline = malloc(flen + NSTRING)) == (char *) NULL)
	return (FIOMEM);
      bytecopy(tmpline, fline, flen);
      flen += NSTRING;
      free(fline);
      fline = tmpline;
    }
  }

  /* test for any errors that may have occured */
  if (c == EOF)
  {
    if (ferror(ffp))
    {
      mlwrite(TEXT158);
      /* "File read error" */
      return (FIOERR);
    }

    if (i != 0)
      eofflag = TRUE;
    else
      return (FIOEOF);
  }

  /* terminate and decrypt the string */
  fline[i] = 0;
  return (FIOSUC);
}

int fexist(char *fname)		  /* does <fname> exist on disk? */
{
  FILE *fp;

  /* try to open the file for reading */
  fp = fopen(fname, "r");

  /* if it fails, just return false! */
  if (fp == (FILE *) NULL)
    return (FALSE);

  /* otherwise, close it and report true */
  fclose(fp);
  return (TRUE);
}
