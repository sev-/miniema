#
#  $Id: makefile,v 1.1 1994/06/24 14:17:12 sev Exp $
#
# ---------------------------------------------------------- 
#
# $Log: makefile,v $
# Revision 1.1  1994/06/24 14:17:12  sev
# Initial revision
#
#
#
 
CC=		rcc
CFLAGS=

OFILES=		basic.o buffer.o char.o display.o \
		file.o fileio.o input.o line.o main.o \
		random.o region.o search.o tcap.o unix.o

CFILES=		basic.c buffer.c char.c display.c \
		file.c fileio.c input.c line.c main.c \
		random.c region.c search.c tcap.c unix.c

HFILES=		estruct.h edef.h ebind.h etype.h

emacs1:		$(OFILES)
		$(CC) $(CFLAGS) -o emacs1 $(OFILES) -ltermcap -lc

$(OFILES):	$(HFILES)

clean:
		rm -f $(OFILES) core *.b
