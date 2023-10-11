#include "cap_displacement.h"
#include "gpio.h"
#include "log.h"
#include "spi.h"
#include <stdint.h>
#include "timer.h"

CapacitorDisplacementMeasurePort * cached_displacement_port[MAX_SPI_PORT + 1] = {0};

void init_capacitor_displacement_measurement(CapacitorDisplacementMeasurePort *devport){
  ASSERT(devport);

  log_printf("init_capacitor_displacement_measurement\n");
  uint8_t spi_idx = get_spi_index(&devport->spi);
  init_spi(&devport->spi);
  devport->zero_value = 0;
  devport->last_value = 0;
  devport->last_ticks = 0;
  devport->recv_idx = 0;
  ASSERT(cached_displacement_port[spi_idx] == 0);
  
  cached_displacement_port[spi_idx] = devport;

  enable_spi_irq(devport->spi.controller, 4, 4);
}

void displacement_set_zero(CapacitorDisplacementMeasurePort * devport) {
  ASSERT(devport);
  devport->zero_value = devport->last_value;
}

int32_t read_displacement(CapacitorDisplacementMeasurePort * devport,
			  uint32_t * delay,
			  bool_t get_raw) {
  ASSERT(devport);
  if (delay) (*delay) = ticks_diff(&devport->last_ticks);

  return devport->last_value - (get_raw ? 0 : devport->zero_value);
}

void cap_irq_handler(int spi_index) {
  /* printf("in spi irq:%d\n", spi_index); */
  if ((spi_index <= MAX_SPI_PORT) && (spi_index > 0) && (cached_displacement_port[spi_index] != NULL)) {
    CapacitorDisplacementMeasurePort * port = cached_displacement_port[spi_index];
    if(spi_i2s_flag_get(port->spi.controller, SPI_I2S_RDBF_FLAG) != RESET) {
      port->spi.rx_buf[port->recv_idx] = spi_i2s_data_receive(port->spi.controller);
      DumpHex(port->spi.rx_buf, port->recv_idx+1);

      /* if (port->recv_idx == 0) { */
      /*   uint32_t delay = ticks_diff(&port->last_ticks); */
      /*   if (delay < 20) { */
      /* 	  printf("force clear, delay:%ld\n", delay); */
      /*     port->recv_idx = 0; */
      /*     return; */
      /*   /\* } else { *\/ */
      /* 	/\*   printf("no clear, delay:%ld\n", delay); *\/ */
      /* 	} */
      /* } */

      if (port->recv_idx == 2) { /* update displacement value */
        port->last_value =
	  ((int32_t)(port->spi.rx_buf[2] & 0xf) << 16) | ((int32_t)port->spi.rx_buf[1] << 8) | port->spi.rx_buf[0];

	if (port->spi.rx_buf[2] & 0b11100000){ /* resync needed */
	  /* printf("out of sync, resync...\n"); */
	  spi_enable(port->spi.controller, FALSE);
	  uint8_t clk_status = FALSE;
	  uint32_t counter = get_ticks();
	  do{
	    uint8_t new_status = gpio_read(&port->spi.clk);
	    if (clk_status != new_status){
	      clk_status = new_status;
	      counter = get_ticks();
	      /* printf("spi clk detected... %ld\n", counter); */
            }
	  } while (get_ticks() - counter < 10);
	  /* printf("spi clk gap detected, resync...\n"); */
	  spi_enable(port->spi.controller, TRUE);
	} else if (port->spi.rx_buf[2] & 0x10) { /* minus */
          port->last_value *= -1;
        } else {		/* OK */
	}
	port->buf[0] = port->spi.rx_buf[0];
	port->buf[1] = port->spi.rx_buf[1];
	port->buf[2] = port->spi.rx_buf[2];
	/* log_printf("displace: %ld\n", port->last_value); */
        port->recv_idx = 0;
        port->last_ticks = get_ticks();
      } else
        port->recv_idx++;
    }
  }
}
