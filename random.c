/*
 * $Id: random.c,v 1.2 1994/08/15 20:42:11 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: random.c,v $
 * Revision 1.2  1994/08/15 20:42:11  sev
 * Indented
 * Revision 1.1  1994/06/24  14:17:12  sev Initial revision
 * 
 * 
 */

/*
 * This file contains the command processing functions for a number of random
 * commands. There is no functional grouping here, for sure.
 */

#include	<stdio.h>
#include	"estruct.h"
#include	"etype.h"
#include	"edef.h"
#include	"english.h"

getcline()			       /* get the current line number */

{
  register LINE *lp;		       /* current line */
  register int numlines;	       /* # of lines before point */

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
getccol(bflg)
int bflg;
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

/*
 * Quote the next character, and insert it into the buffer. All the
 * characters are taken literally, including the newline, which does not then
 * have its line splitting meaning. The character is always read, even if it
 * is inserted 0 times, for regularity. Bound to "C-Q"
 */

quote(f, n)

{
  register int c;

  if (curbp->b_mode & MDVIEW)	       /* don't allow this command if	 */
    return (rdonly());		       /* we are in read only mode	 */
  c = tgetc();
  if (n < 0)
    return (FALSE);
  if (n == 0)
    return (TRUE);
  return (linsert(n, c));
}

/*
 * Set tab size if given non-default argument (n <> 1).  Otherwise, insert a
 * tab into file.  If given argument, n, of zero, change to hard tabs. If n >
 * 1, simulate tab stop every n-characters using spaces. This has to be done
 * in this slightly funny way because the tab (in ASCII) has been turned into
 * "C-I" (in 10 bit code) already. Bound to "C-I".
 */
tab(f, n)
{
  if (n < 0)
    return (FALSE);
  if (n == 0 || n > 1)
  {
    stabsize = n;
    return (TRUE);
  }
  if (!stabsize)
    return (linsert(1, '\t'));
  return (linsert(stabsize - (getccol(FALSE) % stabsize), ' '));
}


/*
 * Insert a newline. Bound to "C-M". If we are in CMODE, do automatic
 * indentation as specified.
 */
newline(f, n)
{
  register int s;

  if (curbp->b_mode & MDVIEW)	       /* don't allow this command if	 */
    return (rdonly());		       /* we are in read only mode	 */
  if (n < 0)
    return (FALSE);

  /* if we are in C mode and this is a default <NL> */
  if (n == 1 && (curbp->b_mode & MDCMOD) &&
      curwp->w_dotp != curbp->b_linep)
    return (cinsert());

  /* insert some lines */
  while (n--)
  {
    if ((s = lnewline()) != TRUE)
      return (s);
  }
  return (TRUE);
}

cinsert()			       /* insert a newline and indentation
				        * for C */

{
  register char *cptr;		       /* string pointer into text to copy */
  register int i;		       /* index into line to copy indent from */
  register int llen;		       /* length of line to copy indent from */
  register int bracef;		       /* was there a brace at the end of
				        * line? */
  register LINE *lp;		       /* current line pointer */
  register int offset;
  char ichar[NSTRING];		       /* buffer to hold indent of last line */

  /* trim the whitespace before the point */
  lp = curwp->w_dotp;
  offset = curwp->w_doto;
  while (offset > 0 &&
	 lgetc(lp, offset - 1) == ' ' ||
	 lgetc(lp, offset - 1) == '\t')
  {
    backdel(FALSE, 1);
    offset--;
  }

  /* check for a brace */
  bracef = (offset > 0 && lgetc(lp, offset - 1) == '{');

  /* put in the newline */
  if (lnewline() == FALSE)
    return (FALSE);

  /* if the new line is not blank... don't indent it! */
  lp = curwp->w_dotp;
  if (lp->l_used != 0)
    return (TRUE);

  /* hunt for the last non-blank line to get indentation from */
  while (lp->l_used == 0 && lp != curbp->b_linep)
    lp = lp->l_bp;

  /* grab a pointer to text to copy indentation from */
  cptr = &(lp->l_text[0]);
  llen = lp->l_used;

  /* save the indent of the last non blank line */
  i = 0;
  while ((i < llen) && (cptr[i] == ' ' || cptr[i] == '\t')
	 && (i < NSTRING - 1))
  {
    ichar[i] = cptr[i];
    ++i;
  }
  ichar[i] = 0;			       /* terminate it */

  /* insert this saved indentation */
  linstr(ichar);

  /* and one more tab for a brace */
  if (bracef)
    tab(FALSE, 1);

  return (TRUE);
}

/*
 * Insert a newline, then enough tabs and spaces to duplicate the indentation
 * of the previous line. Tabs are every tabsize characters. Quite simple.
 * Figure out the indentation of the current line. Insert a newline by
 * calling the standard routine. Insert the indentation by inserting the
 * right number of tabs and spaces. Return TRUE if all ok. Return FALSE if
 * one of the subcomands failed. Normally bound to "C-J".
 */
indent(f, n)
{
  register int nicol;
  register int c;
  register int i;

  if (curbp->b_mode & MDVIEW)	       /* don't allow this command if	 */
    return (rdonly());		       /* we are in read only mode	 */
  if (n < 0)
    return (FALSE);
  while (n--)
  {
    nicol = 0;
    for (i = 0; i < llength(curwp->w_dotp); ++i)
    {
      c = lgetc(curwp->w_dotp, i);
      if (c != ' ' && c != '\t')
	break;
      if (c == '\t')
	nicol += -(nicol % tabsize) + (tabsize - 1);
      ++nicol;
    }
    if (lnewline() == FALSE
	|| ((i = nicol / tabsize) != 0 && linsert(i, '\t') == FALSE)
	|| ((i = nicol % tabsize) != 0 && linsert(i, ' ') == FALSE))
      return (FALSE);
  }
  return (TRUE);
}

/*
 * Delete forward. This is real easy, because the basic delete routine does
 * all of the work. Watches for negative arguments, and does the right thing.
 * If any argument is present, it kills rather than deletes, to prevent loss
 * of text if typed with a big argument. Normally bound to "C-D".
 */
forwdel(f, n)
{
  if (curbp->b_mode & MDVIEW)	       /* don't allow this command if	 */
    return (rdonly());		       /* we are in read only mode	 */
  if (n < 0)
    return (backdel(f, -n));
  if (f != FALSE)
  {				       /* Really a kill.	 */
    if ((lastflag & CFKILL) == 0)
      kdelete();
    thisflag |= CFKILL;
  }
  return (ldelete((long) n, f));
}

/*
 * Delete backwards. This is quite easy too, because it's all done with other
 * functions. Just move the cursor back, and delete forwards. Like delete
 * forward, this actually does a kill if presented with an argument. Bound to
 * both "RUBOUT" and "C-H".
 */
backdel(f, n)
{
  register int s;

  if (curbp->b_mode & MDVIEW)	       /* don't allow this command if	 */
    return (rdonly());		       /* we are in read only mode	 */
  if (n < 0)
    return (forwdel(f, -n));
  if (f != FALSE)
  {				       /* Really a kill.	 */
    if ((lastflag & CFKILL) == 0)
      kdelete();
    thisflag |= CFKILL;
  }
  if ((s = backchar(f, n)) == TRUE)
    s = ldelete((long) n, f);
  return (s);
}

/*
 * Kill text. If called without an argument, it kills from dot to the end of
 * the line, unless it is at the end of the line, when it kills the newline.
 * If called with an argument of 0, it kills from the start of the line to
 * dot. If called with a positive argument, it kills from dot forward over
 * that number of newlines. If called with a negative argument it kills
 * backwards that number of newlines. Normally bound to "C-K".
 */
killtext(f, n)
{
  register LINE *nextp;
  long chunk;

  if (curbp->b_mode & MDVIEW)	       /* don't allow this command if	 */
    return (rdonly());		       /* we are in read only mode	 */
  if ((lastflag & CFKILL) == 0)	       /* Clear kill buffer if */
    kdelete();			       /* last wasn't a kill.	 */
  thisflag |= CFKILL;
  if (f == FALSE)
  {
    chunk = llength(curwp->w_dotp) - curwp->w_doto;
    if (chunk == 0)
      chunk = 1;
  }
  else if (n == 0)
  {
    chunk = curwp->w_doto;
    curwp->w_doto = 0;
  }
  else if (n > 0)
  {
    chunk = llength(curwp->w_dotp) - curwp->w_doto + 1;
    nextp = lforw(curwp->w_dotp);
    while (--n)
    {
      if (nextp == curbp->b_linep)
	return (FALSE);
      chunk += llength(nextp) + 1;
      nextp = lforw(nextp);
    }
  }
  else
  {
    mlwrite(TEXT61);
    /* "%%Negative argumet to kill is illegal" */
    return (FALSE);
  }
  return (ldelete(chunk, TRUE));
}

/*
 * Refresh the screen. With no argument, it just does the refresh. With an
 * argument it recenters "." in the current window. Bound to "C-L".
 */
refresh(f, n)
{
  if (f == FALSE)
    sgarbf = TRUE;
  else
  {
    curwp->w_force = 0;		       /* Center dot. */
    curwp->w_flag |= WFFORCE;
  }

  return (TRUE);
}

char *fixnull(s)		       /* Don't return NULL pointers! */

char *s;

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

char *int_asc(i)

int i;				       /* integer to translate to a string */

{
  register int digit;		       /* current digit being used */
  register char *sp;		       /* pointer into result */
  register int sign;		       /* sign of resulting number */
  static char result[INTWIDTH + 1];    /* resulting string */

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
    *(--sp) = '0' + digit;	       /* and install the new digit */
    i = i / 10;
  } while (i);

  /* and fix the sign */
  if (sign == -1)
  {
    *(--sp) = '-';		       /* and install the minus sign */
  }

  return (sp);
}

int absv(x)			       /* take the absolute value of an
				        * integer */

int x;

{
  return (x < 0 ? -x : x);
}

int ernd()			       /* returns a random integer */

{
  seed = absv(seed * 1721 + 10007);
  return (seed);
}

int execkey(key, f, n)		       /* execute a function bound to a key */

KEYTAB *key;			       /* key to execute */
int f, n;			       /* agruments to C function */

{
  register int status;		       /* error return */

  if (key->k_type == BINDFNC)
    return ((*(key->k_ptr.fp)) (f, n));
  return (TRUE);
}

/* This function looks a key binding up in the binding table	 */

KEYTAB *getbind(c)

int c;				       /* key to find what is bound to it */

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
