#include	<stdio.h>
#include	"estruct.h"
#include	"etype.h"
#include	"edef.h"
#include	"english.h"

typedef struct VIDEO
{
  int v_flag;			  /* Flags */
  char v_text[1];		  /* Screen data. */
} VIDEO;

#define VFCHG	0x0001		  /* Changed flag 		 */
#define VFEXT	0x0002		  /* extended (beyond column 80)	 */
#define VFREV	0x0004		  /* reverse video status 	 */
#define VFREQ	0x0008		  /* reverse video request	 */
#define VFCOL	0x0010		  /* color change requested	 */

static VIDEO **vscreen;		  /* Virtual screen. */
static VIDEO **pscreen;		  /* Physical screen. */

/*
 * Initialize the data structures used by the display code. The edge vectors
 * used to access the screens are set up. The operating system's terminal I/O
 * channel is set up. All the other things get initialized at compile time.
 * The original window has "WFCHG" set, so that it will get completely
 * redrawn on the first call to "update".
 */
vtinit()
{
  register int i;
  register VIDEO *vp;

  TTopen();			  /* open the screen */
  TTkopen();			  /* open the keyboard */
  TTrev(FALSE);


  /* allocate the virtual screen pointer array */
  vscreen = (VIDEO **) malloc(term.t_mrow * sizeof(VIDEO *));
  if (vscreen == (VIDEO **) NULL)
    meexit(1);

  /* allocate the physical shadow screen array */
  pscreen = (VIDEO **) malloc(term.t_mrow * sizeof(VIDEO *));
  if (pscreen == (VIDEO **) NULL)
    meexit(1);

  /* for every line in the display */
  for (i = 0; i < term.t_mrow; ++i)
  {

    /* allocate a virtual screen line */
    vp = (VIDEO *) malloc(sizeof(VIDEO) + term.t_mcol);
    if (vp == (VIDEO *) NULL)
      meexit(1);

    vp->v_flag = 0;		  /* init change clags */
    /* connect virtual line to line array */
    vscreen[i] = vp;

    /* allocate and initialize physical shadow screen line */
    vp = (VIDEO *) malloc(sizeof(VIDEO) + term.t_mcol);
    if (vp == (VIDEO *) NULL)
      meexit(1);

    vp->v_flag = 0;
    pscreen[i] = vp;
  }
}

/*
 * Clean up the virtual terminal system, in anticipation for a return to the
 * operating system. Move down to the last line and clear it out (the next
 * system prompt will be written in the line). Shut down the channel to the
 * terminal.
 */
vttidy()
{
  mlerase();
  movecursor(term.t_nrow, 0);
  TTflush();
  TTclose();
  TTkclose();
}

/*
 * Set the virtual cursor to the specified row and column on the virtual
 * screen. There is no checking for nonsense values; this might be a good
 * idea during the early stages.
 */
vtmove(int row, int col)
{
  vtrow = row;
  vtcol = col;
}

/*
 * Write a character to the virtual screen. The virtual row and column are
 * updated. If we are not yet on left edge, don't print it yet. If the line
 * is too long put a "$" in the last column. This routine only puts printing
 * characters into the virtual terminal buffers. Only column overflow is
 * checked.
 */
vtputc(int c)
{
  register VIDEO *vp;		  /* ptr to line being updated */

  vp = vscreen[vtrow];

  if (c == '\t')
  {
    do
    {
      vtputc(' ');
    } while (((vtcol + taboff) % (tabsize)) != 0);
  }
  else if (vtcol >= term.t_ncol)
  {
    ++vtcol;
    vp->v_text[term.t_ncol - 1] = '>';
  }
  else if (c < 0x20 || c == 0x7F)
  {
    vtputc('^');
    vtputc(c ^ 0x40);
  }
  else
  {
    if (vtcol >= 0)
      vp->v_text[vtcol] = c;
    ++vtcol;
  }
}

/*
 * Erase from the end of the software cursor to the end of the line on which
 * the software cursor is located.
 */
vteeol()
{
  register VIDEO *vp;

  vp = vscreen[vtrow];
  while (vtcol < term.t_ncol)
  {
    if (vtcol >= 0)
      vp->v_text[vtcol] = ' ';
    vtcol++;
  }
}

/*
 * upscreen:	user routine to force a screen update always finishes
 * complete update
 */
