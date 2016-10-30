/********************************************************************
*
*
*
*********************************************************************
* FileName:        Lcd.c
* Dependencies:    See INCLUDES section below
* Processor: 	   PIC18F46K22
* Compiler: 	   C-18
* Company:         Microchip Technology, Inc.

*********************************************************************
* File Description: This modules includes all service functions for LCD
*                   operations.
*
* Change History:
* Author               Date        Comment
* Chris Tucker      10/18/2010  First version of module code
********************************************************************/

/**
*   @file Lcd.c
*   @brief This modules includes all service functions for LCD
*          operations.
*   @defgroup LCD	LCD Display
*
*/

//*****************************************************************************
// Include and Header files
//*****************************************************************************

#include "System.h"

//*****************************************************************************
//                            CONSTANT DEFINITION
//*****************************************************************************


//*****************************************************************************
//                                SUPPORT MACROS
//*****************************************************************************

//*****************************************************************************
//                               SUPPORT FUNCTIONS
//*****************************************************************************

void buttonInit(void){          // Set up buttons
    ANSELBbits.ANSB0=0; //Digital
    ANSELAbits.ANSA0=0; //don't I need this?
    TRISAbits.RA4=1; //Input
    TRISBbits.RB0=1; //Input

    //Set up button on RB1
    ANSELBbits.ANSB1=0; //Digital
    TRISBbits.RB1=1; //Input
}

void LEDInit(void){
    ANSELBbits.ANSB3 = 0; //Digital IO
    ANSELBbits.ANSB1 = 0;
    LATB=0b00000000; //LEDs off
    TRISBbits.RB3 = 0; //LEDs are outputs    //RB0 is tied to the button too
    //TRISBbits.RB1 = 0;
    ANSELBbits.ANSB5 = 0;
    TRISBbits.RB5 = 0;
}

void potentiometerInit(void){
    ANSELAbits.ANSA0 = 1;
    TRISAbits.RA0 = 1; //Analog in
    ADCON2bits.ACQT=001; //2 TAD
    ADCON2bits.ADCS=010; //FOSC/32
    ADCON2bits.ADFM=1; //Left justified
    ADCON0bits.ADON=1; //Turn on A/D
    ADCON0bits.CHS = 0000;
}

void Tmr0Init(void){
    T0CONbits.T0CS = 0;     //use internal clock (4MHz/ 4)
    T0CONbits.T08BIT=0; //16 bit counter
    T0CONbits.PSA=1; //Don't use prescaler (1:1)}
}

unsigned char isLeftBtnPressed(void){
  if (PORTAbits.RA4 == 0){
      Delay10KTCYx(10);
      return 1;
 }
 return 0;
}

unsigned char isRightBtnPressed(void){
  if (PORTBbits.RB0 == 0){
      Delay10KTCYx(10);
      return 1;
 }
 return 0;
}

unsigned char isBtnPressed(void){
    if (PORTBbits.RB1 == 1){
        Delay10KTCYx(10);
      return 1;
    }
 return 0;
}
void enableSleep(bool enableWDT){
    OSCCONbits.IDLEN = 0;
    INTCON3bits.INT1IE = 1;
    if (enableWDT){
        WDTCONbits.SWDTEN = 1;
    }
    Sleep();
    INTCON3bits.INT1F = 0;
    WDTCONbits.SWDTEN = 0;
    //LCDWriteStr("Awake!          ");
}
