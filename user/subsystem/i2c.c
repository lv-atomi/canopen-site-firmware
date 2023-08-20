#include "i2c.h"
#include "gpio.h"
#include "pwm.h"
#include "log.h"

void i2c_clock_enable(i2c_type * dev){
  if (dev == I2C1) {
    crm_periph_clock_enable(CRM_I2C1_PERIPH_CLOCK, TRUE);
  } else if (dev == I2C2) {
    crm_periph_clock_enable(CRM_I2C2_PERIPH_CLOCK, TRUE);
  } else {
    ASSERT(FALSE);
  }
}

void init_i2c(I2CPort *port, uint32_t speed){
  ASSERT(port);
  init_gpio_mux(&port->clk, GPIO_OUTPUT_OPEN_DRAIN, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_MODERATE);
  init_gpio_mux(&port->clk, GPIO_OUTPUT_OPEN_DRAIN, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_MODERATE);

  i2c_clock_enable(port->controller);
  i2c_init(port->controller, I2C_FSMODE_DUTY_2_1, speed);
  i2c_own_address1_set(port->controller, I2C_ADDRESS_MODE_7BIT, port->address);
}
