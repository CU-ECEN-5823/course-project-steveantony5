/*****************************************************************
 *              			Includes
 *****************************************************************/
#include "main.h"

/*****************************************************************
 *              			Mode Selection
 *This selection is not used in Assignment 5
 *Hence it is set to 0 for Clock selection purpose in LETIMER0
 *****************************************************************/
/** Status value for EM0.
sleepEM0 = 0,

 Status value for EM1.
sleepEM1 = 1,

 Status value for EM2.
sleepEM2 = 2,

 Status value for EM3.
sleepEM3 = 3,

 Status value for EM4.
sleepEM4 = 4 */

const int sleep_mode_select = sleepEM0;

/**********************************
 *    GLOBALS
 ***********************************/
int8_t FLAG_PERIODIC_3;
int8_t FLAG_PERIODIC_1;

int8_t FLAG_Delay_80;
int8_t FLAG_DONE;

//for keeping track of number of available doctors
int32_t doctor_count =0;

//error response
uint16_t Response;

//for unicasting to lpn
uint16_t publisher_address;

int local_button_press = 0;

uint8 request_count;


/// transaction identifier
uint8 trid = 0;

/**********************************
 *    Main
 ***********************************/
int main(void)
{

  // Initialize mesh api's
  mesh_init();

  //Clock Management Unit initialize
  cmu_init();

  //Configuring LETIMER0
  letimer_config();

  //initiating display
  displayInit();

  gpioInit();

  //initiating buttons PB0 and PB1
  button_init();

  //enable touch sensors
  touch_sensor_init();

  //enable touch sensor interrupts
  enable_touch_interrupt();

  //initiating logger
  logInit();

  //initiate pins for buzzer
  initiate_alarm();

  //enable interrupt for PB1 button
  //This is used to stop the buzzer on FN as well as send signal to lpn to stop the lpn buzzer
  enable_PB1_interrupt();

  /* Infinite loop */
  while (1) {
	struct gecko_cmd_packet *evt = gecko_wait_event();
	bool pass = mesh_bgapi_listener(evt);
	if (pass) {
		handle_gecko_event_scheduler(BGLIB_MSG_ID(evt->header), evt);


	}
  };
}


/**********************************
 *    Mesh scheduler
 ***********************************/
