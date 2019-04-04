#ifndef INCLULE_LETIMER_H_
#define INCLULE_LETIMER_H_

/*****************************************************************
 *              			INCLUDES
 *****************************************************************/

#include "em_letimer.h"
#include "em_core.h"
#include "cmu.h"
#include <math.h>
#include "log.h"
#include "em_i2c.h"
#include "sleep.h"
/*****************************************************************
 *              			MACROS
 *****************************************************************/
#define PERIOD (1)
#define LED_ON_TIME (0.175)

#define MAX_VALUE (65535)
#define ULFRCO_FREQ (1000)
#define LXFO_FREQ (32768)


//#define TIMER_SUPPORTS_1HZ_TIMER_EVENT	1
/*****************************************************************
 *              			FUNCTION PROTOTYPES
 *****************************************************************/
void letimer_config();
void prescaler_value();
void timerWaitms(int32_t);
void timer_ms(int32_t);
/*****************************************************************
 *              			GLOBAL DECLARATION
 *****************************************************************/
extern int8_t FLAG_PERIODIC_1;

extern int8_t FLAG_DONE;


extern const int sleep_mode_select;
typedef enum  {TRUE = 1, FALSE = 0, ERROR = 2}Flag_status;
extern Flag_status state;

uint32_t newfreq;
int32_t period_clocks;

#endif /* INCLUDE_LETIMER_H_ */
