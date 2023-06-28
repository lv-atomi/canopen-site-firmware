#include "slavestation_14.h"

/*
 * Slaveboard model 1, can station 14:
 * mainly used for air condition control
 * -485 interface
 *     + connect to inverter with modbus
 *     + control inverter output frequency
 *     + recv working status & warnings
 *     + TBD
 * -FAN x3
 *     + Fan speed sense
 *     + Fan speed control
 * -Temperature sensor x3
 *     + DS18B20
 *
 */

/*
 * 4路温度传感器
 * PB13 -> DS18B20/0
 * PB7 -> DS18B20/1
 * PB8 -> DS18B20/2
 * PA8 -> DS18B20/3
 *
 * 3路PWM4线风扇
 * PA9 -> FAN0/PWM       TMR1_CH2
 * PA10 -> FAN0/Sense    TMR1_CH3
 * PB3 -> FAN1/PWM       AF:TMR2_CH2
 * PB4 -> FAN1/Sense     AF:TMR3_CH1
 * PB5 -> FAN2/PWM       AF:TMR3_CH2
 * PB6 -> FAN2/Sense     TMR4_CH1
 *
 * 1路485
 * PB11 -> RX      USART3_RX
 * PB12 -> RE#/DE
 * PB10 -> TX      USART3_TX
 */

typedef struct {
  gpio_type *port;
  uint32_t pin;
} DS18B20Port;

DS18B20Port thermal_sensors[4] = {
    {GPIOB, GPIO_PINS_13},
    {GPIOB, GPIO_PINS_7},
    {GPIOB, GPIO_PINS_8},
    {GPIOA, GPIO_PINS_8},
};

void ds18b20_gpios_init(void)
{
  gpio_init_type gpio_init_struct;

  // Enable clocks
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  // Init structure for GPIO
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_out_type =
      GPIO_OUTPUT_OPEN_DRAIN; // Set open drain for 1-Wire
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP; // Enable internal pull-up resistor

  // Initialize PB13, PB7, PB8 as 1-Wire bus for DS18B20
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_13 | GPIO_PINS_7 | GPIO_PINS_8;
  gpio_init(GPIOB, &gpio_init_struct);

  // Initialize PA8 as 1-Wire bus for DS18B20
  gpio_init_struct.gpio_pins = GPIO_PINS_8;
  gpio_init(GPIOA, &gpio_init_struct);
}

void OneWire_Reset(DS18B20Port *ds_port) {
  // Pull the line low for at least 480us
  gpio_bits_reset(ds_port->port, ds_port->pin);
  delay_us(480);

  // Release the line and wait for 70us
  gpio_bits_set(ds_port->port, ds_port->pin);
  delay_us(70);
}

void OneWire_WriteBit(DS18B20Port *ds_port, uint8_t bit) {
  if (bit) {
    // To write 1, pull the line low for 1us and then release it for at least
    // 60us
    gpio_bits_reset(ds_port->port, ds_port->pin);
    delay_us(1);
    gpio_bits_set(ds_port->port, ds_port->pin);
    delay_us(60);
  } else {
    // To write 0, pull the line low for at least 60us
    gpio_bits_reset(ds_port->port, ds_port->pin);
    delay_us(60);
    gpio_bits_set(ds_port->port, ds_port->pin);
    delay_us(1);
  }
}

uint8_t OneWire_ReadBit(DS18B20Port *ds_port) {
  uint8_t bit = 0;

  // Pull the line low for 1us and then release it
  gpio_bits_reset(ds_port->port, ds_port->pin);
  delay_us(1);
  gpio_bits_set(ds_port->port, ds_port->pin);

  // Wait for 14us and then read the line
  delay_us(14);
  bit = gpio_input_data_read(ds_port->port) & ds_port->pin;

  // Wait for the rest of the read slot (60us - 15us = 45us)
  delay_us(45);

  return bit;
}

void OneWire_WriteByte(DS18B20Port *ds_port, uint8_t byte) {
  for (int i = 0; i < 8; i++) {
    OneWire_WriteBit(ds_port, byte & 0x01);
    byte >>= 1;
  }
}

