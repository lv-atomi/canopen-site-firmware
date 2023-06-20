#include <stdio.h>

#include "camera.h"

#define MAX_CAMERA_NUM   2
OD_extension_t OD_650V_extension[MAX_CAMERA_NUM];

static ODR_t my_OD_read_650V(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  printf("read 650V\n");
  *countRead = 4;
  return ODR_OK;
}

static ODR_t my_OD_write_650V(OD_stream_t *stream, const void *buf,
			      OD_size_t count, OD_size_t *countWritten) {
  printf("write 650V\n");
  return ODR_OK;
}

CO_ReturnError_t camera_init(uint8_t offset) {
  OD_entry_t * param_650V = NULL;

  switch (offset) {
  case 0:
    param_650V = OD_ENTRY_H6417_HBridgeMotor;
    break;
  case 1:
    param_650V = OD_ENTRY_H6417_HBridgeMotor;
    break;
  default:
    log_print("ERROR, unknown camera offset\n");
    return 1;
  }
  
  OD_650V_extension[offset].object = NULL;
  OD_6412_extension[offset].read = my_OD_read_650V;
  OD_6412_extension[offset].write = my_OD_write_650V;

  OD_extension_init(param_650V, &OD_650V_extension);

  return CO_ERROR_NO;
}

CO_ReturnError_t app_camera_init(uint8_t offset) {
  if ((camera_init(0) == CO_ERROR_NO) && (camera_init(1) == CO_ERROR_NO))
    return CO_ERROR_NO;
  return 1;
}
