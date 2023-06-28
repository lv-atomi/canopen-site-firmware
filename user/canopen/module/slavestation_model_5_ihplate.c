#include "slavestation_model_5_ihplate.h"

/*
 * Slaveboard model 5, IHPlate:
 * -Inductor heater controller
 *     + Line Current sense
 *     + Line Voltage sense
 *     + IGBT temperature sense
 *     + Plate temperature sense
 * -IH system load control
 * -FAN control x1
 *     + Fan speed sense
 *     + Fan spped control
 *
 */

/*
 * PB3 -> FAN_Sense     AF:TMR2_CH2
 * PB4 -> FAN_PWM       AF:TMR3_CH1
 *
 * PA1 -> slave/BOOT0
 * PA2:USART2_TX -> slave/PA3:USART2_RX
 * PA3:USART2_RX -> slave/PA2:USART2_TX
 * PB0 -> slave/nRST
 */

void fan_gpio_init(void) {
  gpio_init_type GPIO_InitStructure;

  /* Enable GPIOB clock */
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* Configure PB3 as input for FAN_Sense */
  gpio_default_para_init(&GPIO_InitStructure);
  GPIO_InitStructure.gpio_pins = GPIO_PINS_3;
  GPIO_InitStructure.gpio_mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOB, &GPIO_InitStructure);

  /* Configure PB4 as AF_PP for FAN_PWM */
  GPIO_InitStructure.gpio_pins = GPIO_PINS_4;
  GPIO_InitStructure.gpio_mode = GPIO_MODE_MUX;
  GPIO_InitStructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init(GPIOB, &GPIO_InitStructure);
}

void fan_timer_init(void) {
  tmr_output_config_type tmr_oc_init_structure;
  tmr_input_config_type tmr_ic_init_structure;

  uint16_t prescaler_value;
  prescaler_value = (uint16_t)(system_core_clock / 24000000) - 1;

  /* Enable clock for TIMER2 and TIMER3 */
  crm_periph_clock_enable(CRM_TMR2_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);

  /* TIMER2 for FAN_Sense */
  tmr_input_default_para_init(&tmr_ic_init_structure);
  tmr_ic_init_structure.input_filter_value = 0;
  tmr_ic_init_structure.input_channel_select = TMR_SELECT_CHANNEL_2;
  tmr_ic_init_structure.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
  tmr_ic_init_structure.input_polarity_select = TMR_INPUT_RISING_EDGE;

  tmr_pwm_input_config(TMR2, &tmr_ic_init_structure, TMR_CHANNEL_INPUT_DIV_1);
  tmr_trigger_input_select(TMR2, TMR_SUB_INPUT_SEL_C2DF2);
  tmr_sub_mode_select(TMR2, TMR_SUB_RESET_MODE);
  tmr_sub_sync_mode_set(TMR2, TRUE);
  tmr_counter_enable(TMR2, TRUE);
  tmr_interrupt_enable(TMR2, TMR_C2_INT, TRUE);

  /* TIMER3 for FAN_PWM */
  tmr_output_default_para_init(&tmr_oc_init_structure);
  tmr_oc_init_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_oc_init_structure.oc_idle_state = FALSE;
  tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_oc_init_structure.oc_output_state = TRUE;

  tmr_base_init(TMR3, 665, prescaler_value);
  tmr_cnt_dir_set(TMR3, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR3, TMR_CLOCK_DIV1);

  tmr_output_channel_config(TMR3, TMR_SELECT_CHANNEL_1, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, 500); // Set the pulse width
  tmr_output_channel_buffer_enable(TMR3, TMR_SELECT_CHANNEL_1, TRUE);
  tmr_period_buffer_enable(TMR3, TRUE);
  tmr_counter_enable(TMR3, TRUE);
}

void system_init(void) {
  gpio_init_type GPIO_InitStructure;

  /* Enable GPIOA and GPIOB clock */
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* Configure PA1 as output for slave/BOOT0 */
  gpio_default_para_init(&GPIO_InitStructure);
  GPIO_InitStructure.gpio_pins = GPIO_PINS_1;
  GPIO_InitStructure.gpio_mode = GPIO_MODE_OUTPUT;
  GPIO_InitStructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init(GPIOA, &GPIO_InitStructure);

  /* Configure PB0 as output for slave/nRST */
  GPIO_InitStructure.gpio_pins = GPIO_PINS_0;
  gpio_init(GPIOB, &GPIO_InitStructure);

  /* Configure PA2 as output for USART2_TX */
  GPIO_InitStructure.gpio_pins = GPIO_PINS_2;
  gpio_init(GPIOA, &GPIO_InitStructure);

  /* Configure PA3 as input for USART2_RX */
  GPIO_InitStructure.gpio_pins = GPIO_PINS_3;
  GPIO_InitStructure.gpio_mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.gpio_pull = GPIO_PULL_UP;
  gpio_init(GPIOA, &GPIO_InitStructure);
}

