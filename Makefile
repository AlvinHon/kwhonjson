CC = gcc
CFLAGS = -g -Wall
SRCS = jVoid.c jPATH.c tJSON.c aHEAP.c jParseExpr.c jParser.c jUtil.c

OBJS = $(SRCS:.c=.o)
BINDIR=bin
BINOBJS = $(OBJS:%.o=$(BINDIR)/%.o)
MAIN = cjson

.PHONY: depend clean

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -c $(MAIN).c -o $(BINDIR)/$(MAIN).o
	$(CC) $(CFLAGS) -o $(MAIN) $(BINOBJS) $(BINDIR)/$(MAIN).o

library: $(OBJS)
	ar -cvq lib/kwhonjson.a $(BINOBJS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $(BINDIR)/$@

clean:
	rm ${BINDIR}/*.o lib/*.a $(MAIN)

depend: $(SRCS)
	makedepend $^
	# DO NOT DELETE
