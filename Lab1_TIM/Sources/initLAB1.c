/********************************************************************************
*  File:  initLAB1.c
*  Description: Contains declarations and function definitions
*               used by the main.c file for LAB1                          
*********************************************************************************/

// INCLUDE FILES
#include <hidef.h>        /* common defines and macros */
#include "derivative.h"   /* derivative-specific definitions */
#include "initLAB1.h"

// Global variables
struct MorseCode *codeStart;    // Pointer to start of code
struct MorseCode *currentCode;  // Pointer to current code member

/**** FUNCTION DEFINITIONS ******/

/************************************************************************
*  Function: void setECLK_MODE(void)
*
*  Description: The crystal osscilator on the HCS12DP512 processor 
*       is input into a phase-locked-loop module that multiplies and 
*       divides the ECLK by integer values
*     ECLK = OSCCLK * (SYNR+1)/(REFDV+1)
*     Note: For LABS 1-3, board OSCCLK =  4 MHz
*           For LABS 4-7, board OSCCLK = 16 MHz
*  REFERENCES: Valvano Sec 1.8 Page 46
*    and S12CRGV4.pdf  Clock and Reset Generator Block User Guide (Motorola)
* ***********************************************************************/

void setECLK_MODE(void) 
  {
  // REQUIREMENTS: Set SYNR and REFDV values to obtain the desired ECLK rate

    SYNR = 0x00;
   REFDV = 0x00;
   
  CLKSEL = 0x00; // Ensure the clock is driven from the crystal initially
                 // PLLSEL | PSTP | SYSWAI | ROAWAI | PLLWAI | CWAI | RTIWAI | COPWAI 
  PLLCTL = 0xD1; // Set PLL control
                 // CME | PLLON | AUTO | ACQ | 0 | PRE | PCE | SCME
     
  // Wait for PLLCLK to stabilize
  while   ((CRGFLG & CRGFLG_LOCK_MASK)==0);
  CLKSEL |= CLKSEL_PLLSEL_MASK; // Set the CLKSEL bit to take ECLK = PLLCLK/2
  
   // Set MODE of operation
  MODE = 0x80;   // Set HCS12 in normal single-chip mode
  MISC = 0x01;   // Turn ON the Flash ROM
  // PEAR = PEAR_RDWE_MASK|PEAR_LSTRE_MASK;  // Useful for external interfacing  
  }
  
/********************************************************************************
* Function: void initTIM(void)  
* REQUIREMENTS:
*     - enable TIM
*     - prescale clk
*     - disable all TIM interrups
*     - clear all TIM interrupt flags 
********************************************************************************/ 
void initTIM(void)
 {
 
  // enable TIM
  TSCR1 = TSCR1_TEN_MASK;
  
  // prescale clk
  TSCR2 = 0x06; //00000110
  
  // disable all TIM interrupts
  TIE = 0x00;
  
  // clear all TIM interrupt flags    
  TFLG1 = 0xFF;
 }
 
/********************************************************************************
* Function: initPTM
* REQUIREMENTS:
*  - Initialize pins PTM(7:4) as outputs lines connected to LEDs
*  - Turns off LEDs
*********************************************************************************/
void initPTM(void)
    {
    
  // Initialize pins PTM(7:4) as outputs lines connected to LEDs
  DDRM = 0xF0;
  
  // Turns off LEDs
  PTM = 0x00;
    }
    
    
    
/********************************************************************************
* Function: initPTT
* REQUIREMENTS:
*  - Initialize pins PTT(7:4) as input, and pin PTT(3) as output
*********************************************************************************/
void initPTT(void)
    {
    
  // Initialize pins PTT(7:4) as input, and pin PTT(3) as output
  DDRT = 0x08;
    }
    
    
    

/* ********************************************************************************
* Function: void setLEDs(unsigned char leds)
* 
* REQUIREMENTS: 
*  - Turns LEDS on/off
* Inputs: 8-bit pattern for LEDS    
* Outputs:Set lines PTM(7:4) to turn each LED on/off     
***********************************************************************************/       
void setLEDs(unsigned char leds)
 {
   // Turn LEDS on/off by setting it to new bit pattern
   PTM = leds;
 }

