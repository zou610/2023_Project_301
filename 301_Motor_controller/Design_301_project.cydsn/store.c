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
/*
        if(flag_encoder) {            
            encoder_counter1 = QuadDec_M1_GetCounter();  // 228; // timer - 1s
            encoder_counter2 = QuadDec_M2_GetCounter(); // 228; // timer - ls
            //sprintf(line, "M1_speed: %.2lf, M2_speed: %.2lf ", encoder_counter1, encoder_counter2); 
            
            //usbPutString(message_encoder);
            itoa(encoder_counter1, line, 10);
            //usbPutString(line);
            //usbPutString(" and ");
            itoa(encoder_counter2, line, 10);
            //usbPutString(line);
            //usbPutString("\r");
            
            QuadDec_M1_SetCounter(0);
            QuadDec_M2_SetCounter(0);
            
            flag_encoder = 0;
        }
*/            
        /* Place your application code here. */
/*        
        handle_usb();
*/        


        /* Input a integer to change PWM */
/*
        if (flag_KB_string == 1)
        {
            //usbPutString(line);

            if (atoi(line)){
                PWM_1_WriteCompare((double)(PWM_1_ReadPeriod()* atoi(line)) / 100);
                PWM_1_SaveConfig();
                PWM_2_WriteCompare((double)(PWM_2_ReadPeriod()* atoi(line)) / 100);
                PWM_2_SaveConfig();
                usbPutString("\tCustom input used\n\r");
            }            
            
            flag_KB_string = 0;
        }

*/

    /*
    pre_vo1 = Vo1_Read();
    pre_vo2 = Vo2_Read();
    pre_vo3 = Vo3_Read();
    pre_vo4 = Vo4_Read();
    pre_vo5 = Vo5_Read();
    pre_vo6 = Vo6_Read();
    */
    /*
    int encoder_counter1 = 0.0;
    int encoder_counter2 = 0.0;

    QuadDec_M1_Start();
    QuadDec_M2_Start();  

    QuadDec_M1_SetCounter(0);
    QuadDec_M2_SetCounter(0);  
    */
    /*
    else if (!Vo1_Read() & !Vo2_Read() & !Vo3_Read() & Vo5_Read() & !Vo6_Read()) {
        //current_state = RIGHTCROSS;
    }
        */
    /*
            if(flag_activate){
            M1_set_speed(80);
            M2_set_speed(80);
            CyDelay(500);
            movement(FORWARD);
            flag_activate = 0;
        }else{
            movement(FORWARD);
        }    
    
        */
          
        
        // if(flag_encoder) {            
            // encoder_counter1 = QuadDec_M1_GetCounter(); // 228 / 64.5 * 3.14; // 64.5 diameter // timer - 1s, [cm/s]
            // encoder_counter2 = QuadDec_M2_GetCounter(); // 228 / 64.5 * 3.14; // 64.5 diameter // timer - 1s, [cm/s]
            
            // encoder_counter1 = encoder_counter1  / 228 * 64.5 * 3.14;
            // encoder_counter2 = encoder_counter2  / 228 * 64.5 * 3.14;
            
            // //sprintf(line, "M1_speed: %.2lf, M2_speed: %.2lf ", encoder_counter1, encoder_counter2); 

            // itoa(encoder_counter1, line, 10);
            // usbPutString(line);
            // usbPutString(" and ");

            // itoa(encoder_counter2, line, 10);
            // usbPutString(line);
            // usbPutString("\n");
            // usbPutString("\r");

            // QuadDec_M1_SetCounter(0);
            // QuadDec_M2_SetCounter(0);            

            // flag_encoder = 0;
        // }
        

        
        
    
/* [] END OF FILE */
