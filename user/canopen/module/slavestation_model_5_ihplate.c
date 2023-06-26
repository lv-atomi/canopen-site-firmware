#includes "slavestation_model_5_ihplate.h"

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
  tmr_baseinit_type tmr_base_init_structure;
  tmr_ocinit_type tmr_oc_init_structure;

  /* Enable clock for TIMER2 and TIMER3 */
  crm_periph_clock_enable(CRM_TMR2_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);

  /* Initialize structures for PWM output */
  tmr_output_default_para_init(&tmr_oc_init_structure);
  tmr_oc_init_structure.oc_mode = TMR_OC_MODE_PWM_1;
  tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_oc_init_structure.oc_output_state = ENABLE;

  /* TIMER2 for FAN_Sense */
  tmr_base_init_structure.period = 1000; // Frequency = 1kHz
  tmr_base_init_structure.prescaler =
      (SystemCoreClock / 1000000) - 1; // To have a 1us resolution
  tmr_base_init_structure.repetition_counter = 0;
  tmr_base_init(TMR2, &tmr_base_init_structure);

  /* TIMER3 for FAN_PWM */
  tmr_base_init(TMR3, &tmr_base_init_structure);
  tmr_oc_init_structure.pulse = 500; // Duty cycle = 50%
  tmr_oc_config(TMR3, TMR_CHANNEL_1, &tmr_oc_init_structure);

  /* Enable timer counter for TIMER2 and TIMER3 */
  tmr_counter_enable(TMR2, TRUE);
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

void usart_init(void) {
  usart_init_type USART_InitStructure;

  /* Enable USART2 clock */
  crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, TRUE);

  /* Configure USART2 parameters */
  USART_InitStructure.baud_rate = 9600;
  USART_InitStructure.data_length = USART_DATA_8BITS;
  USART_InitStructure.stop_bits = USART_STOP_1_BIT;
  USART_InitStructure.parity = USART_PARITY_NONE;
  USART_InitStructure.mode = USART_MODE_TX_RX;
  USART_InitStructure.flow_control = USART_FLOW_CONTROL_NONE;
  usart_init(USART2, &USART_InitStructure);

  /* Enable USART2 */
  usart_enable(USART2, TRUE);
}

void init_slavestation_model_5_ihplate(){
  fan_gpio_init();
  fan_timer_init();
  usart_init();
}
