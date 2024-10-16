/*============================================================================
Filename : qtm_acq_saml21_0x0026_api.h
Project : QTouch Modular Library
Purpose : API for Acquisition module - SAML21/PTC
------------------------------------------------------------------------------
------------------------------------------------------------------------------
Copyright (C) 2019 Microchip Inc. All rights reserved.
------------------------------------------------------------------------------
============================================================================*/

#ifndef TOUCH_API_L21_ACQ_H
#define TOUCH_API_L21_ACQ_H

/* Include files */

#include "qtm_common_components_api.h"

/* Calibration auto-tuning options */
#define CAL_OPTION_MASK 0x03u

#define CAL_AUTO_TUNE_NONE 0u
#define CAL_AUTO_TUNE_RSEL 1u
#define CAL_AUTO_TUNE_PRSC 2u

/* Timing auto-cal target */
#define CAL_CHRG_TIME_MASK 0x30u
#define CAL_CHRG_TIME_POS 4u

#define CAL_CHRG_2TAU 0u
#define CAL_CHRG_3TAU 1u
#define CAL_CHRG_4TAU 2u
#define CAL_CHRG_5TAU 3u

#define RSEL_MAX_OPTION RSEL_VAL_100
#define PRSC_MAX_OPTION PRSC_DIV_SEL_32

/* X line bit position */
#define X_NONE 0u
#define X(n) ((uint16_t)(1u << (n)))

/* Y line bit position */
#define Y(n) ((uint16_t)(1u << (n)))

/* Extract Analog / Digital Gain */
#define NODE_GAIN_ANA(m) (uint8_t)(((m)&0xF0u) >> 4u)
#define NODE_GAIN_DIG(m) (uint8_t)((m)&0x0Fu)

/* Combine Analog / Digital Gain */
#define NODE_GAIN(a, d) (uint8_t)(((a) << 4u) | (d))

/* Extract Resistor / Prescaler */
#define NODE_RSEL(m) (uint8_t)(((m)&0xF0u) >> 4u)
#define NODE_PRSC(m) (uint8_t)((m)&0x0Fu)

/* Combine Resistor / Prescaler */
#define NODE_RSEL_PRSC(r, p) (uint8_t)(((r) << 4u) | (p))

typedef enum tag_filter_level_t {
	FILTER_LEVEL_1,
	FILTER_LEVEL_2,
	FILTER_LEVEL_4,
	FILTER_LEVEL_8,
	FILTER_LEVEL_16,
	FILTER_LEVEL_32,
	FILTER_LEVEL_64
} filter_level_t;

/* Touch library GAIN setting */
typedef enum tag_gain_t { GAIN_1, GAIN_2, GAIN_4, GAIN_8, GAIN_16 } gain_t;
/* PTC clock prescale setting.
* For Example: if Generic clock input to PTC = 4MHz, then:
* PRSC_DIV_SEL_4 sets PTC Clock to 1MHz
* PRSC_DIV_SEL_8 sets PTC Clock to 500KHz

*
*/
typedef enum tag_prsc_div_sel_t { PRSC_DIV_SEL_4, PRSC_DIV_SEL_8, PRSC_DIV_SEL_16, PRSC_DIV_SEL_32 } prsc_div_sel_t;

/**
 * PTC series resistor setting, only works in mutual capacitance mode
 * RSEL_VAL_0 sets internal series resistor to 0ohms.
 * RSEL_VAL_20 sets internal series resistor to 20Kohms.
 * RSEL_VAL_50 sets internal series resistor to 50Kohms.
 * RSEL_VAL_100 sets internal series resistor to 100Kohms.
 */
typedef enum tag_rsel_val_t { RSEL_VAL_0, RSEL_VAL_20, RSEL_VAL_50, RSEL_VAL_100 } rsel_val_t;

/**
* PTC acquisition frequency delay setting.

* inserts "n" PTC clock cycles between consecutive measurements
* e.g.  FREQ_HOP_SEL_14 setting inserts 14 PTC clock cycles.
OR
* FREQ_SEL_SPREAD automatically sweeps through all 15 delay values during the series of measurements.
*/
typedef enum tag_freq_config_sel_t {
	FREQ_SEL_0,
	FREQ_SEL_1,
	FREQ_SEL_2,
	FREQ_SEL_3,
	FREQ_SEL_4,
	FREQ_SEL_5,
	FREQ_SEL_6,
	FREQ_SEL_7,
	FREQ_SEL_8,
	FREQ_SEL_9,
	FREQ_SEL_10,
	FREQ_SEL_11,
	FREQ_SEL_12,
	FREQ_SEL_13,
	FREQ_SEL_14,
	FREQ_SEL_15,
	FREQ_SEL_SPREAD
} freq_config_sel_t;