upscreen()
{
  update(TRUE);
  return (TRUE);
}

/*
 * Make sure that the display is right. This is a three part process. First,
 * scan through all of the windows looking for dirty ones. Check the framing,
 * and refresh the screen. Second, make sure that "currow" and "curcol" are
 * correct for the current window. Third, make the virtual and physical
 * screens the same.
 */
update(int force)
{
  register WINDOW *wp;

#if	TYPEAH
  if (force == FALSE && typahead())
    return (TRUE);
#endif
  /* update any windows that need refreshing */
  wp = wheadp;
  while (wp != (WINDOW *) NULL)
  {
    if (wp->w_flag)
    {
      /* if the window has changed, service it */
      reframe(wp);		  /* check the framing */
      if ((wp->w_flag & ~WFMODE) == WFEDIT)
	updone(wp);		  /* update EDITed line */
      else if (wp->w_flag & ~WFMOVE)
	updall(wp);		  /* update all lines */
      if (wp->w_flag & WFMODE)
	modeline(wp);		  /* update modeline */
      wp->w_flag = 0;
      wp->w_force = 0;
    }

    /* on to the next window */
    wp = wp->w_wndp;
  }

  /* recalc the current hardware cursor location */
  updpos();

  /* check for lines to de-extend */
  upddex();

  /* if screen is garbage, re-plot it */
  if (sgarbf != FALSE)
    updgar();

  /* update the virtual screen to the physical screen */
  updupd(force);

  /* update the cursor and flush the buffers */
  movecursor(currow, curcol - lbound);
  TTflush();

  return (TRUE);
}

/*
 * reframe:	check to see if the cursor is on in the window and re-frame
 * it if needed or wanted
 */
reframe(WINDOW *wp)
{
  register LINE *lp;		  /* search pointer */
  register LINE *rp;		  /* reverse search pointer */
  register LINE *hp;		  /* ptr to header line in buffer */
  register int i;		  /* general index/# lines to scroll */
  register int nlines;		  /* number of lines in current window */

  /* figure out our window size */
  nlines = wp->w_ntrows;
  if (modeflag == FALSE)
    nlines++;

  /* if not a requested reframe, check for a needed one */
  if ((wp->w_flag & WFFORCE) == 0)
  {
    lp = wp->w_linep;
    for (i = 0; i < nlines; i++)
    {

      /* if the line is in the window, no reframe */
      if (lp == wp->w_dotp)
	return (TRUE);

      /* if we are at the end of the file, reframe */
      if (lp == wp->w_bufp->b_linep)
	break;

      /* on to the next line */
      lp = lforw(lp);
    }
  }

  /* reaching here, we need a window refresh */
  i = wp->w_force;

  /* if smooth scrolling is enabled, first.. have we gone off the top? */
  if (sscroll && ((wp->w_flag & WFFORCE) == 0))
  {
    /* search thru the buffer looking for the point */
    lp = rp = wp->w_linep;
    hp = wp->w_bufp->b_linep;
    while ((lp != hp) || (rp != hp))
    {

      /* did we scroll downward? */
      if (lp == wp->w_dotp)
      {
	i = nlines - 1;
	break;
      }

      /* did we scroll upward? */
      if (rp == wp->w_dotp)
      {
	i = 0;
	break;
      }

      /* advance forward and back */
      if (lp != hp)
	lp = lforw(lp);
      if (rp != hp)
	rp = lback(rp);
    }
    /* how far back to reframe? */
  }
  else if (i > 0)
  {				  /* only one screen worth of lines max */
    if (--i >= nlines)
      i = nlines - 1;
  }
  else if (i < 0)
  {				  /* negative update???? */
    i += nlines;
    if (i < 0)
      i = 0;
  }
  else
    i = nlines / 2;

  /* backup to new line at top of window */
  lp = wp->w_dotp;
  while (i != 0 && lback(lp) != wp->w_bufp->b_linep)
  {
    --i;
    lp = lback(lp);
  }

  /* and reset the current line at top of window */
  wp->w_linep = lp;
  wp->w_flag |= WFHARD;
  wp->w_flag &= ~WFFORCE;
  return (TRUE);
}

