#includes "slavestation_model_2.h"

/*
 * Slaveboard model 2, can station 1,4,5,6,7:
 * -capacitor displacement measurement
 * -thermocouple / thermal resistor temperature sensor
 *     + AD7705: 2 channels
 * -Motor drive x1
 *     + Hbridge
 *     + Brushless(TBD)
 * -GPinput x2
 * -GPoutput x2 (PWM supported)
 *
 */

/*
 * Motor
 * PA9 -> PWMA  TMR1_CH2
 * PB0 -> PWMB  TMR1_CH2N/TMR3_CH3/TMR8_CH2N
 * PB1 -> DIR
 *
 * Thermocouple / Thermal resistor
 * PA3 -> AD7705_RST
 * PA8 -> AD7705_nCS
 * PB3 -> AD7705_SCLK   SPI3_SCK
 * PB5 -> AD7705_DIN    SPI3_MOSI
 * PB4 -> AD7705_DOUT   SPI3_MISO
 * PA10 -> AD7705_nDRDY
 *
 * GPIO
 * PA0 -> GPIn0     TMR2_CH1/TMR5_CH1
 * PA1 -> GPIn0     TMR2_CH2/TMR5_CH2
 * PA2 -> GPOut0    TMR2_CH3/TMR5_CH3/TMR9_CH1
 * PB6 -> GPOut1    TMR4_CH1
 *
 * Capacitor Displacement Measurement
 * PB14 -> SDATA
 * PB13 -> SCLK
 */

#define NUM_SAMPLES 1000
#define MAX_REG_LENGTH 2
#define AD7705_nCS  GPIO_PINS_8
#define AD7705_nDRDY GPIO_PINS_10
#define SCLK_PIN  GPIO_PINS_13
#define SDATA_PIN GPIO_PINS_14

void AD7705_WriteToReg(uint8_t byteWord);
void AD7705_Read(uint16_t amount, uint8_t regLength);


void init_motor_timer_brush(){
  /* init timer */
  tmr_base_init_type tmr_base_init_structure;
  tmr_oc_init_type tmr_oc_init_structure;

  crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);

  /* Timer base configuration for TMR1 */
  tmr_base_default_para_init(&tmr_base_init_structure);
  tmr_base_init_structure.period = 0xFFFF; // Depending on your PWM frequency
  tmr_base_init_structure.prescaler = 0;
  tmr_base_init_structure.clock_division = TMR_CLK_DIV1;
  tmr_base_init_structure.counter_mode = TMR_COUNTER_MODE_UP;
  tmr_base_init(TMR1, &tmr_base_init_structure);

  /* PWM configuration for TMR1 channel 2 */
  tmr_oc_default_para_init(&tmr_oc_init_structure);
  tmr_oc_init_structure.oc_output_state = TMR_OUTPUT_STATE_ENABLE;
  tmr_oc_init_structure.pulse = 0; // The pulse should be set according to the duty cycle
  tmr_oc_init_structure.oc_polarity = TMR_OC_POLARITY_HIGH;
  tmr_oc_init_structure.oc_mode = TMR_OC_MODE_PWM1;
  tmr_oc_init_structure.oc_idle_state = TMR_OC_IDLE_STATE_RESET;
  
  tmr_oc_init_structure.occ_output_state = TMR_OUTPUT_STATE_ENABLE; // Enable complementary output
  tmr_oc_init_structure.occ_polarity = TMR_OC_POLARITY_HIGH; // Set complementary output polarity
  tmr_oc_init_structure.occ_idle_state = TMR_OC_IDLE_STATE_RESET; // Set complementary output idle state
  
  tmr_oc_init(TMR1, &tmr_oc_init_structure, TMR_CHANNEL_2);

  /* Enable TMR1 counter */
  tmr_counter_enable(TMR1, TRUE);
}

