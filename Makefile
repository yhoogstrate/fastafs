CC=g++
CFLAGS=-g -Wall -Wextra -O2 -Wno-unused-function -std=c++11 -Iinclude

all: fastqfs

build/TwoBitByte.o:
	$(CC) $(CFLAGS) -c -o build/TwoBitByte.o src/TwoBitByte.cpp

build/Fasta.o: build/TwoBitByte.o
	$(CC) $(CFLAGS) -c -o build/Fasta.o src/Fasta.cpp



fastqfs: build/Fasta.o build/TwoBitByte.o
	$(CC) $(CFLAGS) -o bin/fastafs src/main.cpp build/Fasta.o build/TwoBitByte.o

clean:
	rm -rf build/*.o
	rm -rf bin/fastafs



tidy:
	astyle --style=linux include/*.hpp
	astyle --style=linux src/*.cpp
	astyle --style=linux test/*.cpp
