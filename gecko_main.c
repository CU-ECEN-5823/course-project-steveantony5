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
#define ENABLE_TOUCH_1        (100)
#define ENABLE_TOUCH_2        (101)

/**********************************************
 *                GLOBALS
 * ********************************************/
int local_button_press = 0;

uint8 request_count;


/// transaction identifier
uint8 trid = 0;

/// current position of the button
extern button_flag  button_position ;

extern button_flag touch_1_status;

/// current position of the switch
static uint8 switch_pos = 0;
int32_t doctor_count =0;

/// For indexing elements of the node (this example has only one element)
static uint16 _elem_index = 0xffff;

static uint16 _primary_elem_index = 0xffff; /* For indexing elements of the node */

static uint8_t connection_handle;

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
void set_device_name(bd_addr *pAddr);
void send_onoff_request(int retrans);
static void onoff_request(uint16_t model_id,
                          uint16_t element_index,
                          uint16_t client_addr,
                          uint16_t server_addr,
                          uint16_t appkey_index,
                          const struct mesh_generic_request *request,
                          uint32_t transition_ms,
                          uint16_t delay_ms,
                          uint8_t request_flags);
static void onoff_change(uint16_t model_id,
                         uint16_t element_index,
                         const struct mesh_generic_state *current,
                         const struct mesh_generic_state *target,
                         uint32_t remaining_ms);
static errorcode_t onoff_update_and_publish(uint16_t element_index);
static errorcode_t onoff_update(uint16_t element_index);
void init_models();
void handle_button_state(uint8_t button);
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
	//gecko_bgapi_class_mesh_lpn_init();
	gecko_bgapi_class_mesh_friend_init();
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


  gecko_bgapi_classes_init_server_friend();


  // Initialize coexistence interface. Parameters are taken from HAL config.
  gecko_initCoexHAL();

}
/******************************************************
 * Func name:   set_device_name
 * Description: Gets the address of the node in the mesh
 * parameter : bluetooth address
 * ***************************************************/
