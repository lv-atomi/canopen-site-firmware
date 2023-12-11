#include "gpiolink_crc8.h"
#include "gpio.h"
#include "log.h"
#include "one_wire.h"
#include "timer.h"
#include <stdint.h>
#include "crc8.h"

#define GPIOLINK_HEAD  0xa7
#define GPIOLINK_MAX_PORTS  4

gpiolink *gpiolink_monitor[GPIOLINK_MAX_PORTS]={0};
uint8_t gpiolink_monitor_num=0;



void init_gpiolink(gpiolink * link, bool_t enable_pullup) {
  ASSERT(link);

  generate_crc8_table();
  init_one_wire(&link->port, enable_pullup);
}


void gpiolink_master_send_byte(gpiolink *link, uint8_t data) {
  ASSERT(link);
  uint8_t buf[2] = {GPIOLINK_HEAD, data};
  uint8_t crc  = compute_crc8(buf, 2); // 假设compute_crc8函数已经定义
  
  OneWire_Reset(&link->port);
  OneWire_WriteByte(&link->port, buf[0]);
  OneWire_WriteByte(&link->port, buf[1]);
  OneWire_WriteByte(&link->port, crc);
}

bool_t gpiolink_slave_recv_byte(gpiolink *link, uint8_t *data) {
  OneWire *port = &link->port;

  // 等待并确认RESET脉冲，并发送PRESENCE脉冲
  if (!OneWire_SlaveDetectResetAndSendPresence(port)) {
    log_printf("GPIOLink: Failed to detect RESET and send PRESENCE.\n");
    return FALSE; // 如果检测到复位失败，返回FALSE
  }

  // 接收帧头
  uint8_t header;
  if (!OneWire_SlaveReadByte(port, &header)) {
    log_printf("GPIOLink: Failed to read frame header.\n");	
    return FALSE;
  }

  // 从主设备接收一个字节的数据
  uint8_t received_data;
  if (!OneWire_SlaveReadByte(port, &received_data)) {
    log_printf("GPIOLink: Failed to read data byte.\n");
    return FALSE;
  }

  // 接收CRC校验值
  uint8_t received_crc;
  if (!OneWire_SlaveReadByte(port, &received_crc)) {
    log_printf("GPIOLink: Failed to read CRC byte.\n");
    return FALSE;
  }

  uint8_t buf[2] = {header, received_data};
  uint8_t computed_crc = compute_crc8(buf, 2);

  // 进行CRC校验
  if (received_crc != computed_crc) {
    log_printf("GPIOLink: CRC mismatch. Received CRC: %02X, Expected CRC: %02X, Header: %02X, Data: %02X\n", 
               received_crc, computed_crc, header, received_data);

    return FALSE; // CRC校验失败，返回FALSE
  }
  // 检查帧头
  if (header != GPIOLINK_HEAD) {
    log_printf("GPIOLink: Frame header mismatch. Expected: %02X, Got: %02X\n", GPIOLINK_HEAD, header);
    return FALSE; // 帧头不匹配，返回FALSE
  }

  *data = received_data;
  log_printf("GPIOLink: Data received: %02X\n", received_data);

  return TRUE; // 数据接收和校验成功
}
