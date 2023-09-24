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

volatile int16 speedL, speedR;
volatile int16 posL, posR;

enum Position_State detect_position();
enum Motion_State decide_motion(enum Position_State p_state);
void movement(enum Motion_State motion);
void M1_set_speed(bool direction, int num);
void M2_set_speed(bool direction, int num);
void Revise_Direction();
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
    SKEWLEFT,
    SKEWRIGHT,
    WAIT,
    OTHER
};
//* ========================================
volatile int8 flag_encoder = 0, flag_activate = 1;
volatile enum Position_State last_position_state = STRAIGHT, perform = STRAIGHT;
volatile int8 pre_vo1, pre_vo2, pre_vo3, pre_vo4, pre_vo5, pre_vo6;
volatile int16 encoder_counter1 = 0;
volatile int16 encoder_counter2 = 0;
//* ========================================
CY_ISR(isr_timer_EncoderCalculation){
    flag_encoder = 1;
    Timer_TS_ReadStatusRegister();
}

CY_ISR(isr_timer_MotionActivate){
    flag_activate = 0;
    Timer_Activate_ReadStatusRegister();
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
    // USBUART
    RF_BT_SELECT_Write(0);
// --------------------------------
    // PWM enable
    PWM_1_Start();
    PWM_2_Start();

    // Decoder Enable
    QuadDec_M1_Start();
    QuadDec_M2_Start();

    // D1 Setting, default HIGH, NO MOTOR MOVEMENT
    M1_D1_Write(1);
    M2_D1_Write(1);   


    // timer
    Timer_TS_Start();
    isr_TS_StartEx(isr_timer_EncoderCalculation);
    Timer_Activate_Start();
    isr_activate_StartEx(isr_timer_MotionActivate);


    movement(FORWARD); 
    
    for(;;)
    {   
        movement(decide_motion(detect_position()));
        if(detect_position()==STRAIGHT){
            Revise_Direction();        
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
    
    if(last_position_state == WAIT && Vo3_Read())
    {
        current_state = perform;
    }
    
    // Forward
    if (!Vo2_Read() & !Vo3_Read() & !Vo4_Read() & Vo5_Read() & Vo6_Read() ){
        current_state = STRAIGHT;
    }
    
    // Turn Left
    if (!Vo2_Read() & !Vo3_Read() & !Vo5_Read()){
        perform = LEFTCROSS;
        current_state = WAIT;
    }
    
    // Turn Right
    if (!Vo2_Read() & !Vo3_Read() & !Vo6_Read()){
        perform = RIGHTCROSS;
        current_state = WAIT;
    }    
    
    
    if (current_state != OTHER){
        last_position_state = current_state;
    }   
    
    return last_position_state;
    
}
//* ========================================
enum Motion_State decide_motion(enum Position_State p_state) {
    if (p_state == RIGHTCROSS) {return RIGHT;}
    if (p_state == LEFTCROSS) {return LEFT;}
    if (p_state == STRAIGHT) {return FORWARD;}
    if (p_state == OTHER) {return STOP;}
    return STOP;
}
//* ========================================
void movement(enum Motion_State motion) {
    if (motion == FORWARD) {     
        M1_set_speed(1, 20);
        M2_set_speed(1, 20);
    }
    
    if (motion == BACK) {
        M1_set_speed(0, 35);
        M2_set_speed(0, 30);
    }
    
    if (motion == RIGHT) { 
        
        M1_set_speed(1, 35);
        M2_set_speed(0, 30);         
    }
    
    if (motion == LEFT) {        
        M1_set_speed(0, 35);
        M2_set_speed(1, 30); 
    }
    
    if (motion == STOP) {
       M1_D1_Write(1);
       M2_D1_Write(1);
       flag_activate = 1;
    }    
}
//* ========================================
void M1_set_speed(bool direction, int num) {
    // FORWARD = 1; BACKWARD = 0
    // 0 is at full speed, 100 is stopping
    double ratio = 0;
    // 50% of the PWM period would stop the wheel     
    if (direction == TRUE){
        //LED_Write(1);
        num = 100 - num;
        ratio = 0.5 * num;   
        PWM_1_WriteCompare(PWM_1_ReadPeriod() * ratio / 100);
        PWM_1_SaveConfig();
    }else{
        ratio = 50 + 0.5 * num;
        PWM_1_WriteCompare(PWM_1_ReadPeriod() * ratio / 100); 
        PWM_1_SaveConfig();
    }
    // Enable motor
    M1_D1_Write(0);
    M2_D1_Write(0);
};
//* ========================================
void M2_set_speed(bool direction, int num) {
    // FORWARD = 1; BACKWARD = 0
    // 0 is at full speed, 100 is stopping
    double ratio = 0;
    // 50% of the PWM period would stop the wheel  
    if (direction == TRUE){
        ratio = 50 + 0.5 * num;
        PWM_2_WriteCompare(PWM_2_ReadPeriod() * ratio  / 100); 
        PWM_2_SaveConfig();
    }else{
        num = 100 - num;
        ratio = 0.5 * num;   
        PWM_2_WriteCompare(PWM_2_ReadPeriod() * ratio  / 100);
        PWM_2_SaveConfig();
    }
    // Enable motor
    M1_D1_Write(0);
    M2_D1_Write(0);
};
//* ========================================

void Revise_Direction(){
    int ratio_1 = PWM_1_ReadCompare() * 100  / PWM_1_ReadPeriod();
    int ratio_2 = PWM_2_ReadCompare() * 100 / PWM_2_ReadPeriod();
    if(!Vo1_Read() && Vo4_Read()){
        LED_Write(0);
        M2_set_speed(TRUE, ratio_1 + 5);
    }else if(Vo1_Read() && Vo4_Read()){
        //M1_backward();
        LED_Write(1);
        M1_set_speed(TRUE, ratio_2 + 5);
    }else{
        movement(FORWARD);
    }
}
/* [] END OF FILE */
