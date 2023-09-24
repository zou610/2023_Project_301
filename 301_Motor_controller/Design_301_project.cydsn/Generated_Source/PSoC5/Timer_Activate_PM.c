/*******************************************************************************
* File Name: Timer_Activate_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Timer_Activate.h"

static Timer_Activate_backupStruct Timer_Activate_backup;


/*******************************************************************************
* Function Name: Timer_Activate_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_Activate_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_Activate_SaveConfig(void) 
{
    #if (!Timer_Activate_UsingFixedFunction)
        Timer_Activate_backup.TimerUdb = Timer_Activate_ReadCounter();
        Timer_Activate_backup.InterruptMaskValue = Timer_Activate_STATUS_MASK;
        #if (Timer_Activate_UsingHWCaptureCounter)
            Timer_Activate_backup.TimerCaptureCounter = Timer_Activate_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_Activate_UDB_CONTROL_REG_REMOVED)
            Timer_Activate_backup.TimerControlRegister = Timer_Activate_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_Activate_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_Activate_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_Activate_RestoreConfig(void) 
{   
    #if (!Timer_Activate_UsingFixedFunction)

        Timer_Activate_WriteCounter(Timer_Activate_backup.TimerUdb);
        Timer_Activate_STATUS_MASK =Timer_Activate_backup.InterruptMaskValue;
        #if (Timer_Activate_UsingHWCaptureCounter)
            Timer_Activate_SetCaptureCount(Timer_Activate_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_Activate_UDB_CONTROL_REG_REMOVED)
            Timer_Activate_WriteControlRegister(Timer_Activate_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_Activate_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_Activate_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_Activate_Sleep(void) 
{
    #if(!Timer_Activate_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_Activate_CTRL_ENABLE == (Timer_Activate_CONTROL & Timer_Activate_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_Activate_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_Activate_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_Activate_Stop();
    Timer_Activate_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_Activate_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_Activate_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_Activate_Wakeup(void) 
{
    Timer_Activate_RestoreConfig();
    #if(!Timer_Activate_UDB_CONTROL_REG_REMOVED)
        if(Timer_Activate_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_Activate_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
