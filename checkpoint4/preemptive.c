#include <8051.h>

#include "preemptive.h"

/*
 * @@@ [2 pts] declare the static globals here using 
 *        __data __at (address) type name; syntax
 * manually allocate the addresses of these variables, for
 * - saved stack pointers (MAXTHREADS)
 * - current thread ID
 * - a bitmap for which thread ID is a valid thread; 
 *   maybe also a count, but strictly speaking not necessary
 * - plus any temporaries that you need.
 */
__data __at (0x30) char t0_sp;
__data __at (0x31) char t1_sp;
__data __at (0x32) char t2_sp;
__data __at (0x33) char t3_sp;
__data __at (0x34) int bitmap;
__data __at (0x36) ThreadID cur_tid;
__data __at (0x37) char save_sp;
__data __at (0x38) ThreadID target_tid;

__data __at (0x23) char LRU;
/*
 * @@@ [8 pts]
 * define a macro for saving the context of the current thread by
 * 1) push ACC, B register, Data pointer registers (DPL, DPH), PSW
 * 2) save SP into the saved Stack Pointers array
 *   as indexed by the current thread ID.
 * Note that 1) should be written in assembly, 
 *     while 2) can be written in either assembly or C
 */
#define SAVESTATE \
{  __asm\
      PUSH ACC\
      PUSH B\
      PUSH DPL\
      PUSH DPH\
      PUSH PSW\
      mov A, #0x30\
      add A,_cur_tid\
      mov r0, A\
      mov @r0, SP\
   __endasm;\
}

/*
 * @@@ [8 pts]
 * define a macro for restoring the context of the current thread by
 * essentially doing the reverse of SAVESTATE:
 * 1) assign SP to the saved SP from the saved stack pointer array
 * 2) pop the registers PSW, data pointer registers, B reg, and ACC
 * Again, popping must be done in assembly but restoring SP can be
 * done in either C or assembly.
 */
#define RESTORESTATE \
{  __asm\
      mov A, #0x30\
      add A,_cur_tid\
      mov r0, A\
      mov SP, @r0\
      POP PSW\
      POP DPH\
      POP DPL\
      POP B\
      POP ACC\
   __endasm;\
}


 /* 
  * we declare main() as an extern so we can reference its symbol
  * when creating a thread for it.
  */

extern void main(void);

/*
 * Bootstrap is jumped to by the startup code to make the thread for
 * main, and restore its context so the thread can run.
 */

void Bootstrap(void) {
      /*
       * @@@ [2 pts] 
       * initialize data structures for threads (e.g., mask)
       *
       * optional: move the stack pointer to some known location
       * only during bootstrapping. by default, SP is 0x07.
       *
       * @@@ [2 pts]
       *     create a thread for main; be sure current thread is
       *     set to this thread ID, and restore its context,
       *     so that it starts running main().
       */
      TMOD = 0; // timer0 mode 0
      IE = 0x82; // enable timer 0 interrupt;keep consumer polling
      TR0 = 1; // set bit TR0 to start running timer0

      bitmap = 0;
      t0_sp = 0x3F;
      t1_sp = 0x4F;
      t2_sp = 0x5F;
      t3_sp = 0x6F;
      cur_tid = ThreadCreate(&main);
      RESTORESTATE;
}

/*
 * ThreadCreate() creates a thread data structure so it is ready
 * to be restored (context switched in).
 * The function pointer itself should take no argument and should
 * return no argument.
 */