void handle_gecko_event_scheduler(uint32_t evt_id, struct gecko_cmd_packet *evt)
{
		//handle_gecko_event(BGLIB_MSG_ID(evt->header), evt);
		if (evt == NULL)
		{
			return;
		}
		switch (evt_id) {
			case gecko_evt_system_boot_id:

				LOG_INFO("Entered boot id\n");

				/*Factory reset when PB0 is pressed on reset*/
				if (GPIO_PinInGet(BSP_BUTTON0_PORT, BSP_BUTTON0_PIN) == 0 )
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


					case NONE:

						break;

				}

				break;

				case gecko_evt_mesh_node_initialized_id:
				 LOG_INFO("In gecko_evt_mesh_node_initialized_id event\n");

				 Response = gecko_cmd_mesh_generic_client_init()->result;
				if(Response)
				{
					LOG_INFO("Error code: %x\n", Response);
				}

				struct gecko_msg_mesh_node_initialized_evt_t *pData = (struct gecko_msg_mesh_node_initialized_evt_t *)&(evt->data);


				if ((pData->provisioned))
				{
					  //Initialize Friend functionality
					LOG_INFO("Friend mode initialization\r\n");

					//initiate node as friend
					friend_node_init();
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

				//initiate node as friend
				friend_node_init();

				break;

			case gecko_evt_mesh_node_provisioning_failed_id:
				LOG_INFO("Provisioning failed\n");
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

				//enters when touch sensors are touched
				if((((evt->data.evt_system_external_signal.extsignals) & (PRESSED)) || ((evt->data.evt_system_external_signal.extsignals) & (RELEASED))))
				{
					LOG_INFO("Pressed touch\n");
					CORE_DECLARE_IRQ_STATE;

					CORE_ENTER_CRITICAL();

					//For Doctor-in touch sensor
					 if(touch_1_status == PRESSED)
					 {
						 LOG_INFO("doctor entered\n");
						 doctor_count +=1;
						 NVIC_DisableIRQ(GPIO_EVEN_IRQn);
						 gecko_cmd_hardware_set_soft_timer(4 * 32768, ENABLE_TOUCH_1, 1);

					 }

					 //for doctor-out touch sensor
					 if(touch_2_status == PRESSED)
					 {
						 LOG_INFO("doctor left\n");

						 //ensures not to increment below 0
						 if(doctor_count != 0)
							 doctor_count -=1;

						 NVIC_DisableIRQ(GPIO_ODD_IRQn);
						 gecko_cmd_hardware_set_soft_timer(4 * 32768, ENABLE_TOUCH_2, 1);
					 }


					 //clear the button variable
					 touch_1_status = CLEAR;
					 touch_2_status = CLEAR;

					 LOG_INFO("Doctor count %d\n",doctor_count);
					 displayPrintf(DISPLAY_ROW_PASSKEY, "No of doctors %d",doctor_count);


					 CORE_EXIT_CRITICAL();

				}

				//Enters when PB1 is pressed
				//To stop the buzzer on friend node as well as on lpn
				if((evt->data.evt_system_external_signal.extsignals) & (FLAG_STOP_LOCAL_BUFFER))
				{
					CORE_DECLARE_IRQ_STATE;

					CORE_ENTER_CRITICAL();

					//turn off buzzer
					trigger_alarm_off();

					LOG_INFO("Stopping local buffer ringing on Fall detection LPN\n");

					FLAG_STOP_LOCAL_BUFFER = FALSE;

					//sends signal to stop buzzer on FALL_DETECTION_NODE
					generic_level_client_set(FALL_DETECTION_NODE);

					CORE_EXIT_CRITICAL();

				}
				break;

			case gecko_evt_mesh_generic_client_server_status_id:
			    {

			    	publisher_address = evt->data.evt_mesh_generic_client_server_status.server_address;

			    	/*Fall Detection*/
			    	if((publisher_address) == FALL_DETECTION_NODE)
			    	{
			    		LOG_INFO("Received data from FALL_DETECTION_NODE\n");
			    		uint16_t fall_or_tap;

			    		fall_or_tap = ((evt->data.evt_mesh_generic_client_server_status.parameters.data[1])<<8)\
							| (evt->data.evt_mesh_generic_client_server_status.parameters.data[0]);


			    		//When fall is detected
			    		if(fall_or_tap == FALL_DETECTED)
			    		{
			    			LOG_INFO("Patient fell down: Trigger alarm\n");
			    			displayPrintf(DISPLAY_ROW_TEMPVALUE, "Pat Fell down");


			    			trigger_alarm();
			    		}

			    		//When tap is detected
			    		if (fall_or_tap == TAP_DETECTED)
			    		{
			    			LOG_INFO("Patient Tapped: calling nurse\n");
			    			displayPrintf(DISPLAY_ROW_TEMPVALUE, "Calling nurse");


			    			trigger_alarm();
			    		}
			    		fall_or_tap = 0;
			    	}

			    	/*Heart beat node*/
					if((publisher_address) == HEART_BEAT_NODE)
					{
						LOG_INFO("Received data from HEART_BEAT_NODE\n");
						uint16_t heartbeat;

						heartbeat = ((evt->data.evt_mesh_generic_client_server_status.parameters.data[1])<<8)\
							| (evt->data.evt_mesh_generic_client_server_status.parameters.data[0]);


						LOG_INFO("Heart beat : %d\n",heartbeat);

						if((heartbeat > HEARTBEAT_NORMAL_VALUE_MAX) || (heartbeat < HEARTBEAT_NORMAL_VALUE_MIN))
						{
							LOG_INFO("Patient heart beat abnormal: Trigger alarm\n");
							generic_level_client_set(HEART_BEAT_NODE);
						}
					}

					/*People counter node*/
					if((publisher_address) == PEOPLE_COUNT_NODE)
					{
						LOG_INFO("Received data from PEOPLE_COUNT_NODE\n");
						uint16_t people_count;

						people_count = ((evt->data.evt_mesh_generic_client_server_status.parameters.data[1])<<8)\
							| (evt->data.evt_mesh_generic_client_server_status.parameters.data[0]);


						LOG_INFO("Heart beat : %d\n",people_count);

						if((people_count >= PEOPLE_COUNT_MAX))
						{
							LOG_INFO("Many visitors are in the room, increase spray frequency\n");
							generic_level_client_set(PEOPLE_COUNT_NODE);
						}
					}

			    }
		}

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

/******************************************************
 * Func name:   friend_node_init
 * Description: function to initiate the node as friend
 * parameter : none
 * ***************************************************/
void friend_node_init()
{

	mesh_lib_init(malloc, free, 10);

	//Initialize Friend functionality
	LOG_INFO("Friend mode initialization\n");
	Response = gecko_cmd_mesh_friend_init()->result;
	if (Response)
	{
		LOG_INFO("Friend init failed 0x%x\n", Response);
	}
}

/******************************************************
 * Func name:   trigger_alarm
 * Description: function to turn on the buzzer
 * parameter : none
 * ***************************************************/
void trigger_alarm()
{
	trigger_alarm_on();
}
