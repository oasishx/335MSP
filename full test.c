#include <msp430f5419a.h>   //firmware library
#include "RS232.h"
#include "ADC.h"
#include "spi.h"
#include "DAC.h"
#include "flash.h"
unsigned int iareg2;
unsigned char iacom[2];
unsigned int prereg;
unsigned char precom[2];
unsigned int value;
unsigned int value2;
unsigned long stdelay;
unsigned long scdelay;

unsigned int n;
unsigned int x = 0;
unsigned svalue= 0;
                              ///// MSP TO FPGA////////
unsigned char OFF[2]= {0,11};
unsigned char SIMMER[2] = {0,8};
unsigned char ACTIVEON[2] = {0x00,0x09};
unsigned char ACTIVEOFF[2] = {0x00,0x0A};
unsigned char HSIMMER[2] = {0,12};

unsigned char TEMPREF;          //temperature reference



/////INTERRUPT FLAG
unsigned int legflag;           //Laser emission gate
unsigned int stfflag;           //Sedd temp fault
unsigned int scfflag;           //Seed current fault
unsigned int hgeflag;           //Hardware global enable
unsigned int ralflag;           //Hardware alignment laser 



//unsigned char FAULT[2];
//unsigned char STATUS[2];    //15  System Alarm
                            //12  Laser Ready
                            //9   Laser Emission Gate
                            //8   Alignment Laser Enable
                            //4   Laser Disable
                            //3   SW HW SELECT
                            //1   Active
                            //0   Global Enalbe
                            
  

/////////////HW PWR AMP SET IN//////
unsigned char CACT[2];
unsigned char CSIM[2];


 /////////////DAC VARIABLE//////
 long PRE_MON_OUT;
 long PWR_MON_OUT;
 long BASE_PLATE_MON_OUT;
 long PWR_AMP_MON_OUT;
 
/////////Fault flag/////////
unsigned int power_ok_f = 0;
//unsigned int ral_on = 0;
unsigned int power_supply_f = 0;

unsigned int delayflag = 0;
////////////////Laser ready/////
unsigned long readyflag;


                ///////////////////////MAIN////////////////////////