uint8_t OneWire_ReadByte(DS18B20Port *ds_port) {
  uint8_t byte = 0;

  for (int i = 0; i < 8; i++) {
    byte |= (OneWire_ReadBit(ds_port) << i);
  }

  return byte;
}

#define DS18B20_CMD_SKIP_ROM 0xCC
#define DS18B20_CMD_CONVERT_T 0x44
#define DS18B20_CMD_READ_SCRATCHPAD 0xBE

void DS18B20_StartConversion(DS18B20Port *ds_port) {
  OneWire_Reset(ds_port);
  OneWire_WriteByte(ds_port, DS18B20_CMD_SKIP_ROM);
  OneWire_WriteByte(ds_port, DS18B20_CMD_CONVERT_T);
}

float DS18B20_ReadTemperature(DS18B20Port *ds_port) {
  uint8_t scratchpad[9];

  OneWire_Reset(ds_port);
  OneWire_WriteByte(ds_port, DS18B20_CMD_SKIP_ROM);
  OneWire_WriteByte(ds_port, DS18B20_CMD_READ_SCRATCHPAD);

  for (int i = 0; i < 9; i++) {
    scratchpad[i] = OneWire_ReadByte(ds_port);
  }

  // The temperature data is stored in the first two bytes of the scratchpad
  int16_t raw_temperature = scratchpad[0] | (scratchpad[1] << 8);

  // According to the DS18B20 datasheet, the least significant bit of the
  // temperature data represents 1/16th of a degree C. So we divide the raw
  // value by 16.0 to get the actual temperature.
  return raw_temperature / 16.0;
}

void DS18B20_MeasureTemperatures() {
  for (int i = 0; i < sizeof(thermal_sensors) / sizeof(thermal_sensors[0]);
       i++) {
    DS18B20_StartConversion(&thermal_sensors[i]);
  }

  // Wait for the conversions to finish. The maximum conversion time is 750ms
  // according to the DS18B20 datasheet.
  delay_ms(750);

  for (int i = 0; i < sizeof(thermal_sensors) / sizeof(thermal_sensors[0]);
       i++) {
    float temperature = DS18B20_ReadTemperature(&thermal_sensors[i]);
    printf("Temperature sensor %d: %.2f C\n", i, temperature);
  }
}


// Define RS485 Ports
#define RS485_USART USART3
#define RS485_BAUDRATE 9600
#define RS485_RE_DE_PIN GPIO_PINS_12
#define RS485_TX_PIN GPIO_PINS_10
#define RS485_RX_PIN GPIO_PINS_11
#define RS485_BUFFER_SIZE                    128

uint8_t rs485_buffer_rx[RS485_BUFFER_SIZE];
uint8_t rs485_buffer_rx_cnt = 0;                                       

void rs485_config(void)
{
  gpio_init_type gpio_init_struct;

  /* enable the RS485_USART and gpio clock */
  crm_periph_clock_enable(CRM_USART3_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_init_struct);

  /* configure the RS485 USART tx, rx, de pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = RS485_TX_PIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_init_struct.gpio_pins = RS485_RX_PIN;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_init_struct.gpio_pins = RS485_RE_DE_PIN;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_bits_reset(GPIOB, RS485_RE_DE_PIN);

  /* configure USART3 param */
  usart_init(RS485_USART, RS485_BAUDRATE, USART_DATA_8BITS, USART_STOP_1_BIT);

  usart_flag_clear(RS485_USART, USART_RDBF_FLAG);
  usart_interrupt_enable(RS485_USART, USART_RDBF_INT, TRUE);

  usart_receiver_enable(RS485_USART, TRUE);
  usart_transmitter_enable(RS485_USART, TRUE);
  usart_enable(RS485_USART, TRUE);

  nvic_irq_enable(USART3_IRQn, 1, 0);
}

