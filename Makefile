# compiler settings
C_COMPILER   = gcc
C_OPTIONS    = -Wall

# Clean settings
GEN_EXTENSIONS = *.exe
AUX_EXTENSIONS = *.o

# tar bort onödiga filer
clean:
	rm -f *.o *.out *.orig
# kör och kompilerar imalloc
run:
	$(C_COMPILER) $(C_OPTIONS) imalloc.c -o imalloc.out

# Kör testerna
test: unittests.c imalloc.c imalloc.h
	$(C_COMPILER) $(C_OPTIONS) unittests.c imalloc.c -o unittests -lcunit
	./unittests
# fixar all kod till kodstandarden
beautify:
	astyle -A6 *.c *.h
# Backup använder ni an katalog som inte heter git får ni ändra.
backup:
	cp -R ~/git ~/projektbackup
