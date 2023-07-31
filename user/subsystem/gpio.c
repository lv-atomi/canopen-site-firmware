#include "gpio.h"
#include "log.h"

void init_gpio_clock(IOPort* devport){
  ASSERT(devport);
  if (devport->port == GPIOA){
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  } else if (devport->port == GPIOB){
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  } else if (devport->port == GPIOC){
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
  }
#if defined (GPIOD)  
  else if (devport->port == GPIOD){
    crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
  }
#endif
#if defined (GPIOE)
  else if (devport->port == GPIOE){
    crm_periph_clock_enable(CRM_GPIOE_PERIPH_CLOCK, TRUE);
  }
#endif
}

void init_gpio_output(IOPort * devport,
		      gpio_output_type output_type,
		      gpio_drive_type drive_strength) {
  init_gpio_clock(devport);
  
  gpio_init_type gpio_initstructure;

  /* gpio configuration for output pins */
  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_out_type = output_type;
  gpio_initstructure.gpio_drive_strength = drive_strength;

  gpio_initstructure.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_initstructure.gpio_pins = 1 << devport->pin_source;
  gpio_init(devport->port, &gpio_initstructure);
}

void init_gpio_mux(IOPort * devport,
		   gpio_output_type output_type,
		   gpio_pull_type pull_type,
		   gpio_drive_type drive_strength) {
  init_gpio_clock(devport);
  gpio_init_type gpio_initstructure;
  
  /* gpio configuration for output pins */
  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_out_type = output_type;
  gpio_initstructure.gpio_pull = pull_type;
  gpio_initstructure.gpio_drive_strength = drive_strength;

  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins = 1 << devport->pin_source;
  gpio_init(devport->port, &gpio_initstructure);

#if defined (__AT32F421_GPIO_H)
  gpio_pin_mux_config(devport->port, devport->pin_source, devport->mux_sel);
#endif  
}

void init_gpio_input(IOPort * devport,
		     gpio_pull_type pull_type,
		     gpio_drive_type drive_strength) {
  init_gpio_clock(devport);
  gpio_init_type gpio_initstructure;
  
  /* gpio configuration for output pins */
  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_pull = pull_type;
  gpio_initstructure.gpio_drive_strength = drive_strength;

  gpio_initstructure.gpio_mode = GPIO_MODE_INPUT;
  gpio_initstructure.gpio_pins = 1 << devport->pin_source;
  gpio_init(devport->port, &gpio_initstructure);
}

void init_gpio_analogy(IOPort * devport,
		       gpio_pull_type pull_type,
		       gpio_drive_type drive_strength) {
  init_gpio_clock(devport);
  
  gpio_init_type gpio_initstructure;
  
  /* gpio configuration for output pins */
  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_pull = pull_type;
  gpio_initstructure.gpio_drive_strength = drive_strength;

  gpio_initstructure.gpio_mode = GPIO_MODE_ANALOG;
  gpio_initstructure.gpio_pins = 1 << devport->pin_source;
  gpio_init(devport->port, &gpio_initstructure);
}

void gpio_set(IOPort * devport, bool_t bit) {
  ASSERT(devport);
  gpio_bits_write(devport->port, 1 << devport->pin_source, bit);
}

bool_t gpio_read(IOPort * devport) {
  ASSERT(devport);
  return gpio_input_data_bit_read(devport->port, 1 << devport->pin_source);
}
