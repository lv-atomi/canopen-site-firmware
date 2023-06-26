#includes "slavestation_2_15.h"

/*
 * Slaveboard model 1, can station 2, 15:
 * -GPinput x8
 *     + 6 for limit switch of XYZ system: X-/+, Y-/+, Z-/+
 *     + 2 for limit switch of cutter: P1/ P2
 * -Cutter motor driver
 *     + Hbridge: speed control
 *     + Brushless: speed sense, speed control
 *
 */

/*
 * Motor
 * PB8 -> Speed sense   TMR4_CH3 / TMR10_CH1
 * PB7 -> Direction
 * PB6 -> Speed Control TMR4_CH1
 *
 * XYZ三轴的左和右限位
 * PA3 -> CN4,
 * PB0 -> CN5,
 * PB1 -> CN6,
 * PB10 -> CN7,
 * PB11 -> CN8,
 * PB12 -> CN9,
 * 切割限位，分别是左和右
 * PA2 -> CN3,
 * PA1 -> CN2,
 */

void motor_init(void) {
  gpio_init_type gpio_init_struct;
  tmr_baseinit_type tmr_base_init_structure;
  tmr_ocinit_type tmr_oc_init_structure;
  tmr_icinit_type tmr_ic_init_structure;

  // Enable clocks
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR4_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR10_PERIPH_CLOCK, TRUE);

  // Init structure for GPIO
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init_struct.gpio_mode = GPIO_MODE_AF_PP;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_pins = GPIO_PINS_6 | GPIO_PINS_8; // PB6 and PB8
  gpio_init(GPIOB, &gpio_init_struct);

  // Init structure for DIR (PB7)
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_7; // PB7
  gpio_init(GPIOB, &gpio_init_struct);

  // Init structure for PWM output
  tmr_base_default_para_init(&tmr_base_init_structure);
  tmr_base_init_structure.period = 20000;
  tmr_base_init_structure.prescaler = (SystemCoreClock / 20000000) - 1;
  tmr_base_init_structure.repetition_counter = 0;

  tmr_oc_default_para_init(&tmr_oc_init_structure);
  tmr_oc_init_structure.oc_mode = TMR_OC_MODE_PWM_1;
  tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_oc_init_structure.oc_output_state = ENABLE;
  tmr_oc_init_structure.pulse = 10000; // 50% Duty cycle

  // Init TMR4 for PWM
  tmr_base_init(TMR4, &tmr_base_init_structure);
  tmr_oc_config(TMR4, TMR_CHANNEL_1,
                &tmr_oc_init_structure); // Channel 1 for Speed Control

  // Init TMR10 for RPM sensing
  tmr_base_init(TMR10, &tmr_base_init_structure);
  tmr_oc_config(TMR10, TMR_CHANNEL_1,
                &tmr_oc_init_structure); // Channel 1 for RPM sensing

  // Enable timer counter for TMR4 and TMR10
  tmr_counter_enable(TMR4, TRUE);
  tmr_counter_enable(TMR10, TRUE);
}

void limit_switches_init(void)
{
  gpio_init_type gpio_init_struct;

  // Enable clocks
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  // Init structure for GPIO
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;

  // Initialize PA1, PA2, and PA3
  gpio_init_struct.gpio_pins = GPIO_PINS_1 | GPIO_PINS_2 | GPIO_PINS_3;
  gpio_init(GPIOA, &gpio_init_struct);

  // Initialize PB0, PB1, PB10, PB11, PB12
  gpio_init_struct.gpio_pins =
      GPIO_PINS_0 | GPIO_PINS_1 | GPIO_PINS_10 | GPIO_PINS_11 | GPIO_PINS_12;
  gpio_init(GPIOB, &gpio_init_struct);
}

void init_slavestation_2_15(){
  init_motor();
  init_limit_switch();
}

