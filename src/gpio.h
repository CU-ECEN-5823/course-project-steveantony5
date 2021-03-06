/*
 * gpio.h
 *
 *  Created on: April 27, 2019
 *      Author: Steve Antony
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_

/*****************************************************************
 *              			INCLUDES
 *****************************************************************/
#include <stdbool.h>
#include "display.h"
#include "em_core.h"
#include "log.h"
#include "gatt_db.h"
#include "gpiointerrupt.h"
#include "em_gpio.h"
#include <string.h>

/*****************************************************************
 *              			MACROS
 *****************************************************************/
#define PB0_port              (gpioPortF)
#define PB1_port              (gpioPortF)
#define DISPAY_PORT           (gpioPortD)
#define Touch_sensor_1_port   (gpioPortF)
#define Touch_sensor_2_port   (gpioPortF)
#define Alarm_port            (gpioPortC)


#define Alarm_pin             (9)
#define PB0_pin               (6)
#define PB1_pin               (7)
#define Touch_1_pin           (4)
#define Touch_2_pin           (5)
#define DISPLAY_PIN           (15)


#define ALARM_STATE_PS_ID                       (0x4100)
#define GPIO_SET_DISPLAY_EXT_COMIN_IMPLEMENTED 	(1)
#define GPIO_DISPLAY_SUPPORT_IMPLEMENTED		(1)
#define EXTCOMIN_PIN                            (13)
#define RISING_EDGE                             (1)
#define FALLING_EDGE                            (1)


//Interrupt status pin set
#define TOUCH_2_interrupt     (32)
#define PB1_interrupt         (128)
#define TOUCH_1_interrupt     (16)
#define PB0_interrupt         (64)
/*****************************************************************
 *              			GLOBAL DECLARATIONS
 *****************************************************************/
typedef enum {PRESSED = 2, RELEASED = 4, CLEAR = 0}button_flag;

button_flag  button_position ;
button_flag touch_1_status;
button_flag touch_2_status;

extern bool alarm_on;
/*****************************************************************
 *              			FUNCTION PROTYPES
 *****************************************************************/
/*
--------------------------------------------------------------------------------------------
gpioInit
--------------------------------------------------------------------------------------------
*	This initiates led pins
*
* 	@\param			none
*
* 	@\return		none
*
*/
void gpioInit();

/*
--------------------------------------------------------------------------------------------
gpioLed1SetOn
--------------------------------------------------------------------------------------------
*	This turns on LED1
*
* 	@\param			none
*
* 	@\return		none
*
*/
void gpioLed1SetOn();

/*
--------------------------------------------------------------------------------------------
gpioLed1SetOff
--------------------------------------------------------------------------------------------
*	This turns off LED1
*
* 	@\param			none
*
* 	@\return		none
*
*/
void gpioLed1SetOff();

/*
--------------------------------------------------------------------------------------------
gpio_button_PB0
--------------------------------------------------------------------------------------------
*	This enables button PB0
*
* 	@\param			none
*
* 	@\return		none
*
*/
void gpio_button_PB0();

/*
--------------------------------------------------------------------------------------------
enable_PB0_interrupt
--------------------------------------------------------------------------------------------
*	This enables interrupt for button PB0
*
* 	@\param			none
*
* 	@\return		none
*
*/
void enable_PB0_interrupt();

/*
--------------------------------------------------------------------------------------------
touch_sensor_init
--------------------------------------------------------------------------------------------
*	This enables touch sensors
*
* 	@\param			none
*
* 	@\return		none
*
*/
void touch_sensor_init();

/*
--------------------------------------------------------------------------------------------
enable_touch_interrupt
--------------------------------------------------------------------------------------------
*	This enables interrupt for touch sensors
*
* 	@\param			none
*
* 	@\return		none
*
*/
void enable_touch_interrupt();

/*
--------------------------------------------------------------------------------------------
initiate_alarm
--------------------------------------------------------------------------------------------
*	This enables buzzer
*
* 	@\param			none
*
* 	@\return		none
*
*/
void initiate_alarm();

/*
--------------------------------------------------------------------------------------------
enable_PB1_interrupt
--------------------------------------------------------------------------------------------
*	This enables interrupt for PB1
*
* 	@\param			none
*
* 	@\return		none
*
*/
void enable_button_interrupt();

/*
--------------------------------------------------------------------------------------------
trigger_alarm_off
--------------------------------------------------------------------------------------------
*	This turns off buzzer
*
* 	@\param			none
*
* 	@\return		none
*
*/
void trigger_alarm_off();

/*
--------------------------------------------------------------------------------------------
trigger_alarm_on
--------------------------------------------------------------------------------------------
*	This turns on buzzer
*
* 	@\param			none
*
* 	@\return		none
*
*/
void trigger_alarm_on();

/*
--------------------------------------------------------------------------------------------
ps_save_data
--------------------------------------------------------------------------------------------
*	This function helps to store data to flash memory
*
* 	@\param			key - unique key to store and retrive
* 	                *pvalue - address of the value to be stored
 *                  size    - size of the variable to be stored
*
* 	@\return		none
*
*/
void ps_save_data(uint16_t key, void *pValue, uint8_t size);

/*
--------------------------------------------------------------------------------------------
ps_load_data
--------------------------------------------------------------------------------------------
*	This function helps to load stored data to flash memory
*
* 	@\param			key - unique key to store and retrive
* 	                *pvalue - address of the value to be stored
 *                  size    - size of the variable to be stored
*
* 	@\return		none
*
*/
void ps_load_data(uint16_t key, void *pValue, uint8_t size);

#endif /* SRC_GPIO_H_ */
