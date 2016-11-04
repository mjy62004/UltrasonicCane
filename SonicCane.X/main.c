/*
 * Integration
 */

#include <p18f46k22.h>
#include <delays.h>
#include "Lcd.h"
#include <stdio.h>
#include "System.h"

#pragma config WDTEN=OFF
#pragma config FOSC=INTIO67
#pragma config XINST=OFF


#define LOWBYTE(v)   ((unsigned char) (v))
#define HIGHBYTE(v)  ((unsigned char) (((unsigned int) (v)) >> 8))

#define STANDARD 0
#define PULSE 1
#define PULSE_RECORD 2
#define PWM 3
#define DELAY_TEST 999
#define PULSE_RECORD_PWM 4
#define CONT_RECORD_PWM 5
#define NO_OF_STATES 6
//#define TMRL 0x58
//#define TMRH 0x9E

void LCDWriteLevels(int);

int state = 0;

void main(void)
{
    char str[4];
    int volt = 0;
    int myVolt;
    SysInit();
    //if (isLeftBtnPressed){state = 0;}
    //if (isRightBtnPressed){state++;}
    while(1){
         if (isRightBtnPressed()==1){
             state++;
         }
         switch (state%NO_OF_STATES) {
             case STANDARD :
                 //Start A/D Conversion
                 LCDGoto(0, 0);
                 LCDWriteStr("Demo: Pot ADC   ");
                 LCDGoto(0, 1);
                 LCDWriteLevels(analogRead(0));
                 break;
                 
             case PULSE:
                 LATAbits.LATA5 = 1;
                 LATAbits.LATA3 = 0;
                 LCDGoto(0, 0);
                 LCDWriteStr("Demo: Pulse     ");
                 LCDGoto(0, 1);
                 if (isBtnPressed()==1){
                    sendPulse(1);
                 }
                 break;
                 
             case PULSE_RECORD:
                 //Todo: write pulse and record
                 LATAbits.LATA5 = 1;
                 LATAbits.LATA3 = 1;
                 LCDGoto(0, 0);
                 LCDWriteStr("Demo:PulseRecord");
                 LCDGoto(0, 1);
                 if (isBtnPressed() == 1){
                    sendPulse(3);
                    delayMillisecond(30);
                    LCDWriteLevels(analogRead(1));
                 }
                 break;
                 
             case PWM:
                 LCDGoto(0, 0);
                 LCDWriteStr("Demo: PWM       ");
                 LCDGoto(0, 1);  
                 myVolt = analogRead(0)/4;
                 sendPWM(myVolt);
                 LCDWriteLevels(myVolt);
                 break;
                 
             case DELAY_TEST:   //case not implemented
                 LCDGoto(0, 0);
                 LCDWriteStr("Demo: Timer     ");
                 LCDGoto(0, 1);
                 while(1){      //danger danger danger; no escape
                    LATAbits.LATA5 = 1;
                    delayMillisecond(1);
                    LATAbits.LATA5 = 0;
                    delayMillisecond(2);
                 }
                 break;
                 
             case PULSE_RECORD_PWM:
                 LCDGoto(0, 0);
                 LCDWriteStr("Demo: P_R_PWM   ");
                 LCDGoto(0, 1);
                 if (isBtnPressed() == 1){
                    sendPulse(3);
                    delayMillisecond(30);
                    myVolt = analogRead(1);
                    sendPWM(myVolt/4);
                    delayMillisecond(500);
                    LCDWriteLevels(analogRead(1));
                    stopPWM();    
                 }
                 //sendPWM(0);
                 break;
                 
             case CONT_RECORD_PWM:
                 LCDGoto(0, 0);
                 LCDWriteStr("Demo: Continuous");
                 LCDGoto(0, 1); 
                 LCDWriteStr("                ");
                 while (isBtnPressed() != 1){
                     sendPulse(3);
                     delayMillisecond(3);
                     myVolt = analogRead(1);
                     sendPWM(myVolt/4);
                     delayMillisecond(500);
                     LCDWriteLevels(analogRead(1));
                     stopPWM();  
                     delayMillisecond(100);
                     LCDGoto(0, 1);
                 }
                 break;
                 
             default : //error
                 break;
        }
    }
}

void LCDWriteLevels(int volt){
    char str[4];
    sprintf(str, "%04d", volt);
    LCDPutChar(str[0]);
    LCDPutChar(str[1]);
    LCDPutChar(str[2]);
    LCDPutChar(str[3]);
    LCDPutChar("|");

}
