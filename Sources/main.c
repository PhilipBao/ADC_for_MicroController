// filename ******** Main.C ************** 

//***********************************************************************
// Simple ADC example for the Technological Arts EsduinoXtreme board
// by Carl Barnes, 12/03/2014
//***********************************************************************

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"  /* derivative information */
#include "SCI.h"



char string[20];
unsigned short val; 
int count = 0; 
int open = 0;  

void delayby01ms(int k);
void setClk(void);
//---------------------OutCRLF---------------------
// Output a CR,LF to SCI to move cursor to a new line
// Input: none
// Output: none
// Toggle LED each time through the loop

void OutCRLF(void){
  SCI_OutChar(CR);
  SCI_OutChar(LF);
  //PTJ ^= 0x20;          // toggle LED D2
}

void main(void) {
  count = 0;
  open = 0;
// Setup and enable ADC channel 0
// Refer to Chapter 14 in S12G Reference Manual for ADC subsystem details
	setClk();
	//ATDCTL0 = 0x01;	
	ATDCTL1 = 0x2F;		// set for 10-bit resolution  (changed)
	ATDCTL3 = 0x88;		// right justified, one sample per sequence
	ATDCTL4 = 0x01;		// prescaler = 0; ATD clock = 16MHz / (2 * (0 + 1)) == 8MHz       (changed 8 MHz)
	ATDCTL5 = 0x29;		// continuous conversion on channel 9 (changed)
	
  TSCR1 = 0x90;    //Timer System Control Register 1
  TSCR2 = 0x00;    //Timer System Control Register 2
  TIOS = 0xFE;     //Timer Input Capture or Output capture
  PERT = 0x01;     //Enable Pull-Up resistor on TIC[0]
  TCTL3 = 0x00;    //TCTL3 & TCTL4 configure which edge(s) to capture
  TCTL4 = 0x02;    //Configured for falling edge on TIC[0] 
  TIE = 0x01;      //Timer Interrupt Enable
  EnableInterrupts; //CodeWarrior's method of enabling interrupts
  	
// Setup LED and SCI
  DDRJ |= 0x01;     // PortJ bit 0 is output to LED D2 on DIG13
  SCI_Init(19200);
  
  SCI_OutString("Electronic Arts - EsduinoXtreme ADC demo"); OutCRLF();
  for(;;) {
    
    if(open == 1) {
      PTJ ^= 0x01;          // toggle LED
      val=ATDDR0;
      
      SCI_OutUDec(val);
      
      OutCRLF();
    }
    delayby01ms(17);    //sampling rate = 300Hz changed
  }
}






interrupt  VectorNumber_Vtimch0 void ISR_Vtimch0(void){	
  unsigned int temp; //DON'T EDIT THIS
	
	
//SCI_OutUDec(count); OutCRLF();//output count
	 
	if(count%2 == 1){
	  open = 0;
	  PTJ = 0x00;
	}
	else open = 1;
	count++;//add count
	
	temp = TC0; 
}





void delayby01ms(int k){
      int ix;
      //TSCR1 = 0x90;  /* enable timer and fast timer flag clear */
      //TSCR2 = 0x00;  /* disable timer interrupt, set prescaler to 1*/
      TIOS |= 0x02; /* enable OC1 */        //(not necessary)
      TC1 = TCNT + 400;
      for(ix = 0; ix < k; ix++) {
             while(!(TFLG1_C1F));
             TC1 += 400;
      }
      TIOS  &= ~0x02;   /* disable OC2 */  //(not necessary)
}
 
 
#define initSYNR  0x04    //02
#define VCOFRQ    0x40    //40
#define initREFDV 0x00    //00
#define REFFRQ    0x80    //80 
 
 
 
void setClk(void)
{
    
    //CPMUPROT = 0; //Protection of clock configuration is disabled
    CPMUCLKS = 0x00;        //81

    
    CPMUSYNR = initSYNR+VCOFRQ; //set PLL multiplier (0x42 = 01 000010)
    CPMUREFDIV = initREFDV+REFFRQ; //set PLL divider (0x80 = 10 000000)
 
    
    //while (CPMUFLG_LOCK == 0) {} // wait for it
    CPMUPOSTDIV = 0x09; // set Post Divider// 3
    CPMUOSC = 0x80; // turn off Enable external oscillator //C0
    while (CPMUFLG_LOCK == 0) {} // wait for it
    CPMUPLL = CPMUCLKS; // Engage PLL to system
}
 
