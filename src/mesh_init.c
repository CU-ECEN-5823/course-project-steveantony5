
 /*******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************/

/*****************************************************************
 *              			INCLUDES
 *****************************************************************/
#include "mesh_init.h"


/*****************************************************************
 *              			GLOBALS
 *****************************************************************/
static gecko_bluetooth_ll_priorities linklayer_priorities = GECKO_BLUETOOTH_PRIORITIES_DEFAULT;
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS) + BTMESH_HEAP_SIZE + 1760];


const gecko_configuration_t config_custom =
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


/*****************************************************************
 *          Function to initiate mesh stack for friend node
 *****************************************************************/
void mesh_init()
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

  gecko_stack_init(&config_custom);


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
	  //gecko_bgapi_class_mesh_lpn_init();
	  gecko_bgapi_class_mesh_friend_init();



  // Initialize coexistence interface. Parameters are taken from HAL config.
  gecko_initCoexHAL();

}


