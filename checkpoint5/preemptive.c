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

      mytime = 0;
      delay_arr1 = 0;
      delay_arr2 = 0;
      delay_arr3 = 0;

      in1 = 0xFF;
      in2 = 0xFF;
      in3 = 0xFF;
      in4 = 0xFF;
      in5 = 0xFF;
      

      out1 = 0xFF;
      out2 = 0xFF;
      out3 = 0xFF;
      out4 = 0xFF;
      out5 = 0xFF;

      spot1 = 2;
      spot2 = 2;
      spot3 = 2;
      spot4 = 2;
      spot5 = 2;

      car4_tid = 10;
      car5_tid = 10;

      available = 0;

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
      //do {
                /*
                 * @@@ [8 pts] do round-robin policy for now.
                 * find the next thread that can run and 
                 * set the current thread ID to it,
                 * so that it can be restored (by the last line of 
                 * this function).
                 * there should be at least one thread, so this loop
                 * will always terminate.
                 */

      if(available == 3){
         if(out1 == 0xFF && delay_arr1 == 0) cur_tid = 1;
         else if(out2 == 0xFF && delay_arr2 == 0) cur_tid = 2;
         else if(out3 == 0xFF && delay_arr3 == 0) cur_tid = 3;
         else if(out4 == 0xFF && car4_tid == 1 && delay_arr1 == 0) cur_tid = car4_tid;
         else if(out4 == 0xFF && car4_tid == 2 && delay_arr2 == 0) cur_tid = car4_tid;
         else if(out4 == 0xFF && car4_tid == 3 && delay_arr3 == 0) cur_tid = car4_tid;
         else if(out5 == 0xFF && car5_tid == 1 && delay_arr1 == 0) cur_tid = car5_tid;
         else if(out5 == 0xFF && car5_tid == 2 && delay_arr2 == 0) cur_tid = car5_tid;
         else if(out5 == 0xFF && car5_tid == 3 && delay_arr3 == 0) cur_tid = car5_tid;
         else {
            cur_tid = (cur_tid + 1) % 3;
         }
      }
      else {
         if(in1 == 0xFF) cur_tid = 1;
         else if(in2 == 0xFF) cur_tid = 2;
         else if(in3 == 0xFF) cur_tid = 3;
         else if(in4 == 0xFF && car4_tid != 10) cur_tid = car4_tid;
         else if(in5 == 0xFF && car5_tid != 10) cur_tid = car5_tid;
         else {
            cur_tid = (cur_tid + 1) % 3;
         }
      }

        
      //} while (1);
      RESTORESTATE;
      EA = 1;
}

void myTimer0Handler() {
      EA = 0;
      SAVESTATE;
      mytime = mytime + 1;
      if(delay_arr1 > 0) delay_arr1--;
      if(delay_arr2 > 0) delay_arr2--;
      if(delay_arr3 > 0) delay_arr3--;
      //do {
                /*
                 * @@@ [8 pts] do round-robin policy for now.
                 * find the next thread that can run and 
                 * set the current thread ID to it,
                 * so that it can be restored (by the last line of 
                 * this function).
                 * there should be at least one thread, so this loop
                 * will always terminate.
                 */

      if(available == 3){
         if(out1 == 0xFF && delay_arr1 == 0) cur_tid = 1;
         else if(out2 == 0xFF && delay_arr2 == 0) cur_tid = 2;
         else if(out3 == 0xFF && delay_arr3 == 0) cur_tid = 3;
         else if(out4 == 0xFF && car4_tid == 1 && delay_arr1 == 0) cur_tid = car4_tid;
         else if(out4 == 0xFF && car4_tid == 2 && delay_arr2 == 0) cur_tid = car4_tid;
         else if(out4 == 0xFF && car4_tid == 3 && delay_arr3 == 0) cur_tid = car4_tid;
         else if(out5 == 0xFF && car5_tid == 1 && delay_arr1 == 0) cur_tid = car5_tid;
         else if(out5 == 0xFF && car5_tid == 2 && delay_arr2 == 0) cur_tid = car5_tid;
         else if(out5 == 0xFF && car5_tid == 3 && delay_arr3 == 0) cur_tid = car5_tid;
         else {
            cur_tid = (cur_tid + 1) % 3;
         }
      }
      else {
         if(in1 == 0xFF) cur_tid = 1;
         else if(in2 == 0xFF) cur_tid = 2;
         else if(in3 == 0xFF) cur_tid = 3;
         else if(in4 == 0xFF && car4_tid != 10) cur_tid = car4_tid;
         else if(in5 == 0xFF && car5_tid != 10) cur_tid = car5_tid;
         else {
            cur_tid = (cur_tid + 1) % 3;
         }
      }

        
      //} while (1);
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
        bitmap = bitmap - (1 << cur_tid);
        if(car4_tid == 10) car4_tid = cur_tid;
        else if(car5_tid == 10) car5_tid = cur_tid;
        if(bitmap == 1){
         printLOG();
         while(1){}
        }
        cur_tid = 0;
        RESTORESTATE;
        EA = 1;
}

