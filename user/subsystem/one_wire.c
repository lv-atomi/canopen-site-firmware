#include "one_wire.h"
#include "gpio.h"
#include "log.h"

#define TIMEOUT_ITERATIONS 10000000
#define delay_func delay_ms

void init_one_wire(OneWire * port, bool_t enable_pullup) {
  init_gpio_output(&port->port,
		   enable_pullup ? GPIO_OUTPUT_PUSH_PULL : GPIO_OUTPUT_OPEN_DRAIN,
		   GPIO_DRIVE_STRENGTH_STRONGER);
  /* if (enable_pullup) { */
  gpio_set(&port->port, 1);
  /* } else { */
  /*   gpio_set(&port->port, 0); */
  /* } */
}

void OneWire_Reset(OneWire *port) {
  // Pull the line low for at least 480us
  gpio_set(&port->port, 0);
  delay_func(480);

  // Release the line and wait for 70us
  gpio_set(&port->port, 1);
  delay_func(70+60);
}

void OneWire_WriteBit(OneWire *port, uint8_t bit) {
  if (bit) {
    // To write 1, pull the line low for 1us and then release it for at least
    // 60us
    gpio_set(&port->port, 0);
    delay_func(1);
    gpio_set(&port->port, 1);
    delay_func(60);
  } else {
    // To write 0, pull the line low for at least 60us
    gpio_set(&port->port, 0);
    delay_func(60); 
    gpio_set(&port->port, 1);
    delay_func(1);
  }
}

uint8_t OneWire_ReadBit(OneWire *port) {
  uint8_t bit = 0;

  // Pull the line low for 1us and then release it
  gpio_set(&port->port, 0);
  delay_func(1);
  gpio_set(&port->port, 1);

  // Wait for 14us and then read the line
  delay_func(14);
  gpio_set_input_mode(&port->port, TRUE);
  bit = gpio_read(&port->port);
  gpio_set_input_mode(&port->port, FALSE);

  // Wait for the rest of the read slot (60us - 15us = 45us)
  delay_func(45);

  return bit;
}

void OneWire_WriteByte(OneWire *port, uint8_t byte) {
  for (int i = 0; i < 8; i++) {
    OneWire_WriteBit(port, byte & 0x01);
    byte >>= 1;
  }
}

uint8_t OneWire_ReadByte(OneWire *port) {
  uint8_t byte = 0;

  for (int i = 0; i < 8; i++) {
    byte |= (OneWire_ReadBit(port) << i);
  }

  return byte;
}

// 动态地等待主设备的时间槽开始，并返回当前时序的状态
bool_t waitForLineToChange(OneWire *port) {
  uint8_t initial_state = gpio_read(&port->port);
  uint32_t iterations = 0;

  while (gpio_read(&port->port) == initial_state) {
    if (iterations++ > TIMEOUT_ITERATIONS) {
      return FALSE; // 超时，没有检测到期望的状态变化
    }
  }
  return TRUE; // 线状态已改变
}

bool_t OneWire_SlaveWriteBit(OneWire *port, uint8_t bit) {
  // 等待主设备开始时间槽（即线从高到低的跳变）
  if (!waitForLineToChange(port))
    return FALSE;

  // 根据要发送的bit状态来决定何时将线拉低
  if (bit == 0) {
    // 将线拉低到时间槽的大约2/3处，然后释放
    delay_func(40);
    gpio_set(&port->port, 0);
    delay_func(20);
    gpio_set(&port->port, 1);
  } // 对于bit为1，不需要进行任何操作，让线保持释放状态
  return TRUE;
}

uint8_t OneWire_SlaveReadBit(OneWire *port) {
  uint8_t bit;

  char logCache[4][50]; // 定义日志缓存
  int logIndex = 0;    // 当前缓存索引

  // 等待主设备开始时间槽
  gpio_set_input_mode(&port->port, TRUE);
  if (!waitForLineToChange(port)){
    gpio_set_input_mode(&port->port, FALSE);
    //snprintf(logCache[logIndex++], 50, "GPIOLink: Failed waiting for line change.");
    return 0xFF; // 返回一个错误值
  }

  // 在时间槽的中间读取线状态
  delay_func(30);
  
  bit = gpio_read(&port->port);
  /* snprintf(logCache[logIndex++], 50, "GPIOLink: Read bit value %d.", bit); */

  // 等待时间槽结束
  if (!waitForLineToChange(port)){
    /* snprintf(logCache[logIndex++], 50, "GPIOLink: Failed waiting for slot end."); */
  }
  gpio_set_input_mode(&port->port, FALSE);

  // 打印日志缓存的所有条目
  /* for(int i = 0; i < logIndex; i++) { */
  /*   log_printf("%s\n", logCache[i]); */
  /* } */

  return bit;
}

