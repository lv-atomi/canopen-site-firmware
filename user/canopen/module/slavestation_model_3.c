#include "slavestation_model_3.h"

#include "i2c_application.h"

/*
 * Slaveboard model 3, can station 10,11,12,13,15:
 * -slaveboard stackable
 *     + I2C addr auto negotiation
 *     + 8 boards maximum
 * -Motor drive x1
 *     + Hbridge
 *     + Brushless(TBD)
 * -GPinput x2
 * -GPoutput x2
 *
 */

#define I2C_TIMEOUT                      0xFFFFF /* about 0.4s */

#define I2Cx_SPEED                       100000
#define I2Cx_ADDRESS                     0xA0

#define I2Cx_PORT                        I2C2
#define I2Cx_CLK                         CRM_I2C2_PERIPH_CLOCK

#define I2Cx_SCL_PIN                     GPIO_PINS_10
#define I2Cx_SCL_GPIO_PORT               GPIOB
#define I2Cx_SCL_GPIO_CLK                CRM_GPIOB_PERIPH_CLOCK

#define I2Cx_SDA_PIN                     GPIO_PINS_11
#define I2Cx_SDA_GPIO_PORT               GPIOB
#define I2Cx_SDA_GPIO_CLK                CRM_GPIOB_PERIPH_CLOCK

#define BUF_SIZE 8

