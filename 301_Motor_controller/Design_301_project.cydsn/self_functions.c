/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

//* ======== INCLUDE =======================
//* ========================================
#include "defines.h"
#include "vars.h"
#include "self_functions.h"

//* ======== DECLARATION ===================
//* ========================================
void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb();


//* ======== FUNCTION ======================
//* ========================================
void usbPutString(char *s) {
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
void usbPutChar(char c) {
#ifdef USE_USB     
    while (USBUART_CDCIsReady() == 0);
    USBUART_PutChar(c);
#endif    
}
//* ========================================
void handle_usb() {
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


//* ======== SELF FUNCTION==================
//* ========================================
char* enumToString(enum Motion_State state) {
    if (state == FORWARD) {
        return "FORWARD";
    } else if (state == LEFT) {
        return "LEFT";
    } else if (state == RIGHT) {
        return "RIGHT";
    } else if (state == BACKWARD) {
        return "BACKWARD";
    } else if (state == WAIT_STOP) {
        return "WAIT_STOP";
    } else if (state == END) {
        return "END";
    } else {
        return "OTHER";
    }
}
void init() {
// ------Motor SETUP----------------------
    PWM_1_Start();
    PWM_2_Start();
    M1_D1_Write(1);
    M1_D1_Write(1);

// ------DECODER SETUP----------------------
    QuadDec_M1_Start();
    QuadDec_M2_Start();
    QuadDec_M1_SetCounter(0);
    QuadDec_M2_SetCounter(0);

}
//* ======== MOTOR CONTROL =================
void M1_set_speed(int direction, double num){
    if (num > 100) 
    {
        num = 100.0;
    } else if (num < 0) 
    {
        num = 0.0;
    }
    if (direction) {
        num = 0.5 * num + 50.0;
    } else {
        num = 50.0 - 0.5 * num; 
    }
    PWM_1_WriteCompare(((double)PWM_1_ReadPeriod()* (100 - num)) / 100);
    PWM_1_SaveConfig();
    
}
void M2_set_speed(int direction, double num){
    if (num > 100) 
    {
        num = 100.0;
    } else if (num < 0) 
    {
        num = 0.0;
    }
    if (direction) {
        num = 0.5 * num + 50.0;
    } else {
        num = 50.0 - 0.5 * num; 
    }
    PWM_2_WriteCompare(((double)PWM_2_ReadPeriod()* num) / 100);
    PWM_2_SaveConfig();
    
}

/* [] END OF FILE */
