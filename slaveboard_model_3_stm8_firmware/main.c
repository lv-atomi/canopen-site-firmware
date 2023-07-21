#include "stm8s_conf.h"
#include "stdio.h"
#include "board.h"

MotorPort motor = {
    .dir = {GPIOA, GPIO_PIN_2},
    .disable_driver = {GPIOD, GPIO_PIN_6},
    .phase_a_speedsense = {GPIOC, GPIO_PIN_6},
    .phase_b = {GPIOC, GPIO_PIN_3},
    .speed_control = {GPIOD, GPIO_PIN_4},
};

I2CPort i2c = {
  .clk = {GPIOB, GPIO_PIN_4},
  .data= {GPIOB, GPIO_PIN_5},
};

ADCPort sense0 = {
    .port = {GPIOD, GPIO_PIN_2},
    .channel = 3,
};

ADCPort sense1 = {
    .port = {GPIOD, GPIO_PIN_3},
    .channel = 4,
};

IOPort led = {GPIOA, GPIO_PIN_1};
IOPort gpin[2] = {{GPIOC, GPIO_PIN_5}, {GPIOC, GPIO_PIN_7}};
IOPort gpout[2] = {{GPIOA, GPIO_PIN_3}, {GPIOC, GPIO_PIN_4}};

/* i2c declaration */
#define I2C_ADDR_BASE 0x20
uint8_t i2c_addr = 0; /* slave addr */
__IO uint8_t Slave_Buffer_Rx[255];
__IO uint8_t Tx_Idx = 0, Rx_Idx = 0;
__IO uint16_t Event = 0x00;

void i2c_config(uint8_t _i2c_addr, I2CPort * port){
  I2C_DeInit();
  i2c_addr  = _i2c_addr;
  I2C_Init(100000, _i2c_addr, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);
}

void on_i2c_irq(void){
  /* Read SR2 register to get I2C error */
  if ((I2C->SR2) != 0) {
    /* Clears SR2 register */
    I2C->SR2 = 0;
  }
  Event = I2C_GetLastEvent();
  switch (Event) {
      /******* Slave transmitter ******/
      /* check on EV1 */
    case I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED:
      Tx_Idx = 0;
      break;

      /* check on EV3 */
    case I2C_EVENT_SLAVE_BYTE_TRANSMITTING:
      /* Transmit data */
      I2C_SendData(Slave_Buffer_Rx[Tx_Idx++]);
      break;
      /******* Slave receiver **********/
      /* check on EV1*/
    case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED:
      break;

      /* Check on EV2*/
    case I2C_EVENT_SLAVE_BYTE_RECEIVED:
      Slave_Buffer_Rx[Rx_Idx++] = I2C_ReceiveData();
      break;

      /* Check on EV4 */
    case (I2C_EVENT_SLAVE_STOP_DETECTED):
            /* write to CR2 to clear STOPF flag */
            I2C->CR2 |= I2C_CR2_ACK;
      break;

    default:
      break;
  }
}
  
void motor_config(uint8_t is_brushless, MotorPort * devport){
  if (is_brushless){
    gpoutput_config(&devport->dir, 0);
    gpoutput_config(&devport->disable_driver, 1);
    pwm_output_config(&devport->speed_control, 25000, 0);
    gpinput_config(&devport->phase_b);
    pwm_input_config(&devport->phase_a_speedsense);
  } else {
    gpoutput_config(&devport->dir, 0);
    gpoutput_config(&devport->disable_driver, 0);
    pwm_output_config(&devport->speed_control, 0, 25000);
    gpinput_config(&devport->phase_b);
    pwm_output_config(&devport->phase_a_speedsense, 0, 25000);
  }
}

int main(void) {
  uint8_t i;
  board_init();
  gpoutput_config(&led, 0);
  for (i=0; i<2; i++){
    gpinput_config(&gpin[i]);
    gpoutput_config(&gpout[i], 1);
  }
  adc_config(&sense0);
  adc_config(&sense1);
  
  delay_ms(1000);
  uint16_t position = sense_position(&sense0, &sense1);
  
  i2c_addr = I2C_ADDR_BASE + (uint8_t) (position * 2);
  i2c_config(i2c_addr, &i2c);

  motor_config(1, &motor);
  
  while (1);
}
