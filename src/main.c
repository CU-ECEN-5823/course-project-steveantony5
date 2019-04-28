/*
 * main.c
 *
 *  Created on: April 27, 2019
 *      Author: Steve Antony
 */

/*****************************************************************
 *              			Includes
 *****************************************************************/
#include "main.h"

/*****************************************************************
 *            Mode Selection for LETIMER clock selection
 *      sleepEM3 mode is chosen such that ULFRCO_FREQ is used
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

const int sleep_mode_select = sleepEM3;

/**********************************
 *    GLOBALS
 ***********************************/
//for logger count and updating the LCD display
int8_t FLAG_PERIODIC_1;

//for keeping track of number of available doctors
int32_t doctor_count =0;

//error response
uint16_t Response;

//for unicasting to lpn
uint16_t publisher_address;

//For sending spray frequncy to PEOPLE COUNT NODE
uint32_t spray_inc_frequency = 0;

//to store the alarm state such that it can remain in that state when rebooted
bool alarm_on;

/// transaction identifier
uint8 trid = 0;

//store the number of people
uint16_t people_count = 0;

//store the previous people count
uint16_t previous_people_count = 0;

//track the total number of LPNs connected to friend node
int8_t LPN_count = 0;

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

  //initiating buttons PB0 and PB1 to send signals to LPN
  button_init();

  //enable touch sensors for doctor count
  touch_sensor_init();

  //enable touch sensor interrupts
  enable_touch_interrupt();

  //initiating logger
  logInit();

  //initiate pins for buzzer
  initiate_alarm();

  //enable button interrupts
  //This is used to stop the buzzer on FN as well as send signal to lpns
  enable_button_interrupt();



  /* Infinite loop */
  while (true) {

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
		switch (evt_id) {
			case gecko_evt_system_boot_id:

				LOG_INFO("Entered boot id\n");

				/*load doctor count from Flash*/
				ps_load_data(DOCTOR_COUNT_PS_ID, &doctor_count, sizeof(doctor_count));

				/*Display the number of doctors*/
				displayPrintf(DISPLAY_ROW_PASSKEY, "No of doctors %d",doctor_count);
				if(doctor_count == 0)
				{
					trigger_alarm_on();
				}

				/*load alarm state from Flash*/
				ps_load_data(ALARM_STATE_PS_ID,&alarm_on,sizeof(alarm_on));
				if(alarm_on == 1)
				{
					trigger_alarm_on();
				}

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

					displayPrintf(DISPLAY_ROW_NAME, "Friend Node");

#if MESH
					// Initialize Mesh stack in Node operation mode, it will generate initialized event
					gecko_cmd_mesh_node_init();
#endif

				}

				break;

			case gecko_evt_hardware_soft_timer_id:

				switch (evt->data.evt_hardware_soft_timer.handle)
				  {
					case TIMER_ID_RESTART:
					  // restart timer expires, reset the device
					  gecko_cmd_system_reset(0);
					  break;

					//enable EVEN interrupts
					case ENABLE_TOUCH_1:
						 NVIC_EnableIRQ(GPIO_EVEN_IRQn);
						break;

					//enable ODD interrupts
					case ENABLE_TOUCH_2:
						 NVIC_EnableIRQ(GPIO_ODD_IRQn);
						break;

				}

				break;

				case gecko_evt_mesh_node_initialized_id:
					LOG_DEBUG("In gecko_evt_mesh_node_initialized_id event\n");

#if MESH
				 //initiating the friend node as client
				 Response = gecko_cmd_mesh_generic_client_init()->result;
				if(Response)
				{
					LOG_ERROR("Error code: %x\n", Response);
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
#endif
				break;

			case gecko_evt_mesh_node_provisioning_started_id:
				LOG_INFO("Provisioning\n");
				displayPrintf(DISPLAY_ROW_ACTION, "Provisioning");
				break;

			case gecko_evt_mesh_node_provisioned_id:
				displayPrintf(DISPLAY_ROW_ACTION, "Provisioned");
				LOG_INFO("Provisioned\n");

#if MESH
				//initiate node as friend
				friend_node_init();
#endif

				break;

			case gecko_evt_mesh_node_provisioning_failed_id:
				LOG_ERROR("Provisioning failed\n");
				displayPrintf(DISPLAY_ROW_ACTION, "Provisioned Failed");
				break;

			case gecko_evt_le_connection_opened_id:
				displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");
				LOG_DEBUG("gecko_evt_le_connection_opened_id: Opened connection\n");
				break;

			case gecko_evt_le_connection_closed_id:
				displayPrintf(DISPLAY_ROW_CONNECTION, "");
				LOG_DEBUG("gecko_evt_le_connection_closed_id: Closed connection\n");
				break;

			case gecko_evt_mesh_node_reset_id:
				gecko_cmd_flash_ps_erase_all();
				gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_RESTART, 1);
				LOG_DEBUG("Entered gecko_evt_mesh_node_reset_id\n");

				break;

			case gecko_evt_mesh_generic_server_client_request_id:
				mesh_lib_generic_server_event_handler(evt);
				LOG_DEBUG("Entered gecko_evt_mesh_generic_server_client_request_id\n");
				break;

			case gecko_evt_mesh_generic_server_state_changed_id:
				mesh_lib_generic_server_event_handler(evt);
				LOG_DEBUG("Entered gecko_evt_mesh_generic_server_state_changed_id\n");
				break;

			//Triggered when Friendship is established
			case gecko_evt_mesh_friend_friendship_established_id:
				LOG_INFO("evt gecko_evt_mesh_friend_friendship_established, lpn_address=%x\r\n", evt->data.evt_mesh_friend_friendship_established.lpn_address);

				//Increment the LPN friendship count after establishing friendship
				LPN_count++;
				displayPrintf(DISPLAY_ROW_BTADDR2, "No of LPNs %d",LPN_count);

				break;

				//Triggered when Friendship is terminated
			case gecko_evt_mesh_friend_friendship_terminated_id:
				LOG_INFO("evt gecko_evt_mesh_friend_friendship_terminated, reason=%x\r\n", evt->data.evt_mesh_friend_friendship_terminated.reason);
				LPN_count--;
				displayPrintf(DISPLAY_ROW_BTADDR2, "No of LPNs %d",LPN_count);

				break;

			case gecko_evt_system_external_signal_id:
				//checks if the event triggered due to change 1 sec interrupt
				if((evt->data.evt_system_external_signal.extsignals) & (FLAG_PERIODIC_1 == true))
				{
					CORE_DECLARE_IRQ_STATE;

					CORE_ENTER_CRITICAL();

					FLAG_PERIODIC_1 = false;

					CORE_EXIT_CRITICAL();

					displayUpdate();

				}

				//enters when touch sensors are touched
				if((((evt->data.evt_system_external_signal.extsignals) & (PRESSED)) || ((evt->data.evt_system_external_signal.extsignals) & (RELEASED))))
				{

					//For Doctor-in touch sensor
					 if(touch_1_status == PRESSED)
					 {
						 LOG_INFO("doctor entered\n");

						 //increment doctor count by 1
						 doctor_count +=1;

						 //disable the touch sensor to avoid multiple triggers due to single touch
						 NVIC_DisableIRQ(GPIO_EVEN_IRQn);

						 //enable the touch sensor 1 interrupt after 3 seconds
						 gecko_cmd_hardware_set_soft_timer(3 * 32768, ENABLE_TOUCH_1, ONE_SHOT);

						 LOG_INFO("Doctor count %d\n",doctor_count);
						 displayPrintf(DISPLAY_ROW_PASSKEY, "No of doctors %d",doctor_count);

					 }

					 //for doctor-out touch sensor
					 if(touch_2_status == PRESSED)
					 {
						 LOG_INFO("doctor left\n");

						 //ensures not to decrement below 0
						 if(doctor_count != 0)
							 //decrement doctor count by 1
							 doctor_count -=1;

						 //disable the touch sensor to avoid multiple triggers due to single touch
						 NVIC_DisableIRQ(GPIO_ODD_IRQn);

						 //enable the touch sensor 2 interrupt after 3 seconds
						 gecko_cmd_hardware_set_soft_timer(3 * 32768, ENABLE_TOUCH_2, ONE_SHOT);

						 LOG_INFO("Doctor count %d\n",doctor_count);
						 displayPrintf(DISPLAY_ROW_PASSKEY, "No of doctors %d",doctor_count);
					 }

					 CORE_DECLARE_IRQ_STATE;

					 CORE_ENTER_CRITICAL();
					 //clear the flag variables
					 touch_1_status = CLEAR;
					 touch_2_status = CLEAR;

					 CORE_EXIT_CRITICAL();




					 /*Turn on the alarm if the doctor count is zero*/
					 if(doctor_count == 0)
					 {
						 trigger_alarm_on();
					 }
					 else
					 {
						 trigger_alarm_off();
					 }

					 //Store the doctor count in persistent memory
					 ps_save_data(DOCTOR_COUNT_PS_ID, &doctor_count, sizeof(doctor_count));

				}

				//Enters when PB1 is pressed
				//To stop the buzzer on friend node as well as on lpn
				if((evt->data.evt_system_external_signal.extsignals) & (FLAG_STOP_LOCAL_BUFFER))
				{
					CORE_DECLARE_IRQ_STATE;
					CORE_ENTER_CRITICAL();

					FLAG_STOP_LOCAL_BUFFER = false;

					CORE_EXIT_CRITICAL();

					//turn off buzzer
					trigger_alarm_off();

					LOG_INFO("Stopping local buffer ringing on Fall detection LPN\n");
					displayPrintf(DISPLAY_ROW_TEMPVALUE, " ");

					//sends signal to stop buzzer on FALL_DETECTION_NODE
					generic_onoff_client_set(FALL_DETECTION_NODE, true);


				}

				if((evt->data.evt_system_external_signal.extsignals) & (FLAG_STOP_DEFRIBRILLATION))
				{
					CORE_DECLARE_IRQ_STATE;
					CORE_ENTER_CRITICAL();

					FLAG_STOP_DEFRIBRILLATION = false;

					CORE_EXIT_CRITICAL();

					LOG_INFO("Stopping defibrillator on Heart beat LPN\n");
					displayPrintf(DISPLAY_ROW_CLIENTADDR, " ");

					//sends signal to stop buzzer on FALL_DETECTION_NODE
					generic_onoff_client_set(HEART_BEAT_NODE, true);

					//turn off buzzer
					trigger_alarm_off();

				}
				break;

			case gecko_evt_mesh_generic_client_server_status_id:
			    {

			    	publisher_address = evt->data.evt_mesh_generic_client_server_status.server_address;

			    	/*Fall Detection*/
			    	if((publisher_address) == FALL_DETECTION_NODE)
			    	{
			    		uint16_t fall_or_tap;

			    		fall_or_tap = ((evt->data.evt_mesh_generic_client_server_status.parameters.data[1])<<8)\
							| (evt->data.evt_mesh_generic_client_server_status.parameters.data[0]);


			    		//When fall is detected
			    		if(fall_or_tap == FALL_DETECTED)
			    		{
			    			LOG_INFO("Patient fell down: Trigger alarm\n");
			    			displayPrintf(DISPLAY_ROW_TEMPVALUE, "Patient Fell down");


			    			trigger_alarm_on();
			    		}

			    		//When tap is detected
			    		if (fall_or_tap == TAP_DETECTED)
			    		{
			    			LOG_INFO("Patient Tapped: calling nurse\n");
			    			displayPrintf(DISPLAY_ROW_TEMPVALUE, "Patient Calls nurse");


			    			trigger_alarm_on();
			    		}
			    		fall_or_tap = false;
			    	}

			    	/*Heart beat node*/
					if((publisher_address) == HEART_BEAT_NODE)
					{
						uint16_t heartbeat;

						heartbeat = ((evt->data.evt_mesh_generic_client_server_status.parameters.data[1])<<8)\
							| (evt->data.evt_mesh_generic_client_server_status.parameters.data[0]);

						LOG_INFO("Abnormal Heart beat : %d\n",heartbeat);
						displayPrintf(DISPLAY_ROW_CLIENTADDR, "Patient HB abnormal");

						LOG_INFO("Patient heart beat is abnormal: Trigger alarm\n");
						trigger_alarm_on();

					}

					/*People counter node*/
					if((publisher_address) == PEOPLE_COUNT_NODE)
					{
						LOG_INFO("Received data from PEOPLE_COUNT_NODE\n");


						people_count = ((evt->data.evt_mesh_generic_client_server_status.parameters.data[1])<<8)\
							| (evt->data.evt_mesh_generic_client_server_status.parameters.data[0]);


						LOG_INFO("People count : %d\n",people_count);

						/*send command to increase spray frequency when people count is higher than threshold*/
						if((people_count >= PEOPLE_COUNT_MAX) && (previous_people_count < people_count))
						{
							spray_inc_frequency++;
							LOG_INFO("Many visitors are in the room, increased spray frequency\n");
							generic_onoff_client_set(PEOPLE_COUNT_NODE, true);
							displayPrintf(DISPLAY_ROW_CONNECTION, "Spray freq inc",spray_inc_frequency);
						}

						/*send command to decrease spray frequency when people count is higher than threshold
						 * but lower than previous people count*/
						else if((people_count >= PEOPLE_COUNT_MAX) &&(previous_people_count > people_count))
						{
							if(spray_inc_frequency > 0)
							{
								LOG_INFO("Less visitors are in the room, decreased spray frequency\n");
								spray_inc_frequency--;
								generic_onoff_client_set(PEOPLE_COUNT_NODE, false);
								displayPrintf(DISPLAY_ROW_CONNECTION, "Spray freq dec",spray_inc_frequency);
							}
						}
						else
						{
							displayPrintf(DISPLAY_ROW_CONNECTION,"People within limit");
						}

						LOG_INFO("spray_inc_frequency : %d\n",spray_inc_frequency);

						/*Storing the previous people count*/
						previous_people_count = people_count;
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

  // create unique device name using the last two bytes of the Bluetooth address
  sprintf(name, "23FN:02x%02x", pAddr->addr[1], pAddr->addr[0]);

  LOG_INFO("Device node name: '%s'\n", name);
  displayPrintf(DISPLAY_ROW_CLIENTADDR,"Node: %s",name);


  // write device name to the GATT database
  res = gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name, 0, strlen(name), (uint8 *)name)->result;
  if (res)
  {
	  LOG_ERROR("gecko_cmd_gatt_server_write_attribute_value() failed, code %x\n", res);
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
		LOG_ERROR("Friend init failed 0x%x\n", Response);
	}
}


/******************************************************
 * Func name:   ps_save_data
 * Description: save data to Flash
 * parameter : key     - unique key to store
 *             *pvalue - address of the value to be stored
 *             size    - size of the variable to be stored
 * ***************************************************/
uint16_t ps_save_data(uint16_t key, void *pValue, uint8_t size)
{
	struct gecko_msg_flash_ps_save_rsp_t *pResp;

	pResp = gecko_cmd_flash_ps_save(key, size, pValue);

	return(pResp->result);
}

/******************************************************
 * Func name:   ps_load_data
 * Description: load data from Flash
 * parameter : key     - unique key to store
 *             *pvalue - address of the value to be stored
 *             size    - size of the variable to be stored
 * ***************************************************/
uint16_t ps_load_data(uint16_t key, void *pValue, uint8_t size)
{
	struct gecko_msg_flash_ps_load_rsp_t *pResp;

	pResp = gecko_cmd_flash_ps_load(key);

	if(pResp->result == 0)
	{
		memcpy(pValue, pResp->value.data, pResp->value.len);

		// sanity check: length of data stored in PS key must match the expected value
		if(size != pResp->value.len)
		{
			return(bg_err_unspecified);
		}
	}

	return(pResp->result);
}
