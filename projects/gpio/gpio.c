/*  
 *  Author:       Furkan CATKAYA
 *  Date:         September 2021
 * 
 *  Description:  This program blinks two LEDs LED1 and LED2
 *                on MSPEXP430G2 board connected on P1.0 and P1.6 
 *                respectively. The button S2 which connected P1.3
 *                stops the program using a key changed by an interrupt.
 *                
 *  Hardware:     MSP-EXP430G2 LaunchPad, no additional hardware
 * 
 */



#include "msp430g2553.h"


void GPIO_Init();    // Function declaration
unsigned char key=0;

int main(void){

    GPIO_Init();   
    unsigned int i=0;

    while(1){
        for(i=0; i<50000; i++);
        P1OUT ^= (1<<0);  // Toggle P1.0 and P1.6
        P1OUT ^= (1<<6);
        while(key);       // Stop if key=1
    }
}

void GPIO_Init(){

    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    P1OUT |=  BIT0;     // Set first bit of P1OUT
    P1OUT |=  BIT3;     // Set third bit of P1OUT
    P1OUT |=  BIT6;     // Set sixth bit of P1OUT
    P2OUT |=  0xFF;     // Set all bit of P1OUT

    P1DIR |=  BIT0;     // Set P1.0 as output
    P1DIR |=  BIT6;     // Set P1.6 as output
    P1DIR &= ~BIT3;     // Set P1.3 as input
    P2DIR &= ~0xFF;     // Clear all bits of P2DIR

    P1REN |=  0xBE;     // Enable all pullup/pulldown resistors except P1.0 and P1.6
    P2REN |=  0xFF;     // Enable all pullup/pulldown resistors

    P1IE  |=  BIT3;     // Interrupts Enabled for P1.3
    __bis_SR_register(GIE);   // General Interrupts Enabled

}

//#pragma vector = PORT1_VECTOR           // This cannot be used for GNU Compiler Collection
__attribute__ ((interrupt(PORT1_VECTOR))) // Puts ISR function on PORT1 interrupt vector
void port1_ISR (void){                    // ISR(Interrupt Service Routine) Function
  if( (P1IFG & BIT3) ){   // If the interrupt source is P1.3                  
    key ^= BIT0;          // Toggle "key"
    P1IFG &= ~BIT3;       // Clear the interrupt flag
  }
}