void rs485_send_data(u8* buf, u8 cnt)
{
  gpio_bits_set(GPIOB, GPIO_PINS_12);
  while(cnt--){
    while(usart_flag_get(USART3, USART_TDBE_FLAG) == RESET);
    usart_data_transmit(USART3, *buf++);
  }
  while(usart_flag_get(USART3, USART_TDC_FLAG) == RESET);
  gpio_bits_reset(GPIOB, GPIO_PINS_12);
}

void USART3_IRQHandler(void)
{
  uint16_t tmp;
  
  if(usart_flag_get(USART3, USART_RDBF_FLAG) != RESET)
  {
    tmp = usart_data_receive(USART3);
    if(rs485_buffer_rx_cnt < RS485_BUFFER_SIZE)
    {
      rs485_buffer_rx[rs485_buffer_rx_cnt++] = tmp;
    }
  }
}

// 定义风扇的GPIO
#define FAN0_PWM_PIN GPIO_PINS_9
#define FAN0_SENSE_PIN GPIO_PINS_10
#define FAN1_PWM_PIN GPIO_PINS_3
#define FAN1_SENSE_PIN GPIO_PINS_4
#define FAN2_PWM_PIN GPIO_PINS_5
#define FAN2_SENSE_PIN GPIO_PINS_6

void fan_init(void) {
  gpio_init_type gpio_init_struct;

  /* Enable the GPIOA and GPIOB clock */
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* Initialize as default state */
  gpio_default_para_init(&gpio_init_struct);

  /* Configure FAN PWM pins as alternate function push-pull */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;

  /* GPIOA */
  gpio_init_struct.gpio_pins = FAN0_PWM_PIN;
  gpio_init(GPIOA, &gpio_init_struct);

  /* GPIOB */
  gpio_init_struct.gpio_pins = FAN1_PWM_PIN | FAN2_PWM_PIN;
  gpio_init(GPIOB, &gpio_init_struct);

  /* Configure FAN Sense pins as input */
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;

  /* GPIOA */
  gpio_init_struct.gpio_pins = FAN0_SENSE_PIN;
  gpio_init(GPIOA, &gpio_init_struct);

  /* GPIOB */
  gpio_init_struct.gpio_pins = FAN1_SENSE_PIN | FAN2_SENSE_PIN;
  gpio_init(GPIOB, &gpio_init_struct);

  /* /\* Declare initialization structures *\/ */
  /* tmr_baseinit_type tmr_base_init_structure; */
  /* tmr_ocinit_type tmr_oc_init_structure; */

  /* /\* Enable clock for TMR1, TMR2 and TMR3 *\/ */
  /* crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE); */
  /* crm_periph_clock_enable(CRM_TMR2_PERIPH_CLOCK, TRUE); */
  /* crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE); */

  /* /\* Initialize structures for PWM output *\/ */
  /* tmr_output_default_para_init(&tmr_oc_init_structure); */
  /* tmr_oc_init_structure.oc_mode = TMR_OC_MODE_PWM_1; */
  /* tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH; */
  /* tmr_oc_init_structure.oc_output_state = ENABLE; */

  /* /\* TMR1 for FAN0 *\/ */
  /* tmr_base_init_structure.period = 1000; // Frequency = 20kHz */
  /* tmr_base_init_structure.prescaler = */
  /*     (SystemCoreClock / 20000000) - 1; // To have a 1us resolution */
  /* tmr_base_init_structure.repetition_counter = 0; */
  /* tmr_base_init(TMR1, &tmr_base_init_structure); */

  /* tmr_oc_init_structure.pulse = 500; // Duty cycle = 50% */
  /* tmr_oc_config(TMR1, TMR_CHANNEL_2, &tmr_oc_init_structure); */

  /* /\* TMR2 for FAN1 *\/ */
  /* tmr_base_init(TMR2, &tmr_base_init_structure); */
  /* tmr_oc_config(TMR2, TMR_CHANNEL_2, &tmr_oc_init_structure); */

  /* /\* TMR3 for FAN2 *\/ */
  /* tmr_base_init(TMR3, &tmr_base_init_structure); */
  /* tmr_oc_config(TMR3, TMR_CHANNEL_2, &tmr_oc_init_structure); */

  /* /\* Enable timer counter for TMR1, TMR2 and TMR3 *\/ */
  /* tmr_counter_enable(TMR1, TRUE); */
  /* tmr_counter_enable(TMR2, TRUE); */
  /* tmr_counter_enable(TMR3, TRUE); */
  
  /* 计算预分频器值 */
  uint16_t prescaler_value = (uint16_t)(SystemCoreClock / 24000000) - 1;

  /* 初始化结构体 */
  tmr_output_config_type tmr_oc_init_structure;

  /* 为PWM输出初始化结构体 */
  tmr_output_default_para_init(&tmr_oc_init_structure);
  tmr_oc_init_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_oc_init_structure.oc_idle_state = FALSE;
  tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_oc_init_structure.oc_output_state = TRUE;

  /* TMR1用于FAN0 */
  tmr_base_init(TMR1, 665, prescaler_value);
  tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR1, TMR_CLOCK_DIV1);
  tmr_output_channel_config(TMR1, TMR_SELECT_CHANNEL_1, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, 500); // Duty cycle = 50%
  tmr_output_channel_buffer_enable(TMR1, TMR_SELECT_CHANNEL_1, TRUE);

  /* TMR2用于FAN1 */
  tmr_base_init(TMR2, 665, prescaler_value);
  tmr_cnt_dir_set(TMR2, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR2, TMR_CLOCK_DIV1);
  tmr_output_channel_config(TMR2, TMR_SELECT_CHANNEL_2, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_2, 500); // Duty cycle = 50%
  tmr_output_channel_buffer_enable(TMR2, TMR_SELECT_CHANNEL_2, TRUE);

  /* TMR3用于FAN2 */
  tmr_base_init(TMR3, 665, prescaler_value);
  tmr_cnt_dir_set(TMR3, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR3, TMR_CLOCK_DIV1);
  tmr_output_channel_config(TMR3, TMR_SELECT_CHANNEL_3, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_3, 500); // Duty cycle = 50%
  tmr_output_channel_buffer_enable(TMR3, TMR_SELECT_CHANNEL_3, TRUE);

  /* 启用TMR1，TMR2和TMR3的计时器计数器 */
  tmr_counter_enable(TMR1, TRUE);
  tmr_counter_enable(TMR2, TRUE);
  tmr_counter_enable(TMR3, TRUE);
}

