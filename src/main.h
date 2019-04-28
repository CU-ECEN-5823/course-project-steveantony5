

#ifndef __MAIN_H_
#define __MAIN_H_

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"
#include <stdbool.h>
#include "native_gecko.h"
#include "log.h"
#include "letimer.h"
#include "ble_mesh_device_type.h"

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
#include "sleep_config.h"
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
//set as 1 when it has to function as friend node
#define MESH                   (1)

//For soft timer
#define TIMER_ID_RESTART       (78)
#define TIMER_ID_FACTORY_RESET (77)
#define ONE_SHOT                (1)
#define ENABLE_TOUCH_1        (100)
#define ENABLE_TOUCH_2        (101)


/*Node Address of Lower power nodes*/
#define FALL_DETECTION_NODE                    (0x6)
#define HEART_BEAT_NODE                        (0x1)
#define PEOPLE_COUNT_NODE                      (0x2)

/*Threshold for maximum people count*/
#define PEOPLE_COUNT_MAX                       (5)

/*Unique id to detect if it is tap signal or fall signal from LPN*/
#define FALL_DETECTED                          (40)
#define TAP_DETECTED                           (41)

/*Unique key for Persistant storage of doctor count*/
#define DOCTOR_COUNT_PS_ID                     (0x4000)
/**********************************************
 *                GLOBALS
 * ********************************************/


/// current position of the button
extern button_flag  button_position ;

extern button_flag touch_1_status;


extern int8_t FLAG_PERIODIC_1;
extern int32_t FREQUENCY_OSC;

extern void button_init(void);

// bluetooth stack configuration
extern const struct bg_gattdb_def bg_gattdb_data;

extern bool FLAG_STOP_LOCAL_BUFFER;
extern bool FLAG_STOP_DEFRIBRILLATION;

// bluetooth stack heap
#define MAX_CONNECTIONS 5


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
