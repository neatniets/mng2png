.POSIX:
.SUFFIXES:
.SUFFIXES: .c .o
.PHONY: all clean install uninstall

include config.mk

SRC=main.c image.c malloc2.c mngfile.c verbose.c pngfile.c
OBJ=$(SRC:.c=.o)

all: mng2png

clean:
	rm -f mng2png $(OBJ)
install: all
	mkdir -p $(BIN)
	cp -f mng2png $(BIN)/mng2png
	chmod 755 $(BIN)/mng2png
	mkdir -p $(MAN1)
	cp -f mng2png.1 $(MAN1)/mng2png.1
	chmod 644 $(MAN1)/mng2png.1
uninstall:
	rm -f $(BIN)/mng2png
	rm -f $(MAN1)/mng2png.1

mng2png: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)

main.o: main.c types.h err.h image.h mngfile.h verbose.h pngfile.h
malloc2.o: malloc2.c malloc2.h err.h
image.o: image.c image.h err.h malloc2.h types.h
mngfile.o: mngfile.c mngfile.h types.h err.h image.h verbose.h
verbose.o: verbose.c verbose.h
pngfile.o: pngfile.c pngfile.h err.h types.h

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<
