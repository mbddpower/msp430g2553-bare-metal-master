/*  
 *  Author:       Furkan CATKAYA
 *  Date:         September 2021
 * 
 *  Description:  This program is an implementation of UART on the
 *                development board MSP-EXP430G2. You can communicate
 *                with your PC via USB, using a COM port. You need an
 *                serial monitor such as Putty. The program sends back
 *                what you sent via UART. It will send "Button is pressed!"
 *                if you press S2 on the board.                  
 * 
 *  Hardware:     MSP-EXP430G2 LaunchPad, no additional hardware
 * 
 */


#include "msp430g2553.h"

#define __MCLK_1MHZ__
#define BAUD_RATE 20

#ifdef  __MCLK_1MHZ__
    #define PRESCALER (1000000/BAUD_RATE)
#endif
#ifdef  __MCLK_16MHZ__
    #define PRESCALER (16000000/BAUD_RATE)
#endif

void delay_cycles(unsigned long c);
void blink();
void set_clock();
void set_clock_to_16MHZ();
void set_clock_to_1MHZ();
void GPIO_Init();
void UART_Init();
char UART_rx();
void UART_tx(char*);

void Init(){
    set_clock();
    GPIO_Init();
    UART_Init();
    __bis_SR_register(GIE);
    blink();blink();
}

int main(){

    Init();

    while(1);              
        UART_tx("A");
    

}

void set_clock(){
#ifdef  __MCLK_1MHZ__
    set_clock_to_1MHZ();
#endif
#ifdef  __MCLK_16MHZ__
    set_clock_to_16MHZ();
#endif
}

void set_clock_to_1MHZ(){
    DCOCTL  = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;
}

void set_clock_to_16MHZ(){
    DCOCTL  = CALDCO_16MHZ;
    BCSCTL1 = CALBC1_16MHZ;
}


void GPIO_Init(){
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    P1OUT &= ~(BIT0 | BIT6);    // Clear P1.0 and P1.6 pins
    P1DIR |=  (BIT0 | BIT6);    // P1.0 and P1.6 are set as output pins

    P1OUT |=  BIT3;     // Pullup resistor
    P1REN |=  BIT3;     // Pullup/down resistor is enabled
    P1DIR &= ~BIT3;     // P1.3 is set as input pin
    P1IE  |=  BIT3;     // Enable interrrupt for P1.3
}

void UART_Init(){

    UCA0CTL1 |= UCSWRST;        // Disable the USCI module

    UCA0CTL0  = 0;              // UCA0 Control 1 Register is cleaned just to make sure
    UCA0CTL1 |= UCSSEL_2;       // UCA0 Control 1 Register Clock Source Selection - MCLK
    UCA0BR0   = 118;            // ( PRESCALER & 0x00FF );          // Baud Rate prescaler register lower byte ()
    UCA0BR1   = 8;              // ((PRESCALER & 0xFF00) >> 8 );    // Baud Rate prescaler register upper byte

    UCA0MCTL |= UCBRS_3;        // Modulation Control - UCBRS = 3

    P1SEL   |= (BIT1 | BIT2);   // Select secondary peripheral module function
    P1SEL2  |= (BIT1 | BIT2);   // for P1.1 and P1.2 which are RX and TX for UART

    UCA0CTL1 &= ~UCSWRST;       // Enable the USCI module
    IE2 |= UCA0RXIE;            // Enable UCA0 Receive Interrupt
}

char UART_rx(){
    if((IFG2 & UCA0RXIFG))      // if UART rx interrupt flag is set
        return UCA0RXBUF;       // give what UART received
    else
        return -1;              // nothing in the UART rx buffer
}

void UART_tx(char *chr){                // takes the first address of the string as argument
    while(*chr != 0 ){                  // continue until the end of the string
        UCA0TXBUF = *chr++;             // Load the character to the UCA0 Transmit Buffer
        while((UCA0STAT & UCBUSY));     // Wait until the transmission is done
    }
    blink();                            // as a sign of a succesful transmission
}

void blink(){           // toggling LEDs in turn twice
    P1OUT |=  BIT6;
    P1OUT &= ~BIT0;
    delay_cycles(20000);
    P1OUT |=  BIT0;
    P1OUT &= ~BIT6;
    delay_cycles(20000);
    P1OUT &= ~BIT6;
    P1OUT &= ~BIT0;  
}

void volatile delay_cycles(volatile unsigned long c){   // classic delay function
    while(c>0)--c;
}
__attribute__ ((interrupt(USCIAB0RX_VECTOR))) // assigning the function as ISR for UART rx
void UART_RX_ISR(void){
    if((IFG2 & UCA0RXIFG)){     // if rx interrrupt flag 1
        P1OUT |= BIT6;          // light up!
        UCA0TXBUF = UCA0RXBUF;  // send what you receive
        IFG2 &= ~UCA0RXIFG;     // clear the rx interrrupt flag
    }
}

__attribute__ ((interrupt(PORT1_VECTOR)))   // assigning the function as ISR for Port1
void PORT1_ISR(void){
    if(P1IFG & BIT3){                       // if the source of the interrupt is P1.3(button)
        UART_tx("Button is pressed!");      // send the message via UART
        UCA0TXBUF = 10;                     // "\n" new line in ASCII
        P1IFG &= ~BIT3;                     // clear the P1.3 interrupt flag
    }
}