/*********************************************************************************
* Function   void initCODE(struct MorseCode *code) 
* REQUIREMENTS:
*    - Initialize pointer to start of code
*    - Initialize pointer to current code member,
*    - Enable Output Compare channels, disconnect speaker,
*    - Disable Speaker and Duration channel interrupts,
*    - Clear Speaker and Duration channel interrupt flags,
*    - Set LED pattern to 1111. 
*  Inputs:  Pointer to beginning of Morse Code
*  Outputs: LED pattern 
*********************************************************************************/                   
void initCode(struct MorseCode *code)
  {   
  //Initialize pointer to start of code
  codeStart = code;
    
  //Initialize pointer to current code member,
  currentCode = code;   
  
  //Enable Output Compare channels, disconnect speaker. Ch(7:4) (for buttons) default to input 
  TIOS |= SPEAKER | TONEDURATION;
  
  
  
  /* NOTE: With help from Techs, deduced that perhaps setting the TC1 channel to one by setting TCTL2(1:0) to SET may have been the issue. 
    Below we force them all to be zero (disconnect) upon initialization, which suprisingly fixed the problem.
    
    Logically, having TCTL2 &= SPKR_OFF should not be an issue, yet in praticallity it happened to be the issue in this case. */
  //TCTL2 &= SPKR_OFF;
  TCTL2 = 0x00;
  
  //Disable Speaker and Duration channel interrupts,
  TIE &= ~(SPEAKER | TONEDURATION);  
  
  //Clear Speaker and Duration channel interrupt flags,
  TFLG1 |= SPEAKER | TONEDURATION;
  
  //Set LED pattern to 1111.
  setLEDs(0xF0);                     // Swapped from 0x0F to 0xF0 to change PTM(7:4)
  
  /*** Additional code for buttons ***
  *  REQUIREMENTS:
  *  - Set buttons for Input Capture
  *  - Disconnect Input Capture of buttons
  *  - Disarm interrupts by buttons
  *  - Clear button channels interrupt flags  */ 
  
  
  // Ch(7:4) in TIOS automatically set to 0 (input) 
  
  // Disconnect all Input Capture of buttons --> THIS CAUSES THE SAME ISSUE! NEED TO EXCLUDE
  // TCTL3 = 0x00;
  
  // Disarm interrupts of buttons
  TIE &= 0x0F;
  
  // Clear button channel flags
  TFLG1 |= 0xF0;

   
  }
  
/*********************************************************************************
* Function   void sendCode(void)
* REQUIREMENTS: 
* Uses current code pointer to
*    - Set tone value in SPEAKER_TC
*    - Set duration value in DURATION_TC
*    - Set LED pattern for current tone
*    and
*    - Enable interrupts for Speaker and Duration channels 
*    - Clear interrupt flags for Speaker and Duration channels
*    - Enable Speaker toggle without affecting other channels    
*  Inputs: none      
*  Outputs: LED pattern for note and tone heard on speaker.   
*********************************************************************************/                    
void sendCode(void)
  {             
  //Set tone value in SPEAKER_TC
  SPEAKER_TC = (currentCode -> tone) + TCNT;      //TCNT
  
  //Set duration value in DURATION_TC
  DURATION_TC = (currentCode -> duration) + TCNT;	
  
  //Set LED pattern for current tone
  setLEDs(currentCode -> leds);  

  //Enable interrupts for Speaker and Duration channels 
  TIE |= SPEAKER | TONEDURATION;                            // Changed from SPEAKER_TC (the register) to SPEAKER (the mask)
                                                            // Changed from DURATION_TC (the register) to TONEDURATION (the mask) 
    
  //Clear interrupt flags for Speaker and Duration channels
  TFLG1 |= SPEAKER | TONEDURATION;  
  
  //Enable Speaker toggle without affecting other channels
  TCTL2 |= SPKR_ON;   
  }
  
 
  
/********************************************************************************* 
* Function   void stopCode(void) 
* REQUIREMENTS: 
*    - Disable speaker and duration interrupts,
*    - Clear speaker and duration interrupt flags,
*    - Disable speaker toggling (turns off speaker),
*    - Turn ON all LEDs to indicate end of code,
*  Inputs:  none
*  Outputs: All LEDs are turned ON.  
*********************************************************************************/                
void stopCode(void)
  {
  //Disable speaker and duration interrupts,
  TIE &= ~(SPEAKER | TONEDURATION);
  
  //Clear speaker and duration interrupt flags,
  TFLG1 |= SPEAKER | TONEDURATION;
  
  //Disable speaker toggling (turns off speaker),
  TCTL2 &= SPKR_OFF;
  
  //Turn ON all LEDs to indicate end of code,
  setLEDs(~LEDSOFF);  
 
  /*** Additional code for buttons ***
  * REQUIREMENTS:
  *  - Arm SW1 interrupts
  *  - Clear button channels interrupt flags
  *  - Enable fallling edge detection of buttons */
  
  // Arming SW1 interrupts
  TIE = BUT_CH4_M;
  
  // Clear button channel interrupt flags
  TFLG1 |= 0xF0;
  
  // Have button ISRs detect falling edge
  TCTL3 = 0xAA;

  } 
 
