/*
 * gpio.c
 *
 *  Created on: April 27, 2019
 *      Author: Steve Antony
 */

/**********************************
 *    Include
 ***********************************/
#include "gpio.h"

typedef enum  {TRUE = 1, FALSE = 0, ERROR = 2}Flag_status;

/**********************************
 *    Global
 ***********************************/
/*to stop local buzzer on FALL DETECTION NODE*/
bool FLAG_STOP_LOCAL_BUFFER = FALSE;

/*to stop defribrillator on HEART BEAT NODE*/
bool FLAG_STOP_DEFRIBRILLATION = FALSE;

/******************************************************
 * Func name:   button_init
 * Description: function initiate buttons PB1 and PB0
 * parameter : none
 * ***************************************************/
void button_init(void)
{
  // configure pushbutton PB0 and PB1 as inputs, with pull-up enabled
  GPIO_PinModeSet(PB0_port, PB0_pin, gpioModeInput, 0);
  GPIO_PinModeSet(PB0_port, PB1_pin, gpioModeInput, 0);
}

/******************************************************
 * Func name:   touch_sensor_init
 * Description: function initiate touch sensors
 * parameter : none
 * ***************************************************/
void touch_sensor_init(void)
{
	GPIO_PinModeSet(Touch_sensor_1_port, Touch_1_pin, gpioModeInput, 0);
	GPIO_PinModeSet(Touch_sensor_2_port, Touch_2_pin, gpioModeInput, 0);
}

/*****************************************************************
 * Function name: gpioSetDisplayExtcomin
 * Parameters : None
 *Description : For refreshing the display
 *****************************************************************/
void gpioSetDisplayExtcomin(bool high)
{
	if(high)
	{
		GPIO_PinOutSet(DISPAY_PORT,EXTCOMIN_PIN);
	}
	else
	{
		GPIO_PinOutClear(DISPAY_PORT,EXTCOMIN_PIN);
	}
}

/*****************************************************************
 * Function name: gpioEnableDisplay
 * Parameters : None
 *Description : For enabling the display module
 *****************************************************************/
void gpioEnableDisplay()
{
	GPIO_PinOutSet(DISPAY_PORT,DISPLAY_PIN);
}

/******************************************************
 * Func name:   enable_touch_interrupt
 * Description: function enable touch sensors interrupts
 * parameter : none
 * ***************************************************/
void enable_touch_interrupt()
{
	//enables the odd and even interrupts
	GPIOINT_Init();

	GPIO_IntConfig(Touch_sensor_1_port, Touch_1_pin, RISING_EDGE, false, true);
	GPIO_IntConfig(Touch_sensor_2_port, Touch_2_pin, RISING_EDGE, false, true);

}

/******************************************************
 * Func name:   enable_PB1_interrupt
 * Description: function to enable PB1 button interrupt
 * parameter : none
 * ***************************************************/
void enable_button_interrupt()
{
	GPIO_IntConfig(PB0_port, PB0_pin, RISING_EDGE, false, true);
	GPIO_IntConfig(PB1_port, PB1_pin, RISING_EDGE, false, true);
}
/*****************************************************************
 * Function name: GPIO_EVEN_IRQHandler
 * Parameters : None
 *
 * Interrupt handler for even pins
 *****************************************************************/
void GPIO_EVEN_IRQHandler()
{
	//disabling interrupts
	CORE_ATOMIC_IRQ_DISABLE();

	//gets the status of the interrupt
	int Status_register = GPIO_IntGet();

	//Check if the interrupt occurred due to Touch sensor
		if(Status_register == TOUCH_1_interrupt)
		{

			int touch_1 =  !(GPIO_PinInGet(Touch_sensor_1_port, Touch_1_pin));

			if (touch_1 == CLEAR)
				touch_1_status = PRESSED; //2  - 0b10

			//signaling the state machine that a change has occurred
			gecko_external_signal(touch_1_status);
		}

		//Check if the interrupt occurred due to button PB0
		if(Status_register == PB0_interrupt)
		{

			FLAG_STOP_DEFRIBRILLATION = TRUE;

			//signaling the state machine that a change has occurred
			gecko_external_signal(FLAG_STOP_DEFRIBRILLATION);

		}

	//clearing the interrupt status register
	GPIO_IntClear(Status_register);

	//enabling interrupts
	CORE_ATOMIC_IRQ_ENABLE();

}
/*****************************************************************
 * Function name: GPIO_ODD_IRQHandler
 * Parameters : None
 *
 * Interrupt handler for odd pins
 *****************************************************************/
void GPIO_ODD_IRQHandler()
{
	//disabling interrupts
	CORE_ATOMIC_IRQ_DISABLE();

	//gets the status of the interrupt
	int Status_register = GPIO_IntGet();

	//Check if the interrupt occurred due to Touch sensor
	if(Status_register == TOUCH_2_interrupt)
	{

		//reads the state of PB0 pin whether it is 0 or 1
		int touch_2 =  !(GPIO_PinInGet(Touch_sensor_2_port, Touch_2_pin));

		if (touch_2 == CLEAR)
			touch_2_status = PRESSED;

		//signaling the state machine that a change has occurred
		gecko_external_signal(touch_2_status);
	}

	//Check if the interrupt occurred due to button PB1
	if(Status_register == PB1_interrupt)
		{

			FLAG_STOP_LOCAL_BUFFER = TRUE;

			//signaling the state machine that a change has occurred
			gecko_external_signal(FLAG_STOP_LOCAL_BUFFER);

		}

	//clears the status registers
	GPIO_IntClear(Status_register);

	//enabling interrupts
	CORE_ATOMIC_IRQ_ENABLE();


}

/******************************************************
 * Func name:   initiate_alarm
 * Description: function to initiate buzzer pins
 * parameter : none
 * ***************************************************/
void initiate_alarm()
{
	/*Drive strength is chosen as strong so as to support the buzzer
	 * Limits at 10mA */
	GPIO_DriveStrengthSet(Alarm_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(Alarm_port, Alarm_pin, gpioModePushPull, 0);
}

/******************************************************
 * Func name:   trigger_alarm_on
 * Description: function to turn on buzzer
 * parameter : none
 * ***************************************************/
void trigger_alarm_on()
{
	alarm_on = TRUE;
	ps_save_data(ALARM_STATE_PS_ID, &alarm_on, sizeof(alarm_on));
	GPIO_PinOutSet(Alarm_port,Alarm_pin);

}

/******************************************************
 * Func name:   trigger_alarm_off
 * Description: function to turn off buzzer
 * parameter : none
 * ***************************************************/
void trigger_alarm_off()
{
	alarm_on = FALSE;
	ps_save_data(ALARM_STATE_PS_ID, &alarm_on, sizeof(alarm_on));
	GPIO_PinOutClear(Alarm_port,Alarm_pin);

}
