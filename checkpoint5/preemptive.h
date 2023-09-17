/*
 * file: cooperative.h
 *
 * this is the include file for the cooperative multithreading
 * package.  It is to be compiled by SDCC and targets the EdSim51 as
 * the target architecture.
 *
 * CS 3423 Fall 2018
 */

#ifndef __PREEMPTIVE_H__
#define __PREEMPTIVE_H__

#define MAXTHREADS 4  /* not including the scheduler */
/* the scheduler does not take up a thread of its own */

#define CNAME(s) _ ## s
#define CON1(a, b) a ## b
#define CON2(a,b) CON1(a,b)
#define CONCATZERO(b) CON2(ZERO, b)
#define ZERO #0

#define MONEY $
#define CONCATMONEY(a) CON2(a, MONEY)

typedef char ThreadID;
typedef void (*FunctionPtr)(void);

ThreadID ThreadCreate(FunctionPtr);
void ThreadYield(void);
void ThreadExit(void);


#define SemaphoreCreate(s, n){ \
    __asm\
    MOV CNAME(s), CONCATZERO(n)\
    __endasm;\
}\

#define SemaphoreSignal(s){\
    __asm\
    INC CNAME(s)\
    __endasm;\
}\

#define SemaphoreWait(s){\
    SemaphoreWaitBody(s, __COUNTER__)\
}

#define SemaphoreWaitBody(s, label){\
    __asm\
    CONCATMONEY(label): \
    MOV ACC, CNAME(s)\
    JB ACC.7, CONCATMONEY(label)\
    JZ CONCATMONEY(label)\
    DEC CNAME(s)\
    __endasm;\
}
unsigned char now(void);

void delay(unsigned char n);

__data __at (0x30) char t0_sp;
__data __at (0x31) char t1_sp;
__data __at (0x32) char t2_sp;
__data __at (0x33) char t3_sp;
__data __at (0x34) int bitmap;
__data __at (0x36) ThreadID cur_tid;
__data __at (0x37) char save_sp;
__data __at (0x38) ThreadID target_tid;
__data __at (0x39) unsigned char ttt;
__data __at (0x3A) unsigned char mytime;

__data __at (0x20) char in1; //20,21,22,23,24
__data __at (0x21) char in2; //20,21,22,23,24
__data __at (0x22) char in3; //20,21,22,23,24
__data __at (0x23) char in4; //20,21,22,23,24
__data __at (0x24) char in5; //20,21,22,23,24


__data __at (0x25) char out1; //25,26,27,28,29
__data __at (0x26) char out2; //25,26,27,28,29
__data __at (0x27) char out3; //25,26,27,28,29
__data __at (0x28) char out4; //25,26,27,28,29
__data __at (0x29) char out5; //25,26,27,28,29

__data __at (0x2A) char spot1; //2A,2B,2C,2D,2E
__data __at (0x2B) char spot2; 
__data __at (0x2C) char spot3; 
__data __at (0x2D) char spot4; 
__data __at (0x2E) char spot5; 

__data __at (0x3D) char delay_arr1; //3D,3E,3F
__data __at (0x3E) char delay_arr2; //3D,3E,3F
__data __at (0x3F) char delay_arr3; //3D,3E,3F

__data __at (0x3C) char spot_mutex;

__data __at (0x2F) char available;

__data __at (0x3B) char car4_tid;
__data __at (0x1D) char car5_tid;

void printLOG(void);

#endif //__PREEMPTIVE_H__

