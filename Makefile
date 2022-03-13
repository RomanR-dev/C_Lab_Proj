flags = -Wall -pedantic -ansi
misc = misc/definitions.h misc/definitions.c misc/parsers.h misc/parsers.c misc/utils.h misc/utils.c
passes = passes/first_pass.h passes/first_pass.c passes/second_pass.h passes/second_pass.c passes/pre_assembler.h passes/pre_assembler.c
dotCfiles = misc/definitions.c misc/parsers.c misc/utils.c passes/first_pass.c passes/second_pass.c passes/pre_assembler.c

assembler: ${misc} ${passes}
	gcc ${flags} -o assembler ${dotCfiles} assembler.c -lm