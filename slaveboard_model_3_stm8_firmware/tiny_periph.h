#ifndef TINY_PERIPH_H
#define TINY_PERIPH_H

#include "stm8s_conf.h"

void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_Pin_TypeDef GPIO_Pin,
               GPIO_Mode_TypeDef GPIO_Mode);

void GPIO_WriteLow(GPIO_TypeDef *GPIOx, GPIO_Pin_TypeDef PortPins);

void GPIO_WriteHigh(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins);

FlagStatus UART1_GetFlagStatus(UART1_Flag_TypeDef UART1_FLAG);

uint8_t UART1_ReceiveData8(void);

void UART1_Init(uint32_t BaudRate, UART1_WordLength_TypeDef WordLength,
                UART1_StopBits_TypeDef StopBits, UART1_Parity_TypeDef Parity,
                UART1_SyncMode_TypeDef SyncMode, UART1_Mode_TypeDef Mode);

void UART1_SendData8(uint8_t Data);

void UART1_DeInit(void);

void ADC1_Init(ADC1_ConvMode_TypeDef ADC1_ConversionMode, ADC1_Channel_TypeDef ADC1_Channel, ADC1_PresSel_TypeDef ADC1_PrescalerSelection, ADC1_ExtTrig_TypeDef ADC1_ExtTrigger, FunctionalState ADC1_ExtTriggerState, ADC1_Align_TypeDef ADC1_Align, ADC1_SchmittTrigg_TypeDef ADC1_SchmittTriggerChannel, FunctionalState ADC1_SchmittTriggerState);

void ADC1_StartConversion(void);

void ADC1_DeInit(void);

uint16_t ADC1_GetConversionValue(void);

void I2C_Init(uint32_t OutputClockFrequencyHz, uint16_t OwnAddress, 
              I2C_DutyCycle_TypeDef I2C_DutyCycle, I2C_Ack_TypeDef Ack, 
              I2C_AddMode_TypeDef AddMode, uint8_t InputClockFrequencyMHz );

void I2C_SendData(uint8_t data);

I2C_Event_TypeDef I2C_GetLastEvent(void);

uint8_t I2C_ReceiveData(void);

void I2C_DeInit(void);

void TIM1_DeInit(void);

void TIM1_CtrlPWMOutputs(FunctionalState NewState);

void TIM1_Cmd(FunctionalState NewState);

void TIM1_OC1Init(TIM1_OCMode_TypeDef TIM1_OCMode,
                  TIM1_OutputState_TypeDef TIM1_OutputState,
                  TIM1_OutputNState_TypeDef TIM1_OutputNState,
                  uint16_t TIM1_Pulse, TIM1_OCPolarity_TypeDef TIM1_OCPolarity,
                  TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity,
                  TIM1_OCIdleState_TypeDef TIM1_OCIdleState,
                  TIM1_OCNIdleState_TypeDef TIM1_OCNIdleState);


void TIM1_TimeBaseInit(uint16_t TIM1_Prescaler,
                       TIM1_CounterMode_TypeDef TIM1_CounterMode,
                       uint16_t TIM1_Period, uint8_t TIM1_RepetitionCounter);

uint16_t TIM1_GetCapture1(void);
void TIM1_ClearFlag(TIM1_FLAG_TypeDef TIM1_FLAG);
uint16_t TIM1_GetCapture2(void);

typedef enum
{
  TIM2_OCMODE_TIMING     = ((uint8_t)0x00),
  TIM2_OCMODE_ACTIVE     = ((uint8_t)0x10),
  TIM2_OCMODE_INACTIVE   = ((uint8_t)0x20),
  TIM2_OCMODE_TOGGLE     = ((uint8_t)0x30),
  TIM2_OCMODE_PWM1       = ((uint8_t)0x60),
  TIM2_OCMODE_PWM2       = ((uint8_t)0x70)
}TIM2_OCMode_TypeDef;

typedef enum
{
  TIM2_OUTPUTSTATE_DISABLE           = ((uint8_t)0x00),
  TIM2_OUTPUTSTATE_ENABLE            = ((uint8_t)0x11)
}TIM2_OutputState_TypeDef;

/** TIM2 Output Compare Polarity */
typedef enum
{
  TIM2_OCPOLARITY_HIGH               = ((uint8_t)0x00),
  TIM2_OCPOLARITY_LOW                = ((uint8_t)0x22)
}TIM2_OCPolarity_TypeDef;

