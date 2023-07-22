#include "board.h"
/* #include "spl/STM8S_StdPeriph_Lib/Libraries/STM8S_StdPeriph_Driver/inc/stm8s.h" */
/* #include "spl/STM8S_StdPeriph_Lib/Libraries/STM8S_StdPeriph_Driver/inc/stm8s_gpio.h" */
#include "spl/STM8S_StdPeriph_Lib/Libraries/STM8S_StdPeriph_Driver/inc/stm8s_gpio.h"
#include "spl/STM8S_StdPeriph_Lib/Libraries/STM8S_StdPeriph_Driver/inc/stm8s_tim1.h"
#include "stm8s_conf.h"
#include "stdio.h"
#include <stdint.h>
#include "tiny_periph.h"

/*
 Sense:
 PD2: sense1      AIN3
 PD3: sense2      AIN4

 GPIO:
 PA1: LED

 I2C:
 PB4: I2C_SCL
 PB5: I2C_SDA

 Motor:
 PA2: DIR
 PC6: PhaseA/Speed Sense     TIM1_CH1(AF)
 PC3: [PhaseB]/Speed Control   TIM1_CH3 / TIM1_CH1N(AF)
 PC5: PhaseB/[Speed Control]   TIM2_CH1
 PD6: Disable HBridge driver

 GPInput:
 PD4: In0
 PC7: In1

 GPOutput:
 PA3: Out0
 PC4: Out1
*/

/*
  motor brushless mode:
  PC6:TIM1_CH1(AF)       OPT2.AFR0 = 1
  PC5:TIM2_CH1(AF)       OPT2.AFR0 = 1

  motor brush mode:
  PC6: TIM1_CH1(AF)      OPT2.AFR0 = 1
  PC3: TIM1_CH1N(AF)     OPT2.AFR7 = 1

  OPT2.AFR0 = 1:
  PC5->TIM2_CH1
  PC6->TIM1_CH1
  PC7->TIM1_CH2

  OPT2.AFR7 = 1:
  PC3->TIM1_CH1N
  PC4->TIM1_CH2N
*/

/**
  * @addtogroup UART1_Printf
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifdef _RAISONANCE_
  #define PUTCHAR_PROTOTYPE int putchar (char c)
  #define GETCHAR_PROTOTYPE int getchar (void)
#elif defined (_COSMIC_)
  #define PUTCHAR_PROTOTYPE char putchar (char c)
  #define GETCHAR_PROTOTYPE char getchar (void)
#elif defined (_SDCC_)         /* SDCC patch: ensure same types as stdio.h */
  #if SDCC_VERSION >= 30605      // declaration changed in sdcc 3.6.5 (officially with 3.7.0)
    #define PUTCHAR_PROTOTYPE int putchar(int c)
    #define GETCHAR_PROTOTYPE int getchar(void)
  #else
    #define PUTCHAR_PROTOTYPE void putchar(char c)
    #define GETCHAR_PROTOTYPE char getchar(void)
  #endif 
#else /* _IAR_ */
  #define PUTCHAR_PROTOTYPE int putchar (int c)
  #define GETCHAR_PROTOTYPE int getchar (void)
#endif /* _RAISONANCE_ */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief Retargets the C library printf function to the UART.
  * @param c Character to send
  * @retval char Character sent
  */
PUTCHAR_PROTOTYPE
{
  /* Write a character to the UART1 */
  UART1_SendData8(c);
  /* Loop until the end of transmission */
  while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);

  return (c);
}

/**
  * @brief Retargets the C library scanf function to the USART.
  * @param None
  * @retval char Character to Read
  */
GETCHAR_PROTOTYPE
{
#ifdef _COSMIC_
  char c = 0;
#else
  int c = 0;
#endif
  /* Loop until the Read data register flag is SET */
  while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);
    c = UART1_ReceiveData8();
  return (c);
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
#if defined(_SDCC_)
#pragma save
#pragma disable_warning 85
#endif

void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#if defined(_SDCC_)
#pragma restore
#endif

#endif

/**
  * @}
  */

/* Simple busy loop delay, 250000 ~ 0.5s */
void delay_ms(uint16_t count) {
  uint32_t count2;
  while (count--)
    for(count2=0; count2 < 500; count2++)
      nop();
}

uint16_t ADC_readonce(ADCPort * port) {
  ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
	    port->channel,
	    ADC1_PRESSEL_FCPU_D4,					\
            ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, port->channel,	\
            DISABLE);

  /*Start Conversion */
  ADC1_StartConversion();
  return ADC1_GetConversionValue();
}

