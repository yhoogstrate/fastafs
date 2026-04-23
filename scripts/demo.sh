#!/bin/bash

wget https://hgdownload.cse.ucsc.edu/goldenpath/hg19/bigZips/hg19.fa.gz
gunzip hg19.fa.gz

fastafs cache hg19 hg19.fa
fastafs list

mkdir -p /bio/data/genomes/hg19
fastafs mount hg19 /bio/data/genomes/hg19

fastaf ps



