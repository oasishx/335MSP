#include <msp430f5419a.h>

void shutflag(void)
{
        
        GE_FLAG = 0;
        LR = 0;
        STATUS[1] &= 0xFE;                     ////global enable status off
        STATUS[1] &= 0xFD;                     ////Active status off
        STATUS[0] &= 0xEF;                     ////LASER READY Status off
        P2OUT |= 0x08;                         ////LASER READY OUT OFF////      
        ampreg = 0;                               ////Amplitude register clear
       
  
}

void normal_shut(void)
{
         P2OUT &= 0xFE;  ///////global enable off
       //  P9OUT &= 0xF7;           //////SMP_EN OFF 
         uartSend(OFF,2);
         __delay_cycles(4000);                     ////100us delay
         P2OUT &= 0xFD;  ///GLOBAL ENABLE(NO INVERT) OFF
        GE_FLAG = 0;
        LR = 0;
        STATUS[1] &= 0xFE;                     ////global enable status off
        STATUS[1] &= 0xFD;                     ////Active status off
        STATUS[0] &= 0xEF;                     ////LASER READY Status off
        P2OUT |= 0x08;                         ////LASER READY OUT OFF//// 
        ampreg = 0;                               ////Amplitude register clear
       
  
}
void smp_shut(void)
{
         P2OUT &= 0xFE;  ///////global enable off
         P9OUT &= 0xF7;           //////SMP_EN OFF 
         uartSend(OFF,2);
         __delay_cycles(4000);                     ////100us delay
         P2OUT &= 0xFD;  ///GLOBAL ENABLE(NO INVERT) OFF
        GE_FLAG = 0;
        LR = 0;
        STATUS[1] &= 0xFE;                     ////global enable status off
        STATUS[1] &= 0xFD;                     ////Active status off
        STATUS[0] &= 0xEF;                     ////LASER READY Status off
        P2OUT |= 0x08;                         ////LASER READY OUT OFF//// 
        ampreg = 0;                               ////Amplitude register clear
       
  
}
