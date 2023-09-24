/* ========================================
 * Fully working code: 
 * PWM      : 
 * Encoder  : 
 * ADC      :
 * USB      : port displays speed and position.
 * CMD: "PW xx"
 * Copyright Univ of Auckland, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Univ of Auckland.
 *
 * ========================================
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <project.h>
//* ========================================
#include "defines.h"
#include "vars.h"
//* ========================================
void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb();
//* ========================================
void M1_set_speed();
void M2_set_speed();
void straightForward();
void turnLeft();
void turnRight();
//* ========================================
volatile enum Motion_State current_Motion, next_Motion;


//* ========================================
enum Motion_State {
    FORWARD,
    LEFT, 
    RIGHT, 
    BACKWARD,
    STOP
};

int main()
{   
// --------------------------------    
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
// ------USB SETUP ----------------    
    #ifdef USE_USB    
        USBUART_Start(0,USBUART_5V_OPERATION);
    #endif
    // USBUART
    RF_BT_SELECT_Write(0);
// ------Motor----------------------
    PWM_1_Start();
    PWM_2_Start();
    M1_D1_Write(1);
    M1_D1_Write(1);
    
    for(;;)
    {   
        // Forward
        if (!Vo2_Read() & !Vo3_Read() & !Vo4_Read() & Vo5_Read() & Vo6_Read() ){
            next_Motion = FORWARD;
        }        
        // Turn Left
        else if (!Vo2_Read() & !Vo3_Read() & !Vo5_Read() & Vo4_Read() & Vo6_Read()){
            next_Motion = LEFT;
        }        
        // Turn Right
        else if (!Vo2_Read() & !Vo3_Read() & !Vo6_Read() & Vo4_Read() & Vo5_Read()){
            next_Motion = RIGHT;
        }
        current_Motion = next_Motion;
        
        if(current_Motion == FORWARD){
            straightForward();
            if((!Vo1_Read()) & Vo4_Read()){
                M1_set_speed(PWM_1_ReadCompare()-1);            
            }else if(Vo1_Read() & Vo4_Read() & Vo5_Read() & Vo6_Read()){
                M2_set_speed(PWM_1_ReadCompare()+1); 
            }
        }else if (current_Motion == LEFT){
            turnLeft();
        }else if (current_Motion == RIGHT){
            turnRight();
        }

    }
      
}
//* ========================================
void usbPutString(char *s)
{
// !! Assumes that *s is a string with allocated space >=64 chars     
//  Since USB implementation retricts data packets to 64 chars, this function truncates the
//  length to 62 char (63rd char is a '!')

#ifdef USE_USB     
    while (USBUART_CDCIsReady() == 0);
    s[63]='\0';
    s[62]='!';
    USBUART_PutData((uint8*)s,strlen(s));
#endif
}
//* ========================================
void usbPutChar(char c)
{
#ifdef USE_USB     
    while (USBUART_CDCIsReady() == 0);
    USBUART_PutChar(c);
#endif    
}
//* ========================================
void handle_usb()
{
    // handles input at terminal, echos it back to the terminal
    // turn echo OFF, key emulation: only CR
    // entered string is made available in 'line' and 'flag_KB_string' is set
    
    static uint8 usbStarted = FALSE;
    static uint16 usbBufCount = 0;
    uint8 c; 
    

    if (!usbStarted)
    {
        if (USBUART_GetConfiguration())
        {
            USBUART_CDC_Init();
            usbStarted = TRUE;
        }
    }
    else
    {
        if (USBUART_DataIsReady() != 0)
        {  
            c = USBUART_GetChar();

            if ((c == 13) || (c == 10))
            {
                if (usbBufCount > 0)
                {
                    entry[usbBufCount]= '\0';
                    strcpy(line,entry);
                    usbBufCount = 0;
                    flag_KB_string = 1;
                }
            }
            else 
            {
                if (((c == CHAR_BACKSP) || (c == CHAR_DEL) ) && (usbBufCount > 0) )
                    usbBufCount--;
                else
                {
                    if (usbBufCount > (BUF_SIZE-2) ) // one less else strtok triggers a crash
                    {
                       USBUART_PutChar('!');        
                    }
                    else
                        entry[usbBufCount++] = c;  
                }  
            }
        }
    }    
}
//* ========================================
// Motor speed control
void M1_set_speed(int num){
    if (num > 100) {
        num = 100;
    } else if (num < 0) {
        num = 0;
    }
    PWM_1_WriteCompare((double)(PWM_1_ReadPeriod()* num) / 100);
    PWM_1_SaveConfig();
    
}
void M2_set_speed(int num){
    if (num > 100) {
        num = 100;
    } else if (num < 0) {
        num = 0;
    }
    PWM_2_WriteCompare((double)(PWM_2_ReadPeriod()* (100 - num)) / 100);
    PWM_2_SaveConfig();
    
}
// Motion execution
void movement(enum Motion_State motion) {
    if (motion == FORWARD) {
        M1_set_speed(60);
        M2_set_speed(60);
    } else if (motion == LEFT) {
        M1_set_speed(80);
        M2_set_speed(30);
    } else if (motion == RIGHT) {
        M1_set_speed(30);
        M2_set_speed(80);
    }
}
// decide motion


//* ========================================


/* [] END OF FILE */
