
#include "msp430f5419a.h"
/*float pre_float;
float pwr_float;
float temp1_float;
float temp2_float;
*//*
extern long PRE_MON_OUT;
extern long PWR_MON_OUT;
extern long TEMP1OUT;
extern long TEMP2OUT;
*/
/*
void PRE_MON(void)
{
 
  PRE_MON_OUT = pre_current * 0.4938;
  TBCCR2 = PRE_MON_OUT;
}

void  PWR_MON(void)  
{
 
  PWR_MON_OUT =  pwr_current * 0.4598;
  TBCCR1 = PWR_MON_OUT;
}
 void BASE_PLATE_TEMP_OUT(void) ///BASE PLATE TEMP MON OUT
{
 //temp1_float = average1 *1.1012 +9.9426;
   TEMP1OUT = average1 *1.1012 +9.9426;
   TBCCR5 = TEMP1OUT; 
}

void PWR_AMP_TEMP_OUT(void)
{
// temp2_float = average2 *1.1012 +9.9426;
   TEMP2OUT = average2 *1.1012 +9.9426;
   TBCCR4 = TEMP2OUT;
}
*/
void DAC(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT

  P4SEL |= 0x36;                            // P4 option select
  P4DIR |= 0x36;                            // P4 outputs

  TBCCR0 = 1023;                             // PWM Period	
  TBCCTL1 = OUTMOD_7;                       // CCR4 reset/set                         
  TBCCTL2 = OUTMOD_7;                       // CCR5 reset/set                            	
  TBCCTL4 = OUTMOD_7;                       // CCR4 reset/set                              	
  TBCCTL5 = OUTMOD_7;                       // CCR5 reset/set                           	
    
  TBCTL = TBSSEL_2 + MC_1 + TBCLR;          // SMCLK, upmode, clear TBR
                      
}
