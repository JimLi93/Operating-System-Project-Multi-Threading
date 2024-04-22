# Programming Project Checkpoint 1

## Content

A cooperative multithreading package and a test case based on the single-buffer producer-consumer example.

## Code Specification

`testcoop.c` :  
Contains startup code and sets up the producer-consumer example.

`cooperative.c cooperative.h` :     
Contains the cooperative threading code.

## Compile

`sdcc -c testcoop.c`

`sdcc -c cooperative.c`

`sdcc -o testcoop.hex testcoop.rel cooperative.rel`