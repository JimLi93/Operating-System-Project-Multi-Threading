# Programming Project Checkpoint 3

## Content

Implement semaphore with busy wait for preemptive multithreading.

Test code using the classical bounded-buffer example.

## Code Specification

`testpreempt.c` :  
Contains startup code and sets up the classical bounded-buffer example.

`preemptive.c preemptive.h` :     
Contains the preemptive threading code with semaphore and busy wait.

## Compile

`sdcc -c testpreempt.c`

`sdcc -c preemptive.c`

`sdcc -o testpreempt.hex testpreempt.rel preemptive.rel`