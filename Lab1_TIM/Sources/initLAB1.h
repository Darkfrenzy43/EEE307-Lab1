/* ********************************************************************************
**
** File: initLAB1.h 
**
** Author: Dr. Rozon
**
** Description: Header file for LAB1
**
** Revision History: Rev 0: Jan 2023
**
******************************************************************************** */

#include <hidef.h>       /* common defines and macros */
#include "derivative.h"  /* derivative-specific definitions */

// Constants for TIM channels
/* Speaker  */
#define SPEAKER_TC   TC3          // Name for TC3
#define DURATION_TC  TC0          // Name for TC0
#define SPKR_OFF   0b00111111     // TC3 toggle off mask
#define SPKR_ON    0b01000000     // TC3 toggle on mask
#define SPEAKER       0b00001000  // TC3 mask
#define TONEDURATION  0b00000001  // TC0 mask
#define BUT_CH7_M     0b10000000  // TC7 mask
#define BUT_CH6_M     0b01000000  // TC7 mask
#define BUT_CH5_M     0b00100000  // TC7 mask
#define BUT_CH4_M     0b00010000  // TC7 mask


// Constants used for setting active high LEDs
#define LED1     0x80
#define LED2     0x40
#define LED3     0x20
#define LED4     0x10
#define LED34    0x30
#define LED234   0x70
#define LEDSOFF  0x00

// Tone, tone duration (based on ECLK=4MHz/64) and led pattern 
#define dot   32   // 500 Hz tone
#define dash  64   // 250 Hz tone
#define blank 1    // value need to be >0; 
#define brk 0      // indicates end of code to transmit
#define dot_duration  9000 // 256 msec
#define dash_duration 4*dot_duration
#define blank_duration dot_duration
#define brk_duration dot_duration
#define dotLED   LED4
#define dashLED  LED3

// Define data structure of Morse code
struct MorseCode
  {
  unsigned int tone;
  unsigned int duration;
  unsigned char leds;
  };

/**** Function DECLARATIONS ****/
void setECLK_MODE(void);      // to set ECLK speed and mode of operation
void initTIM(void);           // to prepare Enhanced Capture Timer (TIM: Timer Interface Module)
void initPTM(void);           // to set I/O lines for Port M connected to LEDs
void setLEDs(unsigned char);  // to set pattern on LEDs
void initCode(struct MorseCode *code); // to initialize hardware to send code 
void sendCode(void);                   // to send code
void stopCode(void);                   // to stop sending code

// Added
void initPTT(void);


/*** Additional code/constants for buttons ***/ 
// As you see fit // 


