/*
 * $Id: input.c,v 1.3 1994/08/15 21:27:30 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: input.c,v $
 * Revision 1.3  1994/08/15 21:27:30  sev
 * i'm sorry, but this indent IMHO more better ;-)
 * Revision 1.2  1994/08/15  20:42:11  sev Indented Revision
 * 1.1  1994/06/24  14:17:12  sev Initial revision
 * 
 * 
 */

/*
 * Input:	Various input routines for MicroEMACS written by Daniel
 * Lawrence 5/9/86
 */

/*
 * Notes:
 * 
 * MicroEMACS's kernel processes two distinct forms of characters.  One of these
 * is a standard unsigned character which is used in the edited text.  The
 * other form, called an EMACS Extended Character is a 2 byte value which
 * contains both an ascii value, and flags for certain prefixes/events.
 * 
 * Bit	Usage ---	----- 0 = 7	Standard 8 bit ascii character 8
 * Control key flag 9	META prefix flag 10	^X prefix flag 11 Function
 * key flag 12	Mouse prefix 13	Shifted flag (not needed on alpha shifted
 * characters) 14	Alterate prefix (ALT key on PCs)
 * 
 * The machine dependent driver is responsible for returning a byte stream from
 * the various input devices with various prefixes/events embedded as escape
 * codes.  Zero is used as the value indicating an escape sequence is next.
 * The format of an escape sequence is as follows:
 * 
 * 0		Escape indicator <prefix byte>	upper byte of extended
 * character {<col><row>}	col, row position if the prefix byte
 * indicated a mouse event <event code>	value of event
 * 
 * Two successive zeroes are used to indicate an actual null being input.  These
 * values are then interpreted by getkey() to construct the proper extended
 * character sequences to pass to the MicroEMACS kernel.
 */

#include	<stdio.h>
#include	"estruct.h"
#include	"etype.h"
#include	"edef.h"
#include	"english.h"

/*
 * Ask a yes or no question in the message line. Return either TRUE, FALSE,
 * or ABORT. The ABORT status is returned if the user bumps out of the
 * question with a ^G. Used any time a confirmation is required.
 */

mlyesno(prompt)

char *prompt;

{
  int c;			  /* input character */
  char buf[NPAT];		  /* prompt to user */

  for (;;)
  {
    /* build and prompt the user */
    strcpy(buf, prompt);
    strcat(buf, TEXT162);
    /* " [y/n]? " */
    mlwrite(buf);

    /* get the response */
    c = getcmd();		  /* getcmd() lets us check for anything that
				   * might */
    /* generate a 'y' or 'Y' in case use screws up */

    if (c == ectoc(abortc))	  /* Bail out! */
      return (ABORT);

    if ((c == 'n') || (c == 'N')
	|| (c & (SPEC | ALTD | CTRL | META | CTLX | MOUS)))
      return (FALSE);		  /* ONLY 'y' or 'Y' allowed!!! */

    if (c == 'y' || c == 'Y')
      return (TRUE);

    return (FALSE);
  }
}

/*
 * ectoc:	expanded character to character collapse the CTRL and SPEC
 * flags back into an ascii code
 */

ectoc(c)

int c;

{
  if (c & CTRL)
    c = c & ~(CTRL | 0x40);
  if (c & SPEC)
    c = c & 255;
  return (c);
}

/*
 * tgetc:	Get a key from the terminal driver, resolve any keyboard
 * macro action
 */

int tgetc()

{
  int c;			  /* fetched character */

  /* if we are playing a keyboard macro back, */
  if (kbdmode == PLAY)
  {

    /* if there is some left... */
    if (kbdptr < kbdend)
      return ((int) *kbdptr++);

    /* at the end of last repitition? */
    if (--kbdrep < 1)
    {
      kbdmode = STOP;
      /* force a screen update after all is done */
      update(FALSE);
    }
    else
    {

      /* reset the macro to the begining for the next rep */
      kbdptr = &kbdm[0];
      return ((int) *kbdptr++);
    }
  }

  /* fetch a character from the terminal driver */
  c = TTgetc();

  /* record it for $lastkey */
  lastkey = c;

  /* save it if we need to */
  if (kbdmode == RECORD)
  {
    *kbdptr++ = c;
    kbdend = kbdptr;

    /* don't overrun the buffer */
    if (kbdptr == &kbdm[NKBDM - 1])
    {
      kbdmode = STOP;
      TTbeep();
    }
  }

  /* and finally give the char back */
  return (c);
}