/* updone: update the current line to the virtual screen		 */
updone(WINDOW *wp)
{
  register LINE *lp;		  /* line to update */
  register int sline;		  /* physical screen line to update */
  register int i;

  /* search down the line we want */
  lp = wp->w_linep;
  sline = wp->w_toprow;
  while (lp != wp->w_dotp)
  {
    ++sline;
    lp = lforw(lp);
  }

  /* and update the virtual line */
  vscreen[sline]->v_flag |= VFCHG;
  vscreen[sline]->v_flag &= ~VFREQ;
  taboff = wp->w_fcol;
  vtmove(sline, -taboff);
  for (i = 0; i < llength(lp); ++i)
    vtputc(lgetc(lp, i));
  vteeol();
  taboff = 0;
}

/* updall: update all the lines in a window on the virtual screen */
updall(WINDOW *wp)
{
  register LINE *lp;		  /* line to update */
  register int sline;		  /* physical screen line to update */
  register int i;
  register int nlines;		  /* number of lines in the current window */

  /* search down the lines, updating them */
  lp = wp->w_linep;
  sline = wp->w_toprow;
  nlines = wp->w_ntrows;
  if (modeflag == FALSE)
    nlines++;
  taboff = wp->w_fcol;
  while (sline < wp->w_toprow + nlines)
  {

    /* and update the virtual line */
    vscreen[sline]->v_flag |= VFCHG;
    vscreen[sline]->v_flag &= ~VFREQ;
    vtmove(sline, -taboff);
    if (lp != wp->w_bufp->b_linep)
    {
      /* if we are not at the end */
      for (i = 0; i < llength(lp); ++i)
	vtputc(lgetc(lp, i));
      lp = lforw(lp);
    }

    /* make sure we are on screen */
    if (vtcol < 0)
      vtcol = 0;

    /* on to the next one */
    vteeol();
    ++sline;
  }
  taboff = 0;
}

/*
 * updpos: update the position of the hardware cursor and handle extended
 * lines. This is the only update for simple moves.
 */
updpos()
{
  register LINE *lp;
  register int c;
  register int i;

  /* find the current row */
  lp = curwp->w_linep;
  currow = curwp->w_toprow;
  while (lp != curwp->w_dotp)
  {
    ++currow;
    lp = lforw(lp);
  }

  /* find the current column */
  curcol = 0;
  i = 0;
  while (i < curwp->w_doto)
  {
    c = lgetc(lp, i++);
    if (c == '\t')
      curcol += -(curcol % tabsize) + (tabsize - 1);
    else
    {
      if (c > 0x7f)
      {
	curcol += 2;
	c -= 0x80;
      }
      if (c < 0x20 || c == 0x7f)
	++curcol;
    }
    ++curcol;
  }

  /* adjust by the current first column position */
  curcol -= curwp->w_fcol;

  /* make sure it is not off the left side of the screen */
  while (curcol < 0)
  {
    if (curwp->w_fcol >= hjump)
    {
      curcol += hjump;
      curwp->w_fcol -= hjump;
    }
    else
    {
      curcol += curwp->w_fcol;
      curwp->w_fcol = 0;
    }
    curwp->w_flag |= WFHARD | WFMODE;
  }

  /* if horizontall scrolling is enabled, shift if needed */
  if (hscroll)
  {
    while (curcol >= term.t_ncol - 1)
    {
      curcol -= hjump;
      curwp->w_fcol += hjump;
      curwp->w_flag |= WFHARD | WFMODE;
    }
  }
  else
  {
    /* if extended, flag so and update the virtual line image */
    if (curcol >= term.t_ncol - 1)
    {
      vscreen[currow]->v_flag |= (VFEXT | VFCHG);
      updext();
    }
    else
      lbound = 0;
  }

  /* update the current window if we have to move it around */
  if (curwp->w_flag & WFHARD)
    updall(curwp);
  if (curwp->w_flag & WFMODE)
    modeline(curwp);
  curwp->w_flag = 0;
}

