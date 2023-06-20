#include <stdio.h>

#include "aircondition.h"

OD_extension_t OD_6419_extension;
OD_extension_t OD_641A_extension;
OD_extension_t OD_641B_extension;
OD_extension_t OD_641C_extension;

static ODR_t my_OD_read_6419(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  printf("read 6419\n");
  *countRead = 4;
  return ODR_OK;
}

Static ODR_t my_OD_read_641A(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  printf("read 641A\n");
  *countRead = 4;
  return ODR_OK;
}

static ODR_t my_OD_write_641A(OD_stream_t *stream, const void *buf,
			      OD_size_t count, OD_size_t *countWritten){
  printf("write 641A\n");
  return ODR_OK;
}

static ODR_t my_OD_read_641B(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  printf("read 641B\n");
  *countRead = 4;
  return ODR_OK;
}

static ODR_t my_OD_write_641B(OD_stream_t *stream, const void *buf,
			      OD_size_t count, OD_size_t *countWritten){
  printf("write 641B\n");
  return ODR_OK;
}

static ODR_t my_OD_read_641C(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  printf("read 641C\n");
  *countRead = 4;
  return ODR_OK;
}


CO_ReturnError_t app_psu_init() {
  OD_entry_t * param_6419 = OD_ENTRY_H6419_airConditionThermalSensor
  OD_6412_extension.object = NULL;
  OD_6412_extension.read = my_OD_read_6419;
  OD_6412_extension.write = NULL;

  OD_extension_init(param_6419, &OD_6419_extension);

  OD_entry_t * param_641A = OD_ENTRY_H641A_airConditionFAN_Control;
  OD_6413_extension.object = NULL;
  OD_6413_extension.read = my_OD_read_641A;
  OD_6413_extension.write = my_OD_write_641A;

  OD_extension_init(param_641A, &OD_641A_extension);

  OD_entry_t * param_641B = OD_ENTRY_H641B_airConditionWorkload;
  OD_6414_extension.object = NULL;
  OD_6414_extension.read = my_OD_read_641B;
  OD_6414_extension.write = my_OD_write_641B;

  OD_extension_init(param_641B, &OD_641B_extension);

  OD_entry_t * param_641C = OD_ENTRY_H641C_airConditionStatus;
  od_6415_extension.object = NULL;
  OD_6415_extension.read = my_OD_read_641C;
  OD_6415_extension.write = NULL;

  OD_extension_init(param_641C, &OD_641C_extension);

  return CO_ERROR_NO;
}
