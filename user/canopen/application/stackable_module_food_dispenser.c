#include <stdio.h>

#include "stackable_module_food_dispenser.h"

#define MAX_STACKABLE 8

OD_extension_t OD_660V_extension[MAX_STACKABLE];

static ODR_t my_OD_read_660V(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  printf("read 660V\n");
  *countRead = 4;
  return ODR_OK;
}

static ODR_t my_OD_write_660V(OD_stream_t *stream, const void *buf,
			      OD_size_t count, OD_size_t *countWritten){
  printf("write 660V\n");
  return ODR_OK;
}

CO_ReturnError_t stackable_module_food_dispenser_init(uint8_t offset) {
  OD_entry_t * param_660V = NULL;

  switch (offset) {
  case 0: param_660V = OD_ENTRY_H6600_stackableModule0;
    break;
  case 1: param_660V = OD_ENTRY_H6601_stackableModule1;
    break;
  case 2: param_660V = OD_ENTRY_H6602_stackableModule2;
    break;
  case 3: param_660V = OD_ENTRY_H6603_stackableModule3;
    break;
  case 4: param_660V = OD_ENTRY_H6604_stackableModule4;
    break;
  case 5: param_660V = OD_ENTRY_H6605_stackableModule5;
    break;
  case 6: param_660V = OD_ENTRY_H6606_stackableModule6;
    break;
  case 7: param_660V = OD_ENTRY_H6607_stackableModule7;
    break;
  default:
    log_print("ERROR, unknown offset in stackable module:%d\n", offset);
    return 1;
  }
  
  OD_660V_extension.object = param_660V;
  OD_660V_extension.read = my_OD_read_660V;
  OD_660V_extension.write = my_OD_write_660V;

  OD_extension_init(param_660V, &OD_660V_extension);
 
  return CO_ERROR_NO;
}

CO_ReturnError_t app_stackable_module_food_dispenser_init() {
  uint8_t i;
  for (i=0; i < MAX_STACKABLE; i++){
    if (stackable_module_food_dispenser_init(i) != CO_ERROR_NO)
      return 1;
  }
  return CO_ERROR_NO;
}
