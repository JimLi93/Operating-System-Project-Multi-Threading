# Programming Project Checkpoint 4

## Content

Test code using 2-producers, 1 consumer example.

## Code Specification

`test3threads.c` :  
Contains startup code and sets up 2-producers, 1 consumer example.

`preemptive.c preemptive.h` :     
Contains the preemptive threading code with semaphore and busy wait.

## Compile

`sdcc -c test3threads.c`

`sdcc -c preemptive.c`

`sdcc -o test3threads.hex test3threads.rel preemptive.rel`