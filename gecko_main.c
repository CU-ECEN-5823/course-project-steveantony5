/***************************************************************************//**
 * @file
 * @brief Silicon Labs BT Mesh Empty Example Project
 * This example demonstrates the bare minimum needed for a Blue Gecko BT Mesh C application.
 * The application starts unprovisioned Beaconing after boot
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

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

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include <em_gpio.h>
#include "em_core.h"
#include "src/log.h"
#include "src/display.h"
#include "src/gpio.h"
#include "src/letimer.h"
#include "mesh_generic_model_capi_types.h"

/* Device initialization header */
#include "hal-config.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif
#include "src/ble_mesh_device_type.h"

extern int8_t FLAG_PERIODIC_1;
extern int32_t FREQUENCY_OSC;

/**********************************************
 *                MACROS
 * ********************************************/
#define TIMER_ID_RESTART        (78)
#define ONE_SHOT                (1)

/**********************************************
 *                GLOBALS
 * ********************************************/
int local_button_press = 0;

uint8 request_count;

uint16 _my_index = 0x00;
/// transaction identifier
uint8 trid = 0;

/// current position of the button
extern button_flag  button_position ;


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

uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS) + BTMESH_HEAP_SIZE + 1760];

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

static gecko_bluetooth_ll_priorities linklayer_priorities = GECKO_BLUETOOTH_PRIORITIES_DEFAULT;

// bluetooth stack configuration
extern const struct bg_gattdb_def bg_gattdb_data;

// Flag for indicating DFU Reset must be performed
uint8_t boot_to_dfu = 0;

const gecko_configuration_t config =
{
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.max_advertisers = MAX_ADVERTISERS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap) - BTMESH_HEAP_SIZE,
  .bluetooth.sleep_clock_accuracy = 100,
  .bluetooth.linklayer_priorities = &linklayer_priorities,
  .gattdb = &bg_gattdb_data,
  .btmesh_heap_size = BTMESH_HEAP_SIZE,
#if (HAL_PA_ENABLE)
  .pa.config_enable = 1, // Set this to be a valid PA config
#if defined(FEATURE_PA_INPUT_FROM_VBAT)
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#else
  .pa.input = GECKO_RADIO_PA_INPUT_DCDC,
#endif // defined(FEATURE_PA_INPUT_FROM_VBAT)
#endif // (HAL_PA_ENABLE)
  .max_timers = 16,
};

void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);
void mesh_native_bgapi_init(void);
bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);

/**
 * See light switch app.c file definition
 */
void gecko_bgapi_classes_init_server_friend(void)
{
	gecko_bgapi_class_dfu_init();
	gecko_bgapi_class_system_init();
	gecko_bgapi_class_le_gap_init();
	gecko_bgapi_class_le_connection_init();
	//gecko_bgapi_class_gatt_init();
	gecko_bgapi_class_gatt_server_init();
	gecko_bgapi_class_hardware_init();
	gecko_bgapi_class_flash_init();
	gecko_bgapi_class_test_init();
	//gecko_bgapi_class_sm_init();
	//mesh_native_bgapi_init();
	gecko_bgapi_class_mesh_node_init();
	//gecko_bgapi_class_mesh_prov_init();
	gecko_bgapi_class_mesh_proxy_init();
	gecko_bgapi_class_mesh_proxy_server_init();
	//gecko_bgapi_class_mesh_proxy_client_init();
	//gecko_bgapi_class_mesh_generic_client_init();
	gecko_bgapi_class_mesh_generic_server_init();
	//gecko_bgapi_class_mesh_vendor_model_init();
	//gecko_bgapi_class_mesh_health_client_init();
	//gecko_bgapi_class_mesh_health_server_init();
	//gecko_bgapi_class_mesh_test_init();
	gecko_bgapi_class_mesh_lpn_init();
	//gecko_bgapi_class_mesh_friend_init();
}


/**
 * See main function list in soc-btmesh-switch project file
 */
