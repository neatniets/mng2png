.POSIX:
.SUFFIXES:
.SUFFIXES: .c .o
.PHONY: all clean

include config.mk
LDFLAGS2=$(LDFLAGS) -lmng

SRC=main.c mng_callbacks.c
OBJ=$(SRC:.c=.o)

all: mng2png

clean:
	rm -f mng2png $(OBJ)

mng2png: $(OBJ)
	$(CC) $(LDFLAGS2) -o $@ $(OBJ)

mng_callbacks.o: mng_callbacks.c mng_callbacks.h
main.o: main.c mng_callbacks.h
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<
