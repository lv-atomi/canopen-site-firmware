#include "spl/STM8S_StdPeriph_Lib/Libraries/STM8S_StdPeriph_Driver/inc/stm8s_gpio.h"
#include "stm8s_conf.h"
#include "stdio.h"
#include "board.h"

MotorPort motor = {
    .dir = {GPIOA, GPIO_PIN_2},
    .disable_driver = {GPIOD, GPIO_PIN_6},
    .phase_a_speedsense = {GPIOC, GPIO_PIN_6},
    .phase_b = {GPIOC, GPIO_PIN_3},
    .speed_control = {GPIOC, GPIO_PIN_5},
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
IOPort gpin[2] = {{GPIOD, GPIO_PIN_4}, {GPIOC, GPIO_PIN_7}};
IOPort gpout[2] = {{GPIOA, GPIO_PIN_3}, {GPIOC, GPIO_PIN_4}};

/* i2c declaration */
#define I2C_ADDR_BASE 0x20
uint8_t i2c_addr = 0; /* slave addr */
__IO uint8_t Slave_Buffer_Rx[2];
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
  
void motor_config(MotorPort * devport, uint8_t is_brushless){
  uint8_t *opt2_ptr = &OPT->OPT2;
  if (is_brushless){
    //FLASH_ProgramOptionByte((uint16_t)opt2_ptr, 0b10000001);
    gpoutput_config(&devport->dir, 0);
    gpoutput_config(&devport->disable_driver, 1);
    gpinput_config(&devport->phase_b, GPIO_MODE_IN_FL_NO_IT); /* disable pc3 */
    tmr1_ch1_sense();		/* pc6 as pwm in */
    tmr2_ch1_output(25000, 50);	/* pd4 as pwm out */
  } else {
    //    FLASH_ProgramOptionByte((uint16_t)opt2_ptr, 0b10000001);
    gpoutput_config(&devport->dir, 0);
    gpoutput_config(&devport->disable_driver, 0);
    gpinput_config(&devport->speed_control, GPIO_MODE_IN_FL_NO_IT); /* disable pd4 */
    tmr1_ch1_ch1n_output(25000, 50); /* pc6 & pc3 as pwm out */
  }
}

void motor_set_speed(MotorPort *devport, int8_t speed, uint8_t is_brushless) {
  printf("motor speed:%d\n", speed);
  if(is_brushless){
    if (speed > 0){
      tmr2_duty_update(speed);
      gpio_set(&devport->dir, 0);
    } else if (speed < 0) {
      tmr2_duty_update(-speed);
      gpio_set(&devport->dir, 1);
    } else {
      tmr2_duty_update(0);
    }
    
  } else {
    tmr1_duty_update(50 + speed/2);
  }
}

int main(void) {
  uint8_t i;
  board_init();
  gpoutput_config(&led, 0);
  for (i=0; i<2; i++){
    gpinput_config(&gpin[i], GPIO_MODE_IN_PU_NO_IT);
    gpoutput_config(&gpout[i], 1);
  }
  adc_config(&sense0);
  adc_config(&sense1);
  
  delay_ms(1000);
  uint16_t position = sense_position(&sense0, &sense1);
  
  i2c_addr = I2C_ADDR_BASE + (uint8_t) (position * 2);
  i2c_config(i2c_addr, &i2c);

  uint8_t is_brushless = 0;
  motor_config(&motor, is_brushless);

  int8_t speed = 0;
  int8_t direction = 1;
  while (1){
    //tmr1_ch1_sense();
    /* gpio_set(&gpout[0], tag); */
    /* gpio_set(&gpout[1], !tag); */
    /* printf("tag:%u in0:%u in1:%u\n", */
    /* 	   tag, */
    /* 	   gpio_read(&gpin[0]), */
    /* 	   gpio_read(&gpin[1])); */
    delay_ms(1000);
    speed += 10*direction;
    if (speed>=100){
      direction = -1;
      speed=100;
    } else if (speed <=-100){
      speed=-100;
      direction = 1;
    }
    motor_set_speed(&motor, speed, is_brushless);
  }
}
