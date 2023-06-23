#include <stdint.h>
#include "stm8.h"

// Motor phase:
// PC7 TIM1_CH2  => B-
// PC6 TIM1_CH1  => A-
// PC4 TIM1_CH2N  => B+
// PC3 TIM1_CH1N  => A+

// OPT2: 0x81, NOPT2: 0x7e
//AFR7:1
//AFR0:1

#define PWM_PORT    PC
#define PWM_PIN     (PIN3 | PIN4 | PIN6 | PIN7)
#define DeadTime    3

/* Simple busy loop delay */
void delay(unsigned long count) {
    while (count--)
        nop();
}

int main_pwm(void)
{
    /* Set clock to full speed (16 Mhz) */
    CLK_CKDIVR = 0;

    /* GPIO setup */
    // Set pin data direction as output
    PORT(PWM_PORT, CR2)  &= ~PWM_PIN;
    PORT(PWM_PORT, DDR)  |= PWM_PIN;
    // Set pin as "Push-pull"
    PORT(PWM_PORT, CR1)  |= PWM_PIN;
    /* Set pin as high speed */
    PORT(PWM_PORT, CR2)  |= PWM_PIN;

    /* capture enable compare */
    TIM1_CCMR1 = 0x60;	/* PWM mode.1 */
    TIM1_CCER1 = 0x05;

    /* reload frequency */
    TIM1_ARRH = 0; 
    TIM1_ARRL = 52;

    /* duty cycle */
    TIM1_CCR1H = 0;
    TIM1_CCR1L = 39;

    TIM1_CCER1 |= (uint8_t)(0x50);
    TIM1_CCMR2 = 0x70;
    TIM1_CCR2H = 0;
    TIM1_CCR2L = 13;
    TIM1_DTR = (uint8_t)(DeadTime);

    /* counter enable */
    TIM1_CR1 = 0x01 | 0x80;
    TIM1_BKR = 0x80;

    /* pre-scaler */
    TIM1_PSCRH = 16;
    TIM1_PSCRL = 0;


    while(1) {
      delay(300000L);
    }
}

int main(void){
  CLK_CKDIVR = 0;

  /* GPIO setup */
  // Set pin data direction as output
  PORT(PWM_PORT, CR2)  &= ~PWM_PIN;
  PORT(PWM_PORT, DDR)  |= PWM_PIN;
  // Set pin as "Push-pull"
  PORT(PWM_PORT, CR1)  |= PWM_PIN;
  /* Set pin as high speed */
  PORT(PWM_PORT, CR2)  |= PWM_PIN;

  while(1){
    PORT(PWM_PORT, ODR) = (uint8_t)PIN3;
    delay(2500L);
    PORT(PWM_PORT, ODR) = (uint8_t)PIN4;
    delay(2500L);
    PORT(PWM_PORT, ODR) = (uint8_t)PIN6;
    delay(2500L);
    PORT(PWM_PORT, ODR) = (uint8_t)PIN7;
    delay(2500L);
   } 
}
