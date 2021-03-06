/*
 * BME 464 - Lab B
 *
 * Run a real-time clock and display the time on the LCD
 * Template: Nick Bottenus - 9/3/12
 * Student: (your name here)
 *
 */


#include "Lcd.h"
#include <delays.h>
#include <p18f46k22.h>
#include <stdio.h>

#pragma config FOSC = INTIO67   // Internal OSC block, Port Function on RA6/7
#pragma config WDTEN = OFF      // Watch Dog Timer disabled. SWDTEN no effect
#pragma config XINST = OFF      // Instruction set Extension and indexed Addressing mode disabled

//Define statements
#define One_Sec  0x80 	//Load high byte of timer 1 with this for 1 second
// Timer 1 clock source is crystal oscillator on T1OS1/T1OS0, 1:1,
// Dedicated enabled, Do Not Synch, Enable Timer1
#define Timer1  0x89

//Variable definitions
unsigned char Sec; //Variables for keeping time
unsigned char Min;
unsigned char Hour;
//unsigned char test;
unsigned char state;

//Function definitions
void RTC_ISR(void);
void SysInit(void);
void High_Priority_ISR(void);

//To be written by the student
void RTCIncSec(void);
void RTCIncMin(void);
void RTCIncHour(void);
void WriteTime(void);
unsigned char isLeftBtnPressed(void);
unsigned char isRightBtnPressed(void);

//High priority interrupt
#pragma code InterruptVectorHigh = 0x08
void InterruptVectorHigh (void)
{
  _asm
    goto High_Priority_ISR
  _endasm
}

#pragma interrupt High_Priority_ISR
void High_Priority_ISR(void)
{
    RTC_ISR(); //Call real-time clock service routine
}

void main(void)
{
    //Initialize
    SysInit();
    LCDClear();
    LCDGoto(0, 1);
    LCDWriteStr("LB=Mode");

    //Loop (except while servicing interrupts)
    while(1)
    {
		WriteTime();                // Show on LCD

		LCDGoto(0,1);
		//Display any instructions to user

		//Check for button presses, etc
        isLeftBtnPressed();
        isRightBtnPressed();
		Delay10KTCYx(10);           // Delay 1/10 second
    };
}

//Initialize necessary systems
void SysInit(void)
{
    OSCCON=0b01010110; //4 MHz internal oscillator

    //Set up buttons
    ANSELBbits.ANSB0=0; //Digital
    TRISAbits.RA4=1; //Input
    TRISBbits.RB0=1; //Input

    //Set up LCD
    ANSELD = 0x00;
    TRISD = 0x00; //Digital out

    LCDInit(); //Start LCD
    LCDWriteStr("Starting clock...");

    //Set up timer
    TMR1H  = One_Sec;
    TMR1L  = 0;
    T1CON  = Timer1;		// Configure Timer 1
    T1GCON = 0;                 // Timer 1 Gate function disabled
    RCONbits.IPEN=1;            // Allow interrupt priorities
    PIR1bits.TMR1IF = 0;        // Clear any pending Timer 1 Interrupt indication
    PIE1bits.TMR1IE = 1;        // Enable Timer 1 Interrupt
    INTCONbits.GIE=1;           // Enable interrupts

    //Reset variables
    Hour  = 0;
    Min   = 0;
    Sec   = 0;
    state = 0;
}

//Called every second by the interrupt
void RTC_ISR (void)
{
    if (PIR1bits.TMR1IF)            // If timer overflowed
    {
        TMR1H  |= One_Sec;          // Reset timer to one second
        RTCIncSec();                // Increment count
        PIR1bits.TMR1IF = 0;        // Clear timer flag
        INTCONbits.INT0IF = 0;      // Clear interrupt flag
    }
}

//***********************
//BME 464 Students: Edit subroutines below here BEFORE class
//***********************

//Increment Sec variable, correctly roll over Sec, Min and Hour
void RTCIncSec(void)
{
    if (state%3 == 0){
        Sec++;
        if (Sec == 60){
            Sec = 0;
            RTCIncMin();
        }
    }
}

//Increment Min variable, correctly roll over Min and Hour
void RTCIncMin(void)
{
    Min++;
    if (Min == 60){
        Min = 0;
        RTCIncHour();
    }
}

//Increment Hour variable, correctly roll over Hour
void RTCIncHour(void)
{
    Hour++;
    if (Hour == 24){
        Hour = 0;
    }
}

//Display time on LCD as HH:MM:SS at row and column 0
//The hour is in variable Hour
//The minute is in variable Min
//The second is in variable Sec
void WriteTime(void)
{
    /*char time_str[16];
    sprintf(time_str, "%02d:%02d:%02d", Hour, Min, Sec);
    LCDClear();
    LCDGoto(0, 0);
    LCDWriteStr("hi");*/
    LCDGoto(0, 0);
    LCDPutByte(Hour);
    LCDWriteStr(":");
    LCDPutByte(Min);
    LCDWriteStr(":");
    LCDPutByte(Sec);
}

//***********************
//BME 464 Students: Edit subroutines below here DURING class
//***********************

//Return 1 if RA4 button has been pressed, 0 otherwise
//
//Be sure to include some form of debouncing
//
//The button is accessed using PORTAbits.RA4, a 0 means the button is pressed
unsigned char isLeftBtnPressed(void)
{
    if (PORTAbits.RA4 == 0){
        Delay10KTCYx(5);
        state++;
        //LCDGoto(10, 1);
        //test++;
        //LCDPutByte(test);
        if (state%3 == 1){
            LCDGoto(0, 1);
            LCDWriteStr(" ^          ");
            Sec = 0;
            //RTCIncHour();
        }else if(state%3 == 2){
            LCDGoto(0, 1);
            LCDWriteStr("    ^       ");
            Sec = 0;
            //RTCIncMin();
        }else{
            LCDClear();
            LCDGoto(0, 1);
            LCDWriteStr("                ");
        }
        return 1; //button pressed
    }
}

//Return 1 if RB0 button has been pressed, 0 otherwise
//
//Be sure to include some form of debouncing
//
//
//The button is accessed using PORTBbits.RB0, a 0 means the button is pressed
unsigned char isRightBtnPressed(void)
{
    if (PORTBbits.RB0 == 0){
        Delay10KTCYx(5);
        //LCDGoto(10, 1);
        //test++;
        //LCDPutByte(test);
        if (state%3 == 1){
            Sec = 0;
            RTCIncHour();
        }else if(state%3 == 2){
           Sec = 0;
            RTCIncMin();
        }else{
            //LCDClear();
        }
        return 1; //button pressed
    }
}
