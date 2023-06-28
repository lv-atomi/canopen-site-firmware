#include "slavestation_model_4.h"

/*
 * Slaveboard model 4, can station 9
 * -Weight sensor x4
 * PA0 -> HX711_CH0 / DOUT
 * PA1 -> HX711_CH0 / CLK

 * PA2 -> HX711_CH0 / DOUT
 * PA3 -> HX711_CH0 / CLK

 * PB0 -> HX711_CH0 / DOUT
 * PB1 -> HX711_CH0 / CLK

 * PB10 -> HX711_CH0 / DOUT
 * PB11 -> HX711_CH0 / CLK
 *
 */

#define MAX_PORTS 4

typedef struct {
  gpio_type * port;
  uint32_t pin_clk;
  uint32_t pin_data;
} WeightSensorPort;

WeightSensorPort Channels[MAX_PORTS] = {
  {GPIOA, GPIO_PINS_0, GPIO_PINS_1},
  {GPIOA, GPIO_PINS_2, GPIO_PINS_3},
  {GPIOB, GPIO_PINS_0, GPIO_PINS_1},
  {GPIOB, GPIO_PINS_10, GPIO_PINS_11},
};


void init_slavestation_model_4(){
  gpio_init_type gpio_initstructure;
  uint8_t i;
  
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  for (i=0; i<MAX_PORTS; i++){
    /* gpio configuration */
    gpio_default_para_init(&gpio_initstructure);
    gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
    gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
    gpio_initstructure.gpio_mode           = GPIO_MODE_OUTPUT;
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_initstructure.gpio_pins = Channels[i].pin_clk;
    gpio_init(Channels[i].port, &gpio_initstructure);
    
    gpio_default_para_init(&gpio_initstructure);
    gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_OPEN_DRAIN;
    gpio_initstructure.gpio_pull           = GPIO_PULL_NONE;
    gpio_initstructure.gpio_mode           = GPIO_MODE_INPUT;
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_initstructure.gpio_pins = Channels[i].pin_data;
    gpio_init(Channels[i].port, &gpio_initstructure);
  }
}

uint32_t weight_read(uint8_t ch){
  uint8_t i;
  uint32_t result = 0;
  
  ASSERT(ch < 4);
  /* set clk to low */
  Channels[ch].port->clr = Channels[ch].pin_clk;
  /* waiting for data ready */
  while((Channels[ch].port->idt & Channels[ch].pin_data) != 0);
  for (i=0; i<25; i++) {		/* sending 25 pulses for channel A, gain 128 mode */
    /* set clk to high */
    Channels[ch].port->scr = Channels[ch].pin_clk;
    delay_us(1);
    /* read data */
    result = result << 1;
    result |= (Channels[ch].port->idt & Channels[ch].pin_data) != 0;
    /* set clk to low */
    Channels[ch].port->clr = Channels[ch].pin_clk;
    delay_us(1);
  }

  return result;
}

uint32_t * weight_read_all() {
  static uint32_t result_buffer[MAX_PORTS] = {0};
  uint8_t i;

  for (i=0; i < MAX_PORTS; i++) {
    result_buffer[i] = weight_read(i);
  }
  return result_buffer;
}

#define MAX_WEIGHT_SENSORS 4

OD_extension_t OD_6600_extension;

static ODR_t my_OD_read_6600(OD_stream_t *stream, void *buf, OD_size_t count,
                             OD_size_t *countRead) {
  printf("read 6600, subidx:%d\n", stream->subIndex);

  if (stream->subIndex == 0) {
    CO_setUint8(buf, MAX_WEIGHT_SENSORS);
    *countRead = sizeof(uint8_t);
  } else if (stream->subIndex <= MAX_WEIGHT_SENSORS) {
    // Assume get_weight_sensor_value is a function that retrieves sensor values
    uint32_t value = 0; // get_weight_sensor_value(stream->subIndex - 1);
    CO_setUint32(buf, value);
    *countRead = sizeof(uint32_t);
  } else {
    return ODR_SUB_NOT_EXIST;
  }

  return ODR_OK;
}

CO_ReturnError_t app_weight_sensor_init() {
  OD_entry_t *param_6600 = OD_ENTRY_H6600_weightSensorX4;
  OD_6600_extension.object = NULL;
  OD_6600_extension.read = my_OD_read_6600;
  OD_6600_extension.write = NULL;

  OD_extension_init(param_6600, &OD_6600_extension);

  return CO_ERROR_NO;
}
