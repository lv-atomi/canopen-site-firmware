#include <stdio.h>

#include "brushless.h"

OD_extension_t OD_641D_extension;

static ODR_t my_OD_read_641D(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  printf("read 641D\n");
  *countRead = 4;
  return ODR_OK;
}

static ODR_t my_OD_write_641D(OD_stream_t *stream, const void *buf,
			      OD_size_t count, OD_size_t *countWritten) {
  printf("write 641D\n");
  return ODR_OK;
}

CO_ReturnError_t app_brushless_init() {
  OD_entry_t * param_641D = OD_ENTRY_H641D_brushlessMotor;
  OD_641D_extension.object = NULL;
  OD_641D_extension.read = my_OD_read_641D;
  OD_641D_extension.write = my_OD_write_641D;

  OD_extension_init(param_641D, &OD_641D_extension);

  return CO_ERROR_NO;
}