i2c_handle_type hi2cx;
uint8_t tx_buf[BUF_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t rx_buf[BUF_SIZE] = {0};
uint8_t number_of_slaves = 0;

void error_handler() {
  while(1) {
    //at32_led_toggle(LED2);
    delay_ms(500);
  }
}

void i2c_lowlevel_init(i2c_handle_type* hi2c) {
  gpio_init_type gpio_initstructure;

  /* i2c periph clock enable */
  crm_periph_clock_enable(I2Cx_CLK, TRUE);
  crm_periph_clock_enable(I2Cx_SCL_GPIO_CLK, TRUE);
  crm_periph_clock_enable(I2Cx_SDA_GPIO_CLK, TRUE);

  /* gpio configuration */
  gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;

  /* configure i2c pins: scl */
  gpio_initstructure.gpio_pins = I2Cx_SCL_PIN;
  gpio_init(I2Cx_SCL_GPIO_PORT, &gpio_initstructure);

  /* configure i2c pins: sda */
  gpio_initstructure.gpio_pins = I2Cx_SDA_PIN;
  gpio_init(I2Cx_SDA_GPIO_PORT, &gpio_initstructure);

  i2c_init(I2Cx_PORT, I2C_FSMODE_DUTY_2_1, I2Cx_SPEED);

  i2c_own_address1_set(hi2c->i2cx, I2C_ADDRESS_MODE_7BIT, I2Cx_ADDRESS);
}

/* returns: 0 if discoveried, 1 if timeout */
uint32_t cmd_discovery(i2c_handle_type* hi2c, uint16_t slave_addr) { /* FIXME: working */
  i2c_status_type i2c_status;

  if((i2c_status = i2c_master_transmit(&hi2cx, slave_addr, tx_buf, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK) {
    printf("error during discovery\n");
    //error_handler();
  }
  delay_ms(10);
  if((i2c_status = i2c_master_receive(&hi2cx, slave_addr, rx_buf, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK) { /* recv failed */
    return 1;
  }
  /* FIXME: check recved buffer */
  return 0;
}

void cmd_gpout_set(i2c_handle_type* hi2c, uint16_t slave_addr, uint8_t channel, bool_t value){ /* FIXME: working */
  i2c_status_type i2c_status;

  if((i2c_status = i2c_master_transmit(&hi2cx, slave_addr, tx_buf, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK) {
    error_handler();
  }
  /* delay_ms(10); */
  /* if((i2c_status = i2c_master_receive(&hi2cx, slave_addr, rx_buf, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK) { /\* recv failed *\/ */
  /*   return 1; */
  /* } */
  /* FIXME: check recved buffer */
}

bool_t cmd_gpout_read(i2c_handle_type* hi2c, uint16_t slave_addr, uint8_t channel){ /* FIXME: working */
  i2c_status_type i2c_status;

  if((i2c_status = i2c_master_transmit(&hi2cx, slave_addr, tx_buf, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK) {
    error_handler();
  }
  /* delay_ms(10); */
  /* if((i2c_status = i2c_master_receive(&hi2cx, slave_addr, rx_buf, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK) { /\* recv failed *\/ */
  /*   return 1; */
  /* } */
  /* FIXME: check recved buffer */
  return TRUE;
}

int32_t cmd_motor_speed_read(i2c_handle_type* hi2c, uint16_t slave_addr){ /* FIXME: working */
  i2c_status_type i2c_status;

  if((i2c_status = i2c_master_transmit(&hi2cx, slave_addr, tx_buf, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK) {
    error_handler();
  }
  delay_ms(10);
  if((i2c_status = i2c_master_receive(&hi2cx, slave_addr, rx_buf, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK) { /* recv failed */
    error_handler();
  }
  /* FIXME: check recved buffer */
  return 0;
}

int32_t cmd_motor_speed_set(i2c_handle_type* hi2c, uint16_t slave_addr, int32_t speed) { /* FIXME: working */
  i2c_status_type i2c_status;

  if((i2c_status = i2c_master_transmit(&hi2cx, slave_addr, tx_buf, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK) {
    error_handler();
  }
  /* delay_ms(10); */
  /* if((i2c_status = i2c_master_receive(&hi2cx, slave_addr, rx_buf, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK) { /\* recv failed *\/ */
  /*   error_handler(); */
  /* } */
  /* FIXME: check recved buffer */
  return 0;
}

/* exported interfaces */

void init_slavestation_model_3(){
  uint8_t i;

  hi2cx.i2cx = I2Cx_PORT;
  
  i2c_config(&hi2cx);

  /* probe number of total i2c slaves */
  for (i=0; i<=8; i++) {
    if (cmd_discovery(&hi2cx, i) != 0){
      number_of_slaves = i;
      break;
    }
  }
  printf("num of slaves:%d\n", i);
}

void GPOutSet(uint16_t slave_addr, uint8_t channel, bool_t val) {
  cmd_gpout_set(&hi2cx, slave_addr, channel, val);
}

bool_t GPOutRead(uint16_t slave_addr, uint8_t channel) {
  return cmd_gpout_read(&hi2cx, slave_addr, channel);
}

/* int32_t motor_speed_sense(uint16_t slave_addr){ */
/*   int32_t speed = 0; */
/*   speed = cmd_motor_speed_read(&hi2cx, slave_addr); */
/*   return speed; */
/* } */

/* void motor_speed_set(uint16_t slave_addr, int32_t speed){ */
/*   cmd_motor_speed_set(&hi2cx, slave_addr, speed); */
/* } */

/* FIXME: HT8574 don't have pwm input/output mode, cannot do speed sense/control
 * for motor */


#define MAX_STACKABLE_MODULES 8

OD_extension_t OD_650X_extension[MAX_STACKABLE_MODULES];

static ODR_t my_OD_read_650X(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead) {
  uint8_t index = (uint8_t)(uintptr_t)(stream->object);
  printf("read 650%d, subidx:%u\n", index, stream->subIndex);

  switch (stream->subIndex) {
  case 0:                // highestSub_indexSupported
    CO_setUint8(buf, 7); // number of sub-indexes
    *countRead = sizeof(uint8_t);
    break;
  case 1: // out0
    // read value from the actual hardware or software model
    // CO_setUint8(buf, read_out0(module_idx));
    *countRead = sizeof(bool_t);
    break;
  case 2: // out1
    // read value from the actual hardware or software model
    // CO_setUint8(buf, read_out1(module_idx));
    *countRead = sizeof(bool_t);
    break;
  case 3: // in0
    // read value from the actual hardware or software model
    // CO_setUint8(buf, read_in0(module_idx));
    *countRead = sizeof(bool_t);
    break;
  case 4: // in1
    // read value from the actual hardware or software model
    // CO_setUint8(buf, read_in1(module_idx));
    *countRead = sizeof(bool_t);
    break;
  case 5: // HBridgeMotor
    // read value from the actual hardware or software model
    // CO_setUint32(buf, read_HBridgeMotor(module_idx));
    *countRead = sizeof(int32_t);
    break;
  case 6: // I2CAddr
    // read value from the actual hardware or software model
    // CO_setUint8(buf, read_I2CAddr(module_idx));
    *countRead = sizeof(uint8_t);
    break;
  default:
    return ODR_SUB_NOT_EXIST;
  }

  return ODR_OK;
}

static ODR_t my_OD_write_650X(OD_stream_t *stream, const void *buf, OD_size_t count, OD_size_t *countWritten)
{
  uint8_t module_idx = (uint8_t)(uintptr_t)(stream->object); // extract the module index from the object
  printf("write 650%d, subidx:%d\n", module_idx, stream->subIndex);

  switch (stream->subIndex) {
  case 1: // out0
    // write value to the actual hardware or software model
    // write_out0(module_idx, CO_getUint8(buf));
    break;
  case 2: // out1
    // write value to the actual hardware or software model
    // write_out1(module_idx, CO_getUint8(buf));
    break;
  case 3:                 // in0
  case 4:                 // in1
  case 6:                 // I2CAddr
    return ODR_READONLY; // These values are read-only
  case 5:                 // HBridgeMotor
    // write value to the actual hardware or software model
    // write_HBridgeMotor(module_idx, CO_getUint32(buf));
    break;
  default:
    return ODR_SUB_NOT_EXIST;
  }

  return ODR_OK;
}

CO_ReturnError_t app_stackable_module_init() {
  init_slavestation_model_3();
  for (uint8_t i = 0; i < MAX_STACKABLE_MODULES; i++) {
    OD_entry_t *param_650X = NULL;
    switch (i) {
    case 0:
      param_650X = OD_ENTRY_H6500_stackableModule0;
      break;
    case 1:
      param_650X = OD_ENTRY_H6501_stackableModule1;
      break;
    case 2:
      param_650X = OD_ENTRY_H6502_stackableModule2;
      break;
    case 3:
      param_650X = OD_ENTRY_H6503_stackableModule3;
      break;
    case 4:
      param_650X = OD_ENTRY_H6504_stackableModule4;
      break;
    case 5:
      param_650X = OD_ENTRY_H6505_stackableModule5;
      break;
    case 6:
      param_650X = OD_ENTRY_H6506_stackableModule6;
      break;
    case 7:
      param_650X = OD_ENTRY_H6507_stackableModule7;
      break;
    default:
      param_650X = NULL; // Or some other error handling
      break;
    }
    ASSERT(param_650X != NULL);

    OD_650X_extension[i].object = (OD_entry_t *)(intptr_t)i; /* FIXME */
    OD_650X_extension[i].read = my_OD_read_650X;
    OD_650X_extension[i].write = my_OD_write_650X;

    OD_extension_init(param_650X, &(OD_650X_extension[i]));
  }

  return CO_ERROR_NO;
}
