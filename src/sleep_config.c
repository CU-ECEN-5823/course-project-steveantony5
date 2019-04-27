
/*
 * sleep_mode.c
 *
 *  Created on: Jan 30, 2019
 *      Author: Steve Antony
 */

/*****************************************************************
 *              			INCLUDES
 *****************************************************************/

#include "sleep_config.h"

/*****************************************************************
 *              			GLOBALS
 *****************************************************************/
/*****************************************************************
 * Function name: sleep_config
 * Parameters : None
 *Description : For initializing SLEEP_InitEx
 *****************************************************************/

void sleep_config()
{
	const SLEEP_Init_t init_values = { 0 };

	SLEEP_InitEx(&init_values);

}
