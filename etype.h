/*
 *  $Id: etype.h,v 1.1 1994/06/24 14:17:12 sev Exp $
 *
 * ---------------------------------------------------------- 
 *
 * $Log: etype.h,v $
 * Revision 1.1  1994/06/24 14:17:12  sev
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

BUFFER *PASCAL NEAR bfind();
BUFFER *PASCAL NEAR getdefb();
char *PASCAL NEAR bytecopy();
char *PASCAL NEAR fixnull();
char *PASCAL NEAR int_asc();
char *PASCAL NEAR makename();
int PASCAL NEAR absv();
int PASCAL NEAR eq();
int PASCAL NEAR ernd();
int PASCAL NEAR fbound();
int PASCAL NEAR fexist();
int PASCAL NEAR getkey();
int PASCAL NEAR nextch();
int PASCAL NEAR readpattern();
int PASCAL NEAR replaces();
int PASCAL NEAR scanner();
int PASCAL NEAR tgetc();
LINE *PASCAL NEAR lalloc();
PASCAL NEAR anycb();
PASCAL NEAR asc_int();
PASCAL NEAR backchar();
PASCAL NEAR backdel();
PASCAL NEAR backhunt();
PASCAL NEAR backline();
PASCAL NEAR backpage();
PASCAL NEAR backsearch();
PASCAL NEAR bclear();
PASCAL NEAR cex();
PASCAL NEAR cinsert();
PASCAL NEAR copyregion();
PASCAL NEAR crypt();
PASCAL NEAR ctlxe();
PASCAL NEAR ctlxlp();
PASCAL NEAR ctlxrp();
PASCAL NEAR ctrlg();
PASCAL NEAR dcline();
PASCAL NEAR delins();
PASCAL NEAR ectoc();
PASCAL NEAR edinit();
PASCAL NEAR editloop();
PASCAL NEAR execute();
PASCAL NEAR expandp();
PASCAL NEAR ffclose();
PASCAL NEAR ffgetline();
PASCAL NEAR ffputline();
PASCAL NEAR ffropen();
PASCAL NEAR ffwopen();
PASCAL NEAR filesave();
PASCAL NEAR forwchar();
PASCAL NEAR forwdel();
PASCAL NEAR forwhunt();
PASCAL NEAR forwline();
PASCAL NEAR forwpage();
PASCAL NEAR forwsearch();
PASCAL NEAR getccol();
PASCAL NEAR getcline();
PASCAL NEAR getcmd();
PASCAL NEAR getgoal();
PASCAL NEAR getregion();
PASCAL NEAR gotobob();
PASCAL NEAR gotobol();
PASCAL NEAR gotoeob();
PASCAL NEAR gotoeol();
PASCAL NEAR gotomark();
PASCAL NEAR indent();
PASCAL NEAR initchars();
PASCAL NEAR insspace();
PASCAL NEAR kdelete();
PASCAL NEAR killregion();
PASCAL NEAR killtext();
PASCAL NEAR kinsert();
PASCAL NEAR lchange();
PASCAL NEAR ldelete();
PASCAL NEAR ldelnewline();
PASCAL NEAR lfree();
PASCAL NEAR linsert();
PASCAL NEAR linstr();
PASCAL NEAR lnewline();
PASCAL NEAR meexit();
PASCAL NEAR meta();
PASCAL NEAR mlerase();
PASCAL NEAR mlout();
PASCAL NEAR mlputf();
PASCAL NEAR mlputi();
PASCAL NEAR mlputli();
PASCAL NEAR mlputs();
PASCAL NEAR mlyesno();
PASCAL NEAR modeline();
PASCAL NEAR movecursor();
PASCAL NEAR newline();
PASCAL NEAR nextbuffer();
PASCAL NEAR nullproc();
PASCAL NEAR qreplace();
PASCAL NEAR quickexit();
PASCAL NEAR quit();
PASCAL NEAR quote();
PASCAL NEAR rdonly();
PASCAL NEAR readin();
PASCAL NEAR reframe();
PASCAL NEAR refresh();
PASCAL NEAR resterr();
PASCAL NEAR rvstrcpy();
PASCAL NEAR savematch();
PASCAL NEAR setjtable();
PASCAL NEAR setmark();
PASCAL NEAR spawncli();
PASCAL NEAR sreplace();
PASCAL NEAR swbuffer();
PASCAL NEAR tab();
PASCAL NEAR ttclose();
PASCAL NEAR ttflush();
PASCAL NEAR ttgetc();
PASCAL NEAR ttopen();
PASCAL NEAR ttputc();
PASCAL NEAR typahead();
PASCAL NEAR unqname();
PASCAL NEAR updall();
PASCAL NEAR update();
PASCAL NEAR updateline();
PASCAL NEAR upddex();
PASCAL NEAR updext();
PASCAL NEAR updgar();
PASCAL NEAR updone();
PASCAL NEAR updpos();
PASCAL NEAR updupd();
PASCAL NEAR upmode();
PASCAL NEAR upscreen();
PASCAL NEAR vteeol();
PASCAL NEAR vtinit();
PASCAL NEAR vtmove();
PASCAL NEAR vtputc();
PASCAL NEAR vttidy();
PASCAL NEAR writeout();
PASCAL NEAR yank();
PASCAL NEAR zotbuf();
unsigned int PASCAL NEAR chcase();
KEYTAB *getbind();

char PASCAL NEAR upperc();

int PASCAL NEAR islower();
int PASCAL NEAR isupper();
int PASCAL NEAR isletter();

CDECL NEAR mlwrite();

/* some library redefinitions */

char *strcat();
char *strcpy();
char *malloc();


