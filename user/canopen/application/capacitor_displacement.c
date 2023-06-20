#include <stdio.h>

#include "capacitor_displacement.h"

OD_extension_t OD_6416_extension;

static ODR_t my_OD_read_6416(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead)
{
  printf("read 6416\n");
  *countRead = 4;
  return ODR_OK;
}


CO_ReturnError_t app_capacitor_displacement_init() {
  OD_entry_t * param_6416 = OD_ENTRY_H6416_capacitorDisplacement;
  OD_6416_extension.object = NULL;
  OD_6416_extension.read = my_OD_read_6416;
  OD_6416_extension.write = NULL;

  OD_extension_init(param_6416, &OD_6416_extension);

  return CO_ERROR_NO;
}
