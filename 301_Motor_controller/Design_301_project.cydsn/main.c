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

enum Position_State detect_position();
enum Motion_State decide_motion(enum Position_State p_state);
void movement(enum Motion_State motion);
void M1_set_speed(double num);
void M2_set_speed(double num);
void M1_forward();
void M1_backward();
void M2_forward();
void M2_backward();
//* ========================================
enum Motion_State {
    FORWARD,
    LEFT, 
    RIGHT, 
    BACK,
    STOP
};
enum Position_State {
    STRAIGHT, 
    RIGHTCROSS, // Right
    LEFTCROSS,  // Left
    TENCROSS,   // X
    THIRDCROSS,  // T
    OTHER
};
//* ========================================
volatile int8 flag_encoder = 0;
volatile enum Position_State last_position_state = STRAIGHT;
volatile int8 pre_vo1, pre_vo2, pre_vo3, pre_vo4, pre_vo5, pre_vo6;
//* ========================================
CY_ISR(isr_timer_EncoderCalculation){
    flag_encoder = 1;
    LED_Write(0);
}

int main()
{   
// --------------------------------    
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
// ------USB SETUP ----------------    
#ifdef USE_USB    
    USBUART_Start(0,USBUART_5V_OPERATION);
#endif
// --------------------------------
    // PWM enable
    PWM_1_Start();
    PWM_2_Start();
    /*
    PWM_1_WritePeriod(800);
    PWM_1_WriteCompare(400);
    PWM_1_SaveConfig();
    
    PWM_2_WritePeriod(800);
    PWM_2_WriteCompare(400);
    PWM_2_SaveConfig();
    */
    M1_set_speed(0);
    M2_set_speed(0);
    
    M1_forward();
    M2_forward();
    
    // timer
    Timer_TS_Start();
    isr_TS_StartEx(isr_timer_EncoderCalculation);
    


    // USBUART
    RF_BT_SELECT_Write(0);
    
    for(;;)
    {   
        movement(decide_motion(detect_position()));
        if (Vo6_Read()) {
            LED_Write(1);
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
enum Position_State detect_position() {
    enum Position_State current_state = OTHER;

    if (Vo5_Read()) {
        current_state = LEFTCROSS;
    } 
    // STRAIGHT
    else if (!Vo1_Read() & !Vo2_Read() & !Vo3_Read() & Vo5_Read()) {
        current_state = STRAIGHT;
        //return STRAIGHT;// 1 2 3 4 - dark, normal straight
    }
    else if (!Vo1_Read() & !Vo2_Read() & !Vo3_Read() ) {
        current_state = STRAIGHT;
        //return STRAIGHT; // 1 2 3 - dark, turning done
    }

    if ((last_position_state == current_state) && (last_position_state != STRAIGHT && (current_state != OTHER))) {
        movement(STOP);
        CyDelay(400);
        movement(decide_motion(current_state));
        CyDelay(200);
    }
    if (current_state == OTHER) {
        movement(STOP);
        CyDelay(500);
        movement(FORWARD);
        CyDelay(1000);
    }
    last_position_state = current_state;
    return last_position_state;
}
//* ========================================
enum Motion_State decide_motion(enum Position_State p_state) {
    if (p_state == RIGHTCROSS) {return RIGHT;}
    if (p_state == LEFTCROSS) {return LEFT;}
    if (p_state == STRAIGHT) {return FORWARD;}
    if (p_state == OTHER) {return FORWARD;}
    return STOP;
}
//* ========================================
void movement(enum Motion_State motion) {
    if (motion == FORWARD) {
        M1_set_speed(65);
        M2_set_speed(60);
        // Direction  
        M1_forward();
        M2_forward();
    }
    
    if (motion == BACK) {
        M1_set_speed(50);
        M2_set_speed(55);
        // Direction
        M1_backward();
        M2_backward();
    }
    
    if (motion == RIGHT) {
        M1_set_speed(50);
        M2_set_speed(50);        
        // Direction
        M1_forward();
        M2_backward();
        
        
    }
    
    if (motion == LEFT) {
        M1_set_speed(70);
        M2_set_speed(70);
        // Direction      
        M1_backward();
        M2_forward();    
        
    }
    
    if (motion == STOP) {
        M1_set_speed(0);
        M2_set_speed(0);
    }    
}
//* ========================================
void M1_set_speed(double num) {
    if (num > 100) {
        num = 100;
    }
    if (num < 0) {
        num = 0;
    }
    PWM_1_WriteCompare((double)(PWM_1_ReadPeriod()* num) / 100);
    PWM_1_SaveConfig();
    //usbPutString("M1 speed changed\n\r");    
};
//* ========================================
void M2_set_speed(double num) {
    if (num > 100) {
        num = 100;
    }
    if (num < 0) {
        num = 0;
    }
    PWM_2_WriteCompare((double)(PWM_2_ReadPeriod()* num) / 100);
    PWM_2_SaveConfig();
    //usbPutString("M2 speed changed\n\r");   
};
//* ========================================
void M1_forward() {
    // set motor 1 direction - IN1 - high
    M1_IN1_Write(1);
    M1_IN2_Write(0);
};

void M1_backward() {
    // set motor 1 direction - IN1 - low
    M1_IN1_Write(0);
    M1_IN2_Write(1);
}

void M2_forward() {
    // set motor 2 direction - IN1 - high
    M2_IN1_Write(0);
    M2_IN2_Write(1);
}

void M2_backward() {
    // set motor 2 direction - IN1 - low
    M2_IN1_Write(1);
    M2_IN2_Write(0);
};
/* [] END OF FILE */
