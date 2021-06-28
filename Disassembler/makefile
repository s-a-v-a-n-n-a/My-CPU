CFLAGS = -g -masm=intel -no-pie -march=native -O1

all: MainDisasm.o Disasm.o ReadingFromFile.o
	g++ MainAssembler.o Assembler.o -o disasm

Disasm.o: Disassembler.c
	g++ $(CFLAGS) -c -o Disasm.o Disassembler.c

MainDisasm.o: MainDisassembler.cpp
	g++ $(CFLAGS) -c -o MainDisasm.o MainDisassembler.cpp

ReadingFromFile.o: ../FileWork/ReadingFromFile.cpp
	g++ $(CFLAGS) -c -o ReadingFromFile.o ReadingFromFile.cpp

run: all
	valgrind ./disasm