/*
 *  $Id: etype.h,v 1.3 1994/06/27 11:18:43 sev Exp $
 *
 * ---------------------------------------------------------- 
 *
 * $Log: etype.h,v $
 * Revision 1.3  1994/06/27 11:18:43  sev
 * Menu...
 *
 * Revision 1.2  1994/06/24  17:22:21  sev
 * Added mainmenu and ttputs proto
 *
 * Revision 1.1  1994/06/24  14:17:12  sev
 * Initial revision
 *
 *
 */
 
/*	ETYPE:		Global function type definitions for
			MicroEMACS 3.9

			written by Daniel Lawrence
			based on code by Dave G. Conroy,
				Steve Wilhite and George Jones
*/


/* ALL global fuction declarations */

BUFFER *NEAR bfind();
BUFFER *NEAR getdefb();
char *NEAR bytecopy();
char *NEAR fixnull();
char *NEAR int_asc();
char *NEAR makename();
int NEAR absv();
int NEAR eq();
int NEAR ernd();
int NEAR fbound();
int NEAR fexist();
int NEAR getkey();
int NEAR nextch();
int NEAR readpattern();
int NEAR replaces();
int NEAR scanner();
int NEAR tgetc();
LINE *NEAR lalloc();
NEAR anycb();
NEAR asc_int();
NEAR backchar();
NEAR backdel();
NEAR backhunt();
NEAR backline();
NEAR backpage();
NEAR backsearch();
NEAR bclear();
NEAR cex();
NEAR cinsert();
NEAR copyregion();
NEAR crypt();
NEAR ctlxe();
NEAR ctlxlp();
NEAR ctlxrp();
NEAR ctrlg();
NEAR dcline();
NEAR delins();
NEAR ectoc();
NEAR edinit();
NEAR editloop();
NEAR execute();
NEAR expandp();
NEAR ffclose();
NEAR ffgetline();
NEAR ffputline();
NEAR ffropen();
NEAR ffwopen();
NEAR filesave();
NEAR forwchar();
NEAR forwdel();
NEAR forwhunt();
NEAR forwline();
NEAR forwpage();
NEAR forwsearch();
NEAR getccol();
NEAR getcline();
NEAR getcmd();
NEAR getgoal();
NEAR getregion();
NEAR gotobob();
NEAR gotobol();
NEAR gotoeob();
NEAR gotoeol();
NEAR gotomark();
NEAR indent();
NEAR initchars();
NEAR insspace();
NEAR kdelete();
NEAR killregion();
NEAR killtext();
NEAR kinsert();
NEAR lchange();
NEAR ldelete();
NEAR ldelnewline();
NEAR lfree();
NEAR linsert();
NEAR linstr();
NEAR lnewline();
NEAR meexit();
NEAR meta();
NEAR mlerase();
NEAR mlout();
NEAR mlputf();
NEAR mlputi();
NEAR mlputli();
NEAR mlputs();
NEAR mlyesno();
NEAR modeline();
NEAR movecursor();
NEAR newline();
NEAR nextbuffer();
NEAR nullproc();
NEAR qreplace();
NEAR quickexit();
NEAR quit();
NEAR quote();
NEAR rdonly();
NEAR readin();
NEAR reframe();
NEAR refresh();
NEAR resterr();
NEAR rvstrcpy();
NEAR savematch();
NEAR setjtable();
NEAR setmark();
NEAR spawncli();
NEAR sreplace();
NEAR swbuffer();
NEAR tab();
NEAR ttclose();
NEAR ttflush();
NEAR ttgetc();
NEAR ttopen();
NEAR ttputc();
NEAR typahead();
NEAR unqname();
NEAR updall();
NEAR update();
NEAR updateline();
NEAR upddex();
NEAR updext();
NEAR updgar();
NEAR updone();
NEAR updpos();
NEAR updupd();
NEAR upmode();
NEAR upscreen();
NEAR vteeol();
NEAR vtinit();
NEAR vtmove();
NEAR vtputc();
NEAR vttidy();
NEAR writeout();
NEAR yank();
NEAR zotbuf();
unsigned int NEAR chcase();
KEYTAB *getbind();

char NEAR upperc();

int NEAR islower();
int NEAR isupper();
int NEAR isletter();

CDECL NEAR mlwrite();

/* some library redefinitions */

char *strcat();
char *strcpy();
char *malloc();

/* menu.c */
NEAR mainmenu();
NEAR updoneline();	/* display.c */
NEAR initmenus();
