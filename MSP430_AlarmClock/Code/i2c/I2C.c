#include "I2C.h"
#include "msp430g2553.h"


unsigned char BCD2Dec(unsigned char BCD){
    unsigned char L, H;
    L = BCD & 0x0F;
    H = (BCD>>4)*10;
    return (H+L);
}
unsigned char Dec2BCD(unsigned char Dec){
    unsigned char L, H;
    L=Dec % 10;
    H=(Dec/10)<<4;
    return (H+L);
}
//==============================================================================

void I2C_Init(unsigned char slave_add)
{
     P1SEL |= BIT6 + BIT7;                      // Assign I2C pins to USCI_B0
     P1SEL2|= BIT6 + BIT7;                      // Assign I2C pins to USCI_B0
     UCB0CTL1 |= UCSWRST;                       // Enable SW reset
     UCB0CTL0 = UCMST+UCMODE_3+UCSYNC;          // I2C Master, synchronous mode
     UCB0CTL1 = UCSSEL_2+UCSWRST;               // Use SMCLK, keep SW reset
     UCB0BR0 = 12;                              // fSCL = SMCLK/12 = ~100kHz
     UCB0BR1 = 0;
     UCB0I2CSA = slave_add;                       // Set slave address
     UCB0CTL1 &= ~UCSWRST;                      // Clear SW reset, resume operation
}

void I2C_Write(unsigned char address, unsigned char numbyte, unsigned char *poisend)
{
    while (UCB0CTL1 & UCTXSTP);                // Loop until I2C STT is sent
    UCB0CTL1 |= UCTR + UCTXSTT;                // I2C TX, start condition

    while (!(IFG2&UCB0TXIFG));
    UCB0TXBUF = address;

    while (!(IFG2&UCB0TXIFG));
    unsigned char i;
    for( i=0;i<numbyte;i++)
     {
        UCB0TXBUF= *(poisend+i) ;
        while (!(IFG2&UCB0TXIFG));
     }
    UCB0CTL1 |= UCTXSTP;                       // I2C stop condition after 1st TX
    IFG2 &= ~UCB0TXIFG;
}

void I2C_Read(unsigned char address, unsigned char numbyte, unsigned char *poirev)
{
    while (UCB0CTL1 & UCTXSTP);                // Loop until I2C STT is sent
    UCB0CTL1 |= UCTR + UCTXSTT;                // I2C TX, start condition

     while (!(IFG2&UCB0TXIFG));
     UCB0TXBUF = address;

     while (!(IFG2&UCB0TXIFG));
     UCB0CTL1 &= ~UCTR;                         // I2C RX
     UCB0CTL1 |= UCTXSTT;                       // I2C start condition
     IFG2 &= ~UCB0TXIFG;                        // Clear USCI_B0 TX int flag

     while (UCB0CTL1 & UCTXSTT);                // Loop until I2C STT is sent
     while (!(IFG2&UCB0RXIFG));
     unsigned char i;
     for(i=0; i<numbyte; i++)
     {
         *(poirev+i-1)= UCB0RXBUF;
         while (!(IFG2&UCB0RXIFG));
     }
     UCB0CTL1 |= UCTXSTP;
}