void gecko_bgapi_classes_init_client_lpn(void)
{
	gecko_bgapi_class_dfu_init();
	gecko_bgapi_class_system_init();
	gecko_bgapi_class_le_gap_init();
	gecko_bgapi_class_le_connection_init();
	//gecko_bgapi_class_gatt_init();
	gecko_bgapi_class_gatt_server_init();
	gecko_bgapi_class_hardware_init();
	gecko_bgapi_class_flash_init();
	gecko_bgapi_class_test_init();
	//gecko_bgapi_class_sm_init();
	//mesh_native_bgapi_init();
	gecko_bgapi_class_mesh_node_init();
	//gecko_bgapi_class_mesh_prov_init();
	gecko_bgapi_class_mesh_proxy_init();
	gecko_bgapi_class_mesh_proxy_server_init();
	//gecko_bgapi_class_mesh_proxy_client_init();
	gecko_bgapi_class_mesh_generic_client_init();
	//gecko_bgapi_class_mesh_generic_server_init();
	//gecko_bgapi_class_mesh_vendor_model_init();
	//gecko_bgapi_class_mesh_health_client_init();
	//gecko_bgapi_class_mesh_health_server_init();
	//gecko_bgapi_class_mesh_test_init();
	gecko_bgapi_class_mesh_lpn_init();
	//gecko_bgapi_class_mesh_friend_init();

}
void gecko_main_init()
{
  // Initialize device
  initMcu();
  // Initialize board
  initBoard();
  // Initialize application
  initApp();

  // Minimize advertisement latency by allowing the advertiser to always
  // interrupt the scanner.
  linklayer_priorities.scan_max = linklayer_priorities.adv_min + 1;

  gecko_stack_init(&config);

  if( DeviceUsesClientModel() ) {
	  gecko_bgapi_classes_init_client_lpn();
  } else {
	  gecko_bgapi_classes_init_server_friend();
  }

  // Initialize coexistence interface. Parameters are taken from HAL config.
  gecko_initCoexHAL();

}
/******************************************************
 * Func name:   set_device_name
 * Description: Gets the address of the node in the mesh
 * parameter : bluetooth address
 * ***************************************************/
static void set_device_name(bd_addr *pAddr)
{
  char name[20];
  uint16 res;

#if DEVICE_IS_ONOFF_PUBLISHER
  // create unique device name using the last two bytes of the Bluetooth address
  sprintf(name, "23P:02x%02x", pAddr->addr[1], pAddr->addr[0]);

#else
  // create unique device name using the last two bytes of the Bluetooth address
  sprintf(name, "23S:02x%02x", pAddr->addr[1], pAddr->addr[0]);

#endif
  LOG_INFO("Device name: '%s'\n", name);
  displayPrintf(DISPLAY_ROW_CLIENTADDR,"Node: %s",name);


  // write device name to the GATT database
  res = gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name, 0, strlen(name), (uint8 *)name)->result;
  if (res)
  {
	  LOG_INFO("gecko_cmd_gatt_server_write_attribute_value() failed, code %x\n", res);
  }
}