/* upddex: de-extend any line that derserves it		 */
upddex()
{
  register WINDOW *wp;
  register LINE *lp;
  register int i, j;
  register int nlines;		  /* number of lines in the current window */

  wp = wheadp;

  while (wp != (WINDOW *) NULL)
  {
    lp = wp->w_linep;
    i = wp->w_toprow;
    nlines = wp->w_ntrows;
    if (modeflag == FALSE)
      nlines++;

    while (i < wp->w_toprow + nlines)
    {
      if (vscreen[i]->v_flag & VFEXT)
      {
	if ((wp != curwp) || (lp != wp->w_dotp) ||
	    (curcol < term.t_ncol - 1))
	{
	  taboff = wp->w_fcol;
	  vtmove(i, -taboff);
	  for (j = 0; j < llength(lp); ++j)
	    vtputc(lgetc(lp, j));
	  vteeol();
	  taboff = 0;

	  /* this line no longer is extended */
	  vscreen[i]->v_flag &= ~VFEXT;
	  vscreen[i]->v_flag |= VFCHG;
	}
      }
      lp = lforw(lp);
      ++i;
    }
    /* and onward to the next window */
    wp = wp->w_wndp;
  }
}

/*
 * updgar: if the screen is garbage, clear the physical screen and the
 * virtual screen and force a full update
 */
updgar()
{
  register int i;
  register int j;
  register char *txt;

  for (i = 0; i < term.t_nrow; ++i)
  {
    vscreen[i]->v_flag |= VFCHG;
    vscreen[i]->v_flag &= ~VFREV;
    txt = pscreen[i]->v_text;
    for (j = 0; j < term.t_ncol; ++j)
      txt[j] = 7;		  /* unimpossible character */
  }

  movecursor(0, 0);		  /* Erase the screen. */
  (*term.t_eeop) ();
  sgarbf = FALSE;		  /* Erase-page clears */
  mpresf = FALSE;		  /* the message area. */
}

/* updupd: update the physical screen from the virtual screen	 */
updupd(int force)
{
  register VIDEO *vp1;
  register int i;

  for (i = 0; i < term.t_nrow; ++i)
  {
    vp1 = vscreen[i];

    /* for each line that needs to be updated */
    if ((vp1->v_flag & VFCHG) != 0)
    {
#if	TYPEAH
      if (force == FALSE && typahead())
	return (TRUE);
#endif
      updateline(i, vp1, pscreen[i]);
    }
  }
  return (TRUE);
}

/*
 * updext: update the extended line which the cursor is currently on at a
 * column greater than the terminal width. The line will be scrolled right or
 * left to let the user see where the cursor is
 */
updext()
{
  register int rcursor;		  /* real cursor location */
  register LINE *lp;		  /* pointer to current line */
  register int j;		  /* index into line */

  /* calculate what column the real cursor will end up in */
  rcursor = ((curcol - term.t_ncol) % term.t_scrsiz)
       + term.t_margin;
  lbound = curcol - rcursor + 1;
  taboff = lbound + curwp->w_fcol;

  /* scan through the line outputing characters to the virtual screen */
  /* once we reach the left edge					 */
  vtmove(currow, -taboff);	  /* start scanning offscreen */
  lp = curwp->w_dotp;		  /* line to output */
  for (j = 0; j < llength(lp); ++j)	/* until the end-of-line */
    vtputc(lgetc(lp, j));

  /* truncate the virtual line, restore tab offset */
  vteeol();
  taboff = 0;

  /* and put a '$' in column 1 */
  vscreen[currow]->v_text[0] = '$';
}

/*
 * Update a single line. This does not know how to use insert or delete
 * character sequences; we are using VT52 functionality. Update the physical
 * row and column variables. It does try an exploit erase to end of line.
 */
