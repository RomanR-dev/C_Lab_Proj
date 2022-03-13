<b>CI status:</b><br>
![CI](https://github.com/RomanR-dev/C_Lab_Proj/workflows/CI/badge.svg?branch=main) <br>
##
Systems lab course in the open university:
    C language (C90) - project

## Files
1. `assembler` - main function definition, argv & argc processing, single file processing.
2. `pre_asemlber` - contains functions for processing macro's in original file and unpacking.
3. `first_pass` - contains functions for processing a single line in the first pass and a code line in the first pass.
4. `second_pass` - contains function for processing a single line in second pass and replacing symbol by it's address.
5. `definitions` - contains type and constant definitions.
6. `parsers` - contains useful function for parsing data.
7. `utils` - contains useful function for processing code.
## Build
1. Using `make` command (Makefile)
## Usage
`./assembler file1 file2 file3 ...`
Will start the processing of each file (specify WITHOUT extension!).
