#include	<stdio.h>
#include	"estruct.h"
#include	"etype.h"
#include	"edef.h"
#include	"english.h"

/*
 * Find a buffer, by name. Return a pointer to the BUFFER structure
 * associated with it. If the buffer is not found and the "cflag" is TRUE,
 * create it. The "bflag" is the settings for the flags in in buffer.
 */
BUFFER *bfind(char *bname, int cflag)
{
  register BUFFER *bp;
  register BUFFER *sb;		  /* buffer to insert after */
  register LINE *lp;
  int cmark;			  /* current mark */

  bp = bheadp;
  while (bp != (BUFFER *) NULL)
  {
    if (strcmp(bname, bp->b_bname) == 0)
      return (bp);
    bp = bp->b_bufp;
  }
  if (cflag != FALSE)
  {
    if ((bp = (BUFFER *) malloc(sizeof(BUFFER))) == (BUFFER *) NULL)
      return ((BUFFER *) NULL);
    if ((lp = lalloc(0)) == (LINE *) NULL)
    {
      free((char *) bp);
      return ((BUFFER *) NULL);
    }
    /* find the place in the list to insert this buffer */
    if (bheadp == (BUFFER *) NULL || strcmp(bheadp->b_bname, bname) > 0)
    {
      /* insert at the beginning */
      bp->b_bufp = bheadp;
      bheadp = bp;
    }
    else
    {
      sb = bheadp;
      while (sb->b_bufp != (BUFFER *) NULL)
      {
	if (strcmp(sb->b_bufp->b_bname, bname) > 0)
	  break;
	sb = sb->b_bufp;
      }

      /* and insert it */
      bp->b_bufp = sb->b_bufp;
      sb->b_bufp = bp;
    }

    /* and set up the other buffer fields */
    bp->b_topline = (LINE *) NULL;
    bp->b_botline = (LINE *) NULL;
    bp->b_active = TRUE;
    bp->b_dotp = lp;
    bp->b_doto = 0;
    for (cmark = 0; cmark < NMARKS; cmark++)
    {
      bp->b_markp[cmark] = (LINE *) NULL;
      bp->b_marko[cmark] = 0;
    }
    bp->b_fcol = 0;
    bp->b_flag = 0;
    bp->b_mode = gmode;
    bp->b_nwnd = 0;
    bp->b_linep = lp;
    strcpy(bp->b_fname, "");
    strcpy(bp->b_bname, bname);
    lp->l_fp = lp;
    lp->l_bp = lp;
  }
  return (bp);
}

/*
 * This routine blows away all of the text in a buffer. If the buffer is
 * marked as changed then we ask if it is ok to blow it away; this is to save
 * the user the grief of losing text. The window chain is nearly always wrong
 * if this gets called; the caller must arrange for the updates that are
 * required. Return TRUE if everything looks good.
 */
bclear(BUFFER *bp)
{
  register LINE *lp;
  register int s;
  int cmark;			  /* current mark */

  if ((bp->b_flag & BFINVS) == 0  /* Not scratch buffer.	 */
      && (bp->b_flag & BFCHG) != 0/* Something changed	 */
      && (s = mlyesno(TEXT32)) != TRUE)
    /* "Discard changes" */
    return (s);
  bp->b_flag &= ~BFCHG;		  /* Not changed	       */
  while ((lp = lforw(bp->b_linep)) != bp->b_linep)
    lfree(lp);
  bp->b_dotp = bp->b_linep;	  /* Fix "."	       */
  bp->b_doto = 0;
  for (cmark = 0; cmark < NMARKS; cmark++)
  {
    bp->b_markp[cmark] = (LINE *) NULL;	/* Invalidate "mark"    */
    bp->b_marko[cmark] = 0;
  }
  bp->b_fcol = 0;
  return (TRUE);
}
