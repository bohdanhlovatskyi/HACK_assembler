gcc -c parser.c
gcc -c assembler.c
gcc -o compileHACK.out assembler.o parser.o