void set_device_name(bd_addr *pAddr)
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


		//handle_gecko_event(BGLIB_MSG_ID(evt->header), evt);
		if (evt == NULL) {
			return;
		}
		switch (evt_id) {
			case gecko_evt_system_boot_id:

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

			case gecko_evt_hardware_soft_timer_id:

				switch (evt->data.evt_hardware_soft_timer.handle)
				    	      {
				    	        case TIMER_ID_RESTART:
				    	          // restart timer expires, reset the device
				    	          gecko_cmd_system_reset(0);
				    	          break;

				    	        case ENABLE_TOUCH_1:
				    				 NVIC_EnableIRQ(GPIO_EVEN_IRQn);
				    				break;

				    	        case ENABLE_TOUCH_2:
				    	        	 NVIC_EnableIRQ(GPIO_ODD_IRQn);
				    	        	break;


				    	      }

				break;

				case gecko_evt_mesh_node_initialized_id:
									 LOG_INFO("In gecko_evt_mesh_node_initialized_id event\n");
									struct gecko_msg_mesh_node_initialized_evt_t *pData = (struct gecko_msg_mesh_node_initialized_evt_t *)&(evt->data);

									if ((pData->provisioned)) {
										  //Initialize Friend functionality
										LOG_INFO("Friend mode initialization\r\n");
										uint16 res;
										res = gecko_cmd_mesh_friend_init()->result;
										if (res) {
									    LOG_INFO("Friend init failed 0x%x\r\n", res);
										 }

										mesh_lib_init(malloc,free,9);
										init_models();
										onoff_update_and_publish(0);
										gecko_cmd_mesh_generic_server_init();
									}
									else
									{
										gecko_cmd_mesh_node_start_unprov_beaconing(0x3);
									}

									break;

			case gecko_evt_mesh_node_provisioning_started_id:
				LOG_INFO("Provisioning\n");
				displayPrintf(DISPLAY_ROW_ACTION, "Provisioning");
				break;

			case gecko_evt_mesh_node_provisioned_id:
				displayPrintf(DISPLAY_ROW_ACTION, "Provisioned");
				LOG_INFO("Provisioned\n");
				 mesh_lib_init(malloc, free, 10);


				  //Initialize Friend functionality
				 LOG_INFO("Friend mode initialization\r\n");
				  uint16 res;
				  res = gecko_cmd_mesh_friend_init()->result;
				  if (res) {
					  LOG_INFO("Friend init failed 0x%x\r\n", res);
				  }

				break;

			case gecko_evt_mesh_node_provisioning_failed_id:
				displayPrintf(DISPLAY_ROW_ACTION, "Provisioned Failed");
				break;

			case gecko_evt_le_connection_opened_id:
				displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");
				break;

			case gecko_evt_le_connection_closed_id:
				displayPrintf(DISPLAY_ROW_CONNECTION, "");
				break;

			case gecko_evt_mesh_node_reset_id:
				gecko_cmd_flash_ps_erase_all();
				gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_RESTART, 1);

				break;

			case gecko_evt_mesh_generic_server_client_request_id:
				mesh_lib_generic_server_event_handler(evt);
				break;

			case gecko_evt_mesh_generic_server_state_changed_id:
				mesh_lib_generic_server_event_handler(evt);
				break;

			case gecko_evt_mesh_friend_friendship_established_id:
			    	      LOG_INFO("evt gecko_evt_mesh_friend_friendship_established, lpn_address=%x\r\n", evt->data.evt_mesh_friend_friendship_established.lpn_address);
			    	      break;

			    	case gecko_evt_mesh_friend_friendship_terminated_id:
			    		LOG_INFO("evt gecko_evt_mesh_friend_friendship_terminated, reason=%x\r\n", evt->data.evt_mesh_friend_friendship_terminated.reason);
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
				    		if((((evt->data.evt_system_external_signal.extsignals) & (PRESSED)) || ((evt->data.evt_system_external_signal.extsignals) & (RELEASED))))
				    		{
				    			LOG_INFO("Pressed touch\n");
				    			CORE_DECLARE_IRQ_STATE;

				    			CORE_ENTER_CRITICAL();

				    			 if(touch_1_status == PRESSED)
				    			 {
				    				 LOG_INFO("doctor entered\n");
				    				 doctor_count +=1;
				    				 NVIC_DisableIRQ(GPIO_EVEN_IRQn);
				    				 gecko_cmd_hardware_set_soft_timer(4 * 32768, ENABLE_TOUCH_1, 1);

				    			 }

				    			 if(touch_2_status == PRESSED)
				    			 {
				    			 	 LOG_INFO("doctor left\n");
				    			 	 doctor_count -=1;
				    			 	 NVIC_DisableIRQ(GPIO_ODD_IRQn);
				    			 	 gecko_cmd_hardware_set_soft_timer(4 * 32768, ENABLE_TOUCH_2, 1);
 				    			 }


				    			 //clear the button variable
				    			 touch_1_status = CLEAR;
				    			 touch_2_status = CLEAR;

				    			 LOG_INFO("Doctor count %d\n",doctor_count);

				    			CORE_EXIT_CRITICAL();

				    		}
				 break;
		}




	}

	void send_onoff_request(int retrans)
	{
	  uint16 resp;
	  uint16 delay;
	  struct mesh_generic_request req;
	  const uint32 transtime = 0; /* using zero transition time by default */
	  _elem_index = 0;

	  req.kind = mesh_generic_request_on_off;
	  req.on_off = switch_pos ? 0 : 1;

	  // increment transaction ID for each request, unless it's a retransmission
	  if (retrans == 0) {
	    trid++;
	  }

	  /* delay for the request is calculated so that the last request will have a zero delay and each
	   * of the previous request have delay that increases in 50 ms steps. For example, when using three
	   * on/off requests per button press the delays are set as 100, 50, 0 ms
	   */
	  //delay = (request_count - 1) * 50;

	  resp = mesh_lib_generic_client_publish(
	    MESH_GENERIC_ON_OFF_CLIENT_MODEL_ID,
	    _elem_index,
	    trid,
	    &req,
	    transtime,   // transition time in ms
	    delay,
	    0     // flags
	    );

	  if (resp) {
		 LOG_INFO("gecko_cmd_mesh_generic_client_publish failed,code %x\r\n", resp);
	  } else {
		 LOG_INFO("request sent, trid = %u \r\n", trid);
	  }

	  /* keep track of how many requests has been sent */
	  if (request_count > 0) {
	    request_count--;
	  }
	}

	static void onoff_request(uint16_t model_id,
	                          uint16_t element_index,
	                          uint16_t client_addr,
	                          uint16_t server_addr,
	                          uint16_t appkey_index,
	                          const struct mesh_generic_request *request,
	                          uint32_t transition_ms,
	                          uint16_t delay_ms,
	                          uint8_t request_flags)
	{
		LOG_INFO("request->on_off = %d\n\r",request->on_off);

		if (request->on_off == 0) {
			//LOG_INFO("");
			displayPrintf(DISPLAY_ROW_ACTION, "PB0_BUTTON_RELEASED");
		} else {
			displayPrintf(DISPLAY_ROW_ACTION, "PB0_BUTTON_PRESSED");
		}
		onoff_update_and_publish(element_index);
	}

	static void onoff_change(uint16_t model_id,
	                         uint16_t element_index,
	                         const struct mesh_generic_state *current,
	                         const struct mesh_generic_state *target,
	                         uint32_t remaining_ms)
	{

	}

	static errorcode_t onoff_update(uint16_t element_index)
	{
	  struct mesh_generic_state current, target;

	  current.kind = mesh_generic_state_on_off;
	  current.on_off.on = 0;

	  target.kind = mesh_generic_state_on_off;
	  target.on_off.on = 1;

	  //displayPrintf(DISPLAY_ROW_ACTION, "onoff_update");

	  return mesh_lib_generic_server_update(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
	                                        element_index,
	                                        &current,
	                                        &target,
	                                        0);
	}

	static errorcode_t onoff_update_and_publish(uint16_t element_index)
	{
		onoff_update(element_index);

		//displayPrintf(DISPLAY_ROW_ACTION, "onoff_update_publish");

		mesh_lib_generic_server_publish(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
				element_index,
				mesh_generic_state_on_off);
	}

	void init_models()
	{
		mesh_lib_generic_server_register_handler(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID, 0, onoff_request, onoff_change);
	}

	void handle_button_state(uint8_t button)
	{
		if (button == 0) {
			displayPrintf(DISPLAY_ROW_ACTION, "PB0_BUTTON_PRESSED");
			switch_pos = 0;
		}
		if (button == 1) {
			displayPrintf(DISPLAY_ROW_ACTION, "PB0_BUTTON_RELEASED");
			switch_pos = 1;
		}

		send_onoff_request(0);
	}
