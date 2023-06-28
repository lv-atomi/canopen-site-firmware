#include <stdint.h>
#include "stm8.h"
#include <stdio.h>

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
void uart_init() {
  UART1_BRR2 = 0x03;
  UART1_BRR1 = 0x68;		/* 9600 */
  UART1_CR3 &= ~(UART_CR3_STOP1 | UART_CR3_STOP2); // 1 stop bit
  UART1_CR2 = UART_CR2_TEN	/* allow tx */
    // | UART_CR2_REN           /* allow rx */
    ;
}

void uart_write(uint8_t data) {
  while (!(UART1_SR & UART_SR_TXE))
    ;
  UART1_DR = data;
}

int putchar(int c) {
  uart_write(c);
  return 0;
}


/* Simple busy loop delay */
void delay(unsigned long count) {
  while (count--)
    nop();
}


uint16_t ADC_readonce(uint8_t channel) {
  ADC_CSR |= channel; // Select channel 1
  ADC_CR1 |= ADC_CR1_ADON;  // Start conversion
  /* Right-align data */
  ADC_CR2 |= ADC_CR2_ALIGN;

  // Wait until conversion is done
  while ((ADC_CSR & ADC_CSR_EOC) == 0);

  // Load ADC reading (least-significant byte must be read first)

  uint16_t result = ADC_DRL;
  result |= (ADC_DRH << 8);
  
  // Clear done flag
  ADC_CSR &= ~ADC_CSR_EOC;

  return result;
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

uint16_t sense_position(){
  uint16_t sense1_val = ADC_readonce(3);
  /* printf("sense1:%u\n", sense1_val); */
  uint16_t sense2_val = ADC_readonce(4);
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

void init_adc(){
  // 设置 ADC 
  ADC_CR1 |= ADC_CR1_ADON;  // Turn on ADC (done't forget this one)
  /* Right-align data */
  ADC_CR2 |= ADC_CR2_ALIGN;
}

int main(void){
  CLK_CKDIVR = 0;
  GPOutput_init(PD, PIN5);	/* enable PD5, UART_TX */
  GPOutput_init(ADDR_PORT, ADDR_PIN); /* enable PC3~PC7 */
  GPOutput_init(CONTROL_PORT, CONTROL_PIN); /* enable PA.1(LED), PA.3(Ready) */

  uart_init();
  init_adc();
  
  
  PORT(PA, ODR) |= PIN1;	/* LED off */
  PORT(PA, ODR) &= ~PIN3;	/* Ready off */

  uint16_t pos = sense_position();
  /* printf("2, pos:%d\n", pos); */
  
  uint16_t i;
  for (i=0; i<pos; i++){
    /* printf("i:%d pos:%d\n", i, pos); */
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
  PORT(PA, ODR) &= ~PIN1;	/* LED on */

  while (1){};
}


int main_hello(void){
  CLK_CKDIVR = 0;
  uart_init();
  GPOutput_init(PD, PIN5);	/* enable PD5, UART_TX */
  GPOutput_init(PA, PIN1);	/* enable PD5, UART_TX */
  
  
  while (1){
    printf("hello\n");
    PORT(PA, ODR) &= ~PIN1;
    delay(250000);
    PORT(PA, ODR) |= PIN1;	/* LED off */
    delay(250000);
  }
}
