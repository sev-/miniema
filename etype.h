/*
 * $Id: etype.h,v 1.5 1994/08/15 21:27:30 sev Exp $
 * 
 * ----------------------------------------------------------
 * 
 * $Log: etype.h,v $
 * Revision 1.5  1994/08/15 21:27:30  sev
 * i'm sorry, but this indent IMHO more better ;-)
 * Revision 1.4  1994/08/15  20:42:11  sev Indented Revision
 * 1.3  1994/06/27  11:18:43  sev Menu...
 * 
 * Revision 1.2  1994/06/24  17:22:21  sev Added mainmenu and ttputs proto
 * 
 * Revision 1.1  1994/06/24  14:17:12  sev Initial revision
 * 
 * 
 */

/*
 * ETYPE:		Global function type definitions for MicroEMACS 3.9
 * 
 * written by Daniel Lawrence based on code by Dave G. Conroy, Steve Wilhite and
 * George Jones
 */


/* ALL global fuction declarations */

BUFFER *bfind();
BUFFER *getdefb();
char *bytecopy();
char *fixnull();
char *int_asc();
char *makename();
int absv();
int eq();
int ernd();
int fbound();
int fexist();
int getkey();
int nextch();
int readpattern();
int replaces();
int scanner();
int tgetc();
LINE *lalloc();
int anycb();
int asc_int();
int backchar();
int backdel();
int backhunt();
int backline();
int backpage();
int backsearch();
int bclear();
int cex();
int cinsert();
int copyregion();
int crypt();
int ctlxe();
int ctlxlp();
int ctlxrp();
int ctrlg();
int dcline();
int delins();
int ectoc();
int edinit();
int editloop();
int execute();
int expandp();
int ffclose();
int ffgetline();
int ffputline();
int ffropen();
int ffwopen();
int filesave();
int forwchar();
int forwdel();
int forwhunt();
int forwline();
int forwpage();
int forwsearch();
int getccol();
int getcline();
int getcmd();
int getgoal();
int getregion();
int gotobob();
int gotobol();
int gotoeob();
int gotoeol();
int gotomark();
int indent();
int initchars();
int insspace();
int kdelete();
int killregion();
int killtext();
int kinsert();
int lchange();
int ldelete();
int ldelnewline();
int lfree();
int linsert();
int linstr();
int lnewline();
int meexit();
int meta();
int mlerase();
int mlout();
int mlputf();
int mlputi();
int mlputli();
int mlputs();
int mlyesno();
int modeline();
int movecursor();
int newline();
int nextbuffer();
int nullproc();
int qreplace();
int quickexit();
int quit();
int quote();
int rdonly();
int readin();
int reframe();
int refresh();
int resterr();
int rvstrcpy();
int savematch();
int setjtable();
int setmark();
int spawncli();
int sreplace();
int swbuffer();
int tab();
int ttclose();
int ttflush();
int ttgetc();
int ttopen();
int ttputc();
int typahead();
int unqname();
int updall();
int update();
int updateline();
int upddex();
int updext();
int updgar();
int updone();
int updpos();
int updupd();
int upmode();
int upscreen();
int vteeol();
int vtinit();
int vtmove();
int vtputc();
int vttidy();
int writeout();
int yank();
int zotbuf();
unsigned int chcase();
KEYTAB *getbind();

char upperc();

int islower();
int isupper();
int isletter();

int mlwrite();

/* some library redefinitions */

char *strcat();
char *strcpy();
char *malloc();

/* menu.c */
int mainmenu();
int updoneline();		  /* display.c */
int initmenus();
