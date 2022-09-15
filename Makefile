CC=gcc
CFLAGS = -std=c11 -O3 -Wall
objects = line.o file_io.o

albaform: albaform.c $(objects)
	$(CC) $(CFLAGS) $^ -o $@

$(objects): %.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY : clean
clean:
	rm *.o
	rm albaform