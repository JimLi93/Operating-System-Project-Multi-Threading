# Programming Project Checkpoint 5

## Content

Add a `delay(n)` function.

Implement thread recycling to free up spaces when terminated.

Test delay and thread recycling using the parking lot example.

## Code Specification

`testparking.c` :  
Contains the parking lot example.

`preemptive.c preemptive.h` :     
Contains `delay(n)` function, where `n` is the number of time units to delay.

## Compile

`sdcc -c testparking.c`

`sdcc -c preemptive.c`

`sdcc -o testparking.hex testparking.rel preemptive.rel`