void adc_config(ADCPort * port){
  /*  Init GPIO for ADC1 */
  gpinput_config(&port->port, GPIO_MODE_IN_FL_NO_IT);
  
  /* De-Init ADC peripheral*/
  ADC1_DeInit();
  ADC_readonce(port);
}


void board_init(){
  /*High speed internal clock prescaler: 1*/
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  UART1_DeInit();
  /* UART1 configuration ------------------------------------------------------*/
  /* UART1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Receive and transmit enabled
        - UART1 Clock disabled
  */
  UART1_Init((uint32_t)115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
              UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);

  /* Output a message on Hyperterminal using printf function */
  printf("\nUART1 inited.\n");

  uint8_t opt2 = OPT->OPT2;
  uint8_t *opt2_ptr = &OPT->OPT2;
  opt2 |= 0b10000001;		/* set afr0 & afr7 to 1 */
  if(opt2 != *opt2_ptr)
    FLASH_ProgramOptionByte((uint16_t)opt2_ptr, opt2);
}

uint32_t my_round(uint32_t val){
  /* printf("round in:%ld\n", val); */
  uint32_t integer = val / 100;
  /* printf("round integer:%ld\n", integer); */
  uint32_t decimal = val - integer * 100;
  /* printf("round decimal:%ld\n", decimal); */
  if (decimal > 50) return integer+1;
  return integer;
}

uint16_t sense_position(ADCPort * sense0, ADCPort * sense1){
  uint16_t sense1_val = ADC_readonce(sense0);
  /* printf("sense1:%u\n", sense1_val); */
  uint16_t sense2_val = ADC_readonce(sense1);
  /* printf("sense2:%u\n", sense2_val); */
  uint32_t total = 409600;
  /* printf("total:%ld\n", total); */
  uint16_t num_boards;
  uint16_t curr_board_num;
  uint32_t delta = sense1_val;
  delta -= sense2_val;
  
  num_boards = my_round(total / delta);
  curr_board_num = my_round((uint32_t)sense1_val * 100 / (total / num_boards / 100));

  /* printf("sense1:%u, sense2:%u delta:%ld num_boards:%u curr_board_num:%u\n", */
  /* 	 sense1_val, sense2_val, delta, num_boards, curr_board_num); */
  return curr_board_num;
}

void gpinput_config(IOPort *devport, GPIO_Mode_TypeDef mode){
  GPIO_Init(devport->port, devport->pins, mode);
}

void gpoutput_config(IOPort *devport, uint8_t default_status){
  GPIO_Init(devport->port, devport->pins,
	    default_status ? GPIO_MODE_OUT_PP_HIGH_SLOW: GPIO_MODE_OUT_PP_LOW_SLOW);
  if (default_status) 
    GPIO_WriteHigh(devport->port, devport->pins);
  else
    GPIO_WriteLow(devport->port, devport->pins);
}

void gpio_set(IOPort * devport, uint8_t bit) {
  /* ASSERT(devport); */
  if (bit)
    GPIO_WriteHigh(devport->port, devport->pins);
  else
    GPIO_WriteLow(devport->port, devport->pins);
}

uint8_t gpio_read(IOPort *devport) {
  return GPIO_ReadInputPin(devport->port, devport->pins);
}

void calc_period(uint32_t freq, uint16_t * period, uint16_t * cycle){
  uint32_t clock = CLK_GetClockFreq();
  uint32_t _period = clock / 100 / freq - 1;
  printf("clock:%ld freq:%ld period:%d\n", clock, freq, period);
  if(_period > 65535){
    *period = clock / 1000 / freq - 1;
    *cycle = 999;
  } else {
    *period = _period;
    *cycle = 99;
  }
}
void tmr1_ch1_ch1n_output(uint32_t freq, uint8_t duty) {
  uint16_t period = 0, cycle=0;
  calc_period(freq, &period, &cycle);
  TIM1_DeInit();
  TIM1_TimeBaseInit(period, TIM1_COUNTERMODE_UP, cycle, 0);
  TIM1_OC1Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_ENABLE,
               cycle / 2, TIM1_OCPOLARITY_LOW, TIM1_OCNPOLARITY_LOW, TIM1_OCIDLESTATE_SET,
               TIM1_OCNIDLESTATE_RESET);
  /* TIM1 counter enable */
  TIM1_Cmd(ENABLE);

  /* TIM1 Main Output Enable */
  TIM1_CtrlPWMOutputs(ENABLE);
}

