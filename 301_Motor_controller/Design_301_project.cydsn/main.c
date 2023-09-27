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
#include "self_functions.h"

//* ======== Variable ==================
//* ========================================
volatile enum Motion_State 
current_Motion = FORWARD, 
next_Motion = FORWARD;

volatile int 
flag_encoder = 0,
flag_bias = 0
;

volatile int 
count_encoder = 0,
count_check_stop = 0
;

volatile int 
c_M1, 
c_M2,
c_sum = -2100;
;

volatile int 
v1, 
v2,
v3,
v4,
v5,
v6
;

volatile double 
speed_M1 = 30,
speed_M2 = 30,
bias_M1 = 0, 
bias_M2 = 0,
p = 0,
target = 100
;

//* ======== DECLARATION ===================
//* ========================================
void movement(enum Motion_State);
void decide_motion();
void speed_balance();
void bias_check();

//* ======== ISR ===========================
//* ========================================
CY_ISR(isr_UpdateState) {
    current_Motion = next_Motion;
    count_encoder++;
    // encoder timer
    if (count_encoder == 10000) {
        flag_encoder = 1;
        count_encoder = 0;
    }
}
CY_ISR(assignment) {
    v1 = Vo1_Read();
    v2 = Vo2_Read();
    v3 = Vo3_Read();
    v4 = Vo4_Read();
    v5 = Vo5_Read();
    v6 = Vo6_Read();
}

int main()
{   
// ----- INITIALIZATIONS ----------
    CYGlobalIntEnable;
    init();
// ------USB SETUP----------------    
    #ifdef USE_USB    
        USBUART_Start(0,USBUART_5V_OPERATION);
    #endif
// ------USBUART----------------------
    RF_BT_SELECT_Write(0);    
// ------TIMER SETUP----------------------
    Timer_1_Start();
    isr_US_StartEx(isr_UpdateState);
    
    Timer_2_Start();
    isr_T2_StartEx(assignment);
    
    for(;;) 
    {
        if (!v3 || !v6 || bias_M1 != 0 || bias_M2 != 0) {
            if (!v3 || v6) {
                bias_M1 = -5;
                bias_M2 = 5;
            } else if (!v6 || v3) {
                bias_M1 = 5;
                bias_M2 = -5;
            } else if ((v3 && v6) || flag_bias){
                bias_M1 = 0;
                bias_M2 = 0;
            }
        }

        /*
        if (c_sum >= (target / (64.5 * 3.14)) * 228) {
            flag_stop = 1;
            M1_D1_Write(1);
            M2_D1_Write(1);
            LED_Write(1);
            
        }
        */
        decide_motion();
        movement(current_Motion);
        
        if (flag_encoder) {
            c_M1 = QuadDec_M1_GetCounter();
            c_M2 = QuadDec_M2_GetCounter();
            
            QuadDec_M1_SetCounter(0);
            QuadDec_M2_SetCounter(0);
            
            if (current_Motion == FORWARD){
                speed_balance();
            }
            
            flag_encoder = 0;
        }

    }
}


//* ========================================
// Motion execution
void movement(enum Motion_State motion) {
    
    Timer_1_Enable();
    if (motion == FORWARD) {
        M1_set_speed(1, speed_M1 + bias_M1);
        M2_set_speed(1, speed_M2 + bias_M2);
        // start motor
        M1_D1_Write(0);
        M2_D1_Write(0);
        
    } else if (motion == LEFT) {        
        M1_set_speed(0, 35.0);
        M2_set_speed(1, 30.0);
        // start motor
        M1_D1_Write(0);
        M2_D1_Write(0);
        
    } else if (motion == RIGHT) {
        M1_set_speed(1, 35.0);
        M2_set_speed(0, 30.0);
        // start motor
        M1_D1_Write(0);
        M2_D1_Write(0);
        
    } else if (motion == OTHER) {        
        M1_set_speed(1, 0.0);
        M2_set_speed(1, 0.0);
        // stop motor
        M1_D1_Write(1);
        M2_D1_Write(1);
        
    } else if (motion == WAIT_STOP){
        M1_set_speed(1, 0.0);
        M2_set_speed(1, 0.0);
        // stop motor
        M1_D1_Write(1);
        M2_D1_Write(1);
    }
}
// decide motion
void decide_motion() {
    
    if ( (!v4 && (current_Motion == RIGHT || current_Motion == LEFT))) {
        next_Motion = FORWARD;
        flag_bias = 1;
        return;
    }

    if (!v5 && v3 && v4 && v6 && v1) { // && Vo3_Read() && Vo4_Read() && Vo6_Read()
        next_Motion = LEFT;
        return;
    }
    
    if (!v1 && v3 && v4 && v6 && v5) { // && Vo3_Read() && Vo4_Read() && Vo6_Read()
        next_Motion = RIGHT;
        return;
    }

    
    if (!v2 && !v4) {
        next_Motion = FORWARD;
        flag_bias = 0;
        return;
    }
    if ((!v3 && current_Motion == LEFT) || (!v6 &&  current_Motion == RIGHT)) {
        next_Motion = FORWARD;
        return;
    }
}

// use the ratio to control the speed
void speed_balance() {
    double ratio = (double) abs(c_M2) / abs(c_M1);
    if (ratio > 1) {
        ratio = 1;
    }
    speed_M1 = 2 + speed_M2 + speed_M2 * ratio * 0.2;
}
/* [] END OF FILE */
