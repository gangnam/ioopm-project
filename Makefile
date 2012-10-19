# compiler settings
C_COMPILER   = gcc
C_OPTIONS    = -Wall

# Clean settings
GEN_EXTENSIONS = *.exe
AUX_EXTENSIONS = *.o


clean:
	rm -f *.o *.out

run:
	$(C_COMPILER) $(C_OPTIONS) imalloc.c -o imalloc.out

test: unittests.c imalloc.c priv_imalloc.h
	$(C_COMPILER) $(C_OPTIONS) unittests.c queue.c -o unittests -lcunit
	./unittests

beautify:
	astyle -A7 *.c *.h

