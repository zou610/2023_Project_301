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
#define ENCODER_M1 5
#define ENCODER_M2 15
//* ========================================
void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb();
//* ========================================
enum Motion_State {
    FORWARD,
    LEFT, 
    RIGHT, 
    BACKWARD,
    WAIT_STOP,
    END,
    OTHER
};
//* ========================================
void M1_set_speed();
void M2_set_speed();
void movement(enum Motion_State);
void decide_motion();
void speed_balance();
char* enumToString(enum Motion_State);
void bias_check();
//* ========================================
volatile enum Motion_State current_Motion = OTHER, next_Motion = OTHER;
volatile int Stop_flag = 1, decoder_flag = 0, speed_flag = 0;
volatile int count_waitstop = 0, count_encoder = 0, count_forward = 0;
volatile int c_M1, c_M2, speed_M1 = 65, speed_M2 = 65;
//* ========================================
CY_ISR(isr_UpdateState) {
    current_Motion = next_Motion;
    count_waitstop++;
    count_encoder++;
    // wait stop signal timer
    if (count_waitstop == 200) {
        Stop_flag = 1;
        count_waitstop = 0;
    }
    // encoder timer
    if (count_encoder == 5) {
        decoder_flag = 1;
        count_encoder = 0;
        if (count_forward >= 5) {
            speed_flag = 1;
            count_forward = 0;
        }
    }
    // forward timer
    if (current_Motion == FORWARD) {
        count_forward++;
    } else {
        count_forward = 0;
    }
    if (current_Motion == FORWARD) {
        bias_check();
    }
}
CY_ISR(isr_StopInterval) {
    current_Motion = next_Motion;
    Timer_2_Sleep();
    Timer_2_Init();
    count_waitstop = 0;
}

int main()
{   
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
// ------USB SETUP ----------------    
    #ifdef USE_USB    
        USBUART_Start(0,USBUART_5V_OPERATION);
    #endif
    // USBUART
    RF_BT_SELECT_Write(0);
// ------Motor SETUP----------------------
    PWM_1_Start();
    PWM_2_Start();
    M1_D1_Write(1);
    M1_D1_Write(1);
// ------TIMER SETUP----------------------
    Timer_1_Start();
    isr_US_StartEx(isr_UpdateState);
    
    Timer_2_Start();
    Timer_2_Sleep();
    Timer_2_Init();
    isr_T2_StartEx(isr_StopInterval);
// ------DECODER SETUP----------------------
    QuadDec_M1_Start();
    QuadDec_M2_Start();
    QuadDec_M1_SetCounter(0);
    QuadDec_M2_SetCounter(0);
    
    for(;;) 
    {
        decide_motion();
        movement(current_Motion);
        
        
        if (decoder_flag) {
            c_M1 = QuadDec_M1_GetCounter();
            c_M2 = QuadDec_M2_GetCounter();
            
            if (speed_flag) {
                speed_balance();
                speed_flag = 0;
            }
            
            QuadDec_M1_SetCounter(0);
            QuadDec_M2_SetCounter(0);
            
            //usbPutString("M1:");
            //usbPutString(itoa(c_M1, line, 10));
            //usbPutString(" and M2:");
            //usbPutString(itoa(c_M2, line, 10));
            //usbPutString("\t\t C_Motion - ");
            //usbPutString(enumToString(current_Motion));
            //usbPutString("\n\r");
            
            
            decoder_flag = 0;
        }
    }
}
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
//* ========================================
// Motor speed control
void M1_set_speed(double num){
    if (num > 100) 
    {
        num = 100;
    } else if (num < 0) 
    {
        num = 0;
    }
    PWM_1_WriteCompare(((double)PWM_1_ReadPeriod()* (100 - num)) / 100);
    PWM_1_SaveConfig();
    
}
void M2_set_speed(double num){
    if (num > 100) 
    {
        num = 100;
    } else if (num < 0) 
    {
        num = 0;
    }
    PWM_2_WriteCompare(((double)PWM_2_ReadPeriod()* num) / 100);
    PWM_2_SaveConfig();
    
}
// Motion execution
void movement(enum Motion_State motion) {
    /*
    if (motion == WAIT_STOP) {
        Timer_1_Sleep();
        Timer_1_Init();
        
        // Stop
        M1_D1_Write(1);
        M2_D1_Write(1);
        
        // enable timer2
        Timer_2_Wakeup();
        return;
    }
    */
    Timer_1_Enable();
    
    if (motion == FORWARD) {
        M1_set_speed(speed_M1);
        M2_set_speed(speed_M2);
        
        // start motor
        M1_D1_Write(0);
        M2_D1_Write(0);
    } else if (motion == LEFT) {
        M1_set_speed(65);
        M2_set_speed(35);
        // start motor
        M1_D1_Write(0);
        M2_D1_Write(0);
    } else if (motion == RIGHT) {
        M1_set_speed(30);
        M2_set_speed(65);
        // start motor
        M1_D1_Write(0);
        M2_D1_Write(0);
    } else if (motion == OTHER) {
        M1_set_speed(50);
        M2_set_speed(50);
        // stop motor
        M1_D1_Write(1);
        M2_D1_Write(1);
    }
}
// decide motion
void decide_motion() {
    /*
    // start detect the current position and decide the motion which need to take
    if ((!Vo5_Read() || !Vo6_Read()) && Stop_flag) {
        current_Motion = WAIT_STOP;
        // not allow to stop again
        Stop_flag = 0;
    }
    */
    int state_assign = 1;
    
    if (!Vo5_Read() && !Vo2_Read() && Vo4_Read() && Vo6_Read()) {
        next_Motion = LEFT;
        state_assign = 0;
    }
    if (!Vo6_Read() && !Vo2_Read() && Vo4_Read() && Vo5_Read()) {
        next_Motion = RIGHT;
        state_assign = 0;
    }
    if ((!Vo1_Read() || !Vo3_Read()) && !Vo2_Read() && Vo5_Read() && Vo6_Read()) {
        next_Motion = FORWARD;
        state_assign = 0;
    }
    if (!(Vo4_Read()) && !Vo2_Read() && Vo5_Read() && Vo6_Read()) {
        next_Motion = FORWARD;
        state_assign = 0;
    }
    if (state_assign == 1) {
        next_Motion = OTHER;
    }
}
/*
void speed_balance() {
    if (abs(c_M2) > abs(c_M1)) {
        // M2 speed > M1 speed, M2 decelerate
        speed_M2 = speed_M2 - 3;
    } else
    if (abs(c_M2) < abs(c_M1) - ENCODER_M2_DE) {
        // M2 speed < M1 speed, M2 accelerate
        speed_M2++;
    }
}
*/
void speed_balance() {
    if (c_M2 - abs(c_M1) > 0) {
        // M2 speed > M1 speed, M2 decelerate
        speed_M1 = speed_M1 + 4;
    } else
    if (c_M2 - abs(c_M1) < 0) {
        // M2 speed < M1 speed, M2 accelerate
        speed_M1--;
    }
}

void bias_check() {
    if (Vo1_Read() && !Vo3_Read() && Vo5_Read() && Vo6_Read()) {
        speed_M1 = speed_M1 + 0.01;
        //usbPutString(" Bias-set ");
        return;
    }
    
    if (!Vo1_Read() && Vo3_Read() && Vo5_Read() && Vo6_Read() && (speed_M1 > 51 && speed_M2 >51)) {
        speed_M1 = speed_M1 - 0.01;
        //usbPutString(" Bias-set ");
        return;
    }  
    //usbPutString(" \tBias-not-set\t ");
}

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
/* [] END OF FILE */
