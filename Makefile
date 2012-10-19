# compiler settings
C_COMPILER   = gcc
C_OPTIONS    = -Wall

# Clean settings
GEN_EXTENSIONS = *.exe
AUX_EXTENSIONS = *.o




lintcheck:
	lint hello.c

clean:
	rm -f *.o *.out

test:
	$(C_COMPILER) $(C_OPTIONS) imalloc.c -o imalloc.out



