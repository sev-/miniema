/*
 * $Id: ebind.h,v 1.3 1994/08/15 20:42:11 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: ebind.h,v $
 * Revision 1.3  1994/08/15 20:42:11  sev
 * Indented
 * Revision 1.2  1994/06/24  17:22:21  sev Added mainmenu key
 * 
 * Revision 1.1  1994/06/24  14:17:12  sev Initial revision
 * 
 * 
 */

/*
 * EBIND:		Initial default key to function bindings for
 * MicroEMACS 3.10
 */

/*
 * Command table. This table  is *roughly* in ASCII order, left to right
 * across the characters of the command. This explains the funny location of
 * the control-X commands.
 */
KEYTAB keytab[NBINDS] =
{
  {CTRL | 'A', BINDFNC, gotobol},
  {CTRL | 'B', BINDFNC, backchar},
  {CTRL | 'C', BINDFNC, insspace},
  {CTRL | 'D', BINDFNC, forwdel},
  {CTRL | 'E', BINDFNC, gotoeol},
  {CTRL | 'F', BINDFNC, forwchar},
  {CTRL | 'G', BINDFNC, ctrlg},
  {CTRL | 'H', BINDFNC, backdel},
  {CTRL | 'I', BINDFNC, tab},
  {CTRL | 'J', BINDFNC, indent},
  {CTRL | 'K', BINDFNC, killtext},
  {CTRL | 'L', BINDFNC, refresh},
  {CTRL | 'M', BINDFNC, newline},
  {CTRL | 'N', BINDFNC, forwline},
  {CTRL | 'P', BINDFNC, backline},
  {CTRL | 'O', BINDFNC, mainmenu},
  {CTRL | 'Q', BINDFNC, quote},
  {CTRL | 'R', BINDFNC, backsearch},
  {CTRL | 'S', BINDFNC, forwsearch},
  {CTRL | 'V', BINDFNC, forwpage},
  {CTRL | 'W', BINDFNC, killregion},
  {CTRL | 'X', BINDFNC, cex},
  {CTRL | 'Y', BINDFNC, yank},
  {CTRL | 'Z', BINDFNC, backpage},
  {CTRL | '[', BINDFNC, meta},
  {CTLX | CTRL | 'C', BINDFNC, quit},
  {CTLX | CTRL | 'S', BINDFNC, filesave},
  {CTLX | '(', BINDFNC, ctlxlp},
  {CTLX | ')', BINDFNC, ctlxrp},
  {CTLX | 'C', BINDFNC, spawncli},
  {CTLX | 'E', BINDFNC, ctlxe},
  {CTLX | 'X', BINDFNC, nextbuffer},
  {META | CTRL | 'G', BINDFNC, gotomark},
  {META | CTRL | 'R', BINDFNC, qreplace},
  {META | ' ', BINDFNC, setmark},
  {META | '>', BINDFNC, gotoeob},
  {META | '<', BINDFNC, gotobob},
  {META | 'R', BINDFNC, sreplace},
  {META | 'V', BINDFNC, backpage},
  {META | 'W', BINDFNC, copyregion},
  {META | 'Z', BINDFNC, quickexit},
  {ALTD | 'S', BINDFNC, forwhunt},
  {ALTD | 'R', BINDFNC, backhunt},
  {SPEC | '<', BINDFNC, gotobob},
  {SPEC | 'P', BINDFNC, backline},
  {SPEC | 'Z', BINDFNC, backpage},
  {SPEC | 'B', BINDFNC, backchar},
  {SPEC | 'F', BINDFNC, forwchar},
  {SPEC | '>', BINDFNC, gotoeob},
  {SPEC | 'N', BINDFNC, forwline},
  {SPEC | 'V', BINDFNC, forwpage},
  {SPEC | 'C', BINDFNC, insspace},
  {SPEC | 'D', BINDFNC, forwdel},
  {0x7F, BINDFNC, backdel},
  {0, BINDNUL, (int (*) ()) NULL}
};