void main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT
  __bis_SR_register(SCG0);                  // Disable the FLL control loop
 UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
 UCSCTL1 = DCORSEL_7;                      // Select DCO range  MAX operation
 UCSCTL2 =  0x262;//FLLD_1 + 244;                   // Set DCO Multiplier for 8MHz   0x1E7;
                                         // (N + 1) * FLLRef = Fdco
                                         // (610 + 1) * 32768 = 20MHz
                                         // Set FLL Div = fDCOCLK/2
 __bic_SR_register(SCG0);                  // Enable the FLL control loop*/
                                          //20MHZ
 
  
  GE_FLAG = 0;
  FAULT[0] = 0x00;
  FAULT[1] = 0x00;
  FAULT[2] = 0x00;
  STATUS[0] = 0x00;
  STATUS[1] = 0x00;
                        /////////DIGITAL OUTPUT//////////
  
                              ////P2////
  
  P2SEL = 0x00;
  P2DIR |= BIT0;   //GLOBAL ENABLE
  P2DIR |= BIT1;   ///GLOBAL ENABLE(INVERT)
  P2DIR |= BIT2;   ///MAIN ENABLE
  P2DIR |= BIT3;   ///LASER READY OUT
  P2DIR |= BIT4;   ///PRE AMP CURRENT FAULT OUT
  P2DIR |= BIT5;   ///POWER AMP CURRENT FAULT OUT
  P2DIR |= BIT6;   ///BASE PLATE TEMP FAULT OUT
  P2DIR |= BIT7;   ///POW AMP TEMP FAULT OUT  
  P2OUT &= 0x00;   ///INITIALIZE DIGITAL OUTPUT
  P2OUT |= BIT7;   ///FOR RES2
  //P2OUT |= 0x02;   /// GLOBAL ENABLE(INVERT) INITIALIZE
  
                            ////P9////
  P9SEL = 0x00;
  P9DIR |= BIT1;   ///SEED TEMP FAULT OUT
  P9DIR |= BIT2;   ///HW SW SELECT
  P9DIR |= BIT3;   ///SMP_EN
  P9DIR |= BIT6;   ///ALIGNMENT LASER ENABLE
  P9DIR |= BIT7;   ///POWER SUPPLY V MON FAULT
  P9OUT &= 0x00;   ///INITIALIZE DIGITAL OUTPUT
  P9OUT |= BIT1;   ///FOR RES 1
  P9OUT |= BIT4;   ///FOR RES FAST 1
  P9OUT |= BIT5;   ///FOR RES FAST 2  
  

     
 
           /////////*********DIGITAL INPUT**********//////////
                          
  
                            ////P1////
  
  ////P1.0 HW GLOBAL ENALBE IN
  P1DIR &= 0xFE;    ///HW GLOBAL ENALBE IN
  P1IES |= 0x01;    //GLOBAL ENABLE EDGE ENABLE  
  P1IE |= 0x01;     //GLOBAL ENABLE INTERRUPT ENABLE
  P1IFG &= ~0x01 ;  ///GLOBAL ENABLE INTERRUPT FLAG
  
  ////P1.1 LASER DISABLE IN
  P1DIR &= 0xFD;    ///LASER DISABLE IN
  P1IES |= 0x02;    //LASER DISABLE INTERRUPT EDGE ENABLE
   P1IE |= 0x02;    //LASER DISABLE INTERRUPT ENABLE
   P1IFG &= ~0x02 ; ///LASER DISABLE INTERRUPT FLAG
  
  ////P1.2 SEED TEMP FAULT IN
  P1DIR &= 0xFB;    ///SEED TEMP FAULT IN
  P1IES &= 0xFB;    //SEED TEMP FAULT INTERRUPT EDGE ENABLE
  P1IE |= 0x04;     //SEED TEMP FAULT INTERRUPT ENABLE
  P1IFG &= ~0x04 ;  ///SEED TEMP FAULT INTERRUPT FLAG
   
  ////P1.3 SEED CURRENT FAULT IN
  P1DIR &= 0xF7;    ///SEED CURRENT FAULT IN
  P1IES &= 0xF7;    //SEED CURRENT FAULT INTERRUPT EDGE ENABLE
  P1IE |= 0x08;     //SEED CURRENT FAULT INTERRUPT ENABLE
  P1IFG &= ~0x08 ;  ///SEED CURRENT FAULT INTERRUPT FLAG
  
  ////P1.5 HW_ALIGNMENT_LASER_ENABLE
   P1DIR &= 0xDF;   ///SEED CURRENT FAULT IN
  P1IES |= 0x20;    //SEED CURRENT FAULT INTERRUPT EDGE ENABLE
  P1IE |= 0x20;     //SEED CURRENT FAULT INTERRUPT ENABLE
  P1IFG &= ~0x20 ;  ///SEED CURRENT FAULT INTERRUPT FLAG
  
  ////P1.6 POWER OK
  P1DIR &= 0xBF;    ///POWER OK IN
  P1IES &= 0xBF;    //POWER OK INTERRUPT EDGE ENABLE
  P1IE |= 0x40;     //POWER OK INTERRUPT ENABLE
  P1IFG &= ~0x40 ;  ///POWER OK INTERRUPT FLAG
              
  
  ////P9.2 LASER EMISSION GATE
  P9DIR &= 0xFB;    ///LASER EMISSION GATE IN
   
  
  
  
  
 
 // P9DIR &= 0xBF;   ///HW ALIGNMENT LASER ENABLE IN
 // P9DIR &= 0xFB;   ///HW SW SELECT
  
  P10SEL = 0x00;
 // P10DIR = 0x00;
// P10DIR &= 0xFE;  ///HW SEL STATE BIT0 IN
 // P10DIR |= 0x01; 
 // P10OUT &= 0xFE;