unsigned char now(void){
   return mytime;
}

void delay(unsigned char n){
   EA = 0;
   ttt = n;
   if(cur_tid == 1){
      delay_arr1 = ttt;
   }
   else if(cur_tid == 2){
      delay_arr2 = ttt;
   }
   else if(cur_tid == 3){
      delay_arr3 = ttt;
   }
   EA = 1;
}

void printCAR(void){
      while(!TI){}
      SBUF = 'C';
      TI = 0;
      while(!TI){}
      SBUF = 'a';
      TI = 0;
      while(!TI){}
      SBUF = 'r';
      TI = 0;
}

void printSPOT(void){
      while(!TI){}
      SBUF = ':';
      TI = 0;
      while(!TI){}
      SBUF = 'S';
      TI = 0;
      while(!TI){}
      SBUF = 'p';
      TI = 0;
      while(!TI){}
      SBUF = 'o';
      TI = 0;
      while(!TI){}
      SBUF = 't';
      TI = 0;
}

void printNUM(int input){
      if(input >= 100){
         while(!TI){}
         SBUF = '0' + ((input - input % 100) / 100);
         TI = 0;
      }
      input = input % 100;
      if(input >= 10){
         while(!TI){}
         SBUF = '0' + ((input - input % 10) / 10);
         TI = 0;
      }
      input = input % 10;
      while(!TI){}
      SBUF = '0' + input;
      TI = 0;
}

void printLOG(void){
      TMOD |= 0x20;
      TH1 = (char)-6;
      SCON = 0x50;
      TR1 = 1;
      TI = 1;

      //Car1
      printCAR();
      while(!TI){}
      SBUF = '1';
      TI = 0;
      printSPOT();
      while(!TI){}
      SBUF = '0' + spot1;
      TI = 0;
      while(!TI){}
      SBUF = '{';
      TI = 0;
      printNUM(in1);
      while(!TI){}
      SBUF = '~';
      TI = 0;
      printNUM(out1);
      while(!TI){}
      SBUF = '}';
      TI = 0;
      while(!TI){}
      SBUF = ' ';
      TI = 0;

      //Car2
      printCAR();
      while(!TI){}
      SBUF = '2';
      TI = 0;
      printSPOT();
      while(!TI){}
      SBUF = '0' + spot2;
      TI = 0;
      while(!TI){}
      SBUF = '{';
      TI = 0;
      printNUM(in2);
      while(!TI){}
      SBUF = '~';
      TI = 0;
      printNUM(out2);
      while(!TI){}
      SBUF = '}';
      TI = 0;
      while(!TI){}
      SBUF = ' ';
      TI = 0;

      //Car3
      printCAR();
      while(!TI){}
      SBUF = '3';
      TI = 0;
      printSPOT();
      while(!TI){}
      SBUF = '0' + spot3;
      TI = 0;
      while(!TI){}
      SBUF = '{';
      TI = 0;
      printNUM(in3);
      while(!TI){}
      SBUF = '~';
      TI = 0;
      printNUM(out3);
      while(!TI){}
      SBUF = '}';
      TI = 0;
      while(!TI){}
      SBUF = ' ';
      TI = 0;

      //Car4
      printCAR();
      while(!TI){}
      SBUF = '4';
      TI = 0;
      printSPOT();
      while(!TI){}
      SBUF = '0' + spot4;
      TI = 0;
      while(!TI){}
      SBUF = '{';
      TI = 0;
      printNUM(in4);
      while(!TI){}
      SBUF = '~';
      TI = 0;
      printNUM(out4);
      while(!TI){}
      SBUF = '}';
      TI = 0;
      while(!TI){}
      SBUF = ' ';
      TI = 0;

      //Car5
      printCAR();
      while(!TI){}
      SBUF = '5';
      TI = 0;
      printSPOT();
      while(!TI){}
      SBUF = '0' + spot5;
      TI = 0;
      while(!TI){}
      SBUF = '{';
      TI = 0;
      printNUM(in5);
      while(!TI){}
      SBUF = '~';
      TI = 0;
      printNUM(out5);
      while(!TI){}
      SBUF = '}';
      TI = 0;
      while(!TI){}
      SBUF = ' ';
      TI = 0;


}



