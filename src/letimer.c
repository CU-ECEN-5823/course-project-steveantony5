/*
 * letimer.c
 *
 *  Created on: Jan 30, 2019
 *      Author: Steve Antony
 *      Description : This file contains functions for initializing LETIMER0
 *      			  and for triggering Timer interrupt
 */

/*****************************************************************
 *              			INCLUDES
 *****************************************************************/

#include "letimer.h"

/*****************************************************************
 *              			GLOBALS
 *****************************************************************/
int32_t FREQUENCY_OSC;
int32_t counter = 0;
int32_t prescaler = 0;
/*****************************************************************
 * Function name: letimer_config
 * Parameters : None
 *Description : For initializing the LETIMER0 and setting prescaler value
 *****************************************************************/
void letimer_config()
{

	/*Selecting oscillator frequency based on energy mode*/
	 if((sleep_mode_select < sleepEM3) )
		FREQUENCY_OSC = LXFO_FREQ ;
	 else if (sleep_mode_select >= sleepEM3 )
		 FREQUENCY_OSC =  ULFRCO_FREQ;

	 /* Set configurations for LETIMER 0 */
	 /*Reference Application notes from silicon labs
	 */

	 const LETIMER_Init_TypeDef letimer_Init_values =
	 {
			 .enable         = false,                   /* Start counting when init completed. */
			 .debugRun       = false,                  /*  Counter running during debug */
			 .comp0Top       = true,                   /* Use COMP0 register as TOP value */
			 .bufTop         = false,                  /* Don't load COMP1 into COMP0 when REP0 reaches 0. */
			 .out0Pol        = 0,                      /* Idle value for output 0. */
			 .out1Pol        = 0,                      /* Idle value for output 1. */
			 .ufoa0          = letimerUFOANone,        /* Underflow output 0 action */
			 .ufoa1          = letimerUFOANone,        /* Underflow output 1 action*/
			 .repMode        = letimerRepeatFree       /* Repeat mode */
	 };

	 LETIMER_Init(LETIMER0, &letimer_Init_values);

	 //wait for any previous write to complete or to be synchronized
	 while (LETIMER0->SYNCBUSY!=0);

	 LETIMER_IntEnable(LETIMER0, (LETIMER_IEN_COMP0));
	 NVIC_EnableIRQ(LETIMER0_IRQn);
	 LETIMER_Enable(LETIMER0,true);

	 prescaler_value();
}
/*****************************************************************
 * 						LETIMER IRQ handler
 *****************************************************************/

void LETIMER0_IRQHandler(void)
{
	uint32_t flags_set;


	CORE_ATOMIC_IRQ_DISABLE();
	flags_set = LETIMER_IntGet(LETIMER0);

	// check if interrupt is set because of comp0
	if(flags_set & LETIMER_IF_COMP0)
	{
 		LETIMER_IntClear(LETIMER0, LETIMER_IFC_COMP0);
 		FLAG_PERIODIC_1 = true; //interrupt flag for display update
 		counter++;// counter for time stamp
	}

	CORE_ATOMIC_IRQ_ENABLE();
	gecko_external_signal(FLAG_PERIODIC_1);

}

/*****************************************************************
 * Function name: prescaler_value
 * Parameters : None
 *Description : For setting prescaler value based on clock frequency \
 *****************************************************************/
void prescaler_value()
{

	int32_t on_clocks;

	/*if period is greater than 2 sec, the prescaler (PERIOD*FREQUENCY_OSC) will be greater than \
	 * 16 bits
	 * Hence, the frequency of oscillator has to be prescaled
	 */

	if(PERIOD >= 2)
	{
		int32_t max_clocks = PERIOD*FREQUENCY_OSC;
		int32_t roll_over = max_clocks/MAX_VALUE;
		int32_t roll_over_2_pow = pow(2,roll_over);
		prescaler = roll_over_2_pow;
		CMU_ClockDivSet(cmuClock_LETIMER0,roll_over_2_pow);
		newfreq= FREQUENCY_OSC/roll_over_2_pow;
		period_clocks = PERIOD * newfreq;

		// Set the LETIMER0 comp0 register
		LETIMER_CompareSet(LETIMER0,0,period_clocks);
		while (LETIMER0->SYNCBUSY!=0);

	}

	/*period is less than 2 sec, default prescaler i.e 1 is used
		 */

	else
	{
		newfreq= FREQUENCY_OSC/1;
		prescaler = 1;
		period_clocks = PERIOD * FREQUENCY_OSC;
		on_clocks = (period_clocks)-(LED_ON_TIME * FREQUENCY_OSC);
		LETIMER_CompareSet(LETIMER0,0,period_clocks);
	}

}

/*****************************************************************
 * Function name: timer_ms
 * Parameters : delay required in ms
 *Description : For delay
 *Description   It uses COM1 of LETIMER0 interrupt
 *****************************************************************/
void timer_ms(int32_t delay_ms)
{
	CORE_ATOMIC_IRQ_DISABLE();

	uint32_t wait_time_ticks = ((newfreq * delay_ms) / 1000 );
	uint32_t comp1_value;

	if(LETIMER_CounterGet(LETIMER0) >= wait_time_ticks)
	{
		comp1_value = LETIMER_CounterGet(LETIMER0) - wait_time_ticks ;
	}

	//underflow condition
	else
	{
		comp1_value = period_clocks - (wait_time_ticks - LETIMER_CounterGet(LETIMER0));
	}

	LETIMER_CompareSet(LETIMER0,1,comp1_value);
	LETIMER_IntClear(LETIMER0, LETIMER_IFC_COMP1);
	LETIMER_IntEnable(LETIMER0, (LETIMER_IEN_COMP1));
	CORE_ATOMIC_IRQ_ENABLE();

}