/*  P10DIR &= 0xFD;  ///HW SEL STATE BIT1 IN
  P10DIR &= 0xFB;  ///HW SEL STATE BIT2 IN
  P10DIR &= 0xF7;  ///HW SEL STATE BIT3 IN
  P10DIR &= 0xEF;  ///HW SEL STATE BIT4 IN
 */ 
  ////////////////////ANALOG INPUT
 // P6DIR &= 0xFD;   ///BASE PLATE TEMP IN                       // Enable A/D channel A0
 // P6DIR &= 0xFB;   ///POWER AMP TEMP IN 
 // P6DIR &= 0xDF;   ///PRE AMP MON IN
 
  
  //////////ANALOG OUTPUT
  P4SEL = 0x00;
  P4DIR |= 0x10;   ///POWER AMP TEMP MON OUT
  P4DIR |= 0x20;   ///BASE PLATE TEMP MON OUT
 
  
  RS232();  /////RS232 INITIALIZE
  ADC();    /////ADC INITIALIZE
  SPI();    /////SPI INITIALIZE
  DAC();    /////DAC INITIALIZE
  flash_pwr_read();
  pwrsiml=(pwrvalue2[0] << 8)|pwrvalue2[1];
  flash_pre_read();
   presiml=(prevalue2[0] << 8)|prevalue2[1];
  //flash_pre_read();
 // uartSend(prevalue2,2);
 
 
  
  
  
  P2OUT |= 0x04;            ///////Main_SW_EN   
          __delay_cycles(1500000);
          __delay_cycles(1500000);
          __delay_cycles(1500000);
  s_temp_set = 1;                        ////for seed temp reset
   s_cur_set = 1;                         ////for seed current reset
 // P9OUT |= 0x08;            //////SMP_EN
   
  
 __delay_cycles(2000000);
 __delay_cycles(2000000);
 __delay_cycles(2000000);
 __delay_cycles(2000000);
 __delay_cycles(2000000);   ////0.5s delay
  
  __delay_cycles(2000000);
 __delay_cycles(2000000);
 __delay_cycles(2000000);
 __delay_cycles(2000000);
 __delay_cycles(2000000);   ////0.5s delay
 
  P9OUT |= 0x08;            //////SMP_EN
  __delay_cycles(200000);

 
  __bis_SR_register(GIE); 
  
  while(1)
  {    
 //   P10OUT |= 0x01;
//    P10OUT &= 0xFE;
    
       ////////SPI//////// 
    if ((l2off == 0x00) && (ruptflag ==0))
    {
     SPICHECK();   
     if (spi_fflag == 1)
     {      
        P2OUT &= 0xFE;                            ////global enable off      
        if (power_supply_f ==0)
        uartSend(OFF,2);                          ////TO FPGA SHUT DOWN
        __delay_cycles(4000);                     ////100us delay
        P2OUT &= 0xFD;                            ////global enable(NO invert)off                
        shutflag();
         STATUS[0] |= 0x08;                        ////System alarm status on  
 //       P9OUT &= 0x7F;                          ////POWER SUPPLY V MON FAULT OUT/// ON
        GE2off |= 0x04;
        power_supply_f= 1;
     }
     else
     {
       
 //     P9OUT |= 0x80;                            ////POWER SUPPLY V MON FAULT OUT OFF
      GE2off &= 0xFB;
      power_supply_f = 0;
                        
     }
    }
  
                //////////BASE PLATE TEMP MON ///////////////// 
     if (( power_supply_f == 0)    && (ruptflag ==0))
     {
               
             if ((rt ==1) ||  (GE2off == 0x01))
             {   
               SendBTemp();
               if (rt ==1)
               uartSendTOPC(TEMPERATURE,2) ;
               
               BASE_PLATE_MON_OUT = average1 *1.1366;
               TBCCR5 = BASE_PLATE_MON_OUT; 
                     if ((TEMPERATURE[0] > 55)|| (TEMPERATURE[0] < 5))
                     {
                       P2OUT &= 0xFE;                            ////global enable off                      
                       if (GE2off != 0x01)
                       uartSend(OFF,2);                          ////TO FPGA SHUT DOWN
                       __delay_cycles(4000);                     ////100us delay
                       P2OUT &= 0xFD;                            ////global enable(NO invert)off  
                       shutflag();                                   
                      STATUS[0] |= 0x08;                        ////System alarm status on    
            //           P2OUT &= 0xBF;                            ////BASE PLATE TEMP FAULT ON////
                       GE2off |= 0x01;                            ////LATCH GE TO OFF      BASE PLATE 0x01                       
                       FAULT[0] |= BPTF;                        ////FAULT FAULT   
                       FAULTM[0] |= BPTF;
                     }
                     else
                     {
                      
              //         P2OUT |= 0x40;                            ///BASE PLATE TEMP FAULT OFF
                       FAULT[0] &= ~BPTF;                        ////FAULT FAULT CLEAR 
                        if (GE_FLAG == 1) 
                       FAULTM[0] &= ~BPTF;
                       GE2off &= 0xFE;  
                        
                       ////UNLATCH GE                     
                     }
                     rt = 0;
             }
                  //////////POWER AMP TEMP MON///////////////
                  SendPTemp();
                PWR_AMP_MON_OUT = average2 *1.1366;
               TBCCR4 = PWR_AMP_MON_OUT; 
                       if ((TEMPERATURE[1] > 55) || (TEMPERATURE[1] < 5))
                       {
                          P2OUT &= 0xFE;                            ////global enable off                          
                           if (GE2off != 0x02)
                          uartSend(OFF,2);                          ////TO FPGA SHUT DOWN
                          __delay_cycles(4000);                     ////100us delay
                          shutflag();
                           STATUS[0] |= 0x08;                        ////System alarm status on  
                          P2OUT &= 0xFD;                            ////global enable(NO invert)off                    
             //             P2OUT &= 0x7F;                            ////POWER AMP TEMP FAULT ON
                          GE2off |= 0x02;                           ////LATCH GE TO OFF      PWR_TEMP 0x02
                          FAULT[0] |= LSTF;                         ////FAULT FAULT
                           FAULTM[0] |= LSTF; 
                       }
                       else
                       {
                         
               //         P2OUT |= 0x80;                              ////POWER AMP TEMP FALUT OFF
                //        if (GE_FLAG == 1)
                        FAULT[0] &= ~LSTF;                          ////FAULT FAULT CLEAR   
                          if (GE_FLAG == 1)                      
                         FAULTM[0] &= ~LSTF;   
                        GE2off &= 0xFD;                             ////UNLATCH GE   
                        
                        }
       
       /////////////////////////Temperature compensation////////////////////////                
                        if ((HORS_FLAG == 1) && (ruptflag ==0))
       {
        if ((TEMPERATURE[1] - TEMPREF > 2) || (TEMPREF - TEMPERATURE[1] > 2))
        {
         
     iareg2 =  ampreg;
     if (31 >= TEMPERATURE[1])
     {
      iareg2 = iareg2 * (1 - (0.0025*(31 - TEMPERATURE[1])));///0.003
     prereg = 818 *  (0.002*(31 - TEMPERATURE[1]));
     }
     else
     {
      iareg2 = iareg2 * (1 + (0.003*(TEMPERATURE[1] - 31)));///0.0025
     prereg = 818*(0.004*(TEMPERATURE[1] - 31));
     }
     if (iareg2 > 1023)
       iareg2 = 1023;
     if (prereg > 200)
       prereg = 200;
     iacom[1] = iareg2 & 0xFF;
     iacom[0] = (iareg2 | 0x4000) >> 8;
     precom[1] = prereg & 0xFF;
     precom[0] = (prereg | 0x0400) >> 8;
     uartSend(iacom,2);
     uartSend(precom,2);
     TEMPREF = TEMPERATURE[1];
        } 
       }
               
 
    }
              ////////////Laser Disable////////////////
                    if  (ruptflag ==0)
                    {
                  value = (P1IN | 0xFD);
                      if (value == 0xFF)    
                      {                 
                        l2off &= 0xFE;
                       STATUS[1] &= 0xEF;                ////LASER DISABLE STATUS CLEAR
                       FAULT[0] &= ~ESF;
                       FAULTM[0] &= ~ESF;
                      }
                    }
       
                  /////////////FAULT CLEAR////////////////////
      if ((FAULT[0] == 0x00) && (FAULT[1] == 0x00) && (FAULT[2] == 0x00))
        STATUS[0] &= 0xF7;                              
                //////////////LASER READY/////////////////
                                if ((STATUS[0] | 0xF7) == 0xF7)  
                                {
                                  if (GE_FLAG == 1)
                                  {
                                  P2OUT &= 0xF7;        ////LASER READY ON
                                  STATUS[0] |= 0x10;    ////Status Laser ready on 
                                  LR = 1;
                                  }
                                }
                        
                                
                      /////////Laser Emission Gate from FPGA
                                value = (P9IN | 0xFB);
                                  if (value ==  0xFF)
                                  {
                                    STATUS[0] &= 0xFD;   ////Status LEG clear
                                    if (HORS_FLAG == 0)  ////if Hardware mode
                                    STATUS[1] &= 0xFD;   ////Status Hardware Active clear
                                  }
                                else
                                  {
                                    STATUS[0] |= 0x02;   ////Status LEG on 
                                    P9OUT &= 0xBF;       ////RAL ENABLE OFF
                                    if (HORS_FLAG == 0)  ////if Hardware mode
                                    STATUS[1] |= 0x02;   ////Status Hardware Active on
                                  }
                      ////////SEED FAULT LOOP//////
                                 value = (P1IN | 0xFB); ////SEED TEMP FAULT
                                 if (value ==  0xFB)
                                 {
                                    FAULT[2] &= ~STF;
                                    GE2off &= 0xF7;   ///0x08
                                    P9OUT |= 0x02;
                                 }
                                 value = (P1IN | 0xF7);///SEED CURRENT FAULT
                                 if (value == 0xF7)
                                 {
                                   FAULT[2] &= ~SCF;
                                   GE2off &= 0xEF;    ///0x10
                                 }
                        
                     //////HW PWR AMP SIM SET IN////  
                                          
                           if ((HORS_FLAG == 0)  && (ruptflag ==0))
                            {
                              SendPWRSET();
                             if (SIMFPGA[1] != CSIM[1])
                              {
                              uartSend(SIMFPGA,2);
                               CSIM[0] = SIMFPGA[0];
                               CSIM[1] = SIMFPGA[1];
                              }
                              if ((ACTFPGA[1] != CACT[1]) || (TEMPERATURE[1] - TEMPREF > 2) || (TEMPREF - TEMPERATURE[1] > 2))
                              {
                              uartSend(precom_adc,2);
                              uartSend(ACTFPGA,2);
                              
                              CACT[0] = ACTFPGA[0];
                              CACT[1] = ACTFPGA[1];
                              }
                            }        
                  ////////////Ral reset the GE///////
                            if (GE_FLAG == 1)
                              for_GE_rest = 1;
                           
                    
/////////////////////////SIMMER STATE//////////////////////////////////////////                            
     if  ((GE_FLAG == 1)  && (ruptflag ==0))
     {
    
             /////PRE AMP CURRENT MON ///////////////            
              SendPREMON(); 
           if ((average5 >0x4000)||(average5 == 0))
               {
                 if ((for_GE_rest == 1) && (GE_FLAG ==1))
                 {
                   P2OUT &= 0xFE;                            //global enable off
                  P9OUT &= 0xF7;                            //SMP_EN OFF                  
                   uartSend(OFF,2);                          //TO FPGA SHUT DOWN
                   __delay_cycles(4000);                     //100us delay
                   P2OUT &= 0xFD;                            //global enable(NO invert)off                                                       
                   shutflag();                                      
          //         P2OUT &= 0xEF;                            //PRE AMP CURRENT FAULT ON
                   STATUS[0] |= 0x08;                        ////System alarm status on  
                   FAULT[2] |= PreF;                         //FAULT status on
                   FAULTM[2] |= PreF;                        //FAULTM status on
                   l2off |= 0x02;                            //latch to off   
                 }                
               }
           else
               {
                
        //          P2OUT |= 0x10;                             //PRE AMP CURRENT FAULT OFF
                  FAULT[2] &= ~PreF;                         //FAULT status CLEAR
                   FAULTM[2] &= ~PreF;                       //FAULT status CLEAR
                    FAULTM[2] &= ~STF;
                    FAULTM[2] &= ~SCF;
                  l2off &= 0xFD;                             //release latch
                 
               }             
    }    
                      
           if (ruptflag)
           {    
        if   (ruptflag ==1)
        {  
            uartSend(prevalue2,2);                    //Read Pre Simmer Set
            uartSend(HSIMMER,2);
              ruptflag =0;
        }
        else if   (ruptflag ==4)
        {  
            uartSend(prevalue2,2);                    //Read Pre Simmer Set
            uartSend(counter,2);
              ruptflag =0;
        }
        else if (ruptflag ==2)
        {
            uartSend(OFF,2);
            __delay_cycles(4000);                     //100us delay
             P2OUT &= 0xFD;                              //GLOBAL ENALBE(NO INVERT) OFF
              ruptflag =0;
              __enable_interrupt();
        }
        else if (ruptflag ==3)
        {
            uartSend(counter,2);
              ruptflag =0;
              __enable_interrupt();
        }  
        
           }       
  }
  }




