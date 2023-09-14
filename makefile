# Compilation Macros Defnitions
CC = gcc
CFLAGS = -ansi -Wall -pedantic
GLOBAL_DEPS = constants.h lib.h
EXE_DEPS = assembler.o preprocessor.o first_run.o second_run.o utils.o table.o

# EXE File
assembler: $(EXE_DEPS) $(GLOBAL_DEPS)
		$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@

# Utils
utils.o: utils.c utils.h $(GLOBAL_DEPS)
		$(CC) -c utils.c $(CFLAGS) -o $@

# Table
table.o: table.c table.h $(GLOBAL_DEPS)
		$(CC) -c table.c $(CFLAGS) -o $@

# Preprocessor
preprocessor.o: preprocessor.c preprocessor.h $(GLOBAL_DEPS)
		$(CC) -c preprocessor.c $(CFLAGS) -o $@

# First Run
first_run.o: first_run.c first_run.h $(GLOBAL_DEPS)
		$(CC) -c first_run.c $(CFLAGS) -o $@

# Second Run
second_run.o: second_run.c second_run.h $(GLOBAL_DEPS)
		$(CC) -c second_run.c $(CFLAGS) -o $@

# Main
assembler.o: assembler.c $(GLOBAL_DEPS)
		$(CC) -c assembler.c $(CFLAGS) -o $@

# Clean
clean:
		rm -rf *.o
