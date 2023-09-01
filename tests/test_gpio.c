#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "timer.h"
#include "gpio.h"
#include "log.h"

IOPort ports[] = {
  {GPIOA, GPIO_PINS_SOURCE15},	/* PA15 */
  {GPIOB, GPIO_PINS_SOURCE3},	/* PB3 */
  {GPIOB, GPIO_PINS_SOURCE4},	/* PB4 */
  {GPIOB, GPIO_PINS_SOURCE5},	/* PB5 */
  {GPIOB, GPIO_PINS_SOURCE6},	/* PB6 */
  
  {GPIOA, GPIO_PINS_SOURCE0},	/* PA0 */
  {GPIOA, GPIO_PINS_SOURCE1},	/* PA1 */
  {GPIOA, GPIO_PINS_SOURCE2},	/* PA2 */
  {GPIOA, GPIO_PINS_SOURCE3},	/* PA3 */
  {GPIOB, GPIO_PINS_SOURCE0},	/* PB0 */
  {GPIOB, GPIO_PINS_SOURCE1},	/* PB1 */
  {GPIOB, GPIO_PINS_SOURCE10},	/* PB10 */
  {GPIOB, GPIO_PINS_SOURCE11},	/* PB11 */
  {GPIOB, GPIO_PINS_SOURCE12},	/* PB12 */
  {GPIOB, GPIO_PINS_SOURCE13},	/* PB13 */
  {GPIOB, GPIO_PINS_SOURCE15},	/* PB15 */
  {GPIOB, GPIO_PINS_SOURCE14},	/* PB14 */
  {GPIOA, GPIO_PINS_SOURCE8},	/* PA8 */
  /* {GPIOA, GPIO_PINS_SOURCE9},	/\* PA9 *\/ */
  {GPIOA, GPIO_PINS_SOURCE10},	/* PA10 */
};

int main(void) {
  uint16_t i;
  system_clock_config(); // 8M HSE
  at32_board_init();
  //Timer_Init();
  uart_print_init(115200);	/* init debug uart */
  printf("start\n");
  for (i=0; i<sizeof(ports)/sizeof(ports[0]); i++){
    printf("init port%s.",
	   ports[i].port==GPIOA?"A":"B"
	   );
    DumpBinary((uint8_t*)&ports[i].pin_source, 2);
    init_gpio_output(&ports[i],
		     GPIO_OUTPUT_PUSH_PULL,
		     GPIO_DRIVE_STRENGTH_STRONGER);
  }
  while (1) {
    /* printf("tick:%ld\n", get_ticks()); */
    for (i=0; i<sizeof(ports)/sizeof(ports[0]); i++){
      gpio_set(&ports[i], 1);
    }

    delay_ms(500);
    for (i=0; i<sizeof(ports)/sizeof(ports[0]); i++){
      gpio_set(&ports[i], 0);
    }
    delay_ms(500);
  }
}