/*----------------------------------------------------------------------------
 *     Structure Declarations
 *----------------------------------------------------------------------------*/

/* Acquisition module SAML21 Sequential */
typedef struct {
	uint16_t node_xmask;        /* Selects the X Pins for this node */
	uint16_t node_ymask;        /* Selects the Y Pins for this node */
	uint8_t  node_rsel_prsc;    /* Bits 7:4 = Resistor, Bits 3:0  Prescaler */
	uint8_t  node_gain;         /* Bits 7:4 = Analog gain, Bits 3:0 = Digital gain */
	uint8_t  node_oversampling; /* Accumulator setting  */
} qtm_acq_saml21_node_config_t;

/* Node run-time data - Defined in common api as it will be used with all acquisition modules */

/* Node group configuration */
typedef struct {
	uint16_t num_sensor_nodes;       /* Number of sensor nodes */
	uint8_t  acq_sensor_type;        /* Self or mutual sensors */
	uint8_t  calib_option_select;    /* Hardware tuning: XX | TT 3/4/5 Tau | X | XX None/RSEL/PRSC/CSD */
	uint8_t  freq_option_select;     /* SDS or ASDV setting */
	uint8_t  ptc_interrupt_priority; /* Runtime priority of PTC interrupt */
} qtm_acq_node_group_config_t;

/* Container structure for sensor group */
typedef struct {
	qtm_acq_node_group_config_t(*qtm_acq_node_group_config);
	qtm_acq_saml21_node_config_t(*qtm_acq_node_config);
	qtm_acq_node_data_t(*qtm_acq_node_data);
} qtm_acquisition_control_t;

typedef struct {
	qtm_acquisition_control_t *qtm_acq_control;
	uint16_t                   auto_scan_node_number;
	uint8_t                    auto_scan_node_threshold;
	uint8_t                    auto_scan_trigger;
} qtm_auto_scan_config_t;

#define DRIVEN_SHIELD_DUMMY_ACQ 3u

typedef void (*qtm_drivenshield_callback_t)(uint8_t csd, uint8_t sds, uint8_t prescaler, uint8_t volatile *ptr,
                                            uint8_t value);

typedef struct qtm_drivenshield_config_tag {
	uint8_t flags;
} qtm_drivenshield_config_t;

/*============================================================================
touch_ret_t qtm_drivenshield_setup(qtm_drivenshield_config_t* config);
------------------------------------------------------------------------------
Purpose: Setup the softshield with settings from the user
Input  : qtm_drivenshield_config_t  setup in touch.c and touch.h
Output : touch_ret_t
Notes  : Called by application to load the softshield operating parameters
============================================================================*/
touch_ret_t qtm_drivenshield_setup(qtm_drivenshield_config_t *config);

/*============================================================================
touch_ret_t qtm_drivenshield_register_start_callback(qtm_drivenshield_callback_t callback);
------------------------------------------------------------------------------
Purpose: register the Softshield Start callback with the touch library
Input  : pointer to the applicaion function to start the event system
Output : touch_ret_t
Notes  : the library initialises this with a null, if this remains the
         library will function as normal, if this is not null then the
         application will start the event system and call this callback before
                 start of touch measurement
============================================================================*/
touch_ret_t qtm_drivenshield_register_start_callback(qtm_drivenshield_callback_t callback);

/*============================================================================
touch_ret_t qtm_drivenshield_deregister_start_callback(void);
------------------------------------------------------------------------------
Purpose: de-registers the Softshield start callback with the touch library
Input  : void
Output : touch_ret_t
Notes  : when this function is called driven shield functionality will stop
============================================================================*/
touch_ret_t qtm_drivenshield_deregister_start_callback(void);

/*----------------------------------------------------------------------------
 * prototypes
 *----------------------------------------------------------------------------*/

/* Library prototypes */
/*============================================================================
touch_ret_t qtm_acquisition_process(void)
------------------------------------------------------------------------------
Purpose: Signal capture and processing
Input  : (Measured signals, config)
Output : touch_ret_t
Notes  : Called by application after 'touch_measure_complete_callback'
============================================================================*/
touch_ret_t qtm_acquisition_process(void);

/*============================================================================
touch_ret_t qtm_ptc_init_acquisition_module(qtm_acquisition_control_t* qtm_acq_control_ptr);
------------------------------------------------------------------------------
Purpose: Initialize the PTC & Assign pins
Input  : pointer to acquisition set
Output : touch_ret_t: TOUCH_SUCCESS or INVALID_PARAM
Notes  : ptc_init_acquisition module must be called ONLY once with a pointer to each config set
============================================================================*/
touch_ret_t qtm_ptc_init_acquisition_module(qtm_acquisition_control_t *qtm_acq_control_ptr);

