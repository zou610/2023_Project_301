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
#define ENCODER_M1 5
#define ENCODER_M2 15

enum Motion_State {
    FORWARD,
    LEFT, 
    RIGHT, 
    BACKWARD,
    WAIT_STOP,
    END,
    OTHER,
    BIAS
};


//* ======== USB FUNCTION ==================
//* ========================================

void usbPutString(char *s);
void usbPutChar(char c);
void handle_usb();


//* ======== SELF FUNCTION =================
//* ========================================
void M1_set_speed();
void M2_set_speed();
char* enumToString(enum Motion_State state);

void init();

/* [] END OF FILE */
