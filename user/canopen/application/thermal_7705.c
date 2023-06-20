#include <stdio.h>

#include "thermal_7705.h"
#include "psu.h"

OD_extension_t OD_6002_extension;

static ODR_t my_OD_read_6002(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead)
{
  printf("read 6002\n");
  *countRead = 4;
  return ODR_OK;
}

CO_ReturnError_t app_capacitor_displacement_init() {
  OD_entry_t * param_6002 = OD_ENTRY_H6002_thermal7705;
  OD_6412_extension.object = NULL;
  OD_6412_extension.read = my_OD_read_6002;
  OD_6412_extension.write = NULL;

  OD_extension_init(param_6002, &OD_6002_extension);

  return CO_ERROR_NO;
}
