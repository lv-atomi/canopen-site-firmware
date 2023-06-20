#include <stdio.h>

#include "hbridge.h"
#include "thermal_7705.h"
#include "psu.h"

OD_extension_t OD_6417_extension;

static ODR_t my_OD_read_6417(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  printf("read 6417\n");
  *countRead = 4;
  return ODR_OK;
}

static ODR_t my_OD_write_6417(OD_stream_t *stream, const void *buf,
			      OD_size_t count, OD_size_t *countWritten) {
  printf("write 6417\n");
  return ODR_OK;
}

CO_ReturnError_t app_hbridge_init() {
  OD_entry_t * param_6417 = OD_ENTRY_H6417_HBridgeMotor;
  OD_6412_extension.object = NULL;
  OD_6412_extension.read = my_OD_read_6417;
  OD_6412_extension.write = my_OD_write_6417;

  OD_extension_init(param_6417, &OD_6417_extension);

  return CO_ERROR_NO;
}