/* uint8_t OneWire_SlaveReadBit(OneWire *port) { */
/*   uint8_t bit; */

/*   // 等待主设备开始时间槽 */
/*   gpio_set_input_mode(&port->port, TRUE); */
/*   if (!waitForLineToChange(port)){ */
/*     gpio_set_input_mode(&port->port, FALSE); */
/*     return 0xFF; // 返回一个错误值 */
/*   } */

/*   // 在时间槽的中间读取线状态 */
/*   delay_func(30); */
  
/*   bit = gpio_read(&port->port); */
/*   // 等待时间槽结束 */
/*   waitForLineToChange(port); */
/*   gpio_set_input_mode(&port->port, FALSE); */

/*   return bit; */
/* } */

bool_t OneWire_SlaveWriteByte(OneWire *port, uint8_t byte) {
  for (int i = 0; i < 8; i++) {
    if(!OneWire_SlaveWriteBit(port, byte & 0x01)) return FALSE;
    byte >>= 1;
  }
  return TRUE;
}

char* byte_to_binary_str(uint8_t byte) {
    static char bit_str[9];  // 8位加终止字符'\0'
    bit_str[8] = '\0';
    for (int i = 7; i >= 0; i--) {
        bit_str[7-i] = (byte & (1 << i)) ? '1' : '0';
    }
    return bit_str;
}

bool_t OneWire_SlaveReadByte(OneWire *port, uint8_t *byte) {
  *byte = 0;

  for (int i = 0; i < 8; i++) {
    uint8_t bit = OneWire_SlaveReadBit(port);

    if (bit == 0xFF) { // 错误检测
      return FALSE;    // 读取失败
    }

    *byte >>= 1; // 为下一个bit做准备
    if (bit) {
      *byte |= 0x80; // 如果读取的bit为1，将其设置到byte的最高位
    }
  }
  log_printf("GPIOLink: Received bits: %02X, %s\n", *byte, byte_to_binary_str(*byte));
  return TRUE; // 读取成功
}

// 检测1-Wire RESET 脉冲并回应 PRESENCE 脉冲
bool_t OneWire_SlaveDetectResetAndSendPresence(OneWire *port) {
  // 首先检查线路是否为高，即空闲状态
  if (gpio_read(&port->port) == 0) {
    log_printf("Error: Line is LOW at the start.\n");
    return FALSE; // 如果线路开始时为低，则返回错误
  }

  // 然后，等待线从高变为低，表示RESET脉冲开始
  while (gpio_read(&port->port) != 0)
    ;
  log_printf("RESET pulse detected. Line went LOW.\n");

  // 再次等待线从低变为高，表示RESET脉冲结束
  uint32_t reset_pulse_duration = 0;
  while (gpio_read(&port->port) == 0) {
    reset_pulse_duration++;
    delay_func(1);
    if (reset_pulse_duration > 480) { // 如果RESET脉冲超过480微秒
      log_printf("Error: RESET pulse duration exceeded 480us.\n");
      return FALSE; // 可能是某种错误，不应该这么长
    }
  }

  log_printf("RESET pulse ended. Line went HIGH. Pulse duration: %ld us\n", reset_pulse_duration);

  // 现在，等待恢复期的15-60微秒的适当时机，然后回应 PRESENCE 脉冲
  
  delay_func(30);
  gpio_set_input_mode(&port->port, FALSE);
  gpio_set(&port->port, 0); // 将线拉低
  log_printf("Sending PRESENCE pulse. Line set to LOW.\n");
  delay_func(30);            // 保持线低约60微秒
  gpio_set(&port->port, 1); // 释放线
  gpio_set_input_mode(&port->port, TRUE);
  
  log_printf("Finished sending PRESENCE pulse. Line set to HIGH.\n");
  log_printf("OneWire_SlaveDetectResetAndSendPresence function completed.\n");

  return TRUE; // 成功检测到RESET并发送了PRESENCE
}
