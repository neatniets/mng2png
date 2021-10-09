.POSIX:
.SUFFIXES:
.SUFFIXES: .c .o
.PHONY: all clean

include config.mk
LDFLAGS2=$(LDFLAGS) -lmng

SRC=main.c image.c malloc2.c
OBJ=$(SRC:.c=.o)

all: mng2png

clean:
	rm -f mng2png $(OBJ)

mng2png: $(OBJ)
	$(CC) $(LDFLAGS2) -o $@ $(OBJ)

main.o: main.c types.h err.h image.h
malloc2.o: malloc2.c malloc2.h err.h
image.o: image.c image.h err.h malloc2.h types.h

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<
