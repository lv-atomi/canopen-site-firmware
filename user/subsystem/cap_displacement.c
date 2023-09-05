#include "cap_displacement.h"
#include "at32f403a_407_spi.h"
#include "log.h"
#include "spi.h"
#include "timer.h"

CapacitorDisplacementMeasurePort * cached_displacement_port[MAX_SPI_PORT + 1] = {0};

void init_capacitor_displacement_measurement(CapacitorDisplacementMeasurePort *devport){
  ASSERT(devport);
  
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
  ASSERT(delay);
  uint32_t last_ticks = devport->last_ticks;
  (*delay) = ticks_diff(&last_ticks);

  return devport->last_value - (get_raw ? 0 : devport->zero_value);
}

void cap_irq_handler(int spi_index) {
  printf("in spi irq:%d\n", spi_index);
  if ((spi_index <= MAX_SPI_PORT) && (spi_index > 0) && (cached_displacement_port[spi_index] != NULL)) {
    CapacitorDisplacementMeasurePort * port = cached_displacement_port[spi_index];
    if(spi_i2s_flag_get(port->spi.controller, SPI_I2S_RDBF_FLAG) != RESET) {
      
      port->spi.rx_buf[port->recv_idx] = spi_i2s_data_receive(port->spi.controller);
      if (port->recv_idx == 0) {
        uint32_t delay = ticks_diff(&port->last_ticks);
        if (delay < 20) {
          port->recv_idx = 0;
          return;
        }
      } else if (port->recv_idx == 2) { /* update displacement value */
        port->last_value =
            ((int32_t)port->spi.rx_buf[1] << 8) | port->spi.rx_buf[0];
        if (port->spi.rx_buf[2] == 0x10) {
          port->last_value *= -1;
        }
        port->recv_idx = 0;
        port->last_ticks = get_ticks();
      } else
        port->recv_idx++;
    }
  }
}
