#include "301/CO_ODinterface.h"
#include "305/CO_LSS.h"
#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "at32f403a_407_spi.h"
#include "bmp.h"
/* #include "can.h" */
#include "can.h"
#include "cap_displacement.h"
#include "flash.h"
#include "log.h"
#include "timer.h"
#include "ui.h"
#include "CO_app_STM32.h"
#include <stdint.h>
#include "OD.h"
//#include "app_common.h"

extern unsigned int system_core_clock; /*!< system clock frequency (core clock) */

CapacitorDisplacementMeasurePort displacement = {
  .spi={
    .miso = {GPIOB, GPIO_PINS_SOURCE15},
    .clk = {GPIOB, GPIO_PINS_SOURCE13},
    .controller = SPI2,
    .init_type = {
      .transmission_mode = SPI_TRANSMIT_SIMPLEX_RX,
      .master_slave_mode = SPI_MODE_SLAVE,
      .mclk_freq_division = SPI_MCLK_DIV_32,
      .first_bit_transmission = SPI_FIRST_BIT_LSB,
      .frame_bit_num = SPI_FRAME_8BIT,
      .clock_polarity = SPI_CLOCK_POLARITY_HIGH,
      .clock_phase = SPI_CLOCK_PHASE_2EDGE,
      .cs_mode_selection = SPI_CS_SOFTWARE_MODE,
    }
  }
};

uint8_t persist_dirty = 1;

void ui_store_id_cb(uint8_t id) {
  OD_set_u8(OD_ENTRY_H2114, //stationID
	    0, id, true);
  /* printf("store?:%d\n", */
  OD_set_u32(OD_ENTRY_H1010,
	     1, 0x65766173, false); /* 0x65766173 -> "SAVE" */

  canopen_update_node_id(id);
}

uint8_t ui_get_id_cb(void) {
  uint8_t stored_id;
  if (persist_dirty){
    /* printf("load?:%d\n", */
    OD_set_u32(OD_ENTRY_H1011,
	       1, 0x64616F6C, false); /* 0x64616F6C -> "LOAD" */
    persist_dirty = 0;
  }
  
  OD_get_u8(OD_ENTRY_H2114, //stationID
	    0, &stored_id, true);

  stored_id = CO_LSS_NODE_ID_VALID(stored_id) ? stored_id : 1; /* ensure we return a valid id, which by default is 1. THIS IS IMPORTANT */
  
  canopen_update_node_id(stored_id);
  return stored_id;
}

void ui_store_timeout_cb(uint16_t timeout) {
  OD_set_u8(OD_ENTRY_H2115, //screenOffTimeout
	    0, timeout, true);
  OD_set_u32(OD_ENTRY_H1010,
	     1, 0x65766173, false);
}

uint16_t ui_get_timeout_cb(void) {
  uint16_t stored_timeout;
  if (persist_dirty){
    OD_set_u32(OD_ENTRY_H1011,
	       1, 0x64616F6C, false);
    persist_dirty = 0;
  }
  OD_get_u16(OD_ENTRY_H2115, //screenOffTimeout
	    0, &stored_timeout, true);
  return stored_timeout;  
}

//uint16_t VolShowTest = DEFAULTVOL;
int main(void) {
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();

  uart_print_init(115200);	/* init debug uart */

  /* printf("System clock: %d\n", system_core_clock); */
  /* printf("AHBDIV: %d apb1div:%d apb2div:%d\n", */
	 /* CRM->cfg_bit.ahbdiv, CRM->cfg_bit.apb1div, CRM->cfg_bit.apb2div); */
  can_gpio_config();
  canopen_init();
  init_ui(ui_store_id_cb, ui_get_id_cb, ui_store_timeout_cb, ui_get_timeout_cb);
  
  log_printf("start\n");
  init_capacitor_displacement_measurement(&displacement);
  
  while (1) {
    uint32_t delay = 0;
    canopen_app_process();
    int32_t pos = read_displacement(&displacement, &delay, 1);
    printf("displace: %ld, delay:%ld\n", pos, delay);
    delay_ms(100);
  }
}
