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
void M1_set_speed(int8 num);
void M2_set_speed(int8 num);
void motor_forward();
void motor_backward();
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
    THIRDCROSS  // T
};
//* ========================================
volatile int8 flag_encoder = 0;
volatile enum Position_State last_position_state = STRAIGHT;
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
    PWM_1_Sleep();
    PWM_2_Sleep();
    
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
    QuadDec_M2_SetCounter(0);
    
    RF_BT_SELECT_Write(0);
    usbPutString(displaystring);
    
    for(;;)
    {
        /* encoder calculation - output the Revolution of output shaft*/
        if(flag_encoder) {
            encoder_counter = (double) QuadDec_M1_GetCounter() / 228; // timer - 1s
            sprintf(line, "%.2lf", encoder_counter); 
            
            usbPutString(message_encoder);
            usbPutString(line);
            usbPutString("\r");
            
            QuadDec_M1_SetCounter(0);
            flag_encoder = 0;
        }
        
        // movement
        movement(decide_motion(detect_position()));
        
        /* Place your application code here. */
        handle_usb();
        
        /* Input a integer to change PWM */
        if (flag_KB_string == 1)
        {
            usbPutString(line);

            if (atoi(line)){
                PWM_1_WriteCompare((double)(PWM_1_ReadPeriod()* (100 - atoi(line))) / 100);
                PWM_1_SaveConfig();
                usbPutString("\tCustom input used\n\r");
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
enum Position_State detect_position() {
    if (Vo1_Read() & !Vo2_Read() & !Vo3_Read() & !Vo4_Read() & Vo5_Read() & !Vo6_Read()) {
        return RIGHTCROSS;
    }
    if (Vo1_Read() & !Vo2_Read() & !Vo3_Read() & !Vo4_Read() & !Vo5_Read() & Vo6_Read()) {
        return LEFTCROSS;
    }
    // STRAIGHT
    if (!Vo1_Read() & !Vo2_Read() & !Vo3_Read() & !Vo4_Read() & Vo5_Read() & Vo6_Read()) {
        return STRAIGHT;// 1 2 3 4 - dark, normal straight
    }
    if (!Vo1_Read() & !Vo2_Read() & !Vo3_Read() & Vo4_Read() & Vo5_Read() & Vo6_Read()) {
        return STRAIGHT; // 1 2 3 - dark, turning done
    }
    
    return last_position_state;
}
//* ========================================
enum Motion_State decide_motion(enum Position_State p_state) {
    if (p_state == RIGHTCROSS) {return RIGHT;}
    if (p_state == LEFTCROSS) {return LEFT;}
    if (p_state == STRAIGHT) {return FORWARD;}
    return STOP;
}
//* ========================================
void movement(enum Motion_State motion) {
    if (motion == FORWARD) {
        PWM_1_Enable();
        PWM_2_Enable();
        
        M1_set_speed(50);
        M2_set_speed(50);
        // Direction        
        motor_forward();
    }
    
    if (motion == BACK) {
        PWM_1_Enable();
        PWM_2_Enable();
        
        M1_set_speed(50);
        M2_set_speed(50);
        // Direction
        motor_backward();
    }
    
    if (motion == RIGHT) {
        PWM_1_Enable();
        PWM_2_Enable();
        // Differential Speed
        M1_set_speed(80);
        M2_set_speed(10);
        // Direction
        motor_forward();
        
        
    }
    
    if (motion == LEFT) {
        PWM_1_Enable();
        PWM_2_Enable();
        // Differential Speed
        M1_set_speed(10);
        M2_set_speed(80);
        // Direction
        motor_forward();
    }
    
    if (motion == STOP) {
        PWM_1_Sleep();
        PWM_2_Sleep();
    }    
}
//* ========================================
void M1_set_speed(int8 num) {
    if (num > 100) {
        num = 100;
    }
    if (num < 0) {
        num = 0;
    }
    PWM_1_WriteCompare((double)(PWM_1_ReadPeriod()* (100 - num)) / 100);
    PWM_1_SaveConfig();
    usbPutString("M1 speed changed\n\r");    
};
//* ========================================
void M2_set_speed(int8 num) {
    if (num > 100) {
        num = 100;
    }
    if (num < 0) {
        num = 0;
    }
    PWM_2_WriteCompare((double)(PWM_2_ReadPeriod()* (100 - num)) / 100);
    PWM_2_SaveConfig();
    usbPutString("M2 speed changed\n\r");   
};
//* ========================================
void motor_forward() {
    // set motor 1 direction - IN1 - high
    M1_IN1_Write(1);
    M1_IN2_Write(0);
    // set motor 2 direction - IN1 - high
    M2_IN1_Write(1);
    M2_IN2_Write(0);
};
void motor_backward() {
    // set motor 1 direction - IN1 - low
    M1_IN1_Write(0);
    M1_IN2_Write(1);
    // set motor 2 direction - IN1 - low
    M2_IN1_Write(0);
    M2_IN2_Write(1);
};
/* [] END OF FILE */
