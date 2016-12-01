/*
 * Integration
 */

#include <p18f46k22.h>
#include <delays.h>
#include "Lcd.h"
#include <stdio.h>
//#include "System.h"
#include "Interrupts.h"
#include "Serial.h"
#include "General.h"


#pragma config FOSC = INTIO67   // Internal OSC block, Port Function on RA6/7
#pragma config WDTEN = OFF      // Watch Dog Timer disabled. SWDTEN no effect
#pragma config XINST = OFF      // Instruction set Extension and indexed Addressing mode disabled

#define LOWBYTE(v)   ((unsigned char) (v))
#define HIGHBYTE(v)  ((unsigned char) (((unsigned int) (v)) >> 8))
#define One_ms_h 0xF2;
#define One_ms_l 0xE3;
#define STANDARD 0
#define SLEEP_MODE 3
#define HEAD_ONLY 1
#define RANGE_ONLY 2
#define NUM_OF_STATES = 4

unsigned int state = 0;
unsigned int timeToFire = 0;
unsigned int dataReadyFlag1 = 0;
unsigned int dataReadyFlag2 = 0;
unsigned int sendHeadFlag = 0;
unsigned int headSensorVal = 0;
unsigned int casePWM = 1;
unsigned int sleep_mode = SLEEP_MODE;
unsigned int didStateChange = 2;
unsigned int rangeSensorVal = 0;

void main(void)
{
    char input;
    char str[4];    char str2[4];

    ANSELBbits.ANSB5 = 0;
    TRISBbits.RB5 = 0;
    ANSELBbits.ANSB4 = 0;
    TRISBbits.RB4 = 0;
    SysInit();
    OSCCON = 0b01010110;
    INTCONbits.GIE=0;           // Disable interrupts
    delayMillisecond(1000);
    //ISRInit();
    //headMotorInit();
    //btnISRInit();
    LATDbits.LATD5 = 0;
    
    
    SERTxSave('\r');		 // Carriage return
    SERTxSave('\n');		 // Line Feed

    SERTxSave('\r');
    SERTxSave('\n');
    SERSendStr("Voltage");
  /*
    SERInit();			 // Initialized Serieal Come
    SERTxSave('\r');		 // Carriage return
    SERTxSave('\n');		 // Line Feed

    SERTxSave('\r');
    SERTxSave('\n');
    SERSendStr("Voltage:");
    LCDGoto(0,0);
    LCDWriteStr("Voltage:");
    */
    while (1) {
        INTCONbits.GIE=1;           // Enable interrupts
        switch (state%NUM_OF_STATES) {
             case STANDARD:
                 LATAbits.LATA3 = 1; 
                 LATAbits.LATA5 = 1;
                 casePWM = 1;

                 if (dataReadyFlag1){
                     rangeSensorVal = analogRead(1);
                     if (rangeSensorVal<66){
                         rangeSensorVal = 67;
                     }
                    timeToFire = (rangeSensorVal-66+50)*3;//*6 66 is end of cane
                    dataReadyFlag1 = 0;
                    
                               /* sprintf(str, "%04d", headSensorVal);
                    SERTxSave('L');
            SERTxSave(':');
            SERTxSave(' ');
            SERTxSave(str[0]);
            SERTxSave(str[1]);
            SERTxSave(str[2]);
            SERTxSave(str[3]);
            SERTxSave('\r');
            SERTxSave('\n');*/
                 }
                 if (dataReadyFlag2){
                     headSensorVal = analogRead(2);
                     if (headSensorVal < 90){
                         sendHeadFlag  = 1;
                     }else{
                         sendHeadFlag = 0;
                         LATDbits.LATD5 = 0;
                     }
                     dataReadyFlag2 = 0;
                                 /*sprintf(str2, "%1d", state);

                                 SERTxSave('H');
            SERTxSave(':');
            SERTxSave(str2[0]);
            SERTxSave(str2[1]);
            SERTxSave(str2[2]);
            SERTxSave(str2[3]);
            SERTxSave('\r');
            SERTxSave('\n');*/
                 }
                 //if (analogRead(2) < 60){
                 //    sendHeadWarning(0);
                 //}else{
                 //    LATDbits.LATD5 = 0;
                // }
                

                 break;
                 
            case HEAD_ONLY:
                /*if (didStateChange==1){
                    INTCONbits.GIE=0;           // Disable interrupts
                    didStateChange = 0;
                    sendPWM(150);
                    delayMillisecond(100);
                    stopPWM();
                    sendPWM(150);
                    delayMillisecond(100);
                    stopPWM();
                    INTCONbits.GIE=1;           // Enable interrupts
                }*/
                //LATAbits.LATA3 = 1;
                //LATAbits.LATA5 = 0;
                casePWM = 0;
                stopPWM();
                 /*if (dataReadyFlag1){
                    //timeToFire = (analogRead(1)+50)*6;//*6
                    dataReadyFlag1 = 0;
                 }*/
                 if (dataReadyFlag2){
                     headSensorVal = analogRead(2);
                     if (headSensorVal < 90){
                         sendHeadFlag  = 1;
                     }else{
                         sendHeadFlag = 0;
                         LATDbits.LATD5 = 0;
                     }
                     dataReadyFlag2 = 0;
                 }
                
                break;
            case RANGE_ONLY:
                /*if (didStateChange==1){
                    INTCONbits.GIE=0;           // Disable interrupts
                    didStateChange = 0;
                    //sendPWM(150);
                    //delayMillisecond(100);
                    //stopPWM();
                    //sendPWM(150);
                    //delayMillisecond(100);
                    stopPWM();
                    INTCONbits.GIE=1;           // Enable interrupts
                }*/
                casePWM = 1;
                LATAbits.LATA3 = 0;
                LATAbits.LATA5 = 1;
                sendHeadFlag = 0;
                LATDbits.LATD5 = 0;
                 if (dataReadyFlag1){
                    rangeSensorVal = analogRead(1);
                     if (rangeSensorVal<66){
                         rangeSensorVal = 67;
                     }
                    timeToFire = (rangeSensorVal-66+50)*3;//*6 66 is end of cane
                    dataReadyFlag1 = 0;
                 }
                break;
             case SLEEP_MODE:
                 casePWM = 0;
                 LATAbits.LATA3 = 0;
                 LATAbits.LATA5 = 0;
                 sendHeadFlag = 0;
                 LATDbits.LATD5 = 0;
                 stopPWM();
                 //LCDGoto(0, 0);
                 //LCDWriteStr("Sleep");
                 //LCDGoto(0,1);
                 //LCDPutByte(state);
                 //enableSleep();
                 break;
             default : //error
                 break;
}
        
    }
    
}
