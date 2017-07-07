#!/bin/bash
for i in `cat data/data.lst`;do
	sfbcep -l 20 -d 10 -w Hamming -p 16 -e -D -k 0 -i 300 -u 3400 -F WAVE data/wave/$i.wav data/prm/$i.tmp.prm
done
