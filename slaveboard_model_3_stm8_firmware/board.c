#include "board.h"
#include "spl/STM8S_StdPeriph_Lib/Libraries/STM8S_StdPeriph_Driver/inc/stm8s_gpio.h"
#include "stm8s_conf.h"
#include "stdio.h"
#include <stdint.h>

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
 PC6: PhaseA/Speed Sense     TIM1_ CH1
 PC3: [PhaseB]/Speed Control   TIM1_CH3/TIM1_CH1N
 PD4: PhaseB/[Speed Control]   TIM2_CH1
 PD6: Disable HBridge driver

 GPInput:
 PC5: In0
 PC7: In1

 GPOutput:
 PA3: Out0
 PC4: Out1
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
  gpinput_config(&port->port);
  
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

void gpinput_config(IOPort *devport){
  GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_FL_NO_IT);
}

void gpoutput_config(IOPort *devport, uint8_t default_status){
  GPIO_Init(devport->port, devport->pins,
	    default_status ? GPIO_MODE_OUT_PP_HIGH_SLOW: GPIO_MODE_OUT_PP_LOW_SLOW);
  if (default_status) 
    GPIO_WriteHigh(devport->port, devport->pins);
  else
    GPIO_WriteLow(devport->port, devport->pins);
}


void pwm_output_config(IOPort *devport, uint32_t freq, uint8_t duty) {
  TIM1_DeInit();
  TIM1_TimeBaseInit(0, TIM1_COUNTERMODE_UP, 4095, 0);
  TIM1_OC1Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_ENABLE,
               2048, TIM1_OCPOLARITY_LOW, TIM1_OCNPOLARITY_HIGH, TIM1_OCIDLESTATE_SET,
               TIM1_OCNIDLESTATE_RESET);
  /* TIM1 counter enable */
  TIM1_Cmd(ENABLE);

  /* TIM1 Main Output Enable */
  TIM1_CtrlPWMOutputs(ENABLE);
}

void pwm_input_config(IOPort *devport){
  
}
