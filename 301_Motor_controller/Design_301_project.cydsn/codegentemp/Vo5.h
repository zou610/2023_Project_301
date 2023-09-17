/*******************************************************************************
* File Name: Vo5.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Vo5_H) /* Pins Vo5_H */
#define CY_PINS_Vo5_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Vo5_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Vo5__PORT == 15 && ((Vo5__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Vo5_Write(uint8 value);
void    Vo5_SetDriveMode(uint8 mode);
uint8   Vo5_ReadDataReg(void);
uint8   Vo5_Read(void);
void    Vo5_SetInterruptMode(uint16 position, uint16 mode);
uint8   Vo5_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Vo5_SetDriveMode() function.
     *  @{
     */
        #define Vo5_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Vo5_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Vo5_DM_RES_UP          PIN_DM_RES_UP
        #define Vo5_DM_RES_DWN         PIN_DM_RES_DWN
        #define Vo5_DM_OD_LO           PIN_DM_OD_LO
        #define Vo5_DM_OD_HI           PIN_DM_OD_HI
        #define Vo5_DM_STRONG          PIN_DM_STRONG
        #define Vo5_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Vo5_MASK               Vo5__MASK
#define Vo5_SHIFT              Vo5__SHIFT
#define Vo5_WIDTH              1u

/* Interrupt constants */
#if defined(Vo5__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Vo5_SetInterruptMode() function.
     *  @{
     */
        #define Vo5_INTR_NONE      (uint16)(0x0000u)
        #define Vo5_INTR_RISING    (uint16)(0x0001u)
        #define Vo5_INTR_FALLING   (uint16)(0x0002u)
        #define Vo5_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Vo5_INTR_MASK      (0x01u) 
#endif /* (Vo5__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Vo5_PS                     (* (reg8 *) Vo5__PS)
/* Data Register */
#define Vo5_DR                     (* (reg8 *) Vo5__DR)
/* Port Number */
#define Vo5_PRT_NUM                (* (reg8 *) Vo5__PRT) 
/* Connect to Analog Globals */                                                  
#define Vo5_AG                     (* (reg8 *) Vo5__AG)                       
/* Analog MUX bux enable */
#define Vo5_AMUX                   (* (reg8 *) Vo5__AMUX) 
/* Bidirectional Enable */                                                        
#define Vo5_BIE                    (* (reg8 *) Vo5__BIE)
/* Bit-mask for Aliased Register Access */
#define Vo5_BIT_MASK               (* (reg8 *) Vo5__BIT_MASK)
/* Bypass Enable */
#define Vo5_BYP                    (* (reg8 *) Vo5__BYP)
/* Port wide control signals */                                                   
#define Vo5_CTL                    (* (reg8 *) Vo5__CTL)
/* Drive Modes */
#define Vo5_DM0                    (* (reg8 *) Vo5__DM0) 
#define Vo5_DM1                    (* (reg8 *) Vo5__DM1)
#define Vo5_DM2                    (* (reg8 *) Vo5__DM2) 
/* Input Buffer Disable Override */
#define Vo5_INP_DIS                (* (reg8 *) Vo5__INP_DIS)
/* LCD Common or Segment Drive */
#define Vo5_LCD_COM_SEG            (* (reg8 *) Vo5__LCD_COM_SEG)
/* Enable Segment LCD */
#define Vo5_LCD_EN                 (* (reg8 *) Vo5__LCD_EN)
/* Slew Rate Control */
#define Vo5_SLW                    (* (reg8 *) Vo5__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Vo5_PRTDSI__CAPS_SEL       (* (reg8 *) Vo5__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Vo5_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Vo5__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Vo5_PRTDSI__OE_SEL0        (* (reg8 *) Vo5__PRTDSI__OE_SEL0) 
#define Vo5_PRTDSI__OE_SEL1        (* (reg8 *) Vo5__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Vo5_PRTDSI__OUT_SEL0       (* (reg8 *) Vo5__PRTDSI__OUT_SEL0) 
#define Vo5_PRTDSI__OUT_SEL1       (* (reg8 *) Vo5__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Vo5_PRTDSI__SYNC_OUT       (* (reg8 *) Vo5__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Vo5__SIO_CFG)
    #define Vo5_SIO_HYST_EN        (* (reg8 *) Vo5__SIO_HYST_EN)
    #define Vo5_SIO_REG_HIFREQ     (* (reg8 *) Vo5__SIO_REG_HIFREQ)
    #define Vo5_SIO_CFG            (* (reg8 *) Vo5__SIO_CFG)
    #define Vo5_SIO_DIFF           (* (reg8 *) Vo5__SIO_DIFF)
#endif /* (Vo5__SIO_CFG) */

/* Interrupt Registers */
#if defined(Vo5__INTSTAT)
    #define Vo5_INTSTAT            (* (reg8 *) Vo5__INTSTAT)
    #define Vo5_SNAP               (* (reg8 *) Vo5__SNAP)
    
	#define Vo5_0_INTTYPE_REG 		(* (reg8 *) Vo5__0__INTTYPE)
#endif /* (Vo5__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Vo5_H */


/* [] END OF FILE */
