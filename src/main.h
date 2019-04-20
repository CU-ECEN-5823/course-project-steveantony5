

#ifndef __MAIN_H_
#define __MAIN_H_

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include <gecko_configuration.h>
#include <mesh_sizes.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include <em_gpio.h>
#include "em_core.h"
#include "log.h"
#include "display.h"
#include "gpio.h"
#include "letimer.h"
#include "mesh_generic_model_capi_types.h"
#include "mesh_lighting_model_capi_types.h"
#include "mesh_lib.h"
#include "mesh_init.h"

/* Device initialization header */
#include "hal-config.h"
#include "mesh_models.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif
#include "ble_mesh_device_type.h"


/**********************************************
 *                MACROS
 * ********************************************/
#define TIMER_ID_RESTART       (78)
#define TIMER_ID_FACTORY_RESET (77)
#define TIMER_ID_PROVISIONING  (66)
#define ONE_SHOT                (1)
#define ENABLE_TOUCH_1        (100)
#define ENABLE_TOUCH_2        (101)

#define NONE                  (102)
#define DELAY                 (108)

#define FALL_DETECTION_NODE                    (0x62)
#define HEART_BEAT_NODE                        (0x02)
#define PEOPLE_COUNT_NODE                      (0x03)

#define HEARTBEAT_NORMAL_VALUE_MAX             (90)
#define HEARTBEAT_NORMAL_VALUE_MIN             (40)
#define PEOPLE_COUNT_MAX                       (10)

#define FALL_DETECTED                          (40)
#define TAP_DETECTED                           (41)

/**********************************************
 *                GLOBALS
 * ********************************************/


/// current position of the button
extern button_flag  button_position ;

extern button_flag touch_1_status;


extern int8_t FLAG_PERIODIC_1;
extern int32_t FREQUENCY_OSC;

extern void button_init(void);

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

// bluetooth stack heap
#define MAX_CONNECTIONS 2


// Bluetooth advertisement set configuration
//
// At minimum the following is required:
// * One advertisement set for Bluetooth LE stack (handle number 0)
// * One advertisement set for Mesh data (handle number 1)
// * One advertisement set for Mesh unprovisioned beacons (handle number 2)
// * One advertisement set for Mesh unprovisioned URI (handle number 3)
// * N advertisement sets for Mesh GATT service advertisements
// (one for each network key, handle numbers 4 .. N+3)
//
#define MAX_ADVERTISERS (4 + MESH_CFG_MAX_NETKEYS)


// bluetooth stack configuration
extern const struct bg_gattdb_def bg_gattdb_data;

extern int32_t FLAG_STOP_LOCAL_BUFFER;

/*
--------------------------------------------------------------------------------------------
trigger_alarm
--------------------------------------------------------------------------------------------
*	This is function triggers buzzer
*
* 	@\param			none
*
* 	@\return		none
*
*/
void trigger_alarm();

/*
--------------------------------------------------------------------------------------------
handle_gecko_event_scheduler
--------------------------------------------------------------------------------------------
*	This calls the scheduler for mesh
*
* 	@\param			event occured
*
* 	@\return		none
*
*/
void handle_gecko_event_scheduler(uint32_t evt_id, struct gecko_cmd_packet *evt);

/*
--------------------------------------------------------------------------------------------
mesh_native_bgapi_init
--------------------------------------------------------------------------------------------
*	This inititates models
*
* 	@\param			none
*
* 	@\return		none
*
*/
void mesh_native_bgapi_init(void);

/*
Source code not provided, owned by Silicon labs
*
*/
bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);

/*
--------------------------------------------------------------------------------------------
set_device_name
--------------------------------------------------------------------------------------------
*	This sets the node name
*
* 	@\param			bluetooth address
*
* 	@\return		none
*
*/
void set_device_name(bd_addr *);


/*
--------------------------------------------------------------------------------------------
friend_node_init
--------------------------------------------------------------------------------------------
*	This sets the node as friend node
*
* 	@\param			bluetooth address
*
* 	@\return		none
*
*/
void friend_node_init();

#endif
