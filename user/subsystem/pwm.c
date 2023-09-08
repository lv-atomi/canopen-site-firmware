#include "pwm.h"
#include "gpio.h"
#include "log.h"
#include <stdint.h>

void tmr_clock_enable(tmr_type *tmr_x)
{
  if (tmr_x == TMR1) {
    crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
  }
#if defined(TMR2)
  else if (tmr_x == TMR2) {
    crm_periph_clock_enable(CRM_TMR2_PERIPH_CLOCK, TRUE);
  }
#endif
#if defined(TMR3)
  else if (tmr_x == TMR3) {
    crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);
  }
#endif
#if defined(TMR4)
  else if (tmr_x == TMR4) {
    crm_periph_clock_enable(CRM_TMR4_PERIPH_CLOCK, TRUE);
  }
#endif
#if defined(TMR5)
  else if (tmr_x == TMR5) {
    crm_periph_clock_enable(CRM_TMR5_PERIPH_CLOCK, TRUE);
  }
#endif
#if defined(TMR6)
  else if (tmr_x == TMR6) {
    crm_periph_clock_enable(CRM_TMR6_PERIPH_CLOCK, TRUE);
  }
#endif
#if defined(TMR7)
  else if (tmr_x == TMR7) {
    crm_periph_clock_enable(CRM_TMR7_PERIPH_CLOCK, TRUE);
  }
#endif
#if defined(TMR8)
  else if (tmr_x == TMR8) {
    crm_periph_clock_enable(CRM_TMR8_PERIPH_CLOCK, TRUE);
  }
#endif
#if defined(TMR9)
  else if (tmr_x == TMR9) {
    crm_periph_clock_enable(CRM_TMR9_PERIPH_CLOCK, TRUE);
  }
#endif
#if defined(TMR10)
  else if (tmr_x == TMR10) {
    crm_periph_clock_enable(CRM_TMR10_PERIPH_CLOCK, TRUE);
  }
#endif
#if defined(TMR11)
  else if (tmr_x == TMR11) {
    crm_periph_clock_enable(CRM_TMR11_PERIPH_CLOCK, TRUE);
  }
#endif
#if defined(TMR12)
  else if (tmr_x == TMR12) {
    crm_periph_clock_enable(CRM_TMR12_PERIPH_CLOCK, TRUE);
  }
#endif
#if defined(TMR13)
  else if (tmr_x == TMR13) {
    crm_periph_clock_enable(CRM_TMR13_PERIPH_CLOCK, TRUE);
  }
#endif
#if defined(TMR14)
  else if (tmr_x == TMR14) {
    crm_periph_clock_enable(CRM_TMR14_PERIPH_CLOCK, TRUE);
  }
#endif
  else {
    ASSERT(FALSE);
  }
}

void init_pwm_output(PWMPort * devport, uint32_t freq, uint16_t duty){
  ASSERT(devport);
  
  tmr_output_config_type tmr_oc_init_structure;
  /* gpio configuration for output pins */
  init_gpio_mux(&devport->port,
		GPIO_OUTPUT_PUSH_PULL,
		GPIO_PULL_NONE,
		GPIO_DRIVE_STRENGTH_STRONGER);

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

  /* compute tmr peroid value & tmr div value according to requested frequency  */
  /* just assume all timers are 16 bits */
  uint32_t pr = crm_clocks_freq_struct.sclk_freq / 100 / freq - 1;
  uint16_t period = 99;
  /* compute the prescaler value */
  /* uint16_t prescaler_value = 0; */
  /* prescaler_value = (uint16_t)(system_core_clock / 24000000) - 1; */
  if (pr > 65535){
    pr = crm_clocks_freq_struct.sclk_freq / 10000 / freq -1;
    period = 9999;
  }
  printf("sys clock: %ld, freq:%ld, pr:%ld, period:%u\n", crm_clocks_freq_struct.sclk_freq, freq, pr, period);
  /* tmr time base configuration */
  tmr_base_init(devport->tmr, period, (uint16_t)pr);
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
    tmr_oc_init_structure.occ_idle_state = !tmr_oc_init_structure.oc_idle_state;
  }

  tmr_output_channel_config(devport->tmr, devport->channel, &tmr_oc_init_structure);
  pwm_output_update_duty(devport, duty);

  /* automatic output enable, stop, dead time and lock configuration */
  tmr_brkdt_config_type tmr_brkdt_config_struct = {0};
  tmr_brkdt_default_para_init(&tmr_brkdt_config_struct);
  tmr_brkdt_config_struct.brk_enable = TRUE;
  tmr_brkdt_config_struct.auto_output_enable = TRUE;
  tmr_brkdt_config_struct.deadtime = devport->deadtime;
  tmr_brkdt_config_struct.fcsodis_state = TRUE;
  tmr_brkdt_config_struct.fcsoen_state = TRUE;
  tmr_brkdt_config_struct.brk_polarity = TMR_BRK_INPUT_ACTIVE_HIGH;
  tmr_brkdt_config_struct.wp_level = TMR_WP_LEVEL_3;
  tmr_brkdt_config(devport->tmr, &tmr_brkdt_config_struct);

  tmr_output_enable(devport->tmr, TRUE);

  /* tmr enable counter */
  tmr_counter_enable(devport->tmr, TRUE);
}

