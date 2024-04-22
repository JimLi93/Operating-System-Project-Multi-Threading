# Programming Project Checkpoint 2

## Content

A preemptive multithreading package and a test case based on the single-buffer producer-consumer example.

## Code Specification

`testpreempt.c` :  
Contains startup code and sets up the producer-consumer example.

`preemptive.c preemptive.h` :     
Contains the preemptive threading code.

## Compile

`sdcc -c testpreempt.c`

`sdcc -c preemptive.c`

`sdcc -o testpreempt.hex testpreempt.rel preemptive.rel`