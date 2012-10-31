# compiler settings
C_COMPILER   = gcc 
C_OPTIONS    = -Wall -g -std=c99

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
	$(C_COMPILER) $(C_OPTIONS) unittests.c imalloc.c freelist.c priv_imalloc.h rc.c Garbage.c rootset.c -o unittests -lcunit
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

random: imalloc.c imalloc.h manual.c manual.h priv_imalloc.h rc.c rc.h
	$(C_COMPILER) $(C_OPTIONS) imalloc.c manual.c rc.c -o random
	