void pwm_output_update_duty(PWMPort * devport, uint8_t duty){
  ASSERT(devport);
  uint32_t duty_32 = devport->tmr->pr * duty;
  duty_32 /= 100;
  printf("duty set:%u, duty_32:%ld\n", duty, duty_32);
  tmr_channel_value_set(devport->tmr, devport->channel,
			(uint16_t)duty_32);
}

void pwm_output_update_freq(PWMPort * devport, uint32_t freq){
  ASSERT(devport);

  uint32_t duty_32 = tmr_channel_value_get(devport->tmr, devport->channel) * 100 / devport->tmr->pr;
					
  uint32_t pr = system_core_clock / 100 / freq - 1;
  uint16_t period = 99;
  /* compute the prescaler value */
  /* uint16_t prescaler_value = 0; */
  /* prescaler_value = (uint16_t)(system_core_clock / 24000000) - 1; */
  if (pr > 65535){
    pr = system_core_clock / 10000 / freq -1;
    period = 9999;
  }
  tmr_base_init(devport->tmr, period, (uint16_t)pr);
  pwm_output_update_duty(devport, (uint8_t)duty_32);
}

void init_pwm_input(PWMPort * devport){
  ASSERT(devport);

  if (!(devport->channel == TMR_SELECT_CHANNEL_1 ||
      devport->channel == TMR_SELECT_CHANNEL_2)) {
    printf("Period & duty cycle measure only supported on CH1/CH2\n");
    ASSERT(FALSE);
  }
  
  /* gpio configuration for output pins */
  init_gpio_input(&devport->port, GPIO_PULL_NONE, GPIO_DRIVE_STRENGTH_STRONGER);
  
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
  tmr_trigger_input_select(devport->tmr,
			   //TMR_SUB_INPUT_SEL_C2DF2);
			   devport->channel == TMR_SELECT_CHANNEL_1 ? TMR_SUB_INPUT_SEL_C1DF1 : TMR_SUB_INPUT_SEL_C2DF2);

  /* select the sub mode: reset mode */
  tmr_sub_mode_select(devport->tmr, TMR_SUB_RESET_MODE);

  /* enable the sub sync mode */
  tmr_sub_sync_mode_set(devport->tmr, TRUE);

  /* tmr enable counter */
  tmr_counter_enable(devport->tmr, TRUE);
}

void pwm_input_freq_duty_sense(PWMPort * devport, uint32_t * freq, uint8_t * duty) {
  ASSERT(devport);
  ASSERT(freq);
  ASSERT(duty);

  uint16_t ic1value = tmr_channel_value_get(devport->tmr, TMR_SELECT_CHANNEL_1);
  uint16_t ic2value = tmr_channel_value_get(devport->tmr, TMR_SELECT_CHANNEL_2);

  if ((ic1value == 0) || (ic2value == 0)){
    *freq = 0;
    *duty = 0;
  }
  
  *freq = system_core_clock / (devport->channel == TMR_SELECT_CHANNEL_1 ? ic1value : ic2value);

  uint32_t duty_32 = devport->channel == TMR_SELECT_CHANNEL_1 ? ic2value * 100 / ic1value : ic1value *100 / ic2value;
  //printf("ch1:%u ch2:%u, duty:%ld\n", ic1value, ic2value, duty_32);
  *duty = (uint8_t) duty_32;
}
