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

#endif //__PREEMPTIVE_H__