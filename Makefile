CC=gcc
CFLAGS=-g -I.
DEPS = xci.h hfs0.h sha256.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

xcimake: xci.o hfs0.o sha256.o
	$(CC) -o xci xci.o hfs0.o sha256.o

.PHONY: clean

clean:
	rm -f *.o 
