

#include "msp430f5419a.h"

          //Counter,Integer//
unsigned char counter[2];
extern unsigned int i = 0;
unsigned int ruptflag = 0;              //0 no interrupt
                                        //1 HW global enable
                                        //2 OFF


          // others Flag//
extern unsigned char length = 2;
unsigned int current_ready= 0;
extern unsigned char HORS_FLAG = 0;
unsigned int GE_FLAG = 0; 
unsigned int for_GE_rest = 0;
unsigned LR = 0;
extern unsigned int rt = 0;
extern unsigned int s_cur_set = 0;
extern unsigned int s_temp_set = 0;

          //Temperature flag//
unsigned char TEMPERATURE[2];

          //Version//
unsigned char Version[5] = {0x4E,2,0,5,2};

          //Status flag//
unsigned char STATUS[2];

          //Command to fpga//
unsigned char ROFF[2]= {0,11};

          //Pre flag//
unsigned long presiml=0;
unsigned char PREACT[2];
unsigned char presimfpga[2];
unsigned char prevalue2[2];

          //Pwr flag//
unsigned long pwrsiml= 0;
unsigned long pwrtop = 0;
unsigned long pwrdefault = 0;
unsigned char pwrvalue2[2];
unsigned int ib=0;

          //IA flag//
unsigned int iareg = 0;
extern unsigned int ampreg = 0;
          
         //waveform flag//
unsigned int waveflag = 0;

        //Fault FLag//
unsigned char FAULT[3]; 
unsigned char FAULTM[3];

        //LATCH FLAG//
extern unsigned char GE2off = 0x00;    ////LATCH GE OFF FLAG   0x00 for no latch  01 02 04
extern unsigned char l2off = 0x00;     ////LATCH DC-DC OFF FLAG  0x00 for no latch

///////////////SHUT DOWN FUNCTION///////
void shutflag(void)
{
        
        GE_FLAG = 0;
        LR = 0;
        for_GE_rest = 0;
        STATUS[1] &= 0xFE;                     ////global enable status off
        STATUS[1] &= 0xFD;                     ////Active status off
        STATUS[0] &= 0xEF;                     ////LASER READY Status off
        P2OUT |= 0x08;                         ////LASER READY OUT OFF////           
        ampreg = 0;                               ////Amplitude register clear
       
  
}
//////////////////////////msp to fpga VIA RS232 SEND//////////////
unsigned char uartSend(unsigned char *pucData, unsigned char ucLength) 
{
  while(ucLength)
  {
    // Wait for TX buffer to be ready for new data
    while(!(UCA1IFG & UCTXIFG));

    // Push data to TX buffer
    UCA1TXBUF = *pucData;
 
    // Update variables
    ucLength--;
    pucData++;
  }

  // Wait until the last byte is completely sent
  while(UCA1STAT & UCBUSY);
}

/////////////////////////BOARD TO PC VIA RS232 SEND/////////////////
unsigned char uartSendTOPC(unsigned char *pucData, unsigned char ucLength) 
{
  while(ucLength)
  {
    // Wait for TX buffer to be ready for new data
    while(!(UCA0IFG & UCTXIFG));

    // Push data to TX buffer
    UCA0TXBUF = *pucData;

    // Update variables
    pucData++;
    ucLength--;
    
  }

  // Wait until the last byte is completely sent
  while(UCA0STAT & UCBUSY);
}




void RS232(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  P3SEL = 0x30;
  P5SEL = 0x40;                             // P3.4,5 = USCI_A0 TXD/RXD
  P5DIR = 0x7F;
  P3DIR = 0xFF;
  UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 10;     //6                         // 1MHz 9600 (see User's Guide)
  UCA0BR1 = 0;                              // 1MHz 9600
  UCA0MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0,
                                           // over sampling
  UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
  UCA1CTL1 |= UCSSEL_2;                     // SMCLK
  UCA1BR0 = 10;     //6                         // 1MHz 9600 (see User's Guide)
  UCA1BR1 = 0;                              // 1MHz 9600
  UCA1MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0,  
  UCA0CTL1 &= ~UCSWRST;                            // over sampling
  UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
   
}  


// Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)

