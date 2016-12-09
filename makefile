all: compressR compressT

compressR:
	gcc -std=c99 -o compressR compressR_LOLS.c compressR_worker_LOLS.c
compressT:
	gcc -pthread -std=c99 -o compressT compressT_LOLS.c compress.c
clean:
	find . -type f -iname "*_LOLS*" ! -iname "*.c" -delete
