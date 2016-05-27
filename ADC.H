#include <msp430f5419a.h>
int bt,pt,b,c,d;
int count = 0;
//////Temperature/////
unsigned int temp1;
unsigned int temp2;
unsigned long total1= 0;
unsigned extern int average1 = 0;
unsigned int count2 = 0;
unsigned long total2= 0;
unsigned extern int average2 = 0;

//////HW PWR SET////
unsigned int sim3;
unsigned int act4;
unsigned long total3;
unsigned long total4;
unsigned int average3;
unsigned int average4;
unsigned char PWRSIM[2];
unsigned char PWRACT[2];
 float step3;
unsigned int simtofpga;
float swprecent;
 unsigned char SIMFPGA[2];
 unsigned int prereg_adc;
 unsigned char precom_adc[2];
 float step4;
unsigned long acttofpga;
 unsigned char ACTFPGA[2];
unsigned long debugact;
unsigned char TEMPREF2;
//////PRE AMP MON IN////
unsigned int preamp5;
unsigned long total5;
extern unsigned int average5 = 0;
/////////Send temperature data///////////

void SendPREMON(void);
void SendBTemp(void);
void SendPTemp(void);
void SendPWRSET(void);


 void SendBTemp(void)
{
    for(bt=0;bt<20;bt++)
    {
    ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
    __delay_cycles(70);
    temp1 = ADC12MEM1;          //BASE PLATE TEMP         
 //   temp2 = ADC12MEM2;          //POW AMP TEMP
    total1 = total1 + temp1;
 //   total2 = total2 + temp2;
    }
   average1 = total1 / 20;
//   average2 = total2 / 20; 
  TEMPERATURE[0] = (average1-2.4475)/7.7935;
//  TEMPERATURE[1] = (average2-2.4475)/7.7935;
   total1 = 0;
 //  total2 = 0;
  
   bt = 0;
}
void SendPTemp(void)
{
    for(pt=0;pt<50;pt++)
    {
    ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
    __delay_cycles(100);
//    temp1 = ADC12MEM1;          //BASE PLATE TEMP         
    temp2 = ADC12MEM2;          //POW AMP TEMP
 //   total1 = total1 + temp1;
    total2 = total2 + temp2;
    }
//   average1 = total1 / 20;
   average2 = total2 / 50; 
//  TEMPERATURE[0] = (average1-2.4475)/7.7935;
  TEMPERATURE[1] = (average2-2.4475)/7.7935;
//   total1 = 0;
   total2 = 0;
  
   pt = 0;
}
//////////Send HW PWR AMP SET////////
void SendPWRSET(void)
{
  for(b=0;b<50;b++)
    {
    ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
    __delay_cycles(70);
    sim3 = ADC12MEM3;
    act4 = ADC12MEM4;
    total3 = total3 + sim3;
    total4 = total4 + act4;
    }
   average3 = total3 / 50;
   average4 = total4 / 50;
  
 //  step3 = average3 / 20.40 /100;
 //  swprecent = 19.5;
   swprecent = average3 / 20.20/100 ;//*0.393 +10.258;//* 0.0972 +4.2182;
     if (swprecent > 1)
       swprecent = 1;
   simtofpga = pwrvalue2[1] * swprecent;
 //  simtofpga = simtofpga / 818
    SIMFPGA[1] = simtofpga;
   SIMFPGA[0] = 0x10;
   
   
 //  step4 = average4 / 2.064 /1000;
   //acttofpga = average4 *0.442 +11.541;//90%
   acttofpga = average4 *0.466 +12.182;//95%
   if (acttofpga > 972)
     acttofpga = 972;
   
     if (31 >= TEMPERATURE[1])
     {
      acttofpga = acttofpga * (1 - (0.0025*(31 - TEMPERATURE[1])));
     prereg_adc = 818 *  (0.002*(31 - TEMPERATURE[1]));
     }
     else
     {
      acttofpga = acttofpga * (1 + (0.003*(TEMPERATURE[1] - 31)));
     prereg_adc = 818*(0.004*(TEMPERATURE[1] - 31));
     }
     if (acttofpga > 1023)
       acttofpga = 1023;
     if (prereg_adc > 200)
       prereg_adc = 200;
     TEMPREF2 = TEMPERATURE[1];
     
       
   debugact = acttofpga | 0x4000;
   
   ACTFPGA[0] = (debugact >> 8);
   ACTFPGA[1] = (acttofpga & 0xFF);
   precom_adc[1] = prereg_adc & 0xFF;
   precom_adc[0] = (prereg_adc | 0x0400) >> 8;
   total3 = 0;
   total4 = 0;  
   b = 0;
}

///////Send PRE AMP MON IN////////
void SendPREMON(void)
{
  
  for(c=0;c<20;c++)
    {
    ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
    __delay_cycles(70);
    preamp5 = ADC12MEM5;   
    total5 = total5 + preamp5;
    }
   average5 = total5 / 20;   
   average5 = (average5 -6.0661)/0.32898;
   total5 = 0;  
   c = 0;
}

//////////ADC INITIALIZE//////
void ADC(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  ADC12CTL0 &= ~ADC12ENC;
  REFCTL0 |= REFMSTR+REFVSEL_2+REFON+REFTCOFF; 
 // ADC12CTL0 |= ADC12REF2_5V + ADC12REFON;
  ADC12CTL0 |= ADC12SHT00 + ADC12ON + ADC12MSC;         // Sampling time, ADC12 on
  ADC12CTL1 = ADC12SHP+ADC12CSTARTADD_1 + ADC12CONSEQ_1 ;                     // Use sampling timer
//  ADC12IE = 0x04;                           // Enable interrupt
  ADC12MCTL1 = ADC12SREF_1 + ADC12INCH_1 ;
  ADC12MCTL2 = ADC12SREF_1 + ADC12INCH_2 ;
  ADC12MCTL3 = ADC12SREF_1 + ADC12INCH_3 ;
  ADC12MCTL4 = ADC12SREF_1 + ADC12INCH_4 ;
  ADC12MCTL5 = ADC12SREF_1 + ADC12INCH_5 + ADC12EOS ;
  ADC12CTL0 |= ADC12ENC;
  
  P6SEL |= 0x02;                            //BASE PLATE TEMP 
  P6SEL |= 0x04;                            //POW AMP TEMP
  P6SEL |= 0x08;                            //HW PWR AMP SIM SET
  P6SEL |= 0x10;                            //HW PWR AMP ACT SET
  P6SEL |= 0x20;                            //PRE AMP MON IN
  
  total1 = 0;
  total2 = 0;
  total3 = 0;
  total4 = 0;
  total5 = 0;
}