void init_motor_timer_brushless(){
  // Declare structures
  tmr_baseinit_type tmr_base_init_structure;
  tmr_ocinit_type tmr_oc_init_structure;
  tmr_icinit_type tmr_ic_init_structure;

  // Enable clocks for TMR3, TMR1
  crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);

  // Init structure for PWM output
  tmr_output_default_para_init(&tmr_oc_init_structure);
  tmr_oc_init_structure.oc_mode = TMR_OC_MODE_PWM_1;
  tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_oc_init_structure.oc_output_state = ENABLE;

  tmr_base_init_structure.period = 1000; // Frequency = 20kHz
  tmr_base_init_structure.prescaler =
      (SystemCoreClock / 20000000) - 1; // To have a 1us resolution
  tmr_base_init_structure.repetition_counter = 0;
  tmr_base_init(TMR3, &tmr_base_init_structure);

  tmr_oc_init_structure.pulse = 500; // Duty cycle = 50%
  tmr_oc_config(TMR3, TMR_CHANNEL_3, &tmr_oc_init_structure);

  // Enable timer counter for TMR3
  tmr_counter_enable(TMR3, TRUE);

  // Init structure for PWM input
  tmr_input_default_para_init(&tmr_ic_init_structure);
  tmr_ic_init_structure.input_filter_value = 0;
  tmr_ic_init_structure.input_channel_select = TMR_SELECT_CHANNEL_2;
  tmr_ic_init_structure.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
  tmr_ic_init_structure.input_polarity_select = TMR_INPUT_RISING_EDGE;

  tmr_base_init_structure.period = 0xFFFF; // As big as possible
  tmr_base_init_structure.prescaler =
      (SystemCoreClock / 20000000) - 1; // To have a 1us resolution
  tmr_base_init_structure.repetition_counter = 0;
  tmr_base_init(TMR1, &tmr_base_init_structure);

  tmr_input_config(TMR1, &tmr_ic_init_structure, TMR_CHANNEL_INPUT_DIV_1);

  // Enable timer counter for TMR1
  tmr_counter_enable(TMR1, TRUE);
}

void init_motor(bool_t is_brushless) {
  /* for brushless:
   * PA9 -> Speed sense
   * PB0 -> Speed Control
   * PB1 -> Direction
   *
   * for brush motor:
   * PA9 -> PWM phase A
   * PB0 -> PWM phase B
   * PB1 -> no use
   */
  gpio_init_type gpio_initstructure;

  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* gpio configuration for DIR, PWMA and PWMB */
  gpio_default_para_init(&gpio_init_struct);
  gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode           = GPIO_MODE_OUTPUT;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_pins = GPIO_PINS_0 | GPIO_PINS_1;  // PWMB on GPIOB, DIR on GPIOB
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_default_para_init(&gpio_init_struct);
  if (is_brushless){
    gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
    gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
    gpio_initstructure.gpio_mode           = GPIO_MODE_OUTPUT;
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_pins = GPIO_PINS_9;  // speed sense
    gpio_init(GPIOA, &gpio_init_struct);
  } else {
    gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_OPEN_DRAIN;
    gpio_initstructure.gpio_pull           = GPIO_PULL_NONE;
    gpio_initstructure.gpio_mode           = GPIO_MODE_INPUT;
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_pins = GPIO_PINS_9;  // PWMA on GPIOA
    gpio_init(GPIOA, &gpio_init_struct);
  }

  if (is_brushless){
    init_motor_timer_brushless();
  } else {
    init_motor_timer_brush();
  }
}

void init_ad7705_gpio() {
  gpio_init_type gpio_initstructure;
  spi_init_type spi_init_struct;

  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* gpio configuration for output pins: AD7705_RST, AD7705_nCS, AD7705_SCLK,
   * AD7705_DIN */
  gpio_default_para_init(&gpio_init_struct);
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_pins =
      GPIO_PINS_3 | GPIO_PINS_8; // RST and nCS on GPIOA
  gpio_init(GPIOA, &gpio_init_struct);

  //gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_pins =
      GPIO_PINS_3 | GPIO_PINS_5; // SCLK and DIN on GPIOB
  gpio_init(GPIOB, &gpio_init_struct);

  /* gpio configuration for input pins: AD7705_DOUT, AD7705_nDRDY */
  gpio_default_para_init(&gpio_init_struct);
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
  gpio_initstructure.gpio_mode = GPIO_MODE_INPUT;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_4; // DOUT on GPIOB
  gpio_init(GPIOB, &gpio_init_struct);

  //gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_pins = GPIO_PINS_10; // nDRDY on GPIOA
  gpio_init(GPIOA, &gpio_init_struct);

  /* spi init */
  crm_periph_clock_enable(CRM_SPI3_PERIPH_CLOCK, TRUE);
  spi_default_para_init(&spi_init_struct);
  spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
  spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_8;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  spi_init(SPI3, &spi_init_struct);
  spi_enable(SPI3, TRUE);

  AD7705_WriteToReg(0x20);
  AD7705_WriteToReg(0x0C);
  AD7705_WriteToReg(0x10);
  AD7705_WriteToReg(0x40);
  /* 等待/DRDY线低 */
  while(gpio_input_data_bit_read(GPIOA, AD7705_nDRDY) == RESET);
  for (uint16_t a = 0; a < NUM_SAMPLES; a++) {
    AD7705_WriteToReg(0x38);
    AD7705_Read(NUM_SAMPLES, 2);
  }
}

