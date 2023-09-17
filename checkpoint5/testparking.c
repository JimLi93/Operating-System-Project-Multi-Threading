/* 
 * file: testcoop.c 
 */
#include <8051.h>
#include "preemptive.h"

/* 
 * @@@ [2pt] 
 * declare your global variables here, for the shared buffer 
 * between the producer and consumer.  
 * Hint: you may want to manually designate the location for the 
 * variable.  you can use
 *        __data __at (0x30) type var; 
 * to declare a variable var of the type
 */ 


/* [8 pts] for this function
 * the producer in this test program generates one characters at a
 * time from 'A' to 'Z' and starts from 'A' again. The shared buffer
 * must be empty in order for the Producer to write.
 */
void Car1(void) {
        while (1) {
                // Car first enter
                if(spot1 == 2){ 
                        SemaphoreWait(spot_mutex);
                        EA = 0;
                        //available = 1 means spot is parked
                        if(available == 0 || available == 1){
                                spot1 = 1;
                                available = available + 2;
                        }
                        else if(available == 2){
                                spot1 = 0;
                                available = available + 1;
                        }
                        //no parking spot
                        else ThreadYield();
                        in1 = now();
                        delay(5);
                        EA = 1;
                }
                // spot[i] = 0 or 1 meaning parking
                else if(out1 == 0xFF && spot1 != 2){
                        //delay not 0 not yet to leave spot
                        if(delay_arr1 > 0){
                                //break;
                                //ThreadYield();
                        }
                        // delay = 0 now leave the spot
                        else if(delay_arr1 == 0){
                                EA=0;
                                out1 = now();
                                SemaphoreSignal(spot_mutex);
                                available = available - 1 - spot1;
                                ThreadExit();
                                EA = 1;
                                
                        }
                }
                else {
                        ThreadYield();
                }
        }
}

void Car2(void) {
        while (1) {
                // Car first enter
                if(spot2 == 2){ 
                        
                        SemaphoreWait(spot_mutex);
                        EA = 0;
                        //available = 1 means spot is parked
                        if(available == 0 || available == 1){
                                spot2 = 1;
                                available = available + 2;
                        }
                        else if(available == 2){
                                spot2 = 0;
                                available = available + 1;
                        }
                        //no parking spot
                        else ThreadYield();
                        in2 = now();
                        delay(4);
                        EA = 1;
                }
                // spot[i] = 0 or 1 meaning parking
                else if(out2 == 0xFF && spot2 != 2){
                        //delay not 0 not yet to leave spot
                        if(delay_arr2 > 0){
                                //break;
                                ThreadYield();
                        }
                        // delay = 0 now leave the spot
                        else if(delay_arr2 == 0){
                                EA = 0;
                                out2 = now();
                                SemaphoreSignal(spot_mutex);
                                available = available - 1 - spot2;
                                ThreadExit();
                                EA=1;
                                
                        }
                }
                else {
                        ThreadYield();
                }
        }
}

void Car3(void) {
        while (1) {
                // Car first enter
                if(spot3 == 2){ 
                        SemaphoreWait(spot_mutex);
                        EA = 0;
                        //available = 1 means spot is parked
                        if(available == 0 || available == 1){
                                spot3 = 1;
                                available = available + 2;
                        }
                        else if(available == 2){
                                spot3 = 0;
                                available = available + 1;
                        }
                        //no parking spot
                        else ThreadYield();
                        in3 = now();
                        delay(7);
                        //EA = 1;
                }
                // spot[i] = 0 or 1 meaning parking
                else if(out3 == 0xFF && spot3 != 2){
                        //delay not 0 not yet to leave spot
                        if(delay_arr3 > 0){
                                //break;
                                ThreadYield();
                        }
                        // delay = 0 now leave the spot
                        else if(delay_arr3 == 0){
                                EA = 0;
                                out3 = now();
                                SemaphoreSignal(spot_mutex);
                                available = available - 1 - spot3;
                                ThreadExit();
                                EA=1;
                        }
                }
                else {
                        ThreadYield();
                }
        }
}
void Car4(void) {
        while (1) {
                // Car first enter
                if(spot4 == 2){ 
                        SemaphoreWait(spot_mutex);
                        EA = 0;
                        //available = 1 means spot is parked
                        if(available == 0 || available == 1){
                                spot4 = 1;
                                available = available + 2;
                        }
                        else if(available == 2){
                                spot4 = 0;
                                available = available + 1;
                        }
                        //no parking spot
                        else ThreadYield();
                        in4 = now();
                        delay(7);
                        EA = 1;
                }
                // spot[i] = 0 or 1 meaning parking
                else if(out4 == 0xFF && spot4 != 2){
                        //delay not 0 not yet to leave spot
                        if(car4_tid == 1){
                                if(delay_arr1> 0){
                                        //break;
                                        ThreadYield();
                                }
                                // delay = 0 now leave the spot
                                else if(delay_arr1 == 0){
                                        EA = 0;
                                        out4 = now();
                                        SemaphoreSignal(spot_mutex);
                                        available = available - 1 - spot4;
                                        ThreadExit();
                                        EA=1;
                                }
                        }
                        else if(car4_tid == 2){
                                if(delay_arr2> 0){
                                        //break;
                                        ThreadYield();
                                }
                                // delay = 0 now leave the spot
                                else if(delay_arr2 == 0){
                                        EA = 0;
                                        out4 = now();
                                        SemaphoreSignal(spot_mutex);
                                        available = available - 1 - spot4;
                                        ThreadExit();
                                        EA=1;
                                }
                        }
                        else if(car4_tid == 3){
                                if(delay_arr3> 0){
                                        //break;
                                        ThreadYield();
                                }
                                // delay = 0 now leave the spot
                                else if(delay_arr3 == 0){
                                        EA = 0;
                                        out4 = now();
                                        SemaphoreSignal(spot_mutex);
                                        available = available - 1 - spot4;
                                        ThreadExit();
                                        EA=1;
                                }
                        }
                        
                }
                else {
                        ThreadYield();
                }
        }
}