void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt)
{
	switch (evt_id)
	{
    	case gecko_evt_system_boot_id:
    		/*Initialize Mesh stack in Node operation mode, wait for initialized event*/
    		LOG_INFO("Entered boot id\n");

    		/*Factory reset*/
    		if (GPIO_PinInGet(BSP_BUTTON0_PORT, BSP_BUTTON0_PIN) == 0 || GPIO_PinInGet(BSP_BUTTON1_PORT, BSP_BUTTON1_PIN) == 0)
    		{
    			LOG_INFO("factory reset\n");
    			displayPrintf(DISPLAY_ROW_ACTION, "Factory reset");
    			gecko_cmd_flash_ps_erase_all();

    			/*trigger delay for 2secs and do a reboot*/
    			gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_RESTART, ONE_SHOT);

    		}

    		else
    		{


    			struct gecko_msg_system_get_bt_address_rsp_t *pAddr = gecko_cmd_system_get_bt_address();

    			displayPrintf(DISPLAY_ROW_BTADDR,"BT: %x:%x:%x:%x:%x:%x",pAddr->address.addr[5],\
    					pAddr->address.addr[4],pAddr->address.addr[3],pAddr->address.addr[2],\
						pAddr->address.addr[1], pAddr->address.addr[0]);

    			set_device_name(&pAddr->address);

    			// Initialize Mesh stack in Node operation mode, it will generate initialized event
    			gecko_cmd_mesh_node_init();

    		}

    		break;

    	case gecko_evt_le_connection_opened_id:
    		displayPrintf(DISPLAY_ROW_CONNECTION, "CONNECTED");

    	case gecko_evt_mesh_node_initialized_id:
    		LOG_INFO("entered gecko_evt_mesh_node_initialized_id\n");
    		if (!evt->data.evt_mesh_node_initialized.provisioned)
    		{
    			// The Node is now initialized, start unprovisioned Beaconing using PB-ADV and PB-GATT Bearers
    			gecko_cmd_mesh_node_start_unprov_beaconing(0x3);
    			LOG_INFO("Node unprovisioned id\n");
        		displayPrintf(DISPLAY_ROW_ACTION, "Node UnProvisioned");

    		}

    		//enters if node is provisioned
    		if(evt->data.evt_mesh_node_initialized.provisioned )
    		{
    			displayPrintf(DISPLAY_ROW_ACTION, "Node Provisioned");

    			if(DeviceUsesClientModel())
    			{
    				LOG_INFO("Enabled interrupt for PB0\n");
    				enable_PB0_interrupt();
    			}
    		}

    		//enters if node is provisioned and is client model
    		if((evt->data.evt_mesh_node_initialized.provisioned) && (DeviceUsesClientModel()))
    		{
    			displayPrintf(DISPLAY_ROW_NAME, "Publisher");
    			LOG_INFO("client init success\n");
    			uint16_t error = gecko_cmd_mesh_generic_client_init()->result;

    			if(error)
    			{
    				LOG_INFO("Client Init Failed\n");
    				LOG_INFO("Error code: %x\n", error);
    	       }
    		}

    		//enters if node is provisioned and is server model
    		if((evt->data.evt_mesh_node_initialized.provisioned) && (DeviceUsesServerModel()))
    		{
    			displayPrintf(DISPLAY_ROW_NAME, "Subscriber");

    			LOG_INFO("Server init success\n");

    			uint16_t error = gecko_cmd_mesh_generic_server_init()->result;
    			if(error)
    	       {
    				LOG_INFO("Server Init Failed\n");
    				LOG_INFO("Error code: %x\n", error);
    	       }
    		}
    		if(evt->data.evt_mesh_node_initialized.provisioned && DeviceIsOnOffPublisher())
    		{
    			int error = mesh_lib_init(malloc,free,8);
    			if(error)
    			{
    				LOG_INFO("mesh_lib_init publisher Init Failed\n");
    				LOG_INFO("Error code: %x\n", error);
    			}
    		}

    		if(evt->data.evt_mesh_node_initialized.provisioned && DeviceIsOnOffSubscriber())
    		{
    			int error = mesh_lib_init(malloc,free,9);
    			if(error)
    			{
    				printf("mesh_lib_init subs Init Failed\n\r");
    			    printf("Error code: %x\n", error);

    			}

    		}

    		break;
    	case gecko_evt_le_connection_closed_id:
    		/* Check if need to boot to dfu mode */
    		if (boot_to_dfu)
    		{
    			/* Enter to DFU OTA mode */
    			gecko_cmd_system_reset(2);
    		}
	    	displayPrintf(DISPLAY_ROW_TEMPVALUE, " ");
	    	displayPrintf(DISPLAY_ROW_ACTION, " ");
	    	displayPrintf(DISPLAY_ROW_CONNECTION, " ");
	    	displayPrintf(DISPLAY_ROW_NAME, " ");



       break;

    	case gecko_evt_gatt_server_user_write_request_id:
    		LOG_INFO("entered gecko_evt_gatt_server_user_write_request_id\n");

    		if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control)
    		{
    			/* Set flag to enter to OTA mode */
    			boot_to_dfu = 1;
    			/* Send response to Write Request */
    			gecko_cmd_gatt_server_send_user_write_response(\
    					evt->data.evt_gatt_server_user_write_request.connection,\
						gattdb_ota_control,\
						bg_err_success);

    			/* Close connection to enter to DFU OTA mode */
    			gecko_cmd_le_connection_close(evt->data.evt_gatt_server_user_write_request.connection);
    		}
    		break;

    	case gecko_evt_mesh_node_provisioning_started_id:
    		LOG_INFO("Started provisioning\r\n");
    		displayPrintf(DISPLAY_ROW_ACTION, "Provisioning");

    		break;

    	case gecko_evt_mesh_node_provisioning_failed_id:
    		LOG_INFO("provisioning failed, code %x\r\n", evt->data.evt_mesh_node_provisioning_failed.result);
    		displayPrintf(DISPLAY_ROW_ACTION, "Provisioning failed");

    		/* start a one-shot timer that will trigger soft reset after with a delay of 2 secs */
    		gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_RESTART, ONE_SHOT);

    		break;

    	case gecko_evt_mesh_node_provisioned_id:
    		LOG_INFO("node provisioned, address=%x\r\n", evt->data.evt_mesh_node_provisioned.address);
    		displayPrintf(DISPLAY_ROW_ACTION, "Provisioned");

    		/*do a reboot after provisioning the node with a delay of 2 secs */
    		gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_RESTART, ONE_SHOT);

    		break;

    		/*enters this event if the anything is published*/
    	case gecko_evt_mesh_generic_server_client_request_id:
    		LOG_INFO("evt gecko_evt_mesh_generic_server_client_request_id : button state received\n");

    	    uint8_t press = 0;
    	    press = evt->data.evt_mesh_generic_server_client_request.parameters.data[0];
    	    LOG_INFO("Button state %d\n",press);

    	    if(press == 0)
    		{
    	    	displayPrintf(DISPLAY_ROW_TEMPVALUE, "Button Pressed");
    		}
    	    else
    	    {
    	    	displayPrintf(DISPLAY_ROW_TEMPVALUE, "Button Released");
    	    }


    		break;

    	case gecko_evt_mesh_generic_server_state_changed_id:
    	    LOG_INFO("evt gecko_evt_mesh_generic_server_state_changed_id\n");
    	    if(DeviceUsesServerModel())
    	    	mesh_lib_generic_server_event_handler(evt);

    	    break;

    	case gecko_evt_hardware_soft_timer_id:
    	      switch (evt->data.evt_hardware_soft_timer.handle)
    	      {
    	        case TIMER_ID_RESTART:
    	          // restart timer expires, reset the device
    	          gecko_cmd_system_reset(0);
    	          break;

    	      }

    	 //Factory reset when requested by provisioner
    	case gecko_evt_mesh_node_reset_id:

    		gecko_cmd_flash_ps_erase_all();
    		//do a reboot after provisioning the node
    		gecko_cmd_hardware_set_soft_timer(32768*2, TIMER_ID_RESTART, ONE_SHOT);
    		break;

    	case gecko_evt_system_external_signal_id:

    		//checks if the event triggered due to change 1 sec interrupt
    		if((evt->data.evt_system_external_signal.extsignals) & (FLAG_PERIODIC_1 == TRUE))
    		{
    			CORE_DECLARE_IRQ_STATE;

    			CORE_ENTER_CRITICAL();

    			FLAG_PERIODIC_1 = FALSE;

    			CORE_EXIT_CRITICAL();

    			displayUpdate();

    		}


    		//checks if it has triggered due to change in button state
    		if((DeviceIsOnOffPublisher())&&(((evt->data.evt_system_external_signal.extsignals) & (PRESSED)) || ((evt->data.evt_system_external_signal.extsignals) & (RELEASED))))
    		{
    			LOG_INFO("pressed button\n");
    			CORE_DECLARE_IRQ_STATE;

    			CORE_ENTER_CRITICAL();

    			 if(button_position == PRESSED)
    				 generic_level_update_publish(1);
    			 else
    				 generic_level_update_publish(0);

    			 //clear the button variable
    			 button_position = CLEAR;

    			CORE_EXIT_CRITICAL();

    		}

    		break;

	}
}


/******************************************************
 * Publish function- on_off model
 * ***************************************************/
void generic_level_update_publish(bool data)
{
	//increment the transmission id
	trid +=1;

	struct mesh_generic_request publish_data;
	publish_data.kind = mesh_generic_request_on_off;
	publish_data.on_off = data;

	//publishing the button service
	int resp = mesh_lib_generic_client_publish(MESH_GENERIC_ON_OFF_CLIENT_MODEL_ID,_my_index,trid,&publish_data, false,false,false);

	if(resp)
		LOG_INFO("publish error %x\n",resp);
	else
		LOG_INFO("Client publish successful\n");

}

