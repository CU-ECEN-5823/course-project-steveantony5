/*
 * cmu.c
 *
 *  Created on: Jan 30, 2019
 *      Author: Steve Antony
 *      Description : This file contains functions for intializing clocks for
 *      				LETIMER0 based on energy modes
 */

/*****************************************************************
 *              			INCLUDES
 *****************************************************************/
#include "cmu.h"

/*****************************************************************
 *              			GLOBALS
 *****************************************************************/

/*****************************************************************
 * Function name: cmu_init
 * Parameters : None
 *Description : For initializing the oscillator and enabling the clocks
 *****************************************************************/

void cmu_init()
{
	// ULFRCO is used for Energy Mode 3
	 if((sleep_mode_select >= 3) )
	 {
		 CMU_OscillatorEnable(cmuOsc_ULFRCO , true, false);
		 CMU_ClockSelectSet(cmuClock_LFA , cmuSelect_ULFRCO );
		 CMU_ClockEnable(cmuClock_LFA, true );
		 CMU_ClockEnable(cmuClock_LETIMER0, true);
	 }

	 // ULFRCO is used for Energy Mode 0, 1, 2
	 if((sleep_mode_select < 3) )
	 {
		 CMU_OscillatorEnable(cmuOsc_LFXO , true, false);
		 CMU_ClockSelectSet(cmuClock_LFA , cmuSelect_LFXO );
		 CMU_ClockEnable(cmuClock_LFA, true );
		 CMU_ClockEnable(cmuClock_LETIMER0, true);
	 }

}
