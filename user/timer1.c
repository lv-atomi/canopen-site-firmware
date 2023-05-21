#include "at32f403a_407_clock.h"
#include "timer1.h"
#include "gpio.h"
#include "oled.h"
#include "flash.h"
#include "can.h"

crm_clocks_freq_type crm_clocks_freq_struct = {0};

uint16_t TimerCount_1ms = 0;
uint16_t TimerCountms   = 0;
uint16_t TimerCount = 0;
extern KeyPressMode keySta;
extern error_status err_status;
extern uint16_t buffer_write[TEST_BUFEER_SIZE];
extern uint16_t buffer_read[TEST_BUFEER_SIZE];
extern SysEprom_TypeDef gSysEpromData;
void Timer1_Init()
{
  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);
  /* enable tmr1 clock */
  crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
  /* tmr1 configuration */
  /* time base configuration */
  /* systemclock/24000/10 = 1000hz */
  tmr_base_init(TMR1, TIME_1MS, (crm_clocks_freq_struct.ahb_freq / 10000) - 1);
  tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);

  /* overflow interrupt enable */
  tmr_interrupt_enable(TMR1, TMR_OVF_INT, TRUE);

  /* tmr1 overflow interrupt nvic init */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(TMR1_OVF_TMR10_IRQn, 0, 0);

  /* enable tmr1 */
  tmr_counter_enable(TMR1, TRUE);	
	
}

void TMR1_OVF_TMR10_IRQHandler(void) {
  if (tmr_flag_get(TMR1, TMR_OVF_FLAG) != RESET) {
    /* add user code... */
    TimerCountms++;
    if (TimerCountms % 100 == 0) // 100ms can send circle
    {
      TimerCount_1ms = 1;
    }
    if (TimerCountms % 300 == 0) // key press circly is 300 ms
    {
      Key_ScanFun();
    }
    if (TimerCountms > 10000)
      TimerCountms = 0;

    if (keySta.KeyCnt10s >= 1) {
      keySta.KeyCnt10s--;
      if (keySta.KeyCnt10s == 0) {
        keySta.Dismode = 0;
        /* write data to flash */
        err_status = flash_write(TEST_FLASH_ADDRESS_START, buffer_write,
                                 TEST_BUFEER_SIZE);
      }
    }
    tmr_flag_clear(TMR1, TMR_OVF_FLAG);
  }
}
