/*
 * gpio.c
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 */

/**********************************
 *    Include
 ***********************************/
#include "gpio.h"

/**********************************
 *    Global
 ***********************************/
int32_t FLAG_STOP_LOCAL_BUFFER = 0;


/******************************************************
 * Func name:   gpioInit
 * Description: function initiate led pins
 * parameter : none
 * ***************************************************/
void gpioInit()
{
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, false);


}

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
 * Function name: gpioLed1SetOn
 * Parameters : None
 *Description : For turning on LED1
 *****************************************************************/
void gpioLed1SetOn()
{
	GPIO_PinOutSet(LED1_port,LED1_pin);
}

/*****************************************************************
 * Function name: gpioLed1SetOff
 * Parameters : None
 *Description : For turning off LED1
 *****************************************************************/
void gpioLed1SetOff()
{
	GPIO_PinOutClear(LED1_port,LED1_pin);
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
void enable_PB1_interrupt()
{
	GPIO_IntConfig(PB0_port, PB1_pin, RISING_EDGE, false, true);
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

	//reads the state of PB0 pin whether it is 0 or 1
	int touch_1 =  !(GPIO_PinInGet(Touch_sensor_1_port, Touch_1_pin));

	LOG_INFO("Event occurred touch 1 %d\n",touch_1);

	if (touch_1 == 0)
		touch_1_status = 2; //2  - 0b10


	//clearing the interrupt status register
	GPIO_IntClear(Status_register);

	//enabling interrupts
	CORE_ATOMIC_IRQ_ENABLE();

	//signaling the state machine that a change has occurred
	gecko_external_signal(touch_1_status);


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

	LOG_INFO("Status_register %d\n",Status_register);

	//for touch sensor
	if(Status_register == TOUCH_2_interrupt)
	{

		//reads the state of PB0 pin whether it is 0 or 1
		int touch_2 =  !(GPIO_PinInGet(Touch_sensor_2_port, Touch_2_pin));

		LOG_INFO("Event occurred touch 2 %d\n",touch_2);

		if (touch_2 == 0)
			touch_2_status = 2; //2  - 0b10




		//signaling the state machine that a change has occurred
		gecko_external_signal(touch_2_status);
	}

	//for PB1
	if(Status_register == PB1_interrupt)
		{

			FLAG_STOP_LOCAL_BUFFER = 1;



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
	GPIO_PinOutSet(Alarm_port,Alarm_pin);
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
	GPIO_PinOutSet(Alarm_port,Alarm_pin);

}

/******************************************************
 * Func name:   trigger_alarm_off
 * Description: function to turn off buzzer
 * parameter : none
 * ***************************************************/
void trigger_alarm_off()
{
	GPIO_PinOutClear(Alarm_port,Alarm_pin);

}
