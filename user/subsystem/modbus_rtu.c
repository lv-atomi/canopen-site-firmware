#include "modbus_rtu.h"
#include "log.h"
#include "rs485.h"
#include <stdint.h>

#define CRC_START 0xFFFF

static uint16_t ModbusRTU_CRC16(uint8_t *buffer, uint16_t length, uint16_t crc) {
  for (uint16_t pos = 0; pos < length; pos++) {
    crc ^= (uint16_t)buffer[pos];
    for (int i = 8; i != 0; i--) {
      if ((crc & 0x0001) != 0) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

void ModbusRTU_Init(ModbusRTUPort *port) {
  // 初始化串口设备
  init_rs485(&port->uart_controller);
}

bool ModbusRTU_SendRequest(ModbusRTUPort *port,
                           ModbusFunctionCode function_code,
                           uint16_t start_address, uint16_t quantity,
                           uint8_t *data, uint16_t data_length) {
  uint16_t index = 0;

  port->tx_buf[index++] = port->slave_address;
  port->tx_buf[index++] = function_code;
  port->tx_buf[index++] = start_address >> 8;
  port->tx_buf[index++] = start_address & 0xFF;
  port->tx_buf[index++] = quantity >> 8;
  port->tx_buf[index++] = quantity & 0xFF;

  // 如果有数据需要发送，则加入到请求中
  for (uint16_t i = 0; i < data_length; i++) {
    port->tx_buf[index++] = data[i];
  }

  uint16_t crc = ModbusRTU_CRC16(port->tx_buf, index, CRC_START);
  port->tx_buf[index++] = crc & 0xFF;
  port->tx_buf[index++] = crc >> 8;

  // 发送数据
  /* log_printf("send data, len: %d\n", index); */
  /* DumpHex(port->tx_buf, index); */
  rs485_transmit(&port->uart_controller, port->tx_buf, index);

  return true;
}


bool ModbusRTU_Flush(ModbusRTUPort *port) {
    // 1. 清空输入缓冲区
    rs485_flush(&port->uart_controller); 

    // 2. 延时一段时间（这里假设我们有一个名为"delay_us"的微秒延时函数）
    uint32_t char_time_us = (1000000 / port->uart_controller.port.baud_rate) * 10; // 单字符时间 (10位：1起始，8数据，1停止)
    delay_us(char_time_us * 3.5);

    return true; // 同步完成
}

uint16_t ModbusRTU_ReadResponse(ModbusRTUPort *port, uint8_t *data, uint16_t buffer_length) {
  ASSERT(data);
  if (buffer_length < 3) {
    log_printf("Buffer too small for header.\n");
    return 0;
  }

  // 首先，接收从机地址、功能码和数据长度/异常码
  if (!rs485_receive(&port->uart_controller, data, 3, 10)) {
    /* log_printf("Failed to receive header.\n"); */
    return 0;
  }
  /* log_printf("dump 3:"); DumpHex(data, 3); */

  // 确定消息的总长度
  uint16_t expected_length = 0;
  switch (data[1]) {
  case MODBUS_READ_COILS:
  case MODBUS_READ_DISCRETE_INPUTS:
  case MODBUS_READ_INPUT_REGISTERS:
  case MODBUS_READ_HOLDING_REGISTERS:
    expected_length = data[2] + 5;
    break;

  case MODBUS_WRITE_SINGLE_COIL:
  case MODBUS_WRITE_SINGLE_REGISTER:
  case MODBUS_MASK_WRITE_REGISTER:
  case MODBUS_READ_WRITE_MULTIPLE_REG:
    expected_length = 8;
    break;

  case MODBUS_WRITE_MULTIPLE_COILS:
  case MODBUS_WRITE_MULTIPLE_REGISTERS:
    expected_length = 8;
    break;

    // TODO: 为其他功能码添加对应的消息长度处理逻辑

  default:
    /* log_printf("Unsupported function code: 0x%02X\n", data[1]); */
    /* DumpHex(data, 3); */
    return 0;
  }
  
  /* log_printf("exp len:%d\n", expected_length); */

  if (buffer_length < expected_length) {
    /* log_printf("Buffer too small for expected response. Buffer size: %d, " */
    /*            "Expected length: %d.\n", */
    /*            buffer_length, expected_length); */
    return 0;
  }
  
  uint16_t computed_crc = ModbusRTU_CRC16(data, 3, CRC_START);  // CRC for the header

  // 接收剩下的数据
  if (!rs485_receive(&port->uart_controller, data, expected_length - 3,
                     10)) {
    /* log_printf("Failed to receive the rest of the data.\n"); */
    return 0;
  }
  /* log_printf("dump rest:"); DumpHex(data, expected_length - 3); */


  // 验证CRC
  computed_crc = ModbusRTU_CRC16(data, expected_length - 3, computed_crc);

  // 验证CRC
  uint16_t received_crc = (data[expected_length - 4] << 8) | data[expected_length - 5];
  
  if (0 != computed_crc) {
    /* log_printf("CRC check failed. Got: 0x%04X\n", */
    /*            computed_crc); */
    /* DumpHex(data, expected_length - 3); */
    return 0;
  }

  return expected_length - 5;
}