void ih_usart_init(void) {
  /* Enable USART2 clock */
  crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, TRUE);

  /* Configure USART2 parameters */
  usart_init(USART2, 9600, USART_DATA_8BITS, USART_STOP_1_BIT);
  usart_transmitter_enable(USART2, TRUE);
  usart_receiver_enable(USART2, TRUE);

  /* Enable USART2 */
  usart_enable(USART2, TRUE);
}

void init_slavestation_model_5_ihplate(){
  fan_gpio_init();
  fan_timer_init();
  ih_usart_init();
}

OD_extension_t OD_6700_extension;
OD_extension_t OD_6701_extension;
OD_extension_t OD_6702_extension;
OD_extension_t OD_6703_extension;
OD_extension_t OD_6704_extension;
OD_extension_t OD_6705_extension;

static ODR_t my_OD_read_6700_to_6703(OD_stream_t *stream, void *buf,
                                  OD_size_t count, OD_size_t *countRead) {
  uint32_t *object = (uint32_t *)stream->object; /* FIXME */
  *countRead = sizeof(*object);
  if (count < *countRead) {
    return ODR_OK;
  }
  memcpy(buf, object, *countRead);
  return ODR_OK;
}

static ODR_t my_OD_read_6705(OD_stream_t *stream, void *buf, OD_size_t count,
                          OD_size_t *countRead) {
  /* FIXME */
  *countRead = 2;
  if (count < *countRead) {
    return ODR_OK;
  }
  //memcpy(buf, object, *countRead);
  return ODR_OK;
}

static ODR_t my_OD_write_6704(OD_stream_t *stream, const void *buf,
                           OD_size_t count, OD_size_t *countWritten) {
  /* FIXME */
  uint32_t *object = (uint32_t *)stream->object;
  if (count < sizeof(*object)) {
    return ODR_OK;
  }
  //memcpy(object, buf, count);
  *countWritten = count;
  return ODR_OK;
}

static ODR_t my_OD_write_6705(OD_stream_t *stream, const void *buf,
                           OD_size_t count, OD_size_t *countWritten) {
  /* FIXME */
  /* if (count < sizeof(object->targetSpeed)) { */
  /*   return CO_ERROR_NO; */
  /* } */
  //memcpy(&(object->targetSpeed), buf, count);
  *countWritten = count;
  return ODR_OK;
}

CO_ReturnError_t line_module_init() {
  // Line Current
  OD_6700_extension.object = OD_ENTRY_H6700_lineCurrent;
  OD_6700_extension.read = my_OD_read_6700_to_6703;
  OD_6700_extension.write = NULL;
  if (OD_extension_init(OD_ENTRY_H6700_lineCurrent, &OD_6700_extension) !=
      ODR_OK) {
    log_printf("ERROR, unable to extend OD object 6700\n");
    return CO_ERROR_OD_PARAMETERS;
  }

  // Line Voltage
  OD_6701_extension.object = OD_ENTRY_H6701_lineVoltage;
  OD_6701_extension.read = my_OD_read_6700_to_6703;
  OD_6701_extension.write = NULL;
  if (OD_extension_init(OD_ENTRY_H6701_lineVoltage, &OD_6701_extension) !=
      ODR_OK) {
    log_printf("ERROR, unable to extend OD object 6701\n");
    return CO_ERROR_OD_PARAMETERS;
  }

  // IGBT Temperature
  OD_6702_extension.object = OD_ENTRY_H6702_IGBT_Temperature;
  OD_6702_extension.read = my_OD_read_6700_to_6703;
  OD_6702_extension.write = NULL;
  if (OD_extension_init(OD_ENTRY_H6702_IGBT_Temperature, &OD_6702_extension) !=
      ODR_OK) {
    log_printf("ERROR, unable to extend OD object 6702\n");
    return CO_ERROR_OD_PARAMETERS;
  }

  // Plate Temperature
  OD_6703_extension.object = OD_ENTRY_H6703_plateTemperature;
  OD_6703_extension.read = my_OD_read_6700_to_6703;
  OD_6703_extension.write = NULL;
  if (OD_extension_init(OD_ENTRY_H6703_plateTemperature, &OD_6703_extension) !=
      ODR_OK) {
    log_printf("ERROR, unable to extend OD object 6703\n");
    return CO_ERROR_OD_PARAMETERS;
  }

  // System Load
  OD_6704_extension.object = OD_ENTRY_H6704_systemLoad;
  OD_6704_extension.read = my_OD_read_6700_to_6703;
  OD_6704_extension.write = my_OD_write_6704;
  if (OD_extension_init(OD_ENTRY_H6704_systemLoad, &OD_6704_extension) !=
      ODR_OK) {
    log_printf("ERROR, unable to extend OD object 6704\n");
    return CO_ERROR_OD_PARAMETERS;
  }

  // FAN0
  OD_6705_extension.object = OD_ENTRY_H6705_FAN0;
  OD_6705_extension.read = my_OD_read_6705;
  OD_6705_extension.write = my_OD_write_6705;
  if (OD_extension_init(OD_ENTRY_H6705_FAN0, &OD_6705_extension) !=
      ODR_OK) {
    log_printf("ERROR, unable to extend OD object 6705\n");
    return CO_ERROR_OD_PARAMETERS;
  }

  return CO_ERROR_NO;
}
