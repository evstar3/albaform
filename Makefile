CC=gcc
CFLAGS=-std=c11 -O3

albaform: albaform.c
	$(CC) $(CFLAGS) albaform.c -o albaform