/*
 * getkey: Get one keystroke. The only prefixs legal here are the SPEC and
 * CTRL prefixes.
 */

getkey()

{
  int c;			  /* next input character */
  int upper;			  /* upper byte of the extended sequence */

  /* get a keystroke */
  c = tgetc();

  /* if it exists, process an escape sequence */
  if (c == 0)
  {

    /* get the event type */
    upper = tgetc();

    /* mouse events need us to read in the row/col */
    if (upper & (MOUS >> 8))
    {
      /* grab the x/y position of the mouse */
      xpos = tgetc();
      ypos = tgetc();
    }

    /* get the event code */
    c = tgetc();

    /* if it is a function key... map it */
    c = (upper << 8) | c;

  }

  /* yank out the control prefix */
  if ((c & 255) >= 0x00 && (c & 255) <= 0x1F)
    c = CTRL | (c + '@');

  /* return the character */
  return (c);
}

/*
 * GETCMD: Get a command from the keyboard. Process all applicable prefix
 * keys
 */
getcmd()

{
  int c;			  /* fetched keystroke */
  KEYTAB *key;			  /* ptr to a key entry */

  /* get initial character */
  c = getkey();
  key = getbind(c);

  /* resolve META and CTLX prefixes */
  if (key)
  {
    if (key->k_ptr.fp == meta)
    {
      c = getkey();
      c = upperc(c) | (c & ~255); /* Force to upper */
      c |= META;
    }
    else if (key->k_ptr.fp == cex)
    {
      c = getkey();
      c = upperc(c) | (c & ~255); /* Force to upper */
      c |= CTLX;
    }
  }

  /* return it */
  return (c);
}

/*
 * A more generalized prompt/reply function allowing the caller to specify
 * the proper terminator. If the terminator is not a return('\r'), return
 * will echo as "<NL>"
 */
getstring(prompt, buf, nbuf, eolchar)

char *prompt;
char *buf;
int eolchar;

{
  register int cpos;		  /* current character position in string */
  register int c;		  /* current input character */
  register int quotef;		  /* are we quoting the next char? */

  cpos = 0;
  quotef = FALSE;

  /* prompt the user for the input string */
  if (discmd)
    mlwrite(prompt);
  else
    movecursor(term.t_nrow, 0);

  for (;;)
  {
    /* get a character from the user */
    c = getkey();

    /* if they hit the line terminate, wrap it up */
    if (c == eolchar && quotef == FALSE)
    {
      buf[cpos++] = 0;

      /* clear the message line */
      mlwrite("");
      TTflush();

      /* if we default the buffer, return FALSE */
      if (buf[0] == 0)
	return (FALSE);

      return (TRUE);
    }

    /* change from command form back to character form */
    c = ectoc(c);

    if (c == ectoc(abortc) && quotef == FALSE)
    {
      /* Abort the input? */
      ctrlg(FALSE, 0);
      TTflush();
      return (ABORT);
    }
    else if ((c == 0x7F || c == 0x08) && quotef == FALSE)
    {
      /* rubout/erase */
      if (cpos != 0)
      {
	outstring("\b \b");
	--ttcol;

	if (buf[--cpos] < 0x20)
	{
	  outstring("\b \b");
	  --ttcol;
	}

	if (buf[cpos] == '\r')
	{
	  outstring("\b\b  \b\b");
	  ttcol -= 2;
	}
	TTflush();
      }

    }
    else if (c == 0x15 && quotef == FALSE)
    {
      /* C-U, kill */
      while (cpos != 0)
      {
	outstring("\b \b");
	--ttcol;

	if (buf[--cpos] < 0x20)
	{
	  outstring("\b \b");
	  --ttcol;
	}
      }
      TTflush();

    }
    else if (c == quotec && quotef == FALSE)
    {
      quotef = TRUE;
    }
    else
    {
      quotef = FALSE;
      if (cpos < nbuf - 1)
      {
	buf[cpos++] = c;

	if ((c < ' ') && (c != '\r'))
	{
	  outstring("^");
	  ++ttcol;
	  c ^= 0x40;
	}

	if (c != '\r')
	  mlout(c);
	else
	{			  /* put out <NL> for <ret> */
	  outstring("<NL>");
	  ttcol += 3;
	}
	++ttcol;
	TTflush();
      }
    }
  }
}

outstring(s)			  /* output a string of input characters */

char *s;			  /* string to output */

{
  while (*s)
    mlout(*s++);
}
