#include	<stdio.h>
#include	"estruct.h"
#include	"etype.h"
#include	"edef.h"
#include	"english.h"

getcline()			  /* get the current line number */
{
  register LINE *lp;		  /* current line */
  register int numlines;	  /* # of lines before point */

  /* starting at the beginning of the buffer */
  lp = lforw(curbp->b_linep);

  /* start counting lines */
  numlines = 0;
  while (lp != curbp->b_linep)
  {
    /* if we are on the current line, record it */
    if (lp == curwp->w_dotp)
      break;
    ++numlines;
    lp = lforw(lp);
  }

  /* and return the resulting count */
  return (numlines + 1);
}

/* Return current column.  Stop at first non-blank given TRUE argument. */
getccol(int bflg)
{
  register int c, i, col;

  col = 0;
  for (i = 0; i < curwp->w_doto; ++i)
  {
    c = lgetc(curwp->w_dotp, i);
    if (c != ' ' && c != '\t' && bflg)
      break;
    if (c == '\t')
      col += -(col % tabsize) + (tabsize - 1);
    else if (c < 0x20 || c == 0x7F)
      ++col;
    ++col;
  }
  return (col);
}

tab()
{
  if (!stabsize)
    return (linsert(1, '\t'));
  return (linsert(stabsize - (getccol(FALSE) % stabsize), ' '));
}

/*
 * Insert a newline. Bound to "C-M".
 */
newline()
{
  register int s;

  if (curbp->b_mode & MDVIEW)	  /* don't allow this command if	 */
    return (rdonly());		  /* we are in read only mode	 */

    if ((s = lnewline()) != TRUE)
      return (s);
  return (TRUE);
}

/*
 * Delete forward. This is real easy, because the basic delete routine does
 * all of the work. Watches for negative arguments, and does the right thing.
 * If any argument is present, it kills rather than deletes, to prevent loss
 * of text if typed with a big argument. Normally bound to "C-D".
 */
forwdel()
{
  if (curbp->b_mode & MDVIEW)	  /* don't allow this command if	 */
    return (rdonly());		  /* we are in read only mode	 */
  return (ldelete(1));
}

/*
 * Delete backwards. This is quite easy too, because it's all done with other
 * functions. Just move the cursor back, and delete forwards. Like delete
 * forward, this actually does a kill if presented with an argument. Bound to
 * both "RUBOUT" and "C-H".
 */
backdel()
{
  register int s;

  if (curbp->b_mode & MDVIEW)	  /* don't allow this command if	 */
    return (rdonly());		  /* we are in read only mode	 */
  if ((s = backchar()) == TRUE)
    s = ldelete(1);
  return (s);
}

/*
 * Kill text. Kills from dot to the end of the line. Normally bound to "C-K".
 */
killtext()
{
  long chunk;

  if (curbp->b_mode & MDVIEW)	  /* don't allow this command if	 */
    return (rdonly());		  /* we are in read only mode	 */
  chunk = llength(curwp->w_dotp) - curwp->w_doto;
  if (chunk == 0)
    chunk = 1;

  return (ldelete(chunk));
}

/*
 * Refresh the screen. With no argument, it just does the refresh. With an
 * argument it recenters "." in the current window. Bound to "C-L".
 */
refresh()
{
  sgarbf = TRUE;

  return (TRUE);
}

char *fixnull(char *s)		  /* Don't return NULL pointers! */
{
  if (s == (char *) NULL)
    return ("");
  else
    return (s);
}

/*
 * int_asc:	integer to ascii string.......... This is too inconsistant to
 * use the system's
 */
char *int_asc(int i)
{
  register int digit;		  /* current digit being used */
  register char *sp;		  /* pointer into result */
  register int sign;		  /* sign of resulting number */
  static char result[INTWIDTH + 1];	/* resulting string */

  /* record the sign... */
  sign = 1;
  if (i < 0)
  {
    sign = -1;
    i = -i;
  }

  /* and build the string (backwards!) */
  sp = result + INTWIDTH;
  *sp = 0;
  do
  {
    digit = i % 10;
    *(--sp) = '0' + digit;	  /* and install the new digit */
    i = i / 10;
  } while (i);

  /* and fix the sign */
  if (sign == -1)
  {
    *(--sp) = '-';		  /* and install the minus sign */
  }

  return (sp);
}

int absv(int x)			  /* take the absolute value of an integer */
{
  return (x < 0 ? -x : x);
}

int ernd()			  /* returns a random integer */
{
  seed = absv(seed * 1721 + 10007);
  return (seed);
}

int execkey(KEYTAB *key)	  /* execute a function bound to a key */
{
  if (key->k_type == BINDFNC)
    return ((*(key->k_ptr.fp)) ());
  return (TRUE);
}

/* This function looks a key binding up in the binding table	 */
KEYTAB *getbind(int c)
{
  register KEYTAB *ktp;

  /* scan through the binding table, looking for the key's entry */
  ktp = &keytab[0];
  while (ktp->k_type != BINDNUL)
  {
    if (ktp->k_code == c)
      return (ktp);
    ++ktp;
  }

  /* no such binding */
  return ((KEYTAB *) NULL);
}
