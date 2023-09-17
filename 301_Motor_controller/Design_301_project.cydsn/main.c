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
volatile int8 flag_encoder = 0;
enum Sport_State {
  FORWARD, LEFT, RIGHT, BACK
};
enum Position_State {
  STRAIGHT, CROSS
};
//* ========================================
CY_ISR(isr_timer_EncoderCalculation){
    flag_encoder = 1;
}

int main()
{
// --------------------------------    
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
    PWM_1_Start();
    PWM_2_Start();
    QuadDec_M1_Start();
    QuadDec_M2_Start();
    Timer_TS_Start();
    isr_TS_StartEx(isr_timer_EncoderCalculation);
    
    double encoder_counter = 0.0;
    
// ------USB SETUP ----------------    
#ifdef USE_USB    
    USBUART_Start(0,USBUART_5V_OPERATION);
#endif        
    QuadDec_M1_SetCounter(0);
    
    RF_BT_SELECT_Write(0);
    usbPutString(displaystring);
    
    for(;;)
    {
        /* encoder calculation*/
        if(flag_encoder) {
            encoder_counter = (double) QuadDec_M1_GetCounter() / 57; // timer - 100ms
            sprintf(line, "%.2lf", encoder_counter); 
            
            usbPutString(message_encoder);
            usbPutString(line);
            usbPutString("\r");
            
            QuadDec_M1_SetCounter(0);
            flag_encoder = 0;
        }
        
        /* Place your application code here. */
        handle_usb();
        if (flag_KB_string == 1)
        {
            usbPutString(line);

            if (atoi(line)){
                PWM_1_WriteCompare((double)(PWM_1_ReadPeriod()* (100 - atoi(line))) / 100);
                PWM_1_SaveConfig();
                usbPutString("\t Custom input used\n\r");
            }            
            
            flag_KB_string = 0;
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


/* [] END OF FILE */