void init_slavestation_14() {
  ds18b20_gpios_init();
  rs485_config();
  fan_init();
}

static ODR_t my_OD_read_6300_to_6302(OD_stream_t *stream, void *buf,
				     OD_size_t count, OD_size_t *countRead) {
  uint8_t fan_idx = (uint8_t)(uintptr_t)stream->object;
  printf("read 630%d, subidx:%d\n", fan_idx, stream->subIndex);

  if (stream->subIndex == 0) {
    CO_setUint8(buf, 3); // highestSub_indexSupported
    *countRead = sizeof(uint8_t);
    return ODR_OK;
  }

  if(stream->subIndex == 1 || stream->subIndex == 2) { 
    // currentSpeed, targetSpeed are uint32_t type
    uint32_t v = 123 /* FIXME: read from your data structure */;
    CO_setUint32(buf, v);
    *countRead = sizeof(uint32_t);
    return ODR_OK;
  }

  return ODR_SUB_NOT_EXIST; // unknown subIndex
}

static ODR_t my_OD_write_6300_to_6302(OD_stream_t *stream, const void *buf,
				      OD_size_t count, OD_size_t *countWritten ) {
  uint8_t fan_idx = (uint8_t)(uintptr_t)stream->object;
  printf("write 630%d, subidx:%d\n", fan_idx, stream->subIndex);

  if (stream->subIndex == 0) {
    return ODR_READONLY;
  }

  if(stream->subIndex == 1) { 
    // currentSpeed is readonly
    return ODR_READONLY;
  }

  if(stream->subIndex == 2) { 
    // targetSpeed is uint32_t type
    uint32_t v = CO_getUint32(buf);
    /* write into your data structure */
    return ODR_OK;
  }

  return ODR_SUB_NOT_EXIST; // unknown subIndex
}


