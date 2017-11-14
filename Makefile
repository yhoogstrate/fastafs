CC=g++
CFLAGS=-g -Wall -O2 -Wno-unused-function -std=c++11

all: fastqfs

fastqfs:
	$(CC) $(CFLAGS) -o bin/fastafs src/main.cpp

