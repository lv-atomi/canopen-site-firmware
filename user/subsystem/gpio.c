#include "gpio.h"
#include "log.h"
#include "board.h"

IOPort *global_devport;
bool_t global_waiting_bit;

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
#if defined (AT32F403Axx)
  if (devport->gpio_remap != 0){
    crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
    gpio_pin_remap_config(devport->gpio_remap, TRUE);
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

  /* /\* for debug purpose *\/ */
  /* gpio_set(devport, 1); */
  /* delay_ms(1); */
  /* gpio_set(devport, 0); */
  /* delay_ms(1);    */
}

void gpio_set_input_mode(IOPort * devport, bool_t is_input){
#if defined (AT32F403Axx)
  uint32_t pinval = is_input ? 0b01 : 0b0101;
  if (devport->pin_source < 8) {
    devport->port->cfglr &= (uint32_t)~(0xf << (devport->pin_source * 4));
    devport->port->cfglr |= (uint32_t)(pinval << (devport->pin_source * 4));
  } else{
    devport->port->cfghr &= (uint32_t)~(0xf << ((devport->pin_source - 8) * 4));
    devport->port->cfghr |= (uint32_t)(pinval << ((devport->pin_source - 8) * 4));
  }
#endif
#if defined (__AT32F421_GPIO_H)
  uint32_t pinval = is_input ? 0b00 : 0b01;
  devport->port->cfgr &= (uint32_t)~(0x3 << (devport->pin_source * 2));
  devport->port->cfgr |= (uint32_t)(pinval << (devport->pin_source * 2));
#endif
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

uint8_t gpio_waiting_closure_cb(){
  return gpio_read(global_devport) == global_waiting_bit;
}

uint8_t waiting_for_timeout(IOPort * devport, bool_t waiting_bit, uint32_t timeout_us){
  ASSERT(devport);
  global_devport = devport;
  global_waiting_bit = waiting_bit;
  if (timeout_us < 1000){
    /* log_printf("waiting in us..., us:%ld\n", timeout_us); */
    return waiting_us_while(timeout_us, gpio_waiting_closure_cb);
  }
  if (timeout_us < 1000000){
    /* log_printf("waiting in ms..., us:%ld\n", timeout_us/1000); */
    return waiting_ms_while(timeout_us/1000, gpio_waiting_closure_cb);
  }
  /* log_printf("waiting in s..., us:%ld\n", timeout_us/1000000); */
  uint32_t nms = timeout_us/1000;
  while (nms>0){
    uint16_t wait = nms > 500? 500: nms;
    /* log_printf("sub waiting in ms..., %d\n", wait); */
    
    uint8_t ret = waiting_ms_while(wait, gpio_waiting_closure_cb);
    /* log_printf("ret: %d\n", ret); */
    if (ret == 0) return ret;
    nms -= wait;
  }
  return 1;
}
