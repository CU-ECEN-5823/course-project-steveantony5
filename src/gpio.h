/*
 * gpio.h
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_
#include <stdbool.h>
#include "display.h"
#include "em_core.h"
#include "log.h"
#include "gatt_db.h"
/*****************************************************************
 *              			MACROS
 *****************************************************************/
#define LED1_port (gpioPortF)
#define PB0_port (gpioPortF)

#define DISPAY_PORT (gpioPortD)
#define LED1_pin  (5)
#define PB0_pin (6)
#define PB1_pin (7)
#define DISPLAY_PIN (15)
#define GPIO_SET_DISPLAY_EXT_COMIN_IMPLEMENTED 	(1)
#define GPIO_DISPLAY_SUPPORT_IMPLEMENTED		(1)
#define EXTCOMIN_PIN (13)
#define RISING_EDGE (1)
#define FALLING_EDGE (1)

/*****************************************************************
 *              			GLOBAL DECLARATIONS
 *****************************************************************/
extern enum Flag_status;
typedef enum {PRESSED = 2, RELEASED = 4, CLEAR = 0}button_flag;

button_flag  button_position ;
/*****************************************************************
 *              			FUNCTION PROTYPES
 *****************************************************************/
void gpioInit();
void gpioLed1SetOn();
void gpioLed1SetOff();
void gpio_button_PB0();
void enable_PB0_interrupt();
#endif /* SRC_GPIO_H_ */
