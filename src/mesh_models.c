
 /*******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************/

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
#define Generic_Level_State_Type				2


/*****************************************************************
 *            Function to subscribe
 *****************************************************************/
void generic_level_client_get(uint16_t lpn_addr)
{
	LOG_INFO("Getting publish data from LPN addr %x\n",lpn_addr);

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
 *            Function for Unicast to lpns
 *                  Uses level model
 *****************************************************************/
void generic_level_client_set(uint16_t lpn_addr)
{
	//unicast to FALL_DETECTION_NODE
	if(lpn_addr == FALL_DETECTION_NODE)
	{
		static uint16_t trid;

		errorcode_t	response;
		trid += 1;
		struct mesh_generic_state	publish_data;
		publish_data.kind = mesh_generic_state_level;

		publish_data.level.level = 1;
		response = mesh_lib_generic_client_set(
				MESH_GENERIC_LEVEL_CLIENT_MODEL_ID,
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
			static uint16_t trid;

			errorcode_t	response;
			trid += 1;
			struct mesh_generic_state	publish_data;
			publish_data.kind = mesh_generic_state_level;

			publish_data.level.level = 1;
			response = mesh_lib_generic_client_set(
					MESH_GENERIC_LEVEL_CLIENT_MODEL_ID,
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
				static uint16_t trid;

				errorcode_t	response;
				trid += 1;
				struct mesh_generic_state	publish_data;
				publish_data.kind = mesh_generic_state_level;

				publish_data.level.level = 1;
				response = mesh_lib_generic_client_set(
						MESH_GENERIC_LEVEL_CLIENT_MODEL_ID,
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