void Car5(void) {
        while (1) {
                // Car first enter
                if(spot5 == 2){ 
                        SemaphoreWait(spot_mutex);
                        EA = 0;
                        //available = 1 means spot is parked
                        if(available == 0 || available == 1){
                                spot5 = 1;
                                available = available + 2;
                        }
                        else if(available == 2){
                                spot5 = 0;
                                available = available + 1;
                        }
                        //no parking spot
                        else ThreadYield();
                        in5 = now();
                        delay(1);
                        EA = 1;
                }
                // spot[i] = 0 or 1 meaning parking
                else if(out5 == 0xFF && spot5 != 2){
                        //delay not 0 not yet to leave spot
                        if(car5_tid == 1){
                                if(delay_arr1> 0){
                                        //break;
                                        ThreadYield();
                                }
                                // delay = 0 now leave the spot
                                else if(delay_arr1 == 0){
                                        EA = 0;
                                        out5 = now();
                                        SemaphoreSignal(spot_mutex);
                                        available = available - 1 - spot5;
                                        ThreadExit();
                                        EA=1;
                                }
                        }
                        else if(car5_tid == 2){
                                if(delay_arr2> 0){
                                        //break;
                                        ThreadYield();
                                }
                                // delay = 0 now leave the spot
                                else if(delay_arr2 == 0){
                                        EA = 0;
                                        out5 = now();
                                        SemaphoreSignal(spot_mutex);
                                        available = available - 1 - spot5;
                                        ThreadExit();
                                        EA=1;
                                }
                        }
                        else if(car5_tid == 3){
                                if(delay_arr3> 0){
                                        //break;
                                        ThreadYield();
                                }
                                // delay = 0 now leave the spot
                                else if(delay_arr3 == 0){
                                        EA = 0;
                                        out5 = now();
                                        SemaphoreSignal(spot_mutex);
                                        available = available - 1 - spot5;
                                        ThreadExit();
                                        EA=1;
                                }
                        }
                        
                }
                else {
                        ThreadYield();
                }
        }
}



/* [5 pts for this function]
 * main() is started by the thread bootstrapper as thread-0.
 * It can create more thread(s) as needed:
 * one thread can act as producer and another as consumer.
 */
void main(void) {
          /* 
           * @@@ [1 pt] initialize globals 
           * @@@ [4 pt] set up Producer and Consumer.
           * Because both are infinite loops, there is no loop
           * in this function and no return.
           */
    SemaphoreCreate(spot_mutex, 2);


    ThreadCreate(&Car1);
    ThreadCreate(&Car2);
    ThreadCreate(&Car3);
    
    while(1){
        if(bitmap == 15 && car4_tid == 10) ThreadYield();
        else break;
    }
    ThreadCreate(&Car4);

    while(1){
        if(bitmap == 15 && car5_tid == 10) ThreadYield();
        else break;
    }
    ThreadCreate(&Car5);
    
    while(1){
        //ThreadYield();
    }
    //ThreadCreate(&Car4);
    


    
}

void _sdcc_gsinit_startup(void) {
        __asm
                ljmp  _Bootstrap
        __endasm;
}

void _mcs51_genRAMCLEAR(void) {}
void _mcs51_genXINIT(void) {}
void _mcs51_genXRAMCLEAR(void) {}

void timer0_ISR(void) __interrupt(1) {
        __asm
                ljmp _myTimer0Handler
        __endasm;
}

