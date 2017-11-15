CC=g++
CFLAGS=-g -Wall -Wextra -O2 -Wno-unused-function -std=c++11 -Iinclude

all: fastqfs

build/TwoBitByte.o:
	$(CC) $(CFLAGS) -c -o build/TwoBitByte.o src/TwoBitByte.cpp

build/fasta.o: build/TwoBitByte.o
	$(CC) $(CFLAGS) -c -o build/fasta.o src/fasta.cpp



fastqfs: build/fasta.o build/TwoBitByte.o
	$(CC) $(CFLAGS) -o bin/fastafs src/main.cpp build/fasta.o build/TwoBitByte.o

clean:
	rm -rf build/*.o
	rm -rf bin/fastafs



tidy:
	astyle --style=linux include/*.hpp
	astyle --style=linux src/*.cpp
	#astyle --style=linux test/*.cpp