#pragma vector=PORT1_VECTOR
__interrupt void Port1(void)
{
  if((P1IFG&BIT1) == BIT1)                      //LASER DISABLE INTERRUPT
  { 
    P1IFG &= ~BIT1;
    P2OUT &= 0xFE;                              //global enable off
    P9OUT &= 0xF7;                              ///SMP_EN OFF 
    ruptflag =2;
    shutflag();
    STATUS[0] |= 0x08;                        ////System alarm status on  
    STATUS[1] |= 0x10;                          //LASER DISABLE STATUS
    FAULT[0] |= ESF;                            //FAULT
    FAULTM[0] |= ESF;                           //FAULTM
    l2off |= 0x01; 
  
  }
  else if ((P1IFG&BIT6) == BIT6)                //POWER OK INTERRUPT
  {
    P1IFG &= ~BIT6;
    P2OUT &= 0xFE;                              //GLOBAL ENABLE OFF
    P9OUT &= 0xF7;                              //SMP_EN OFF 
     ruptflag =2;
    shutflag();  
   
  }
  else if ((P1IFG&BIT0) == BIT0)                //HARDWARE GLOGAL ENABLE
  {
     P1IFG &= ~BIT0;
    if (HORS_FLAG == 0) 
    {
      if (hgeflag == 0)
      {
        if ((l2off == 0x02) && ((P5IN | 0x7F) == 0xFF))     //if pre fault and LEG is on
        {
              _NOP();                           //send fault to pc
        }
        
        else
        {
        if ((GE2off == 0x00) && (l2off !=0x01)) //no ge2off and no laser disable
        {
       ruptflag =1;      
      P2OUT |= 0x02;                            //Global enable(invert)on
      P2OUT |= 0x01;                            //Global enable on
      s_temp_set = 1;                        ////for seed temp reset
      s_cur_set = 1;                         ////for seed current reset
       FAULT[0] &= ~STF;
        FAULT[0] &= ~SCF;
        if ((P9OUT & 0x08) == 0x00)
         {
         P9OUT |= 0x08;                                         //SMP_EN on 
         __delay_cycles(200000);
         }
      GE_FLAG = 1;                              //GE flag to 1
      STATUS[1] |= 0x01;                        //global enable status on    
      P1IES &= 0xFE;                            //Rising edge
       hgeflag = 1;   
      }
      }
       
      }
      else
      {
         __delay_cycles(40000);                     //1000us delay
         value2 = P1IN | 0xFE;
         if (value2 == 0xFF)
         {
      
      P2OUT &= 0xFE;                           //global enable off
      ruptflag =2;
      shutflag();
      P1IES |= 0x01;                            //falling edge
      hgeflag = 0;  
         }
      }
    
    length = 2;
    i = 0;
    

    }
    
  }
  else if  ((P1IFG&BIT5) == BIT5)               //HW ALIGNMENT LASER INTERRUPT
  {
    
    P1IFG &= ~BIT5;
    if ((HORS_FLAG == 0) && ((P5IN | 0x7F )== 0x7F)) 
    {
      if (ralflag == 0)
      {
       P9OUT |= 0x40;                           //RAL ENABLE ON                             
       STATUS[0] |= 0x01;                       //FAULT FAULT    
       P1IES &= 0xDF;                           //Rising edge
      }
      else
      {
      P9OUT &= 0xBF;                            //RAL ENABLE OFF
      STATUS[0] &= 0xFE;                        //FAULT CLEAR   
       P1IES |= 0x20;                           //FALLING edge
      }
    
    length = 2;
    i = 0;
    
      if(ralflag == 0)
       ralflag = 1;                             //Rising edge
      else
       ralflag = 0;                             //falling edge
    }
   
  }
  else if ((P1IFG&BIT2) == BIT2) ///SEED TEMP FAULT INTERRUPT
  {
   
  P1IFG &= ~BIT2;
   if (s_temp_set == 1)
       s_temp_set = 0;
    else
    {
    P2OUT &= 0xFE;                            ///////GLOBAL ENABLE OFF
    ruptflag =2;
    shutflag();
    FAULT[2] |= STF;                         //FAULT status on
    FAULTM[2] |= STF;                        //FAULTM status on
    STATUS[0] |= 0x08;                        ////System alarm status on  
    P9OUT &= 0xFE;
    GE2off |= 0x08;   ///0x08
    length = 2;
    i = 0;
    }
  }
  else if ((P1IFG&BIT3) == BIT3) ///SEED CURRENT FAULT INTERRUPT
  {
  P1IFG &= ~BIT3;
    if (s_cur_set == 1)
       s_cur_set = 0;  
    else
    {
    
    P2OUT &= 0xFE;  ///////GLOBAL ENABLE OFF
    ruptflag =2;
    shutflag();  
    FAULT[2] |= SCF;                         //FAULT status on
    FAULTM[2] |= SCF;                        //FAULTM status on 
    STATUS[0] |= 0x08;                        ////System alarm status on   
    GE2off |= 0x10;   ///0x10
    
    length = 2;
    i = 0;
    }
  }
 }

