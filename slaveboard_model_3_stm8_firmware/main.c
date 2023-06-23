#include <stdint.h>
#include "stm8.h"

// Address:
// PC3-> Addr0
// PC4-> Addr1
// PC5-> Addr2
// PC6-> Addr3
// PC7-> Addr4

// Sense:
// PD2: sense1      AIN3
// PD3: sense2      AIN4

// Others: 
// PA1: LED
// PA3: Ready      control mosfet to power on GPIO ic after addr negotiated

// OPT2: 0x81, NOPT2: 0x7e
//AFR7:1
//AFR0:1

#define ADDR_PORT   PC
#define ADDR_PIN (PIN3 | PIN4 | PIN5 | PIN6 | PIN7)

#define CONTROL_PORT PA
#define CONTROL_PIN (PIN1 | PIN3)

#define GPOutput_init(port, pin) do{  \
  /* GPIO setup */				\
  /* Set pin data direction as output */	\
    PORT(port, CR2) &= ~(pin);		\
    PORT(port, DDR) |= (pin);		\
  /* Set pin as "Push-pull" */			\
    PORT(port, CR1) |= (pin);		\
  /* Set pin as high speed */			\
    PORT(port, CR2) |= (pin);		\
} while(0)

/*
 * PD5 -> TX
 * PD6 -> RX
 */
/* void uart_init() { */
/*   UART_BRR2 = 0x00; */
/*   UART_BRR1 = 0x0D; */
/*   UART_CR2 = (1 << UART_TEN) | (1 << UART_REN); */
/* } */

/* void uart_write(uint8_t data) { */
/*   UART_DR = data; */
/*   while (!(UART_SR & (1 << UART_TC))) */
/*     ; */
/* } */

/* int putchar(int c) { */
/*   uart_write(c); */
/*   return 0; */
/* } */


/* Simple busy loop delay */
void delay(unsigned long count) {
  while (count--)
    nop();
}


uint16_t ADC_readonce(uint8_t channel) {
  /* Configure ADC channel 3 (PD2)  */
  ADC_CSR |= channel;

  /* Right-align data */
  ADC_CR2 |= ADC_CR2_ALIGN;
  /* Wake ADC from power down */
  ADC_CR1 |= ADC_CR1_ADON;

  // Wait until conversion is done
  while ((ADC_CSR & ADC_CSR_EOC) == 0);
  // Clear done flag
  ADC_CSR &= ~ADC_CSR_EOC;
  // Load ADC reading (least-significant byte must be read first)
  uint16_t result = ADC_DRL;
  result |= (ADC_DRH << 8);

  return result;
}

uint32_t my_round(uint32_t val){
  uint32_t integer = val / 100;
  uint32_t decimal = val - integer * 100;
  if (decimal > 50) return integer+1;
  return integer;
}

uint16_t sense_position(){
  uint16_t sense1_val = ADC_readonce(3);
  uint16_t sense2_val = ADC_readonce(4);
  uint32_t total = 409600;
  uint16_t num_boards;
  uint16_t curr_board_num;
  uint32_t delta = (sense1_val - sense2_val) * 100;
  num_boards = my_round(total / delta);
  curr_board_num = my_round(sense1_val * 100 / (total / num_boards));

  return curr_board_num;
}

int main(void){
  CLK_CKDIVR = 0;

  GPOutput_init(ADDR_PORT, ADDR_PIN);
  GPOutput_init(CONTROL_PORT, CONTROL_PIN);
  PORT(PA, ODR) |= PIN1;	/* LED off */
  PORT(PA, ODR) &= ~PIN3;	/* Ready off */

  uint16_t pos = sense_position();
  uint16_t i;
  for (i=0; i<pos; i++){
    PORT(PA, ODR) &= ~PIN1;	/* LED on */
    delay(250000);
    PORT(PA, ODR) |= PIN1;	/* LED off */
    delay(250000);
  }

  for (i=0; i<5; i++){
    if (pos & (1<<i)) {
      PORT(PC, ODR) |= ((PIN3) << i);
    } else {
      PORT(PC, ODR) &= ~((PIN3) << i);
    }
  }
  
  PORT(PA, ODR) |= PIN1;	/* Ready on */
  PORT(PA, ODR) &= ~PIN3;	/* LED on */
  while (1){};
}
