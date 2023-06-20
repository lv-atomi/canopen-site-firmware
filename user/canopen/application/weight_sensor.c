#include <stdio.h>

#include "weight_sensor.h"

OD_extension_t OD_6418_extension;

static ODR_t my_OD_read_6418(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  printf("read 6418\n");
  *countRead = 4;
  return ODR_OK;
}

CO_ReturnError_t app_weight_sensor_init() {
  OD_entry_t * param_6418 = OD_ENTRY_H6418_weightSensor;
  OD_6418_extension.object = NULL;
  OD_6418_extension.read = my_OD_read_6418;
  OD_6418_extension.write = NULL;

  OD_extension_init(param_6418, &OD_6418_extension);  

  return CO_ERROR_NO;
}
