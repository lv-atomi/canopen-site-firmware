#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "timer.h"
#include "gpio.h"
#include "log.h"

IOPort ports[] = {
  {GPIOA, GPIO_PINS_15},	/* PA15 */
  {GPIOB, GPIO_PINS_3},	/* PB3 */
  {GPIOB, GPIO_PINS_4},	/* PB4 */
  {GPIOB, GPIO_PINS_5},	/* PB5 */
  {GPIOB, GPIO_PINS_6},	/* PB6 */
  
  {GPIOA, GPIO_PINS_0},	/* PA0 */
  {GPIOA, GPIO_PINS_1},	/* PA1 */
  {GPIOA, GPIO_PINS_2},	/* PA2 */
  {GPIOA, GPIO_PINS_3},	/* PA3 */
  {GPIOB, GPIO_PINS_0},	/* PB0 */
  {GPIOB, GPIO_PINS_1},	/* PB1 */
  {GPIOB, GPIO_PINS_10},	/* PB10 */
  {GPIOB, GPIO_PINS_11},	/* PB11 */
  {GPIOB, GPIO_PINS_12},	/* PB12 */
  {GPIOB, GPIO_PINS_13},	/* PB13 */
  {GPIOB, GPIO_PINS_15},	/* PB15 */
  {GPIOB, GPIO_PINS_14},	/* PB14 */
  {GPIOA, GPIO_PINS_8},	/* PA8 */
  /* {GPIOA, GPIO_PINS_9},	/\* PA9 *\/ */
  {GPIOA, GPIO_PINS_10},	/* PA10 */
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
    DumpBinary((uint8_t*)&ports[i].pin, 2);
    init_gpio_output(&ports[i],
		     GPIO_OUTPUT_PUSH_PULL,
		     GPIO_MODE_OUTPUT,
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
