#include <stdio.h>

#include "gpio.h"

OD_extension_t OD_6001_extension;
OD_extension_t OD_6201_extension;

/*
 * model1
 * Can station 2,15
 * CN2 -> PA.1
 * CN3 -> PA.2
 * CN4 -> PA.3
 * CN5 -> PB.0
 * CN6 -> PB.1
 * CN7 -> PB.10
 * CN8 -> PB.11
 * CN9 -> PB.12
 *
 * model2
 * Input0 -> PA.0
 * Input1 -> PA.1
 * Output0 -> PA.2
 * Output1 -> PB.6
 */

/* Trigger input */
static ODR_t my_OD_read_6001(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead)
{
  printf("read 6001, subidx:%d\n", stream->subIndex);

  if (stream->subIndex == 0) {
    CO_setUint8(buf, 8);

    *countRead = sizeof(uint8_t);
    return ODR_OK;
  }

  bool_t v = 1;
  CO_setUint8(buf, v);
  *countRead = sizeof(bool_t);
  
  return ODR_OK;
}

/* Trigger output */
static ODR_t my_OD_read_6201(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead)
{
  printf("read 6201, subidx:%d\n", stream->subIndex);
  if (stream->subIndex == 0) {
    CO_setUint8(buf, em->fifoCount);

    *countRead = sizeof(uint8_t);
    return ODR_OK;
  }

  *countRead = 4;
  return ODR_OK;
}

static ODR_t my_OD_write_6201(OD_stream_t *stream, const void *buf,
			      OD_size_t count, OD_size_t *countWritten){
  printf("write 6201, subidx:%d\n", stream->subIndex);
  return OD_writeOriginal(stream, buf, count, countWritten);

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
