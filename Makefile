CC=		cc
CFLAGS=		-g -Wall -O2
CPPFLAGS=
INCLUDES=   
PROG=		taxon-utils
BINDIR=		/usr/local/bin
LIBS=		-lz
OBJS=		translate.o lineage.o name.o rank.o lca.o bin.o clade.o filter.o level.o taxid.o kstring.o utils.c

.SUFFIXES:.c .o
.PHONY:all clean

.c.o:
		$(CC) -c $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $< -o $@

all:$(PROG)

taxon-utils:$(OBJS) taxon-utils.o
		$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

install:all
		install taxon-utils $(BINDIR)

clean:
		rm -fr *.o taxon-utils