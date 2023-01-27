/********************************************************************************
*  File:  main.c
*  Description: Contains declarations and function calls for LAB1                          
*********************************************************************************/                          

#include <hidef.h>       /* common defines and macros */
#include "derivative.h"  /* derivative-specific definitions */
#include "initLAB1.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


// MorseCode type is defined in initLAB1.h
// Declare Morse Code to be transmitted 
struct MorseCode SOS[] =  
  {
    { dot, dot_duration, LED4 } ,
    { blank, blank_duration, LEDSOFF } ,
    { dot, dot_duration, LED4 } ,
    { blank, blank_duration, LEDSOFF } ,
    { dot, dot_duration, LED4 } ,
    { blank, blank_duration, LEDSOFF } ,
    { dash, dash_duration, LED34 } ,
    { blank, blank_duration, LEDSOFF } ,
    { dash, dash_duration, LED34 } ,
    { blank, blank_duration, LEDSOFF } ,
    { dash, dash_duration, LED34 } ,
    { blank, blank_duration, LEDSOFF } ,
    { dot, dot_duration, LED4 } ,
    { blank, blank_duration, LEDSOFF } ,
    { dot, dot_duration, LED4 } ,
    { blank, blank_duration, LEDSOFF } , 
    { dot, dot_duration, LED4 } ,
    { blank, blank_duration, LEDSOFF } , 
    { brk, brk_duration, LEDSOFF }        //End of code
  };

void main(void) 
{


 setECLK_MODE();      // set ECLK speed and mode of operation
 initTIM();           // prepare Enhanced Capture Timer (TIM: Timer Interface Module)
 initPTM();           // set I/O lines for Port M connected to LEDs
 initPTT();           // set I/O lines for PTT which connects switches and speaker
 initCode(SOS);       // prepare channels to send code
 EnableInterrupts;    // need to enable interrupts, else hardware will not be served
 sendCode();          // transmit SOS code
   
 for(;;)
   {
     asm("nop");   // loop and wait for interrupt
   }
}
