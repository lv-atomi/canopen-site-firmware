#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <stdbool.h>
#include <stdint.h>
#include "rs485.h"

#define MODBUS_MAX_MESSAGE_LENGTH 260
#define MAX_MODBUS_PORTS 4 // 假设有4个串口支持Modbus RTU

typedef enum {
    MODBUS_READ_COILS = 0x01,                // Read Coils
    MODBUS_READ_DISCRETE_INPUTS = 0x02,     // Read Discrete Inputs
    MODBUS_READ_HOLDING_REGISTERS = 0x03,   // Read Holding Registers
    MODBUS_READ_INPUT_REGISTERS = 0x04,     // Read Input Registers
    MODBUS_WRITE_SINGLE_COIL = 0x05,        // Write Single Coil
    MODBUS_WRITE_SINGLE_REGISTER = 0x06,    // Write Single Register
    MODBUS_READ_EXCEPTION_STATUS = 0x07,    // Read Exception Status (Serial Line only)
    MODBUS_DIAGNOSTICS = 0x08,              // Diagnostics (Serial Line only)
    MODBUS_GET_COMM_EVENT_COUNTER = 0x0B,   // Get Comm Event Counter (Serial Line only)
    MODBUS_GET_COMM_EVENT_LOG = 0x0C,       // Get Comm Event Log (Serial Line only)
    MODBUS_WRITE_MULTIPLE_COILS = 0x0F,     // Write Multiple Coils
    MODBUS_WRITE_MULTIPLE_REGISTERS = 0x10, // Write Multiple registers
    MODBUS_REPORT_SLAVE_ID = 0x11,          // Report Slave ID (Serial Line only)
    MODBUS_READ_FILE_RECORD = 0x14,         // Read File Record
    MODBUS_WRITE_FILE_RECORD = 0x15,        // Write File Record
    MODBUS_MASK_WRITE_REGISTER = 0x16,      // Mask Write Register
    MODBUS_READ_WRITE_MULTIPLE_REG = 0x17,  // Read/Write Multiple registers
    MODBUS_READ_FIFO_QUEUE = 0x18           // Read FIFO Queue
} ModbusFunctionCode;

typedef struct {
  uint8_t slave_address;
  RS485Port uart_controller; // 你需要替换成你的UART controller的指针或者结构体类型
  uint8_t tx_buf[MODBUS_MAX_MESSAGE_LENGTH];
  uint8_t rx_buf[MODBUS_MAX_MESSAGE_LENGTH];
  uint16_t rx_length; // 接收到的消息长度
} ModbusRTUPort;

void ModbusRTU_Init(ModbusRTUPort *port);
bool ModbusRTU_SendRequest(ModbusRTUPort *port,
                           ModbusFunctionCode function_code,
                           uint16_t start_address, uint16_t quantity,
			   uint8_t *data, uint16_t data_length);
uint16_t ModbusRTU_ReadResponse(ModbusRTUPort *port, uint8_t *data,
				uint16_t data_length);
bool ModbusRTU_Flush(ModbusRTUPort *port);

#endif
