#include "slavestation_3.h"

/*
 * Slaveboard model 1, can station 3:
 * dual camera module, for each module:
 * -PSU:
 *     + set supply power voltage & max curret
 *     + monitor power voltage & currentread
 * -Camera:
 *     + Sense camera strobe signal
 *     + Send camera trigger signal
 *
 */

/*
 * PA8 -> camera0.strobe
 * PA9 -> camera0.trigger
 * PA10 -> camera1.strobe
 * PB5 -> camera1.trigger
 */

#include "at32f403a.h"
#include "at32f4xx_gpio.h"
#include "at32f4xx_crm.h"

void camera_gpios_init(void) {
  gpio_init_type gpio_init_struct;

  // Enable clocks
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  // Init structure for GPIO
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;

  // Initialize PA9 and PB5 as Output for trigger
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_9;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_5;
  gpio_init(GPIOB, &gpio_init_struct);

  // Initialize PA8 and PA10 as Input for strobe
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_8 | GPIO_PINS_10;
  gpio_init(GPIOA, &gpio_init_struct);
}

void init_slavestation_3(){
  camera_gpios_init();
}
