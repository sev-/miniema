#
#  $Id: makefile,v 1.5 1996/04/08 09:30:05 sev Exp $
#
# ---------------------------------------------------------- 
#
# $Log: makefile,v $
# Revision 1.5  1996/04/08 09:30:05  sev
# hmm..
#
# Revision 1.4  1994/08/15  20:42:11  sev
# Indented
#
# Revision 1.3  1994/06/28  16:27:33  sev
# Last time I forgot english.h :-(
#
# Revision 1.2  1994/06/24  17:22:21  sev
# added menu.c
#
# Revision 1.1  1994/06/24  14:17:12  sev
# Initial revision
#
#
#
 
CC=gcc
CFLAGS=

OFILES=		basic.o buffer.o display.o\
		file.o fileio.o input.o line.o main.o\
		random.o tcap.o unix.o

CFILES=		basic.c buffer.c display.c\
		file.c fileio.c input.c line.c main.c\
		random.c tcap.c unix.c

HFILES=		estruct.h etype.h english.h

dynamic:	$(OFILES)
		$(CC) $(CFLAGS) -o dynamic $(OFILES) -ltermcap -lc

$(OFILES):	$(HFILES)

main.o:		ebind.h edef.h

clean:
		rm -f $(OFILES) core *.b
		ci $(CFILES) $(HFILES) makefile ema
