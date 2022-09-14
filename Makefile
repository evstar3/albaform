CC=gcc
CFLAGS=-std=c11 -O3 -Wall

albaform: albaform.c line.o file_io.o
	$(CC) $(CFLAGS) albaform.c line.o file_io.o -o albaform

line.o: line.c line.h
	$(CC) $(CFLAGS) -c line.c -o line.o

file_io.o: file_io.c file_io.h
	$(CC) $(CFLAGS) -c file_io.c -o file_io.o

clean:
	rm *.o
	rm albaform