ThreadID ThreadCreate(FunctionPtr fp) {
      EA = 0;
      /*
      * @@@ [2 pts] 
      * check to see we have not reached the max #threads.
      * if so, return -1, which is not a valid thread ID.
      */
      if(bitmap == 15) return -1;
      //char target_tid = 0;
      //c. save the current SP in a temporary
      __asm
         MOV _save_sp, SP
      __endasm;

      /*
       * @@@ [5 pts]
       *     otherwise, find a thread ID that is not in use,
       *     and grab it. (can check the bit mask for threads),
       *
       * @@@ [18 pts] below
       * a. update the bit mask 
            (and increment thread count, if you use a thread count, 
            but it is optional)
         b. calculate the starting stack location for new thread
      */
      if(bitmap == 0 || bitmap == 2 || bitmap == 4 || bitmap == 6 || bitmap == 8 || bitmap == 10 || bitmap == 12 || bitmap == 14) {
         target_tid  = 0;
         bitmap = bitmap + 1;
         SP = 0x3F;
      } 
      else if(bitmap == 1 || bitmap == 5 || bitmap == 9 || bitmap == 13) {
         target_tid  = 1;
         bitmap = bitmap + 2;
         SP = 0x4F;
      } 
      else if(bitmap == 3 || bitmap == 11) {
         target_tid  = 2;
         bitmap = bitmap + 4;
         SP = 0x5F;
      } 
      else if(bitmap == 7) {
         target_tid  = 3;
         bitmap = bitmap + 8;
         SP = 0x6F;
      } 

      /*
      d. push the return address fp (2-byte parameter to
         ThreadCreate) onto stack so it can be the return
         address to resume the thread. Note that in SDCC
         convention, 2-byte ptr is passed in DPTR.  but
         push instruction can only push it as two separate
         registers, DPL and DPH.
      */
      __asm
         PUSH DPL
         PUSH DPH
      __endasm;

      /*
      e. we want to initialize the registers to 0, so we
         assign a register to 0 and push it four times
         for ACC, B, DPL, DPH.  Note: push #0 will not work
         because push takes only direct address as its operand,
         but it does not take an immediate (literal) operand.
      */
      __asm
            MOV ACC,#0
            PUSH ACC
            PUSH ACC
            PUSH ACC
            PUSH ACC
       __endasm;

      /*
      f. finally, we need to push PSW (processor status word)
         register, which consist of bits
         CY AC F0 RS1 RS0 OV UD P
         all bits can be initialized to zero, except <RS1:RS0>
         which selects the register bank.  
         Thread 0 uses bank 0, Thread 1 uses bank 1, etc.
         Setting the bits to 00B, 01B, 10B, 11B will select 
         the register bank so no need to push/pop registers
         R0-R7.  So, set PSW to 
         00000000B for thread 0, 00001000B for thread 1,
         00010000B for thread 2, 00011000B for thread 3.
      g. write the current stack pointer to the saved stack
         pointer array for this newly created thread ID
      */
      ACC = (target_tid << 3);
      __asm 
         PUSH ACC
         mov A, #0x30
         add A, _target_tid
         mov r0, A
         mov @r0, SP
      __endasm;

      //h. set SP to the saved SP in step c.
      __asm
         MOV sp, _save_sp
      __endasm;
         
      //i. finally, return the newly created thread ID.
      EA = 1;
      return target_tid ;
}



/*
 * this is called by a running thread to yield control to another
 * thread.  ThreadYield() saves the context of the current
 * running thread, picks another thread (and set the current thread
 * ID to it), if any, and then restores its state.
 */

void ThreadYield(void) {
      EA = 0;
      SAVESTATE;
      do {
                /*
                 * @@@ [8 pts] do round-robin policy for now.
                 * find the next thread that can run and 
                 * set the current thread ID to it,
                 * so that it can be restored (by the last line of 
                 * this function).
                 * there should be at least one thread, so this loop
                 * will always terminate.
                 */
      cur_tid = (cur_tid + 1) % 3;
      int temp = bitmap << (3-cur_tid);
      if(temp >= 8){break;}
        
      } while (1);
      RESTORESTATE;
      EA = 1;
}

void myTimer0Handler() {
      EA = 0;
      SAVESTATE;
      do {
                /*
                 * @@@ [8 pts] do round-robin policy for now.
                 * find the next thread that can run and 
                 * set the current thread ID to it,
                 * so that it can be restored (by the last line of 
                 * this function).
                 * there should be at least one thread, so this loop
                 * will always terminate.
                 */
      if(cur_tid == 0 && (LRU & 1) == 1){
         cur_tid = 1;
         LRU = LRU - 1;
         if(LRU == 0) LRU = 3;
      }
      else if(cur_tid == 0 && (LRU & 2) == 2){
         cur_tid = 2;
         LRU = LRU - 2;
         if(LRU == 0) LRU = 3;
      }
      else cur_tid = (cur_tid + 1) % 3;
      int temp = bitmap << (3-cur_tid);
      if(temp >= 8){break;}
        
      } while (1);
      RESTORESTATE;
      EA = 1;
      __asm
         RETI
      __endasm;
}


/*
 * ThreadExit() is called by the thread's own code to terminate
 * itself.  It will never return; instead, it switches context
 * to another thread.
 */
void ThreadExit(void) {
        /*
         * clear the bit for the current thread from the
         * bit mask, decrement thread count (if any),
         * and set current thread to another valid ID.
         * Q: What happens if there are no more valid threads?
         */
        EA = 0;
        RESTORESTATE;
        EA = 1;
}