{ 
  while(length)
  {
    // Wait for TX buffer to be ready for new data
    while(!(UCA0IFG & UCRXIFG));

    // Push data to TX buffer
     counter[i] = UCA0RXBUF;

    // Update variables
    length--;
    i++;
    //counter++;
    
  }

  // Wait until the last byte is completely sent
  while(UCA0STAT & UCBUSY);
 
    
     if((counter[0] == 0x72) && (counter[1] == 0x74))           //RT
      {  
       rt = 1;      
      }
     else if ((counter[0] == 0x72) && (counter[1] == 0x76))        //RV
      {    
        uartSendTOPC(Version,5); 
      }
  else if ((counter[0] == 0x72) && (counter[1] == 0x73))        //RS
      {    
        uartSendTOPC(STATUS,2); 
      }
  else if ((counter[0] == 0x72) && (counter[1] == 0x69))        //RI
  {      
      current_ready = 1 ;    
  }
  else if (counter[0] == 0x73)                                  ////select Software or Hardware mode////
  {
    if (counter[1] == 0x73)                                     //SOFTWARE MODE
    {
      if (HORS_FLAG == 0)
      {
   
       P2OUT &= 0xFE;                                           //global enable 
       ruptflag =2;
     //  uartSend(ROFF,2);
    //  __delay_cycles(4000);                                     //100us delay
    //     P2OUT &= 0xFD;                                         //GLOBAL ENALBE(NO INVERT)
          shutflag();    
      }
    HORS_FLAG = 1;
    STATUS[1] |= 0x08;                                          //SW status software         
    }
    else if (counter[1] == 0x68)
    {
      if (HORS_FLAG == 1)                                       //HARDWARE MODE
      {
        P2OUT &= 0xFE;                                          //global enable 
        ruptflag =2;
  //    uartSend(ROFF,2);
  //  __delay_cycles(4000);                                       //100us delay
   //      P2OUT &= 0xFD;                                         //GLOBAL ENALBE(NO INVERT)
          shutflag();
      }
      HORS_FLAG = 0;
      STATUS[1] &= 0xF7;                                         //SW status hardware   
    }
  }
  else if ((counter[0] == 0x72) && (counter[1] == 0x66))        //RF
  {
     uartSendTOPC(FAULT,3);
  }
  else if ((counter[0] == 0x72) && (counter[1] == 0x6D))        //RPF
  {
      uartSendTOPC(FAULTM,3);
  }
 
  else if (counter[0] == 0x62)                                  //IB
  {
   ib = counter[1];
   pwrtop = pwrvalue2[1];
   ib = pwrtop * ib / 100;
   counter[1] = ib;
   counter[0] =  0x10;
   ruptflag =3;
   //uartSend(counter,2);
  }
  else if ((counter[0] | 0x07) == 0x0F)                         //SE
  {
    presiml = (counter[0]  << 8 )| counter[1];  
    flash_write() ;
    ruptflag =3;
    //uartSend(counter,2);
  }
  else if (counter[0] == 0x77)                                  //SW
  {
    pwrsiml =  0x1000 | counter[1]; 
    flash_write();
    counter[0] = 0x10;
     ruptflag =3;
    //uartSend(counter,2);
  }
  else if((counter[0] == 0x00) && (counter[1]== 0x08))          //IS 0
      {
        if (HORS_FLAG == 1)
        {
          if ((l2off == 0x02) && ((P5IN | 0x7F) == 0xFF))       //if pre fault and LEG is on
        {
            _NOP();                                              //send fault to pc
        }
        
        else
        {
          if ((GE2off == 0x00) && (l2off !=0x01))
        {      
       //   uartSend(prevalue2,2);    
        // uartSend(counter ,2); 
        // __delay_cycles(200);
           ruptflag =4;
         P2OUT |= 0x02;                                         //GLOBAL ENALBE(NO INVERT)
         P2OUT |= 0x01;                                         //global enable 
         STATUS[1] |= 0x01;                                     //global enable status on
         STATUS[1] &= 0xFD;                                     //Active status off
         s_temp_set = 1;                        ////for seed temp reset
         s_cur_set = 1;                         ////for seed current reset
         if ((P9OUT & 0x08) == 0x00)
         {
         P9OUT |= 0x08;                                         //SMP_EN on 
         __delay_cycles(200000);
         }
         GE_FLAG = 1;
        }
        }
        }
      }
  else if ((counter[0] == 0x00) && (counter[1] ==0x0B))         //IC 0
      {
        if (HORS_FLAG ==1)
        {
        
         P2OUT &= 0xFE;                                          //global enable 
      //   uartSend(counter ,2); 
      //  __delay_cycles(4000);                                    //100us delay
          ruptflag = 2;
     //    P2OUT &= 0xFD;                                          //GLOBAL ENALBE(NO INVERT)
         shutflag();
        flash_pre_read();
        flash_pwr_read();
        }
      }  
   else if   ((counter[0] == 0x00) && (counter[1]== 0x09))      //IS 1
   {
      if (HORS_FLAG ==1)
        { 
           ruptflag =3;
      //    uartSend(counter ,2); 
           STATUS[1] |= 0x02;                                   //Active status on
        }
   }  
   else if ((counter[0] & 0xF0) == 0x40)                        //IA compensation
   {
     iareg = ((counter[0] & 0x03) << 8) | counter[1];
     ampreg = iareg;
     if (31 >= TEMPERATURE[1])
      iareg = iareg * (1 - (0.0025*(31 - TEMPERATURE[1])));
     else
      iareg = iareg * (1 + (0.003*(TEMPERATURE[1] - 31)));
     if (iareg > 1023)
       iareg = 1023;
     counter[1] = iareg & 0xFF;
     counter[0] = (iareg | 0x4000) >> 8;
    // uartSend(counter,2);
       ruptflag =3; 
   }
  else
   // uartSend(counter,2);                                        //OTHERS COMMANDS
       ruptflag =3;

    length = 2;
    i = 0;
}
