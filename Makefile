# compiler settings
C_COMPILER   = gcc 
C_OPTIONS    = -Wall -g -std=c99 -pedantic

# Clean settings
GEN_EXTENSIONS = *.exe
AUX_EXTENSIONS = *.o

# tar bort onödiga filer
clean:
	rm -f *.o *.out *.orig manual unittests
# kör och kompilerar imalloc
run:
	$(C_COMPILER) $(C_OPTIONS) imalloc.c -o imalloc.out

# Kör testerna
test: unittests.c
	$(C_COMPILER) $(C_OPTIONS) unittests.c imalloc.c freelist.c priv_imalloc.h rc.c Garbage.c rootset.c -o unittests -lcunit
	./unittests
# fixar all kod till kodstandarden
beautify:
	astyle -A2 *.c *.h

fin:
	astyle -A2 *.c *.h
