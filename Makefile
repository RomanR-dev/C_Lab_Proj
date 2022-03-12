
# Basic compilation macros
CC = gcc # GCC Compiler
CFLAGS = -ansi -Wall -pedantic # Flags
GLOBAL_DEPS = misc/utils.h misc/parsers.h misc/definitions.h # Dependencies for everything
EXE_DEPS = assembler.o passes/pre_assembler.o passes/second_pass.o passes/first_pass.o misc/parsers.o misc/utils.o misc/definitions.o # Deps for exe

assembler: $(EXE_DEPS) $(GLOBAL_DEPS)
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@

assembler.o: assembler.c $(GLOBAL_DEPS)
	$(CC) -c assembler.c $(CFLAGS) -o $@

code.o: utils.c utils.h $(GLOBAL_DEPS)
	$(CC) -c utils.c $(CFLAGS) -o $@

fpass.o: first_pass.c first_pass.h $(GLOBAL_DEPS)
	$(CC) -c first_pass.c $(CFLAGS) -o $@

spass.o: second_pass.c second_pass.h $(GLOBAL_DEPS)
	$(CC) -c second_pass.c $(CFLAGS) -o $@

instructions.o: parsers.c parsers.h $(GLOBAL_DEPS)
	$(CC) -c parsers.c $(CFLAGS) -o $@

table.o: definitions.c definitions.h $(GLOBAL_DEPS)
	$(CC) -c definitions.c $(CFLAGS) -o $@

utils.o: utils.c instructions.h $(GLOBAL_DEPS)
	$(CC) -c utils.c $(CFLAGS) -o $@

writefiles.o: pre_assembler.c pre_assembler.h $(GLOBAL_DEPS)
	$(CC) -c pre_assembler.c $(CFLAGS) -o $@

# Clean Target (remove leftovers)
clean:
	rm -rf *.o