/*============================================================================
touch_ret_t qtm_ptc_qtlib_assign_signal_memory(uint16_t* qtm_signal_raw_data_ptr);
------------------------------------------------------------------------------
Purpose: Assign raw signals pointer to array defined in application code
Input  : pointer to raw data array
Output : touch_ret_t: TOUCH_SUCCESS
Notes  : none
============================================================================*/
touch_ret_t qtm_ptc_qtlib_assign_signal_memory(uint16_t *qtm_signal_raw_data_ptr);

/* Scan configuration */
/*============================================================================
touch_ret_t qtm_enable_sensor_node(qtm_acquisition_control_t* qtm_acq_control_ptr, uint16_t qtm_which_node_number);
------------------------------------------------------------------------------
Purpose:  Enables a sensor node for measurement
Input  :  Node configurations pointer, node (channel) number
Output : touch_ret_t:
Notes  :
============================================================================*/
touch_ret_t qtm_enable_sensor_node(qtm_acquisition_control_t *qtm_acq_control_ptr, uint16_t qtm_which_node_number);

/*============================================================================
touch_ret_t qtm_calibrate_sensor_node(ptc_seq_acq_settings* qtm_acq_control_l_ptr, uint16_t which_node_number)
------------------------------------------------------------------------------
Purpose:  Marks a sensor node for calibration
Input  :  Node configurations pointer, node (channel) number
Output : touch_ret_t:
Notes  :
============================================================================*/
touch_ret_t qtm_calibrate_sensor_node(qtm_acquisition_control_t *qtm_acq_control_ptr, uint16_t qtm_which_node_number);

/* Measurement start - sequence or windowcomp */

/*============================================================================
touch_ret_t qtm_ptc_start_measurement_seq(qtm_acquisition_control_t* qtm_acq_control_pointer, void
(*measure_complete_callback) (void));
------------------------------------------------------------------------------
Purpose:  Loads touch configurations for first channel and start,
Input  :  Node configurations pointer, measure complete callback pointer
Output : touch_ret_t:
Notes  :
============================================================================*/
touch_ret_t qtm_ptc_start_measurement_seq(qtm_acquisition_control_t *qtm_acq_control_pointer,
                                          void (*measure_complete_callback)(void));

/*============================================================================
touch_ret_t qtm_autoscan_sensor_node(qtm_auto_scan_config_t* qtm_auto_scan_config_ptr, void
(*auto_scan_callback)(void));
------------------------------------------------------------------------------
Purpose: Configures the PTC for sleep mode measurement of a single node, with window comparator wake
Input  : Acquisition set, channel number, threshold, scan trigger
Output : touch_ret_t
Notes  : none
============================================================================*/
touch_ret_t qtm_autoscan_sensor_node(qtm_auto_scan_config_t *qtm_auto_scan_config_ptr,
                                     void (*auto_scan_callback)(void));

/*============================================================================
touch_ret_t qtm_autoscan_node_cancel(void)
------------------------------------------------------------------------------
Purpose: Cancel auto-scan config
Input  : None
Output : touch_ret_t
Notes  : none
============================================================================*/
touch_ret_t qtm_autoscan_node_cancel(void);

/*============================================================================
void qtm_ptc_de_init(void)
------------------------------------------------------------------------------
Purpose: Clear PTC Pin registers, set TOUCH_STATE_NULL
Input  : none
Output : none
Notes  : none
============================================================================*/
void qtm_ptc_de_init(void);

/*============================================================================
uint16_t qtm_saml21_acq_module_get_id(void);
------------------------------------------------------------------------------
Purpose: Returns the module ID
Input  : none
Output : Module ID
Notes  : none
============================================================================*/
uint16_t qtm_saml21_acq_module_get_id(void);

/*============================================================================
uint8_t qtm_saml21_acq_module_get_version(void);
------------------------------------------------------------------------------
Purpose: Returns the module Firmware version
Input  : none
Output : Module ID - Upper nibble major / Lower nibble minor
Notes  : none
============================================================================*/
uint8_t qtm_saml21_acq_module_get_version(void);

/*============================================================================
void qtm_ptc_clear_interrupt(void)
------------------------------------------------------------------------------
Purpose:  Clears the eoc/wcomp interrupt bits
Input    :  none
Output  :  none
Notes    :  none
============================================================================*/
void qtm_ptc_clear_interrupt(void);

/*============================================================================
void qtm_saml21_ptc_handler_eoc(void)
------------------------------------------------------------------------------
Purpose:  Captures  the  measurement,  starts  the  next  or  End  Of  Sequence  handler
Input    :  none
Output  :  none
Notes    :  none
============================================================================*/
void qtm_saml21_ptc_handler_eoc(void);

#endif /* TOUCH_API_L21_ACQ_H */