updateline(int row, struct VIDEO *vp, struct VIDEO *pp)
{
  register char *cp1;
  register char *cp2;
  register char *cp3;
  register char *cp4;
  register char *cp5;
  register int nbflag;		  /* non-blanks to the right flag? */
  int rev;			  /* reverse video flag */
  int req;			  /* reverse video request flag */
  int upcol;			  /* update column (KRS) */

  /* set up pointers to virtual and physical lines */
  cp1 = &vp->v_text[0];
  cp2 = &pp->v_text[0];

  /*
   * if we need to change the reverse video status of the current line, we
   * need to re-write the entire line
   */
  rev = (vp->v_flag & VFREV) == VFREV;
  req = (vp->v_flag & VFREQ) == VFREQ;
  if ((rev != req))
  {
    /* set rev video if needed */
    if (rev != req)
      (*term.t_rev) (req);

    movecursor(row, 0);		  /* Go to start of line. */

    /*
     * scan through the line and dump it to the screen and the virtual screen
     * array
     */
    cp3 = &vp->v_text[term.t_ncol];
    while (cp1 < cp3)
    {
      TTputc(*cp1);
      ++ttcol;
      *cp2++ = *cp1++;
    }
    /* turn rev video off */
    if (rev != req)
      (*term.t_rev) (FALSE);

    /* update the needed flags */
    vp->v_flag &= ~VFCHG;
    if (req)
      vp->v_flag |= VFREV;
    else
      vp->v_flag &= ~VFREV;
    return (TRUE);
  }

  upcol = 0;

  /* advance past any common chars at the left */
  while (cp1 != &vp->v_text[term.t_ncol] && cp1[0] == cp2[0])
  {
    ++cp1;
    ++upcol;
    ++cp2;
  }

  /*
   * This can still happen, even though we only call this routine on changed
   * lines. A hard update is always done when a line splits, a massive change
   * is done, or a buffer is displayed twice. This optimizes out most of the
   * excess updating. A lot of computes are used, but these tend to be hard
   * operations that do a lot of update, so I don't really care.
   */
  /* if both lines are the same, no update needs to be done */
  if (cp1 == &vp->v_text[term.t_ncol])
  {
    vp->v_flag &= ~VFCHG;	  /* flag this line is changed */
    return (TRUE);
  }

  /* find out if there is a match on the right */
  nbflag = FALSE;
  cp3 = &vp->v_text[term.t_ncol];
  cp4 = &pp->v_text[term.t_ncol];

  while (cp3[-1] == cp4[-1])
  {
    --cp3;
    --cp4;
    if (cp3[0] != ' ')		  /* Note if any nonblank */
      nbflag = TRUE;		  /* in right match. */
  }

  cp5 = cp3;

  /* Erase to EOL ? */
  if (nbflag == FALSE && eolexist == TRUE && (req != TRUE))
  {
    while (cp5 != cp1 && cp5[-1] == ' ')
      --cp5;

    if (cp3 - cp5 <= 3)		  /* Use only if erase is */
      cp5 = cp3;		  /* fewer characters. */
  }

  /* move to the begining of the text to update */
  movecursor(row, upcol);

  if (rev)
    TTrev(TRUE);

  while (cp1 != cp5)
  {				  /* Ordinary. */
    TTputc(*cp1);
    ++ttcol;
    *cp2++ = *cp1++;
  }

  if (cp5 != cp3)
  {				  /* Erase. */
    TTeeol();
    while (cp1 != cp3)
      *cp2++ = *cp1++;
  }
  if (rev)
    TTrev(FALSE);
  vp->v_flag &= ~VFCHG;		  /* flag this line as updated */
  return (TRUE);
}

/*
 * Redisplay the mode line for the window pointed to by the "wp". This is the
 * only routine that has any idea of how the modeline is formatted. You can
 * change the modeline format by hacking at this routine. Called by "update"
 * any time there is a dirty window.
 */
