#include "slavestation_3.h"

/*
 * Slaveboard model 1, can station 3:
 * dual camera module, for each module:
 * -PSU:
 *     + set supply power voltage & max curret
 *     + monitor power voltage & currentread
 * -Camera:
 *     + Sense camera strobe signal
 *     + Send camera trigger signal
 *
 */

/*
 * PA8 -> camera0.strobe
 * PA9 -> camera0.trigger
 * PA10 -> camera1.strobe
 * PB5 -> camera1.trigger
 */

void camera_gpios_init(void) {
  gpio_init_type gpio_init_struct;

  // Enable clocks
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  // Init structure for GPIO
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;

  // Initialize PA9 and PB5 as Output for trigger
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_9;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_5;
  gpio_init(GPIOB, &gpio_init_struct);

  // Initialize PA8 and PA10 as Input for strobe
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_8 | GPIO_PINS_10;
  gpio_init(GPIOA, &gpio_init_struct);
}

void init_slavestation_3(){
  camera_gpios_init();
}

OD_extension_t OD_6200_extension;
OD_extension_t OD_6201_extension;

static ODR_t my_OD_read_620X(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  uint8_t offset = stream->object == OD_ENTRY_H6200_cameraModule0? 0 : 1;
  printf("read 620%d, subidx:%d\n", offset, stream->subIndex);

  if (stream->subIndex == 0) {
    CO_setUint8(buf, 11); // highestSub_indexSupported
    *countRead = sizeof(uint8_t);
    return ODR_OK;
  }

  if(stream->subIndex == 1 || stream->subIndex == 2) { 
    // strobe, trigger are bool_t type
    bool_t v = 123 /* FIXME:read from your data structure */;
    CO_setUint8(buf, v);
    *countRead = sizeof(bool_t);
    return ODR_OK;
  }

  if(stream->subIndex >= 3 && stream->subIndex <= 11) { 
    // XMove, YMove, ZMove, XTilt, YTilt, ZTilt, zoom, iris, focus are int32_t type
    int32_t v = 123 /* FIXME:read from your data structure */;
    CO_setInt32(buf, v);
    *countRead = sizeof(int32_t);
    return ODR_OK;
  }

  return ODR_SUB_NOT_EXIST; // unknown subIndex
}

static ODR_t my_OD_write_620X(OD_stream_t *stream, const void *buf,
			      OD_size_t count, OD_size_t *countWritten) {
  uint8_t offset = stream->object == OD_ENTRY_H6200_cameraModule0? 0 : 1;
  printf("write 620%d, subidx:%d\n", offset, stream->subIndex);

  if (stream->subIndex == 0) {
    return ODR_READONLY;
  }

  if(stream->subIndex == 1 || stream->subIndex == 2) { 
    // strobe, trigger are bool_t type
    bool_t v = CO_getUint8(buf);
    /* write into your data structure */
    return ODR_OK;
  }

  if(stream->subIndex >= 3 && stream->subIndex <= 11) { 
    // XMove, YMove, ZMove, XTilt, YTilt, ZTilt, zoom, iris, focus are int32_t type
    int32_t v = CO_getInt32(buf);
    /* write into your data structure */
    return ODR_OK;
  }

  return ODR_SUB_NOT_EXIST; // unknown subIndex
}

CO_ReturnError_t dual_camera_module_init() {
  OD_entry_t *param_6200 = OD_ENTRY_H6200_cameraModule0;
  OD_entry_t *param_6201 = OD_ENTRY_H6201_cameraModule1;

  OD_6200_extension.object = param_6200;
  OD_6200_extension.read = my_OD_read_620X;
  OD_6200_extension.write = my_OD_write_620X;

  OD_6201_extension.object = param_6201;
  OD_6201_extension.read =
      my_OD_read_620X; // Replace with the correct function name
  OD_6201_extension.write =
      my_OD_write_620X; // Replace with the correct function name

  if (OD_extension_init(param_6200, &OD_6200_extension) != ODR_OK) {
    log_printf("ERROR, unable to extend OD object 6200\n");
    return CO_ERROR_OD_PARAMETERS;
  }

  if (OD_extension_init(param_6201, &OD_6201_extension) != ODR_OK) {
    log_printf("ERROR, unable to extend OD object 6201\n");
    return CO_ERROR_OD_PARAMETERS;
  }

  return CO_ERROR_NO;
}
