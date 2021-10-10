.POSIX:
.SUFFIXES:
.SUFFIXES: .c .o
.PHONY: all clean

include config.mk
LDFLAGS2=$(LDFLAGS) -lmng

SRC=main.c image.c malloc2.c mngfile.c verbose.c
OBJ=$(SRC:.c=.o)

all: mng2png

clean:
	rm -f mng2png $(OBJ)

mng2png: $(OBJ)
	$(CC) $(LDFLAGS2) -o $@ $(OBJ)

main.o: main.c types.h err.h image.h mngfile.h verbose.h
malloc2.o: malloc2.c malloc2.h err.h
image.o: image.c image.h err.h malloc2.h types.h
mngfile.o: mngfile.c mngfile.h types.h err.h image.h verbose.h
verbose.o: verbose.c verbose.h

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<