modeline(WINDOW *wp)
{
  register char *cp;
  register int c;
  register int n;		  /* cursor position count */
  register BUFFER *bp;
  register int i;		  /* loop index */
  register int lchar;		  /* character to draw line in buffer with */
  register int firstm;		  /* is this the first mode? */
  char tline[NLINE];		  /* buffer for part of mode line */

  /* don't bother if there is none! */
  if (modeflag == FALSE)
    return;

  n = wp->w_toprow + wp->w_ntrows;/* Location. */

  /*
   * Note that we assume that setting REVERSE will cause the terminal driver
   * to draw with the inverted relationship of fcolor and bcolor, so that
   * when we say to set the foreground color to "white" and background color
   * to "black", the fact that "reverse" is enabled means that the terminal
   * driver actually draws "black" on a background of "white".  Makes sense,
   * no?  This way, devices for which the color controls are optional will
   * still get the "reverse" signals.
   */

  vscreen[n]->v_flag |= VFCHG | VFREQ | VFCOL;	/* Redraw next time. */
  vtmove(n, 0);			  /* Seek to right line. */
  lchar = ' ';

  bp = wp->w_bufp;
  if ((bp->b_flag & BFTRUNC) != 0)/* "#" if truncated */
    vtputc('#');
  else
    vtputc(lchar);

  if ((bp->b_flag & BFCHG) != 0)  /* "*" if changed. */
    vtputc('*');
  else
    vtputc(lchar);

  if ((bp->b_flag & BFNAROW) != 0)
  {				  /* "<>" if narrowed */
    vtputc('<');
    vtputc('>');
  }
  else
  {
    vtputc(lchar);
    vtputc(lchar);
  }

  n = 4;
  strcpy(tline, " ");		  /* Buffer name. */
  strcat(tline, PROGNAME);
  strcat(tline, " ");
  strcat(tline, VERSION);
  strcat(tline, " ");

  /* are we horizontally scrolled? */
  if (wp->w_fcol > 0)
  {
    strcat(tline, "[<");
    strcat(tline, int_asc(wp->w_fcol));
    strcat(tline, "]");
  }

  /* display the modes */
  strcat(tline, "(");
  firstm = TRUE;
  for (i = 0; i < NUMMODES; i++)  /* add in the mode flags */
    if (wp->w_bufp->b_mode & (1 << i))
    {
      if (firstm != TRUE)
	strcat(tline, " ");
      firstm = FALSE;
      strcat(tline, modename[i]);
    }
  strcat(tline, ") ");

  cp = &tline[0];
  while ((c = *cp++) != 0)
  {
    vtputc(c);
    ++n;
  }

  vtputc(lchar);
  vtputc(lchar);
  vtputc(' ');
  n += 3;
  cp = &bp->b_bname[0];

  while ((c = *cp++) != 0)
  {
    vtputc(c);
    ++n;
  }

  vtputc(' ');
  vtputc(lchar);
  vtputc(lchar);
  n += 3;

  if (bp->b_fname[0] != 0)
  {				  /* File name. */
    vtputc(' ');
    ++n;
    cp = TEXT34;
    /* "File: " */

    while ((c = *cp++) != 0)
    {
      vtputc(c);
      ++n;
    }

    cp = &bp->b_fname[0];

    while ((c = *cp++) != 0)
    {
      vtputc(c);
      ++n;
    }

    vtputc(' ');
    ++n;
  }

  while (n < term.t_ncol)
  {				  /* Pad to full width. */
    vtputc(lchar);
    ++n;
  }
}

upmode()			  /* update all the mode lines */
{
  register WINDOW *wp;

  wp = wheadp;
  while (wp != (WINDOW *) NULL)
  {
    wp->w_flag |= WFMODE;
    wp = wp->w_wndp;
  }
}

/*
 * Send a command to the terminal to move the hardware cursor to row "row"
 * and column "col". The row and column arguments are origin 0. Optimize out
 * random calls. Update "ttrow" and "ttcol".
 */
movecursor(int row, int col)
{
  if (row != ttrow || col != ttcol)
  {
    ttrow = row;
    ttcol = col;
    TTmove(row, col);
  }
}

mlerase()
{
  int i;

  movecursor(term.t_nrow, 0);
  if (discmd == FALSE)
    return;

  if (eolexist == TRUE)
    TTeeol();
  else
  {
    for (i = 0; i < term.t_ncol - 1; i++)
      TTputc(' ');

    /* force the move! */
    /* movecursor(term.t_nrow, 1); */
    movecursor(term.t_nrow, 0);
  }
  TTflush();
  mpresf = FALSE;
}

mlout(int c)
{
  if (ttcol + 1 < term.t_ncol)
    TTputc(c);
  if (c != '\b')
    *lastptr++ = c;
  else if (lastptr > &lastmesg[0])
    --lastptr;
}

mlwrite(char *fmt)
{
  register int c;		  /* current char in format string */

  /* if we are not currently echoing on the command line, abort this */
  if (discmd == FALSE)
    return;

  /* if we can not erase to end-of-line, do it manually */
  if (eolexist == FALSE)
  {
    mlerase();
    TTflush();
  }

  movecursor(term.t_nrow, 0);
  lastptr = &lastmesg[0];	  /* setup to record message */
  while ((c = *fmt++) != 0)
  {
      mlout(c);
      ++ttcol;
  }

  /* if we can, erase to the end of screen */
  if (eolexist == TRUE)
    TTeeol();
  TTflush();
  mpresf = TRUE;
  *lastptr = 0;			  /* terminate lastmesg[] */
}
