/*============================================================================
Filename : qtm_surface_1finger_touch_api.h
Project : QTouch Modular Library
Purpose : Structs and definitions for use within modules
------------------------------------------------------------------------------
Copyright (C) 2019 Microchip. All rights reserved.
------------------------------------------------------------------------------
============================================================================*/

#ifndef TOUCH_API_SURFACE_CS_H
#define TOUCH_API_SURFACE_CS_H

/* Include files */
#include <stdint.h>
#include "qtm_common_components_api.h"

/* Axis status bits */
#define TOUCH_ACTIVE (uint8_t)((uint8_t)1u << 0u)    /* Bit 0 */
#define POSITION_CHANGE (uint8_t)((uint8_t)1u << 1u) /* Bit 1 */
#define POSITION_H_INC (uint8_t)((uint8_t)1u << 2u)  /* Bit 2 */
#define POSITION_H_DEC (uint8_t)((uint8_t)1u << 3u)  /* Bit 3 */
#define POSITION_V_INC (uint8_t)((uint8_t)1u << 4u)  /* Bit 4 */
#define POSITION_V_DEC (uint8_t)((uint8_t)1u << 5u)  /* Bit 5 */
#define SURFACE_REBURST (uint8_t)((uint8_t)1u << 7u) /* Bit 7 */

/* Extract Resolution / Deadband */
#define SCR_RESOLUTION(m) ((uint8_t)(((m)&0xF0u) >> 4u))
#define SCR_DEADBAND(m) ((uint8_t)((m)&0x0Fu))

/* Combine Resolution / Deadband */
#define SCR_RESOL_DEADBAND(r, p) ((uint8_t)(((r) << 4u) | (p)))

/* Position filtering */
#define POSITION_IIR_MASK 0x03u
#define POSITION_MEDIAN_ENABLE 0x10u
#define SCR_MEDIAN_IIR(r, p) ((uint8_t)(((r) << 4u) | (p)))

/* scroller resolution setting */
typedef enum tag_resolution_t {
	RESOL_2_BIT = 2,
	RESOL_3_BIT,
	RESOL_4_BIT,
	RESOL_5_BIT,
	RESOL_6_BIT,
	RESOL_7_BIT,
	RESOL_8_BIT,
	RESOL_9_BIT,
	RESOL_10_BIT,
	RESOL_11_BIT,
	RESOL_12_BIT
} scr_resolution_t;

/* scroller deadband percentage setting */
typedef enum tag_deadband_t {
	DB_NONE,
	DB_1_PERCENT,
	DB_2_PERCENT,
	DB_3_PERCENT,
	DB_4_PERCENT,
	DB_5_PERCENT,
	DB_6_PERCENT,
	DB_7_PERCENT,
	DB_8_PERCENT,
	DB_9_PERCENT,
	DB_10_PERCENT,
	DB_11_PERCENT,
	DB_12_PERCENT,
	DB_13_PERCENT,
	DB_14_PERCENT,
	DB_15_PERCENT
} scr_deadband_t;

/*----------------------------------------------------------------------------
 *     Structure Declarations
 *----------------------------------------------------------------------------*/

/* Surface CS Configuration */
typedef struct {
	uint16_t start_key_h;           /* Start key of horizontal axis */
	uint8_t  number_of_keys_h;      /* Number of keys in horizontal axis */
	uint16_t start_key_v;           /* Start key of vertical axis */
	uint8_t  number_of_keys_v;      /* Number of keys in vertical axis */
	uint8_t  resol_deadband;        /* Resolution 2 to 12 bits | Deadband 0% to 15% */
	uint8_t  position_hysteresis;   /* Distance threshold for initial move or direction change */
	uint8_t  position_filter;       /* Bits 1:0 = IIR (0% / 25% / 50% / 75%), Bit 4 = Enable Median Filter (3-point) */
	uint16_t contact_min_threshold; /* Contact threshold / Sum of 4 deltas */
	qtm_touch_key_data_t *qtm_touch_key_data; /* Pointer to touch key data */
} qtm_surface_cs_config_t;

/* Surface CS Data */
typedef struct {
	uint8_t  qt_surface_status;
	uint16_t h_position_abs;
	uint16_t h_position;
	uint16_t v_position_abs;
	uint16_t v_position;
	uint16_t contact_size;
} qtm_surface_contact_data_t;

/* Container */
typedef struct {
	qtm_surface_contact_data_t *qtm_surface_contact_data;
	qtm_surface_cs_config_t *   qtm_surface_cs_config;
} qtm_surface_cs_control_t;

/*----------------------------------------------------------------------------
 *   prototypes
 *----------------------------------------------------------------------------*/

/*============================================================================
touch_ret_t qtm_init_surface_cs(qtm_surface_cs_control_t *qtm_surface_cs_control);
------------------------------------------------------------------------------
Purpose: Initialize a scroller
Input  : Pointer to scroller group control data
Output : TOUCH_SUCCESS
Notes  : none
============================================================================*/
touch_ret_t qtm_init_surface_cs(qtm_surface_cs_control_t *qtm_surface_cs_control);

/*============================================================================
touch_ret_t qtm_surface_cs_process(qtm_surface_cs_control_t *qtm_surface_cs_control);
------------------------------------------------------------------------------
Purpose: Scroller position calculation and filtering
Input  : Pointer to scroller group control data
Output : TOUCH_SUCCESS
Notes  : none
============================================================================*/
touch_ret_t qtm_surface_cs_process(qtm_surface_cs_control_t *qtm_surface_cs_control);

/*============================================================================
uint16_t qtm_get_scroller_module_id(void)
------------------------------------------------------------------------------
Purpose: Returns the module ID
Input  : none
Output : Module ID
Notes  : none
============================================================================*/
uint16_t qtm_get_surface_cs_module_id(void);

/*============================================================================
uint8_t qtm_get_scroller_module_ver(void)
------------------------------------------------------------------------------
Purpose: Returns the module Firmware version
Input  : none
Output : Module ID - Upper nibble major / Lower nibble minor
Notes  : none
============================================================================*/
uint8_t qtm_get_surface_cs_module_ver(void);

#endif /* TOUCH_API_SCROLLER_H */
