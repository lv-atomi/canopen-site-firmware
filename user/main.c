#include "301/CO_ODinterface.h"
#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "bmp.h"
/* #include "can.h" */
#include "can.h"
#include "flash.h"
#include "timer.h"
#include "ui.h"
#include "CO_app_STM32.h"
#include <stdint.h>
#include "OD.h"
//#include "app_common.h"

extern unsigned int system_core_clock; /*!< system clock frequency (core clock) */

uint8_t persist_dirty = 1;

void ui_store_id_cb(uint8_t id) {
  OD_set_u8(OD_ENTRY_H2114, //stationID
	    0, id, true);
  /* printf("store?:%d\n", */
  OD_set_u32(OD_ENTRY_H1010,
	     1, 0x65766173, false); /* 0x65766173 -> "SAVE" */

  canopenNodeSTM32->activeNodeID = id;
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
  /* printf("1.init 1010h extension write:%p\n", OD->list[9].extension->write); */
  can_gpio_config();
  /* printf("2.init 1010h extension write:%p\n", OD->list[9].extension->write); */
  canopen_init(1);
  /* printf("3.init 1010h extension write:%p %p\n", OD, OD->list[9].extension->write); */
  init_ui(ui_store_id_cb, ui_get_id_cb, ui_store_timeout_cb, ui_get_timeout_cb);
  /* printf("4.init 1010h extension write:%p %p\n", OD, OD->list[9].extension->write); */
  

  printf("start\n");
  /* printf("1010 extension write:%p\n", OD->list[9].extension->write); */
  /* printf("1011 extension write:%p\n", OD->list[10].extension->write); */
  while (1) {
    canopen_app_process();
    //delay_ms(500);
  }
}
