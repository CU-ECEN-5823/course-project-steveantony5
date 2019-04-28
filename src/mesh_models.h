
/*
 * mesh_models.h
 *
 *  Created on: April 27, 2019
 *      Author: Steve Antony
 */

#ifndef MESH_MODELS_H_
#define MESH_MODELS_H_


/**********************************
 *    Include
 ***********************************/
#include <stdio.h>
#include <stdint.h>
#include "main.h"


/**********************************
 *    Globals
 ***********************************/
extern uint16_t Response;

/**********************************
 *    Function Prototypes
 ***********************************/

/*
--------------------------------------------------------------------------------------------
generic_level_client_get
--------------------------------------------------------------------------------------------
*	This subscribes the models on client using level model
*
* 	@\param			lpn address
*
* 	@\return		none
*
*/
void generic_level_client_get(uint16_t);

/*
--------------------------------------------------------------------------------------------
generic_level_client_set
--------------------------------------------------------------------------------------------
*	This does unicast publish to lpn using level model
*
* 	@\param			lpn address
*
* 	@\return		none
*
*/
void generic_level_client_set(uint16_t);

/*
--------------------------------------------------------------------------------------------
generic_onoff_client_set
--------------------------------------------------------------------------------------------
*	This does unicast publish to lpn using onoff model
*
* 	@\param			lpn address
*
* 	@\return		none
*
*/
void generic_onoff_client_set(uint16_t, bool);

#endif