static ODR_t my_OD_read_6303(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  printf("read 6303, subidx:%d\n", stream->subIndex);

  if (stream->subIndex == 0) {
    CO_setUint8(buf, OD_CNT_ARR_6303); // highestSub_indexSupported
    *countRead = sizeof(uint8_t);
    return ODR_OK;
  }

  if(stream->subIndex >= 1 && stream->subIndex <= OD_CNT_ARR_6303) { 
    // 1 to OD_CNT_ARR_6303, int32_t array
    int32_t v = 123 /* FIXME:read from your data structure */;
    CO_setInt32(buf, v);
    *countRead = sizeof(int32_t);
    return ODR_OK;
  }

  return ODR_SUB_NOT_EXIST; // unknown subIndex
}

static ODR_t my_OD_read_6304_6305(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  uint8_t idx = stream->object == OD_ENTRY_H6304_airConditionLoadControl ? 0 : 1;
  printf("read 630%d\n", idx);

  uint8_t v = 123 /* FIXME:read from your data structure */;
  CO_setUint8(buf, v);
  *countRead = sizeof(uint8_t);

  return ODR_OK;
}


OD_extension_t OD_extension_6300;
OD_extension_t OD_extension_6301;
OD_extension_t OD_extension_6302;
OD_extension_t OD_extension_6303;
OD_extension_t OD_extension_6304;
OD_extension_t OD_extension_6305;

CO_ReturnError_t environment_control_module_init() {
  OD_extension_t* OD_extension_arr[] = {&OD_extension_6300, &OD_extension_6301, &OD_extension_6302};

  // Initialize FAN0 to FAN2
  for (uint8_t i = 0; i < 3; i++) {
    OD_entry_t *param = (i == 0)   ? OD_ENTRY_H6300_FAN0
                        : (i == 1) ? OD_ENTRY_H6301_FAN1
                                   : OD_ENTRY_H6302_FAN2;
    OD_extension_arr[i]->object = (OD_entry_t*) (intptr_t) i;
    OD_extension_arr[i]->read = my_OD_read_6300_to_6302;
    OD_extension_arr[i]->write = my_OD_write_6300_to_6302;
    
    if (OD_extension_init(param, OD_extension_arr[i]) != ODR_OK) {
      log_printf("ERROR, unable to extend OD object 630%d\n", i);
      return CO_ERROR_OD_PARAMETERS;
    }
  }

  // Initialize 6303
  OD_extension_6303.object = OD_ENTRY_H6303_thermalDS18B20X4;
  OD_extension_6303.read = my_OD_read_6303;
  OD_extension_6303.write = NULL;

  if (OD_extension_init(OD_ENTRY_H6303_thermalDS18B20X4, &OD_extension_6303) !=
      ODR_OK) {
    log_printf("ERROR, unable to extend OD object 6303\n");
    return CO_ERROR_OD_PARAMETERS;
  }

  // Initialize 6304 - 6305
  OD_extension_t* OD_extension_arr_2[] = {&OD_extension_6304, &OD_extension_6305};

  for (uint8_t i = 0; i < 2; i++) {
    OD_entry_t *param = (i == 0) ? OD_ENTRY_H6304_airConditionLoadControl
                                 : OD_ENTRY_H6305_airConditionStatus;
    OD_extension_arr_2[i]->object = param;
    OD_extension_arr_2[i]->read = my_OD_read_6304_6305;
    OD_extension_arr_2[i]->write = NULL;
    
    if (OD_extension_init(param, OD_extension_arr_2[i]) != ODR_OK) {
      log_printf("ERROR, unable to extend OD object 630%d\n", i + 4);
      return CO_ERROR_OD_PARAMETERS;
    }
  }

  return CO_ERROR_NO;
}
