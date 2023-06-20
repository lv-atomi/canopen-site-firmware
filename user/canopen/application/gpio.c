#include <stdio.h>

#include "gpio.h"

OD_extension_t OD_6001_extension;
OD_extension_t OD_6201_extension;

static ODR_t my_OD_read_6001(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead)
{
  printf("read 6001\n");
  *countRead = 4;
  return ODR_OK;
}

static ODR_t my_OD_read_6201(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead)
{
  printf("read 6201\n");
  *countRead = 4;
  return ODR_OK;
}

static ODR_t my_OD_write_6201(OD_stream_t *stream, const void *buf,
			      OD_size_t count, OD_size_t *countWritten){
  printf("write 6201\n");
  return ODR_OK;
}

CO_ReturnError_t app_gpio_init() {
  OD_entry_t * param_6001 = OD_ENTRY_H6001_triggerInput;
  OD_6001_extension.object = NULL;
  OD_6001_extension.read = my_OD_read_6001;
  OD_6001_extension.write = NULL;

  OD_extension_init(param_6001, &OD_6001_extension);

  OD_entry_t * param_6201 = OD_ENTRY_H6201_triggerOutput;
  OD_6001_extension.object = NULL;
  OD_6001_extension.read = my_OD_read_6201;
  OD_6001_extension.write = my_OD_write_6201;

  OD_extension_init(param_6001, &OD_6001_extension);
  return CO_ERROR_NO;
}
