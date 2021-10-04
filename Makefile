.POSIX:
.SUFFIXES:
.SUFFIXES: .c .o
.PHONY: all clean

include config.mk
LDFLAGS2=$(LDFLAGS) -lmng

SRC=main.c mng_callbacks.c mng_userdata.c
OBJ=$(SRC:.c=.o)

all: mng2png

clean:
	rm -f mng2png $(OBJ)

mng2png: $(OBJ)
	$(CC) $(LDFLAGS2) -o $@ $(OBJ)

main.o: main.c types.h mng_callbacks.h mng_userdata.h
mng_userdata.h: types.h
mng_callbacks.o: mng_callbacks.c types.h mng_callbacks.h mng_userdata.h
mng_userdata.o: mng_userdata.c types.h mng_userdata.h

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<
