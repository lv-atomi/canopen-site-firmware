#include "utils.h"
#include "log.h"

void tmr_clock_enable(tmr_type *tmr_x)
{
  if (tmr_x == TMR1) {
    crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
  } else if (tmr_x == TMR2) {
    crm_periph_clock_enable(CRM_TMR2_PERIPH_CLOCK, TRUE);
  } else if (tmr_x == TMR3) {
    crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);
  } else if (tmr_x == TMR4) {
    crm_periph_clock_enable(CRM_TMR4_PERIPH_CLOCK, TRUE);
  } else if (tmr_x == TMR5) {
    crm_periph_clock_enable(CRM_TMR5_PERIPH_CLOCK, TRUE);
  } else if (tmr_x == TMR6) {
    crm_periph_clock_enable(CRM_TMR6_PERIPH_CLOCK, TRUE);
  } else if (tmr_x == TMR7) {
    crm_periph_clock_enable(CRM_TMR7_PERIPH_CLOCK, TRUE);
  } else if (tmr_x == TMR8) {
    crm_periph_clock_enable(CRM_TMR8_PERIPH_CLOCK, TRUE);
  } else if (tmr_x == TMR9) {
    crm_periph_clock_enable(CRM_TMR9_PERIPH_CLOCK, TRUE);
  } else if (tmr_x == TMR10) {
    crm_periph_clock_enable(CRM_TMR10_PERIPH_CLOCK, TRUE);
  } else if (tmr_x == TMR11) {
    crm_periph_clock_enable(CRM_TMR11_PERIPH_CLOCK, TRUE);
  } else if (tmr_x == TMR12) {
    crm_periph_clock_enable(CRM_TMR12_PERIPH_CLOCK, TRUE);
  } else if (tmr_x == TMR13) {
    crm_periph_clock_enable(CRM_TMR13_PERIPH_CLOCK, TRUE);
  } else if (tmr_x == TMR14) {
    crm_periph_clock_enable(CRM_TMR14_PERIPH_CLOCK, TRUE);
  } else {
    ASSERT(FALSE);
  }
}

void init_pwm_output(PWMPort * devport, uint32_t freq, uint16_t duty){
  ASSERT(devport);
  
  gpio_init_type gpio_initstructure;
  tmr_output_config_type tmr_oc_init_structure;

  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* gpio configuration for output pins */
  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;

  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins = devport->port.pin;
  gpio_init(devport->port.port, &gpio_initstructure);

  if (devport->complementary &&
      (devport->channel == TMR_SELECT_CHANNEL_1C ||
       devport->channel == TMR_SELECT_CHANNEL_2C ||
       devport->channel == TMR_SELECT_CHANNEL_3C)){ /* complementary will be inited in non-complementary channel, we can skip this */
    return;
  }
  
  crm_clocks_freq_type crm_clocks_freq_struct = {0};
  crm_clocks_freq_get(&crm_clocks_freq_struct);
  /* enable tmr1 clock */
  tmr_clock_enable(devport->tmr);
  
  /* compute the prescaler value */
  uint16_t prescaler_value = 0;
  prescaler_value = (uint16_t)(system_core_clock / 24000000) - 1;

  /* tmr time base configuration */
  tmr_base_init(devport->tmr, freq, prescaler_value);
  tmr_cnt_dir_set(devport->tmr, TMR_COUNT_UP);
  tmr_clock_source_div_set(devport->tmr, TMR_CLOCK_DIV1);

  tmr_output_default_para_init(&tmr_oc_init_structure);
  tmr_oc_init_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_oc_init_structure.oc_idle_state = FALSE;
  tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_oc_init_structure.oc_output_state = TRUE;
  if (devport->complementary) {
    tmr_oc_init_structure.occ_output_state = TRUE;
    tmr_oc_init_structure.occ_polarity = tmr_oc_init_structure.oc_polarity;
    tmr_oc_init_structure.occ_idle_state = FALSE;
  }

  tmr_output_channel_config(devport->tmr, devport->channel, &tmr_oc_init_structure);
  tmr_channel_value_set(devport->tmr, devport->channel, duty);

  /* tmr enable counter */
  tmr_counter_enable(devport->tmr, TRUE);
}

void init_pwm_input(PWMPort * devport){
  ASSERT(devport);
  
  gpio_init_type gpio_initstructure;

  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* gpio configuration for output pins */
  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_pull = GPIO_PULL_NONE;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;

  gpio_initstructure.gpio_mode = GPIO_MODE_INPUT;
  gpio_initstructure.gpio_pins = devport->port.pin;
  gpio_init(devport->port.port, &gpio_initstructure);
  
  crm_clocks_freq_type crm_clocks_freq_struct = {0};
  crm_clocks_freq_get(&crm_clocks_freq_struct);
  /* enable tmr1 clock */
  tmr_clock_enable(devport->tmr);
  
  tmr_input_config_type tmr_ic_init_structure;
  
  tmr_input_default_para_init(&tmr_ic_init_structure);
  tmr_ic_init_structure.input_filter_value = 0;
  tmr_ic_init_structure.input_channel_select = devport->channel;
  tmr_ic_init_structure.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
  tmr_ic_init_structure.input_polarity_select = TMR_INPUT_RISING_EDGE;

  tmr_pwm_input_config(devport->tmr, &tmr_ic_init_structure, TMR_CHANNEL_INPUT_DIV_1);

  /* select the tmr input trigger: C2IF2 */
  tmr_trigger_input_select(devport->tmr, TMR_SUB_INPUT_SEL_C2DF2);

  /* select the sub mode: reset mode */
  tmr_sub_mode_select(devport->tmr, TMR_SUB_RESET_MODE);

  /* enable the sub sync mode */
  tmr_sub_sync_mode_set(devport->tmr, TRUE);

  /* tmr enable counter */
  tmr_counter_enable(devport->tmr, TRUE);
}