typedef enum
{
  TIM2_PRESCALER_1  = ((uint8_t)0x00),
  TIM2_PRESCALER_2    = ((uint8_t)0x01),
  TIM2_PRESCALER_4    = ((uint8_t)0x02),
  TIM2_PRESCALER_8     = ((uint8_t)0x03),
  TIM2_PRESCALER_16   = ((uint8_t)0x04),
  TIM2_PRESCALER_32     = ((uint8_t)0x05),
  TIM2_PRESCALER_64    = ((uint8_t)0x06),
  TIM2_PRESCALER_128   = ((uint8_t)0x07),
  TIM2_PRESCALER_256   = ((uint8_t)0x08),
  TIM2_PRESCALER_512   = ((uint8_t)0x09),
  TIM2_PRESCALER_1024  = ((uint8_t)0x0A),
  TIM2_PRESCALER_2048 = ((uint8_t)0x0B),
  TIM2_PRESCALER_4096   = ((uint8_t)0x0C),
  TIM2_PRESCALER_8192 = ((uint8_t)0x0D),
  TIM2_PRESCALER_16384 = ((uint8_t)0x0E),
  TIM2_PRESCALER_32768 = ((uint8_t)0x0F)
}TIM2_Prescaler_TypeDef;

#define IS_TIM2_OC_MODE_OK(MODE) (((MODE) == TIM2_OCMODE_TIMING) ||	\
                                  ((MODE) == TIM2_OCMODE_ACTIVE) || \
                                  ((MODE) == TIM2_OCMODE_INACTIVE) || \
                                  ((MODE) == TIM2_OCMODE_TOGGLE)|| \
                                  ((MODE) == TIM2_OCMODE_PWM1) || \
                                  ((MODE) == TIM2_OCMODE_PWM2))

#define IS_TIM2_OUTPUT_STATE_OK(STATE) (((STATE) == TIM2_OUTPUTSTATE_DISABLE) || \
                                        ((STATE) == TIM2_OUTPUTSTATE_ENABLE))

#define IS_TIM2_OC_POLARITY_OK(POLARITY) (((POLARITY) == TIM2_OCPOLARITY_HIGH) || \
    ((POLARITY) == TIM2_OCPOLARITY_LOW))

void TIM2_DeInit(void);

void TIM2_CtrlPWMOutputs(FunctionalState NewState);

void TIM2_Cmd(FunctionalState NewState);

void TIM2_OC1Init(TIM2_OCMode_TypeDef TIM2_OCMode, TIM2_OutputState_TypeDef TIM2_OutputState, uint16_t TIM2_Pulse, TIM2_OCPolarity_TypeDef TIM2_OCPolarity);

void TIM2_OC1PreloadConfig(FunctionalState NewState);

void TIM2_TimeBaseInit(TIM2_Prescaler_TypeDef TIM2_Prescaler,
                       uint16_t TIM2_Period);

void TIM2_ARRPreloadConfig(FunctionalState NewState);

void CLK_HSIPrescalerConfig(CLK_Prescaler_TypeDef HSIPrescaler);


typedef enum {
#if defined (STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S105) || \
    defined(STM8S005) || defined (STM8AF52Ax) || defined (STM8AF62Ax) || defined(STM8AF626x)
  FLASH_STATUS_END_HIGH_VOLTAGE = (uint8_t)0x40, /*!< End of high voltage */
#endif /* STM8S208, STM8S207, STM8S105, STM8AF62Ax, STM8AF52Ax, STM8AF626x */
  FLASH_STATUS_SUCCESSFUL_OPERATION =
      (uint8_t)0x04,                    /*!< End of operation flag */
  FLASH_STATUS_TIMEOUT = (uint8_t)0x02, /*!< Time out error */
  FLASH_STATUS_WRITE_PROTECTION_ERROR =
      (uint8_t)0x01 /*!< Write attempted to protected page */
} FLASH_Status_TypeDef;

typedef enum {
    FLASH_MEMTYPE_PROG      = (uint8_t)0xFD, /*!< Program memory */
    FLASH_MEMTYPE_DATA      = (uint8_t)0xF7  /*!< Data EEPROM memory */
} FLASH_MemType_TypeDef;

#define OPERATION_TIMEOUT ((uint16_t)0xFFFF)

void FLASH_ProgramOptionByte(uint16_t Address, uint8_t Data);

#endif
