CC=g++
CFLAGS=-g -Wall -O2 -Wno-unused-function -std=c++11

all: fastqfs

build/Fasta.o:
	$(CC) $(CFLAGS) -c -o build/Fasta.o src/Fasta.cpp

fastqfs: build/Fasta.o
	$(CC) $(CFLAGS) -o bin/fastafs src/main.cpp build/Fasta.o