void tmr2_ch1_output(uint32_t freq, uint8_t duty) {
  uint16_t period = 0, cycle=0;
  calc_period(freq, &period, &cycle);
  uint32_t v = 2, clock = CLK_GetClockFreq();
  
  TIM2_DeInit();
  
  for (uint8_t i=0; i<=TIM2_PRESCALER_32768; i++){
    printf("i:%u v:%ld period:%d\n", i, v, period);
    if (period < v) {
      period = i;
      cycle = clock / (v/2) / freq;
      break;
    }
    v*=2;
  }
  printf("v:%ld cycle:%d period:%d\n", v, cycle, period);
  TIM2_TimeBaseInit(period, cycle);
  /* TIM2_TimeBaseInit(TIM2_PRESCALER_1, 999); */
  
  TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 
               cycle / 2, TIM2_OCPOLARITY_LOW);
  TIM2_OC1PreloadConfig(ENABLE);
  TIM2_ARRPreloadConfig(ENABLE);
  /* TIM2 enable counter */
  TIM2_Cmd(ENABLE);
}

void tmr1_duty_update(uint8_t duty) {
  uint32_t period = TIM1->ARRH;
  period = period << 8;
  period |= TIM1->ARRL;

  period = period * duty / 100;
  
  TIM1->CCR1H = (uint8_t)(period >> 8);
  TIM1->CCR1L = (uint8_t)(period);
}

void tmr2_duty_update(uint8_t duty) {
  uint32_t period = TIM2->ARRH;
  period = period << 8;
  period |= TIM2->ARRL;

  period = period * duty / 100;
  
  TIM2->CCR1H = (uint8_t)(period >> 8);
  TIM2->CCR1L = (uint8_t)(period);
}

void tmr1_ch1_sense(){
  uint32_t TIM1ClockFreq = 2000000;
  __IO uint32_t LSIClockFreq = 0;
  uint16_t ICValue1 =0, ICValue2 =0, ICValue3=0;
  uint16_t counter = 0;

  TIM1_DeInit();
  TIM1_TimeBaseInit(0, TIM1_COUNTERMODE_UP, 65000, 0);
  TIM1_ICInit( TIM1_CHANNEL_1, TIM1_ICPOLARITY_RISING, TIM1_ICSELECTION_DIRECTTI,
               TIM1_ICPSC_DIV1, 0x0);
  TIM1_ICInit( TIM1_CHANNEL_2, TIM1_ICPOLARITY_FALLING, TIM1_ICSELECTION_INDIRECTTI,
               TIM1_ICPSC_DIV1, 0x0);

  /* Clear CC1 Flag*/
  TIM1_ClearFlag(TIM1_FLAG_CC1);
  TIM1_ClearFlag(TIM1_FLAG_CC2);

  /* Enable TIM1 */
  TIM1_Cmd(ENABLE);
  
  /* /\* wait a capture on CC1 *\/ */
  /* while((TIM1->SR1 & TIM1_FLAG_CC1) != TIM1_FLAG_CC1); */
  /* /\* Get CCR1 value*\/ */
  /* ICValue1 = TIM1_GetCapture1(); */
  /* /\* TIM1_ClearFlag(TIM1_FLAG_CC1); *\/ */

  /* wait a capture on cc2 */
  counter = 0;
  while (((TIM1->SR1 & TIM1_FLAG_CC2) != TIM1_FLAG_CC2)&&(counter > 65000)) counter++;
  /* Get CCR2 value*/
  ICValue1 = TIM1_GetCapture1();
  ICValue2 = TIM1_GetCapture2();
  TIM1_ClearFlag(TIM1_FLAG_CC1);
  /* TIM1_ClearFlag(TIM1_FLAG_CC2); */

  /* wait a capture on CC1 */
  counter = 0;
  while (((TIM1->SR1 & TIM1_FLAG_CC1) != TIM1_FLAG_CC1)&&(counter < 65000)) counter++;
  /* Get CCR1 value*/
  ICValue3 = TIM1_GetCapture1();
  TIM1_ClearFlag(TIM1_FLAG_CC1);

  /* Compute LSI clock frequency */
  LSIClockFreq = CLK_GetClockFreq() / (ICValue3 - ICValue1);
  uint32_t duty = (ICValue2-ICValue1);
  duty = duty * 100 / (ICValue3 - ICValue1);
  printf("freq:%ld duty:%ld %u %u %u\n", LSIClockFreq, duty, ICValue1, ICValue2, ICValue3);
}
