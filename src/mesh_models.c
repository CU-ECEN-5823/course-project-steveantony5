
/*
 * mesh_models.c
 *
 *  Created on: April 27, 2019
 *      Author: Steve Antony
 * Description: This file contains functions to publish in on-off model and
 *              subscribe in level model
 */

/*****************************************************************
 *              			INCLUDES
 *****************************************************************/
#include "mesh_models.h"


/*****************************************************************
 *              			Globals
 *****************************************************************/
uint16_t Element_Index = 0, Appkey_Index;

/*****************************************************************
 *              			Macros
 *****************************************************************/
/*Model specific state type- tells the kind of state to retrive*/
#define Generic_Level_State_Type		(2)


/*****************************************************************
 *            Function to subscribe using level model
 *****************************************************************/
void generic_level_client_get(uint16_t lpn_addr)
{

	Response = mesh_lib_generic_client_get(
				MESH_GENERIC_LEVEL_CLIENT_MODEL_ID,
				Element_Index,
				lpn_addr,
				Appkey_Index,
				Generic_Level_State_Type
			  );


   if (Response)
   {
	 LOG_INFO("gecko_cmd_mesh_generic_client_get failed,code %d\r\n", Response);
   }

}

/*****************************************************************
 *            Function to publish using on-off model
 *****************************************************************/
void generic_onoff_client_set(uint16_t lpn_addr,bool data)
{
	//unicast to FALL_DETECTION_NODE
	if(lpn_addr == FALL_DETECTION_NODE)
	{
		LOG_INFO("Publising on-off data to FALL_DETECTION_NODE\n");

		static uint16_t trid; //transaction id

		errorcode_t	response;
		trid += 1;
		struct mesh_generic_request	publish_data;
		publish_data.kind = mesh_generic_request_on_off;

		publish_data.on_off = data;
		response = mesh_lib_generic_client_set(
				MESH_GENERIC_ON_OFF_CLIENT_MODEL_ID,
				Element_Index,
				lpn_addr,
				Appkey_Index,
				trid,
				&publish_data,
				false,
				false,
				true);

		  if (response)
		  {
			LOG_INFO("mesh_lib_generic_client_set in FALL_DETECTION_NODE failed,code %d\r\n", response);
		  }
	}

	//unicast to HEART_BEAT_NODE
	if(lpn_addr == HEART_BEAT_NODE)
	{
		LOG_INFO("Publising on-off data to HEART_BEAT_NODE\n");
		static uint16_t trid;

		errorcode_t	response;
		trid += 1;
		struct mesh_generic_request	publish_data;
		publish_data.kind = mesh_generic_request_on_off;

		publish_data.on_off = data;
		response = mesh_lib_generic_client_set(
				MESH_GENERIC_ON_OFF_CLIENT_MODEL_ID,
				Element_Index,
				lpn_addr,
				Appkey_Index,
				trid,
				&publish_data,
				false,
				false,
				true);

		  if (response)
		  {

			LOG_INFO("mesh_lib_generic_client_set in HEART_BEAT_NODE failed,code %d\r\n", response);
		  }
	}

	//unicast to PEOPLE_COUNT_NODE
	if(lpn_addr == PEOPLE_COUNT_NODE)
	{
		LOG_INFO("Publising data %d to PEOPLE_COUNT_NODE\n",data);
		static uint16_t trid;

		errorcode_t	response;
		trid += 1;
		struct mesh_generic_request	publish_data;
		publish_data.kind = mesh_generic_request_on_off;

		publish_data.on_off = data;
		response = mesh_lib_generic_client_set(
				MESH_GENERIC_ON_OFF_CLIENT_MODEL_ID,
				Element_Index,
				lpn_addr,
				Appkey_Index,
				trid,
				&publish_data,
				false,
				false,
				true);

		  if (response)
		  {

			LOG_INFO("mesh_lib_generic_client_set in PEOPLE_COUNT_NODE failed,code %d\r\n", response);
		  }
	}
}