void init_model2_gpio() {
  gpio_init_type gpio_initstructure;

  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* gpio configuration for input pins: GPIn0 */
  gpio_default_para_init(&gpio_init_struct);
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
  gpio_initstructure.gpio_mode = GPIO_MODE_INPUT;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_0 | GPIO_PINS_1; // GPIn0 on GPIOA
  gpio_init(GPIOA, &gpio_init_struct);

  /* gpio configuration for output pins: GPOut0 */
  gpio_default_para_init(&gpio_init_struct);
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_pins = GPIO_PINS_2; // GPOut0 on GPIOA
  gpio_init(GPIOA, &gpio_init_struct);

  /* gpio configuration for output pins: GPOut1 */
  //gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_pins = GPIO_PINS_6; // GPOut1 on GPIOB
  gpio_init(GPIOB, &gpio_init_struct);
}

void init_capacitor_displacement_measurement() {
  gpio_init_type gpio_initstructure;

  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* gpio configuration for input pins: SDATA, SCLK */
  gpio_default_para_init(&gpio_init_struct);
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
  gpio_initstructure.gpio_mode = GPIO_MODE_INPUT;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins =
      GPIO_PINS_14 | GPIO_PINS_13; // SDATA and SCLK on GPIOB
  gpio_init(GPIOB, &gpio_init_struct);
}

void init_slavestation_model_2() {
  bool_t is_brushless = TRUE;
  init_motor(is_brushless);
  init_ad7705_gpio();
  init_model2_gpio();
  init_capacitor_displacement_measurement();
}

void AD7705_WriteToReg(uint8_t byteWord) {
  /* 设置/CS线低 */
  gpio_bits_reset(GPIOA, AD7705_nCS);
  /* 发送字节 */
  while (spi_i2s_flag_get(SPI3, SPI_I2S_TDBE_FLAG) == RESET)
    ;
  spi_i2s_data_transmit(SPI3, reg);
  /* 设置/CS线高 */
  gpio_bits_set(GPIOA, AD7705_nCS);
}

uint16_t * AD7705_Read() {
  static uint16_t buffer[2];
  /* 等待/DRDY线低 */
  while (gpio_input_data_bit_read(GPIOA, AD7705_nDRDY) == RESET)
    ;
  /* 设置/CS线低 */
  gpio_bits_reset(GPIOA, AD7705_nCS);
  for (uint8_t i = 0; i < 2; i++) {
    buffer[i] = 0;
    for (uint8_t b = 0; b < 2; b++) {
      while(spi_i2s_flag_get(SPI3, SPI_I2S_RDBF_FLAG) == RESET);
      buffer[i] = (buffer[i] << 8) | spi_i2s_data_receive(SPI3);
    }
  }
  /* 设置/CS线高 */
  gpio_bits_set(GPIOA, AD7705_nCS);
  return buffer;
}

uint32_t pulse_in(gpio_type *GPIOx, uint16_t GPIO_Pin, flag_status PinState) {
  uint32_t pulseWidth = 0;
  while (gpio_input_data_bit_read(GPIOx, GPIO_Pin) != PinState)
    ;
  while (gpio_input_data_bit_read(GPIOx, GPIO_Pin) == PinState) {
    pulseWidth++;
  }
  return pulseWidth;
}

uint32_t measure(void) {
  uint32_t rawData = 0;
  uint32_t minStartTime = 100000;
  uint32_t maxStartTime = 500000;

  uint32_t startDuration = pulse_in(GPIOB, SCLK_PIN, GPIO_PIN_SET);
  while ((startDuration < minStartTime) || (startDuration > maxStartTime)) {
    startDuration = pulse_in(GPIOB, SCLK_PIN, GPIO_PIN_SET);
  }
  for (uint8_t i = 0; i < 24; i++) {
    rawData = rawData >> 1;
    while (gpio_input_data_bit_read(GPIOB, SCLK_PIN) == RESET)
      ;
    if (gpio_input_data_bit_read(GPIOB, SDATA_PIN) == SET) {
      rawData = rawData | 0x00800000;
    } else {
      rawData = rawData & 0xFF7FFFFF;
    }
    while (gpio_input_data_bit_read(GPIOB, SCLK_PIN) == SET)
      ;
  }
  uint32_t result = rawData & 0x000FFFFF;
  if (rawData & 0x00100000) {
    return result;
  } else {
    return 0;
  }
}
