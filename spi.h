

#include "msp430f5419a.h"


 unsigned char Initialize[2]= {0xFF,0xFF};
 unsigned char C1_AN0[2]= {0x83,0x30};
 unsigned char C1_AN1[2]= {0x87,0x30};
 unsigned char C1_AN2[2]= {0x8B,0x30};
 unsigned char C1_AN3[2]= {0x8F,0x30};
 unsigned char C1_AN4[2]= {0x93,0x30};
 unsigned char C1_AN5[2]= {0x97,0x30};
 unsigned char C1_AN6[2]= {0x9B,0x30};
 unsigned char C1_AN7[2]= {0x9F,0x30};
 
 
 unsigned char C2_AN0[2]= {0x83,0x30};
 unsigned char C2_AN1[2]= {0x87,0x30};
 
 unsigned char sensor[2];
 
 
 /////SPI Fault flag///////
  unsigned char SPI_F[2];
//////FAULT[3]/////

unsigned int PreF = 0x01;        //pre current fault            [2]
extern unsigned int STF = 0x02;         //seed temp fault              [2]
unsigned int SP5F = 0x04;       //seed power fault 5v           [2]
unsigned int SP12F =0x08 ;      //seed power fault 12v          [2]
extern unsigned int SCF = 0x10;         //seed current fault           [2]
unsigned int SMF = 0x20;         //seed  misfiring fault        [2]
unsigned int LoPF = 0x01;        //24v logic main power fault   [1]
unsigned int LaPF = 0x02;        //24v laser main power fault   [1]
unsigned int LPSF = 0x04;         //logic power supply fault    [1]
unsigned int FPF = 0x08;       //fans power supply fault         [1]
unsigned int DP3F = 0x10;      //diode power supply fault 3v    [1]
unsigned int DP12F = 0x20;      //diode power supply fault 12v  [1]
unsigned int BPTF = 0x01;        //base plate temp fault        [0]
unsigned int LSTF = 0x02;         //spool temp fault            [0]
unsigned int ESF = 0x04;         //emergency stop fault         [0]

 extern int spi_fflag = 0;
 
 
//////////////////PRE_POWER CURRENT VALUE;
 long  pre_current;
 
 long pwr_current;
 long total_pre_current;
 long total_pwr_current;
 unsigned int average_pre;
 unsigned int average_pwr;  
/////////////////////////   
unsigned char in = 0;
unsigned int spii = 0;
unsigned char spilength = 1;
int k,l;
long PRE_MON_OUT;
long PWR_MON_OUT;
unsigned long pre_current_pc;
unsigned char current[4];
//unsigned long pre_current_pc;
//unsigned char pwr_char[2];
unsigned long pwr_current_pc;

void spiSend(unsigned char *pucData, unsigned char ucLength) 
{
  while(ucLength)
  {
    // Wait for TX buffer to be ready for new data
    while(!(UCB0IFG & UCTXIFG));

    // Push data to TX buffer
    UCB0TXBUF = *pucData;

    // Update variables
    ucLength--;
    pucData++;  
    __delay_cycles(100);
  }
  // Wait until the last byte is completely sent
  while(UCB0STAT & UCBUSY);
}

void SPI(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer

  
  P3SEL |= 0x0E;                            // P3.5,4,0 option select
  P3DIR |= 0x41;
  P3OUT |= 0x41;
 // P2DIR |= 0x04;
 // P9DIR |= 0x08;
  
  UCB0CTL1 |= UCSWRST;                      // **Put state machine in reset**
  UCB0CTL0 |= UCMST + UCSYNC + UCCKPL + UCCKPH + UCMSB  ;    // 3-pin, 8-bit SPI master
                                            // Clock polarity high, MSB
  UCB0CTL1 |= UCSSEL_2;                     // SMCLK
  UCB0BR0 = 0x02;                           // /2
  UCB0BR1 = 0;                              //
  UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  UCB0IE |= UCRXIE;                        // Enable USCI_A0 RX interrupt
   
  __delay_cycles(100);                      // Wait for slave to initialize
  
}
       
