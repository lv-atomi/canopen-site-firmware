#include "at32f403a_407_clock.h"
#include "at32f403a_407_misc.h"
#include "subsystem/keyboard.h"
#include "timer.h"
/* #include "key.h" */
/* #include "oled.h" */
/* #include "flash.h" */
#include "can.h"
#include "log.h"
// #include <cstdint>
#include "keyboard.h"
#include <stdint.h>

tick_cb timer_ticks_cb[MAX_TICKS];
uint8_t timer_ticks_cb_num = 0;


crm_clocks_freq_type crm_clocks_freq_struct = {0};

/* uint16_t TimerCount_1ms = 0; */
uint16_t TimerCountms   = 0;
uint16_t TimerCount = 0;
uint32_t TimerTick = 0;


void Timer_Init()
{
  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);
  /* enable tmr1 clock */
  crm_periph_clock_enable(CRM_TMR6_PERIPH_CLOCK, TRUE);
  /* tmr configuration */
  /* time base configuration */
  /* systemclock/24000/10 = 1000hz */
  tmr_base_init(TMR6, TIME_1MS, (crm_clocks_freq_struct.ahb_freq / 10000) - 1);
  tmr_cnt_dir_set(TMR6, TMR_COUNT_UP);

  /* overflow interrupt enable */
  tmr_interrupt_enable(TMR6, TMR_OVF_INT, TRUE);

  /* tmr1 overflow interrupt nvic init */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(TMR6_GLOBAL_IRQn, 7, 7);


  timer_ticks_cb_num = 0;
  
  /* enable tmr6 */
  tmr_counter_enable(TMR6, TRUE);
}

uint32_t get_ticks(void) {
  return TimerTick;
}

void timer_add_tick(tick_cb cb) {
  uint8_t i;

  ASSERT(timer_ticks_cb_num < MAX_TICKS);
  for (i = 0; i < timer_ticks_cb_num; i++)
    if (timer_ticks_cb[i] == cb) return;
  
  timer_ticks_cb[timer_ticks_cb_num++] = cb;
}

void TMR6_GLOBAL_IRQHandler(void) {
  uint8_t i;
  if(tmr_flag_get(TMR6, TMR_OVF_FLAG) != RESET) {
    TimerTick++;
    for (i=0; i<timer_ticks_cb_num; i++){
      timer_ticks_cb[i]();
    }
  }
  tmr_flag_clear(TMR6, TMR_OVF_FLAG);
}

void timer_pause(void) {
  nvic_irq_disable(TMR6_GLOBAL_IRQn);
}

void timer_resume(void){
  nvic_irq_enable(TMR6_GLOBAL_IRQn, 7, 7);
}
