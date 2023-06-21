#include <stdio.h>

#include "psu.h"

OD_extension_t OD_6412_extension;
OD_extension_t OD_6413_extension;
OD_extension_t OD_6414_extension;
OD_extension_t OD_6415_extension;

static ODR_t my_OD_read_6412(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  printf("read 6412\n");
  *countRead = 4;
  return ODR_OK;
}

static ODR_t my_OD_read_6413(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead)
{
  printf("read 6413\n");
  *countRead = 4;
  return ODR_OK;
}


static ODR_t my_OD_read_6414(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead)
{
  printf("read 6414\n");
  *countRead = 4;
  return ODR_OK;
}

static ODR_t my_OD_read_6415(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead)
{
  printf("read 6415\n");
  *countRead = 4;
  return ODR_OK;
}

static ODR_t my_OD_write_6414(OD_stream_t *stream, const void *buf,
			      OD_size_t count, OD_size_t *countWritten){
  printf("write 6414\n");
  return ODR_OK;
}

static ODR_t my_OD_write_6415(OD_stream_t *stream, const void *buf,
			      OD_size_t count, OD_size_t *countWritten){
  printf("write 6415\n");
  return ODR_OK;
}

CO_ReturnError_t app_psu_init() {
  OD_entry_t * param_6412 = OD_ENTRY_H6412_PSU_CurrentRead;
  OD_6412_extension.object = NULL;
  OD_6412_extension.read = my_OD_read_6412;
  OD_6412_extension.write = NULL;

  OD_extension_init(param_6412, &OD_6412_extension);

  OD_entry_t * param_6413 = OD_ENTRY_H6413_PSU_VoltageRead;
  OD_6413_extension.object = NULL;
  OD_6413_extension.read = my_OD_read_6413;
  OD_6413_extension.write = NULL;

  OD_extension_init(param_6413, &OD_6413_extension);

  OD_entry_t * param_6414 = OD_ENTRY_H6414_PSU_CurentSet;
  OD_6414_extension.object = NULL;
  OD_6414_extension.read = my_OD_read_6414;
  OD_6414_extension.write = my_OD_write_6414;

  OD_extension_init(param_6414, &OD_6414_extension);

  OD_entry_t * param_6415 = OD_ENTRY_H6415_PSU_VoltageSet;
  OD_6415_extension.object = NULL;
  OD_6415_extension.read = my_OD_read_6415;
  OD_6415_extension.write = my_OD_write_6415;

  OD_extension_init(param_6415, &OD_6415_extension);
  

  return CO_ERROR_NO;
}