/****** Start of PRAGMA and ISRs ******/
/********************************************************************************
*  ISR:  toneDurationISR - is called whenever TCNT hits DURATION_TC.      
*
*       
*  REQUIREMENTS:
*    - If end of code (brk) reached, stop sending code
*      else 
*    - Update tone, duration and LED pattern for current code
*    - Clear duration interrupt flag,
*    - Advance pointer to next code
*  Inputs: None       
*  Outputs:LED pattern for current code
********************************************************************************  */          

#pragma CODE_SEG NON_BANKED
void interrupt VectorNumber_Vtimch0 toneDurationISR(void)
  {
     
     // If currentCode reached end of struct array, stop sending the code
     if (currentCode -> tone == brk) {
     
        stopCode();
        
     } else {  // Otherwise, prep for next duration interrupt...
     
        // update the tone, duration, and LED pattern to that of current code
        SPEAKER_TC = (currentCode -> tone) + TCNT;
        DURATION_TC = (currentCode -> duration) + TCNT;
        setLEDs(currentCode -> leds);  
      
        // Next, clear duration's interrupt flag
        TFLG1 |= TONEDURATION;
        
        // Finally, advance pointer to next morse code through incrementation
        currentCode++;
                              
        
     }

  }                                 

/********************************************************************************
*  ISR: SpeakerISR
*  REQUIREMENTS:
*     - If tone is blank, turn off speaker toggle  
*        else
*     - Turn on speaker toggle
*     - Clear speaker interrupt flag,
*     - Update speaker half-period to continue with current tone
*  Outputs: Current cone continues to be sent 
*********************************************************************************/           

void interrupt VectorNumber_Vtimch3 SpeakerISR(void)
  {
     /* Insert your code here  */
          
        // Ack speakerISR's interrupt flag
        TFLG1 |= SPEAKER;
     
     // If current tone is blank, turn off speaker toggle (so we don't hear anything)
     if (currentCode -> tone == blank) {
     
        TCTL2 &= SPKR_OFF;
      
     }
     
     else {   // Otherwise, prep for the next speakerISR
     
        // Turn on speaker toggle
        TCTL2 = (TCTL2 & 0x3F) | SPKR_ON;     
        
        // Update speaker with the half period of the current tone to continue making the noise
        SPEAKER_TC = (currentCode -> tone) + TCNT;
              
     }      
  }   

  

// ----------- Button switches ISRs -------------


/*********************************************************************************
*  ISR:  SW1_ISR
*  REQUIREMENTS: - Arm/Disarm switches of interest
*                - Set LEDs to required pattern
*                - Clear relevant interrupt flags
*  Outputs: LED pattern of (X I I I)
* ********************************************************************************/           
void interrupt VectorNumber_Vtimch4 SW1_ISR(void)
  { 
     TIE = BUT_CH5_M;
     setLEDs(LED234);
     TFLG1 |= BUT_CH4_M;
     
  } 


/*********************************************************************************
*  ISR:  SW2_ISR
*  REQUIREMENTS: - Arm/Disarm switches of interest
*                - Set LEDs to required pattern
*                - Clear relevant interrupt flags
*  Outputs: LED pattern of (X X I I)
* ********************************************************************************/   
void interrupt VectorNumber_Vtimch5 SW2_ISR(void)
  { 
     TIE = BUT_CH6_M;
     setLEDs(LED34);
     TFLG1 |= BUT_CH5_M;
  } 


/*********************************************************************************
*  ISR:  SW1_ISR
*  REQUIREMENTS: - Arm/Disarm switches of interest
*                - Set LEDs to required pattern
*                - Clear relevant interrupt flags
*  Outputs: LED pattern of (X X X I)
* ********************************************************************************/   
void interrupt VectorNumber_Vtimch6 SW3_ISR(void)
  {                            
     TIE = BUT_CH7_M;
     setLEDs(LED4);
     TFLG1 |= BUT_CH6_M;
  } 


/*********************************************************************************
*  ISR:  SW1_ISR
*  REQUIREMENTS: - Arm/Disarm switches of interest
*                - Set LEDs to required pattern
*                - Clear relevant interrupt flags
*  Outputs: LED pattern of (X X X X)
* ********************************************************************************/   
void interrupt VectorNumber_Vtimch7 SW4_ISR(void)
  { 
     TIE = 0x00;
     setLEDs(LEDSOFF);
     TFLG1 |= BUT_CH7_M;
  }
  
  
  
   
/****** End of PRAGMA ******/
#pragma CODE_SEG DEFAULT 