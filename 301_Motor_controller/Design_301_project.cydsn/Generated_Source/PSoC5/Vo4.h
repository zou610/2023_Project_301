/*******************************************************************************
* File Name: Vo4.h  
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

#if !defined(CY_PINS_Vo4_H) /* Pins Vo4_H */
#define CY_PINS_Vo4_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Vo4_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Vo4__PORT == 15 && ((Vo4__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Vo4_Write(uint8 value);
void    Vo4_SetDriveMode(uint8 mode);
uint8   Vo4_ReadDataReg(void);
uint8   Vo4_Read(void);
void    Vo4_SetInterruptMode(uint16 position, uint16 mode);
uint8   Vo4_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Vo4_SetDriveMode() function.
     *  @{
     */
        #define Vo4_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Vo4_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Vo4_DM_RES_UP          PIN_DM_RES_UP
        #define Vo4_DM_RES_DWN         PIN_DM_RES_DWN
        #define Vo4_DM_OD_LO           PIN_DM_OD_LO
        #define Vo4_DM_OD_HI           PIN_DM_OD_HI
        #define Vo4_DM_STRONG          PIN_DM_STRONG
        #define Vo4_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Vo4_MASK               Vo4__MASK
#define Vo4_SHIFT              Vo4__SHIFT
#define Vo4_WIDTH              1u

/* Interrupt constants */
#if defined(Vo4__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Vo4_SetInterruptMode() function.
     *  @{
     */
        #define Vo4_INTR_NONE      (uint16)(0x0000u)
        #define Vo4_INTR_RISING    (uint16)(0x0001u)
        #define Vo4_INTR_FALLING   (uint16)(0x0002u)
        #define Vo4_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Vo4_INTR_MASK      (0x01u) 
#endif /* (Vo4__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Vo4_PS                     (* (reg8 *) Vo4__PS)
/* Data Register */
#define Vo4_DR                     (* (reg8 *) Vo4__DR)
/* Port Number */
#define Vo4_PRT_NUM                (* (reg8 *) Vo4__PRT) 
/* Connect to Analog Globals */                                                  
#define Vo4_AG                     (* (reg8 *) Vo4__AG)                       
/* Analog MUX bux enable */
#define Vo4_AMUX                   (* (reg8 *) Vo4__AMUX) 
/* Bidirectional Enable */                                                        
#define Vo4_BIE                    (* (reg8 *) Vo4__BIE)
/* Bit-mask for Aliased Register Access */
#define Vo4_BIT_MASK               (* (reg8 *) Vo4__BIT_MASK)
/* Bypass Enable */
#define Vo4_BYP                    (* (reg8 *) Vo4__BYP)
/* Port wide control signals */                                                   
#define Vo4_CTL                    (* (reg8 *) Vo4__CTL)
/* Drive Modes */
#define Vo4_DM0                    (* (reg8 *) Vo4__DM0) 
#define Vo4_DM1                    (* (reg8 *) Vo4__DM1)
#define Vo4_DM2                    (* (reg8 *) Vo4__DM2) 
/* Input Buffer Disable Override */
#define Vo4_INP_DIS                (* (reg8 *) Vo4__INP_DIS)
/* LCD Common or Segment Drive */
#define Vo4_LCD_COM_SEG            (* (reg8 *) Vo4__LCD_COM_SEG)
/* Enable Segment LCD */
#define Vo4_LCD_EN                 (* (reg8 *) Vo4__LCD_EN)
/* Slew Rate Control */
#define Vo4_SLW                    (* (reg8 *) Vo4__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Vo4_PRTDSI__CAPS_SEL       (* (reg8 *) Vo4__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Vo4_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Vo4__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Vo4_PRTDSI__OE_SEL0        (* (reg8 *) Vo4__PRTDSI__OE_SEL0) 
#define Vo4_PRTDSI__OE_SEL1        (* (reg8 *) Vo4__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Vo4_PRTDSI__OUT_SEL0       (* (reg8 *) Vo4__PRTDSI__OUT_SEL0) 
#define Vo4_PRTDSI__OUT_SEL1       (* (reg8 *) Vo4__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Vo4_PRTDSI__SYNC_OUT       (* (reg8 *) Vo4__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Vo4__SIO_CFG)
    #define Vo4_SIO_HYST_EN        (* (reg8 *) Vo4__SIO_HYST_EN)
    #define Vo4_SIO_REG_HIFREQ     (* (reg8 *) Vo4__SIO_REG_HIFREQ)
    #define Vo4_SIO_CFG            (* (reg8 *) Vo4__SIO_CFG)
    #define Vo4_SIO_DIFF           (* (reg8 *) Vo4__SIO_DIFF)
#endif /* (Vo4__SIO_CFG) */

/* Interrupt Registers */
#if defined(Vo4__INTSTAT)
    #define Vo4_INTSTAT            (* (reg8 *) Vo4__INTSTAT)
    #define Vo4_SNAP               (* (reg8 *) Vo4__SNAP)
    
	#define Vo4_0_INTTYPE_REG 		(* (reg8 *) Vo4__0__INTTYPE)
#endif /* (Vo4__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Vo4_H */


/* [] END OF FILE */
