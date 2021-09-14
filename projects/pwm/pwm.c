/*  
 *  Author:       Furkan CATKAYA
 *  Date:         September 2021
 * 
 *  Description:  PWM using timers
 * 
 *  Hardware:     No additional hardware
 * 
 */

#include "msp430g2553.h"

#define PWM BIT0

void GPIO_Init();
void timer_Init();

int main(){

    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL  = CALDCO_1MHZ;
    GPIO_Init();
    timer_Init();
    
    while(1);

}

void GPIO_Init(){

    P1OUT &= ~PWM;  // reset PWM pin
    P1DIR |=  PWM;  // PWM pin assigned as output     

}

void timer_Init(){
    //initialize Timer0_A
    TA0CCR0 = 50000;                  // setting up terminal count
    TA0CTL  = TASSEL_2 + ID_3 + MC_1; // configure and start timer
    
    //enable interrupts
    TA0CCTL0 |= CCIE ;                // enable timer interrupts
    __enable_interrupt();             // set GIE in SR
}

__attribute__ ((interrupt(TIMER0_A0_VECTOR)))
void timerA0_ISR(){
    P1OUT ^= PWM;
}