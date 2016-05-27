
#include <msp430f5419a.h>

extern unsigned char prevalue2[2];
extern unsigned char pwrvalue2[2];
int prec;
int pwrc;





int flash_write(void)
{
  unsigned long * Flash_ptrD;               // Initialize Flash pointer Seg D
  unsigned long * Flash_ptrB; 
  unsigned long prevalue;
  unsigned long pwrvalue;
  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT

  Flash_ptrD = (unsigned long *) 0x1800;    // Initialize Flash pointer
  Flash_ptrB = (unsigned long *) 0x1810;
  prevalue = presiml;                       // Initialize Value
   pwrvalue = pwrsiml; 
  __disable_interrupt();                    // 5xx Workaround: Disable global
                                            // interrupt while erasing. Re-Enable
                                            // GIE if needed
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY+ERASE;                      // Set Erase bit
  *Flash_ptrD = 0;                          // Dummy write to erase Flash seg
  *Flash_ptrB = 0;
  FCTL1 = FWKEY+BLKWRT;                     // Enable long-word write
  *Flash_ptrD = prevalue;                      // Write to Flash
  *Flash_ptrB = pwrvalue; 
  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY+LOCK;                       // Set LOCK bit
                                   // Loop forever, SET BREAKPOINT HERE
//__bis_SR_register(GIE);
}

/*int flash_pwr_write(void)
{
  unsigned long * Flash_ptrB;               // Initialize Flash pointer Seg D
  unsigned long pwrvalue;
  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT

  Flash_ptrB = (unsigned long *) 0x1810;    // Initialize Flash pointer
  pwrvalue = pwrsiml;                       // Initialize Value
  __disable_interrupt();                    // 5xx Workaround: Disable global
                                            // interrupt while erasing. Re-Enable
                                            // GIE if needed
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY+ERASE;                      // Set Erase bit
  *Flash_ptrB = 0;                          // Dummy write to erase Flash seg
  FCTL1 = FWKEY+BLKWRT;                     // Enable long-word write
  *Flash_ptrB = pwrvalue;                      // Write to Flash
  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY+LOCK;                       // Set LOCK bit
                                   // Loop forever, SET BREAKPOINT HERE
}
*/



int flash_pre_read(void)
{
 // WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT
 // prevalue = 0;                                // initialize value

  char *Flash_ptrC;
  

  Flash_ptrC = (char *) 0x1800;             // Initialize Flash segment C ptr
  
  __disable_interrupt();                    // 5xx Workaround: Disable global
                                            // interrupt while erasing. Re-Enable
                                            // GIE if needed
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY+ERASE;                      // Set Erase bit
  FCTL1 = FWKEY+WRT;                        // Set WRT bit for write operation

  for(prec = 2; prec >0; prec--)
  {
    prevalue2[prec-1]= *Flash_ptrC++;          
  }

  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY+LOCK;                       // Set LOCK bit
 // __bis_SR_register(GIE);
}

int flash_pwr_read(void)
{
 // WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT
 // prevalue = 0;                                // initialize value

  char *Flash_ptrB;
  

  Flash_ptrB = (char *) 0x1810;             // Initialize Flash segment C ptr
  
  __disable_interrupt();                    // 5xx Workaround: Disable global
                                            // interrupt while erasing. Re-Enable
                                            // GIE if needed
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY+ERASE;                      // Set Erase bit
  FCTL1 = FWKEY+WRT;                        // Set WRT bit for write operation

  for(pwrc = 2; pwrc >0; pwrc--)
  {
    pwrvalue2[pwrc-1]= *Flash_ptrB++;          
  }

  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY+LOCK;                       // Set LOCK bit
 // __bis_SR_register(GIE);
}