CFLAGS = -g -masm=intel -no-pie -march=native -O3

all: MainAssembler.o Assembler.o ReadingFromFile.o	
	g++ MainAssembler.o Assembler.o ReadingFromFile.o -o translator

MainAssembler.o: Assembler/MainAssembler.cpp
	g++ $(CFLAGS) -c -o MainAssembler.o  Assembler/MainAssembler.cpp

Assembler.o: Assembler/Assembler.cpp
	g++ $(CFLAGS) -c -o Assembler.o Assembler/Assembler.cpp

ReadingFromFile.o: FileWork/ReadingFromFile.cpp
	g++ $(CFLAGS) -c -o ReadingFromFile.o FileWork/ReadingFromFile.cpp

clean: 
	rm *.o

run: all
	valgrind ./translator "factorial.xax"

proc: ReadingFromFile.o Processor.o Main_proc.o Polystack.o
	g++ ReadingFromFile.o Processor.o Main_proc.o Polystack.o -o Proc

Processor.o: Processor/Processor.c
	g++ $(CFLAGS) -c -o Processor.o Processor/Processor.c

Main_proc.o: Processor/MainProcessor.cpp
	g++ $(CFLAGS) -c -o Main_proc.o Processor/MainProcessor.cpp

Polystack.o: Processor/PolyStack.cpp
	g++ $(CFLAGS) -c -o Polystack.o Processor/PolyStack.cpp

proc_all: proc
	time ./Proc "binary.xex"




