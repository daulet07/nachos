#!/bin/bash

echo Format filesystem
./nachos-step5 -f
echo Copy shell
./nachos-step5 -cp shell /shell

./nachos-step5 -mkdir /IO
./nachos-step5 -cp getchar /IO/getchar
./nachos-step5 -cp putchar /IO/putchar
./nachos-step5 -cp getstring /IO/getstring
./nachos-step5 -cp putstring /IO/putstring
./nachos-step5 -cp getint /IO/getint
./nachos-step5 -cp putint /IO/putint

./nachos-step5 -mkdir /vm
./nachos-step5 -cp thread /vm/thread
./nachos-step5 -cp sem /vm/sem
./nachos-step5 -cp pc /vm/pc
#./nachos-step5 -cp multiprocess /vm/multiprocess
#./nachos-step5 -cp multithread /vm/multithreads
./nachos-step5 -cp step4_1 /vm/step4
./nachos-step5 -cp step4_2 /vm/step4MP

./nachos-step5 -mkdir /filesys
./nachos-step5 -cp filesys /filesys/filesys

#for prog in `find -type f -executable`; do
#	./nachos-step5 -cp $prog /bin${prog:1}
#done

#./nachos-step5 -cp shell /shell
#./nachos-step5 -cp muli
#./nachos-step5 -f
#./nachos-step5 -f
