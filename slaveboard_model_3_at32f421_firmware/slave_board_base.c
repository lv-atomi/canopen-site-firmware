#include "slave_board_base.h"
#include "log.h"

int32_t my_round(uint32_t val){
  /* printf("round in:%ld\n", val); */
  uint32_t integer = val / 100;
  /* printf("round integer:%ld\n", integer); */
  uint32_t decimal = val - integer * 100;
  /* printf("round decimal:%ld\n", decimal); */
  if (decimal > 50) return integer+1;
  return integer;
}

uint16_t sense_position(ADCPort * senses, uint16_t num_senses){
  ASSERT(num_senses == 2);
  
  adc_ordinary_software_trigger_enable(ADC1, TRUE);
  uint16_t * sensed = read_adcs(senses, num_senses);
  uint16_t sense1_val = sensed[0];
  /* printf("sense1:%u\n", sense1_val); */
  uint16_t sense2_val = sensed[1];
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
  /*     sense1_val, sense2_val, delta, num_boards, curr_board_num); */
  return curr_board_num;
}

void adc_config(ADCPort * senses, uint8_t num_senses) {
  ASSERT(senses);
  init_adcs(senses, num_senses);
}

void i2c_config(I2CPort * i2c) {
  ASSERT(i2c);
  init_i2c(i2c, 100000);
}
