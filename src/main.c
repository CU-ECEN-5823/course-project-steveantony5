#include <stdbool.h>
#include "native_gecko.h"
#include "log.h"
#include "letimer.h"
#include "ble_mesh_device_type.h"


/*****************************************************************
 *              			Mode Selection
 *This selection is not used in Assignment 5
 *Hence it is set to 0 for Clock selection purpose in LETIMER0
 *****************************************************************/
/** Status value for EM0.
sleepEM0 = 0,

 Status value for EM1.
sleepEM1 = 1,

 Status value for EM2.
sleepEM2 = 2,

 Status value for EM3.
sleepEM3 = 3,

 Status value for EM4.
sleepEM4 = 4 */

const int sleep_mode_select = sleepEM0;
/*****************************************************************
 *
 */

extern void gecko_main_init();
bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);
extern void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);
extern void button_init(void);
int8_t FLAG_PERIODIC_3;
int8_t FLAG_PERIODIC_1;

int8_t FLAG_Delay_80;
int8_t FLAG_DONE;



int main(void)
{

  // Initialize stack
  gecko_main_init();

  //Clock Management Unit initialize
  cmu_init();

  //Configuring LETIMER0
  letimer_config();

  displayInit();

  gpioInit();

  button_init();

  touch_sensor_init();
  enable_touch_interrupt();

  displayInit();

  logInit();

  /* Infinite loop */
  while (1) {
	struct gecko_cmd_packet *evt = gecko_wait_event();
	bool pass = mesh_bgapi_listener(evt);
	if (pass) {
		handle_gecko_event(BGLIB_MSG_ID(evt->header), evt);


	}
  };
}

