/*
 * gpio.c
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 */
#include "gpio.h"
#include "em_gpio.h"
#include <string.h>


void gpioInit()
{
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, false);


}

void button_init(void)
{
  // configure pushbutton PB0 and PB1 as inputs, with pull-up enabled
  GPIO_PinModeSet(PB0_port, PB0_pin, gpioModeInput, 0);
  GPIO_PinModeSet(PB0_port, PB1_pin, gpioModeInput, 0);
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




void enable_PB0_interrupt()
{
	//clearing the previous interrupts
	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);

	//enabling PB0 interrupts
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);

	/*setting Rising edge and Falling edge interrupt
	 * Rising as well as Falling edge interrupt is activated because
	 * the change of state from 1 to 0 as well as 0 to 1 has to be notified
	 */

	GPIO_IntConfig(PB0_port, PB0_pin, RISING_EDGE, FALLING_EDGE, true);
}

/*****************************************************************
 * Function name: GPIO_EVEN_IRQHandler
 * Parameters : None
 *
 * Interrupt handler for PB0 button press
 *****************************************************************/

void GPIO_EVEN_IRQHandler()
{
	//disabling interrupts
	CORE_ATOMIC_IRQ_DISABLE();

	//gets the status of the interrupt
	int Status_register = GPIO_IntGet();

	//reads the state of PB0 pin whether it is 0 or 1
	int button_press =  !(GPIO_PinInGet(PB0_port, PB0_pin));

	LOG_INFO("Event occurred PB0 %d\n",button_press);

	if (button_press == 0)
		button_position = 2; //2  - 0b10
	else
		button_position = 4; //4 - 0b100



	//clearing the interrupt status register
	GPIO_IntClear(Status_register);

	//enabling interrupts
	CORE_ATOMIC_IRQ_ENABLE();

	//signaling the state machine that a change has occurred
	gecko_external_signal(button_position);


}
