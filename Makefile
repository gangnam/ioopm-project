# compiler settings
C_COMPILER   = gcc
C_OPTIONS    = -Wall -g

# Clean settings
GEN_EXTENSIONS = *.exe
AUX_EXTENSIONS = *.o

# tar bort onödiga filer
clean:
	rm -f *.o *.out *.orig manual
# kör och kompilerar imalloc
run:
	$(C_COMPILER) $(C_OPTIONS) imalloc.c -o imalloc.out

# Kör testerna
test: unittests.c
	$(C_COMPILER) $(C_OPTIONS) unittests.c imalloc.c manual.c priv_imalloc.c rc.c -o unittests -lcunit
	./unittests
# fixar all kod till kodstandarden
beautify:
	astyle -A6 *.c *.h
# Backup använder ni an katalog som inte heter git får ni ändra.
backup:
	cp -R ~/git ~/projektbackup
#testkör manual.
manual:
	$(C_COMPILER) manual.c $(C_OPTIONS) -o manual
