#include "i2c_soft.h"
#include "gpio.h"

void init_i2c_soft(I2CPort *port){
  init_gpio_output(&port->clk,
		   GPIO_OUTPUT_OPEN_DRAIN,
		   GPIO_DRIVE_STRENGTH_STRONGER);
  gpio_set(&port->clk, 1);
  init_gpio_output(&port->data,
		   GPIO_OUTPUT_OPEN_DRAIN,
		   GPIO_DRIVE_STRENGTH_STRONGER);
  gpio_set(&port->data, 1);
}

void i2c_delay(void) {
  uint8_t t = 10;
  while (t--)
    ;
}

void i2c_start(I2CPort * port) {
  gpio_set(&port->data, 1); // OLED_SDA_Set();
  gpio_set(&port->clk, 1); // OLED_SCL_Set();
  i2c_delay();
  gpio_set(&port->data, 0); // OLED_SDA_Clr();
  i2c_delay();
  gpio_set(&port->clk, 0); // OLED_SCL_Clr();
  i2c_delay();
}

void i2c_stop(I2CPort * port) {
  gpio_set(&port->data, 0); // OLED_SDA_Clr();
  gpio_set(&port->clk, 0); // OLED_SCL_Clr();
  i2c_delay();
  gpio_set(&port->data, 1); // OLED_SDA_Set();
  gpio_set(&port->clk, 1);
}

void i2c_waitack(I2CPort * port, uint32_t timeout) { //测数据信号的电平
  gpio_set(&port->data, 1); //OLED_SDA_Set();
  i2c_delay();
  gpio_set(&port->clk, 1); //OLED_SCL_Set();
  i2c_delay();
  gpio_set(&port->clk, 0); //OLED_SCL_Clr();
  i2c_delay();
}

/**
  * @brief  the master transmits data through software mode.
  * @param  port: the handle points to.
  * @param  pdata: data buffer.
  * @param  size: data size.
  * @param  timeout: maximum waiting time.
  * @retval i2c status.
  */
void i2c_master_transmit_soft(I2CPort * port, uint8_t * pdata, uint16_t size, uint32_t timeout) {
  uint16_t i;
  uint8_t j;
  
  i2c_start(port);
  for (i=0; i<=size; i++){
    uint8_t dat = i==0? port->address : pdata[i-1];
    for (j=0; j<8; j++) {
      if (dat & 0x80){ //将dat的8位从最高位依次写入
	gpio_set(&port->data, 1); // OLED_SDA_Set();
      } else {
	gpio_set(&port->data, 0); // OLED_SDA_Clr();
      }
      i2c_delay();
      gpio_set(&port->clk, 1); // OLED_SCL_Set();
      i2c_delay();
      gpio_set(&port->clk, 0); // OLED_SCL_Clr(); //将时钟信号设置为低电平
      dat <<= 1;
    }
    i2c_waitack(port, timeout);
  }

  i2c_stop(port);
}