void SPICHECK(void)
{
  
  ///////CHIP 1 INITIALIZE///////
  P3OUT &= 0xFE;
  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready? 
  spiSend(Initialize , 2);                 // Transmit first character
  P3OUT |= 0x01;
  P3OUT &= 0xFE;
  
  
  
  
  ////C1_AN0//// PRE AMP CURRENT MON2
  if (current_ready == 1)
  {
  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready? 
  spiSend(C1_AN0 , 2);                 // Transmit first character
  P3OUT |= 0x01;
  P3OUT &= 0xFE; 
  
  for (k=0;k<100;k++)
  {
  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready? 
  spiSend(C1_AN0 , 2);                 // Transmit first character
  P3OUT |= 0x01;
  P3OUT &= 0xFE; 
  
  pre_current = ((sensor[0] << 8 ) | sensor[1]) & 0x0FFF;
  total_pre_current = total_pre_current + pre_current;
  }
 
  average_pre = total_pre_current /100;
 
  PRE_MON_OUT = average_pre * 0.4938;
  TBCCR2 = PRE_MON_OUT;
  k = 0;
  total_pre_current = 0;
/*  if (average_pre > 0xFFF)
  {
    SPI_F[1] |= 0x01;
  }
  else
  {
    SPI_F[1] &= 0xFE;
  }*/
  ////C1_AN1//// PWR AMP CURRENT MON
  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready? 
  spiSend(C1_AN1 , 2);                 // Transmit first character
  P3OUT |= 0x01;
  P3OUT &= 0xFE; 
  
  for (l=0;l<100;l++)
  {
  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready? 
  spiSend(C1_AN1 , 2);                 // Transmit first character
  P3OUT |= 0x01;
  P3OUT &= 0xFE; 
  
  pwr_current = ((sensor[0] << 8 ) | sensor[1]) & 0x0FFF;
  total_pwr_current = total_pwr_current + pwr_current;
  }
 
  average_pwr = total_pwr_current / 100;  
  
   pre_current_pc = (average_pre -6.0661)/0.32898;
   pwr_current_pc = (average_pwr -26.532)/0.24984;
   
  current[0] = pre_current_pc /100;
  current[1] = pre_current_pc - (current[0] * 100);
  
  current[2] = pwr_current_pc /100;
  current[3] = pwr_current_pc - (current[0] * 100);
  
  uartSendTOPC(current,4);
  current_ready = 0;
  
  PWR_MON_OUT =  average_pwr * 0.4598;
  TBCCR1 = PWR_MON_OUT;
  l = 0;
  total_pwr_current = 0;
/*  if (average_pwr > 0xFFF)
  {
    SPI_F[1] |= 0x02;
  }
  else
  {
    SPI_F[1] &= 0xFD;
  }*/
  current_ready = 0;
  }
  ////C1_AN2//// 12V PWR DRIVER
  
  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready? 
  spiSend(C1_AN2 , 2);                 // Transmit first character 
  P3OUT |= 0x01;
  P3OUT &= 0xFE; 
  
  ////C1_AN3//// 3V PRE DRIVER
  
  
  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready? 
  spiSend(C1_AN3 , 2);                 // Transmit first character
  if (sensor[0] < 0x25)
    {
      SPI_F[1] |= 0x04;
      FAULT[1] |= DP12F;                 //FAULT STATUS ON
      FAULTM[1] |= DP12F;
    //  FAULT[1] &= 0xFE; 
    }
  else
    {
       SPI_F[1] &= 0xFB;
       FAULT[1] &= ~DP12F;                //FAULT STATUS CLEAR
       if (GE_FLAG == 1)
       FAULTM[1] &= ~DP12F;                //FAULT STATUS CLEAR
 //       FAULT[1] |= 0x01; 
    }
  P3OUT |= 0x01;
  P3OUT &= 0xFE; 
  
  ////C1_AN4//// 5V CONTROLLER BOARD SUPPLY
  
  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready? 
  spiSend(C1_AN4 , 2);                 // Transmit first character
  if (sensor[0] < 0x33)
    {
      SPI_F[1] |= 0x08;
      FAULT[1] |= DP3F;
      FAULTM[1] |= DP3F;
    }
  else
    {
       SPI_F[1] &= 0xF7;
        FAULT[1] &= ~DP3F;
       if (GE_FLAG == 1)
       FAULTM[1] &= ~DP3F;
    }
  P3OUT |= 0x01;
  P3OUT &= 0xFE; 
  
  ////C1_AN5//// 5V SEED DRIVER SUPPLY
  
  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready? 
  spiSend(C1_AN5 , 2);                 // Transmit first character
  if (sensor[0] < 0x45)
    {
      SPI_F[1] |= 0x10;
      FAULT[1] |= LPSF;
       FAULTM[1] |= LPSF;
    }
   else
    {
     SPI_F[1] &= 0xEF;
      FAULT[1] &= ~LPSF;
     if (GE_FLAG == 1)
      FAULTM[1] &= ~LPSF;
    }
  P3OUT |= 0x01;
  P3OUT &= 0xFE; 
  
  ////C1_AN6//// 24V LASER SUPPLY
  
  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready? 
  spiSend(C1_AN6 , 2);                 // Transmit first character
  if (sensor[0] < 0x55)
    {
       SPI_F[1] |= 0x20;
        FAULT[2] |= SP5F;
        FAULTM[2] |= SP5F;
    }
   else
    {
     SPI_F[1] &= 0xDF;
     FAULT[2] &= ~SP5F;
     if (GE_FLAG == 1)
      FAULTM[2] &= ~SP5F;
    }
  P3OUT |= 0x01;
  P3OUT &= 0xFE; 
  
  ////C1_AN7//// 24V LOGIC SUPPLY
  
  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready? 
  spiSend(C1_AN7 , 2);                 // Transmit first character
  if (sensor[0] < 0x65)
   {
     SPI_F[1] |= 0x40;
      FAULT[1] |= LaPF;
      FAULTM[1] |= LaPF;
    }
  else
    {
     SPI_F[1] &= 0xBF;
     FAULT[1] &= ~LaPF;
     if (GE_FLAG == 1)
      FAULTM[1] &= ~LaPF;
    }
  P3OUT |= 0x41;
  P3OUT &= 0xFE; 
  
  ////C1_AN7//// 24V LOGIC SUPPLY
  
   
  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready? 
  spiSend(C1_AN7 , 2);                 // Transmit first character
  if (sensor[0] < 0x75) //////*********************/////
    {     
       SPI_F[1] |= 0x80;
        FAULT[1] |= LoPF;
         FAULTM[1] |= LoPF;
    }
  else
    {
     SPI_F[1] &= 0x7F;
      FAULT[1] &= ~LoPF;
     if (GE_FLAG == 1)
      FAULTM[1] &= ~LoPF;
    }
  P3OUT |= 0x41;
  P3OUT &= 0xBF; 
  
  ///////CHIP 2 INITIALIZE///////
  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready? 
  spiSend(Initialize , 2);                 // Transmit first character
  P3OUT |= 0x40;
  P3OUT &= 0xBF;
  
  
  ////C2_AN0//// 12V FANS POWER SUPPLY
  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready? 
  spiSend(C1_AN0 , 2);                 // Transmit first character
  P3OUT |= 0x40;
  P3OUT &= 0xBF;
  
  ////C2_AN1//// 12V SEED DRIVER SUPPLY
  
  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready? 
  spiSend(C1_AN1 , 2);                 // Transmit first character
  if (sensor[0] < 0x05)
    {
     SPI_F[0] |= 0x01;
      FAULT[1] |= FPF;
       FAULTM[1] |= FPF;
    }
  else
    {
      SPI_F[0] &= 0xFE;
       FAULT[1] &= ~FPF;
      if (GE_FLAG == 1)
       FAULTM[1] &= ~FPF;
    }
  P3OUT |= 0x40;
  P3OUT &= 0xBF;
  
  ////C2_AN1//// 12V SEED DRIVER SUPPLY
  
  while (!(UCB0IFG&UCTXIFG));               // USCI_A0 TX buffer ready? 
  spiSend(C1_AN1 , 2);                 // Transmit first character
  if (sensor[0] < 0x15)
    {
      SPI_F[0] |= 0x02;
       FAULT[2] |= SP12F;
         FAULTM[2] |= SP12F;
    }
  else
    {
      SPI_F[0] &= 0xFD;
       FAULT[2] &= ~SP12F;
      if (GE_FLAG == 1)
       FAULTM[2] &= ~SP12F;
    }
  P3OUT |= 0x40;
  P3OUT &= 0xBF;
  
  if ((SPI_F[1] == 0x00)&& (SPI_F[0] == 0x00))
  {
    spi_fflag = 0;
  }
  else
  {
    spi_fflag = 1;
  }
}

#pragma vector=USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
  while(spilength)
  { 
   /* if((UCA0IFG | UCRXIFG) == 1)
    {
         UCB0IE &= ~UCRXIE;   
    }
    else
    {*/
    // Wait for RX buffer to be ready for new data
    while(!(UCB0IFG & UCRXIFG));
    if (in == 1)
    {
      sensor[1] = UCB0RXBUF; 
      in = 0;
    }
    else 
    {
      sensor[0] = UCB0RXBUF;
      in++;
    }
   
    spilength--; 
    spii++;
       
  
 }
  // Wait until the last byte is completely sent
  
  spilength = 1;
  spii = 0;
 
}
