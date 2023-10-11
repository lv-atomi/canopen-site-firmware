#include "laser_displacement.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h> // For memcpy
#include "log.h"
#include "modbus_rtu.h"
#include "rs485.h"

void laser_displacement_resync(LaserDisplacement *sensor){
  ASSERT(sensor);
  uint8_t response[MODBUS_MAX_MESSAGE_LENGTH];

  LaserBaudRate rate;
  /* resync */
  uint16_t max_size = MODBUS_MAX_MESSAGE_LENGTH;
  rs485_receive_variable(&sensor->port.uart_controller,
			 response, &max_size, 3);
  /* log_printf("Resync done, %d bytes dropped.\n", max_size); */
  
  /* ModbusRTU_Flush(&sensor->port); */
  /* LaserDisplacement_GetBaudRate(sensor, &rate); */
  /* ASSERT(rate == sensor->port.uart_controller.port.baud_rate); */
}

/** 辅助函数：发送Modbus请求并接收响应 */
static bool LaserDisplacement_ReadRegister(LaserDisplacement *sensor,
                                           uint16_t registerAddress,
                                           uint16_t registerCount,
                                           void *outValue, uint16_t dataSize, bool recv_only) {
  uint8_t response[MODBUS_MAX_MESSAGE_LENGTH];
  uint16_t actualResponseSize = 0;
  uint16_t expectedResponseSize = registerCount * 2;

  if (dataSize < expectedResponseSize) {
    /* log_printf("Output buffer size is not sufficient:%d, %d\n", dataSize, expectedResponseSize); */
    return false;
  }

  // Send request
  if (!recv_only && !ModbusRTU_SendRequest(&sensor->port, MODBUS_READ_INPUT_REGISTERS,
					   registerAddress, registerCount, NULL, 0)) {
    /* log_printf("Failed to send request.\n"); */
    laser_displacement_resync(sensor);

    return false;
  }

  // Receive response
  if ((actualResponseSize = ModbusRTU_ReadResponse(&sensor->port,
						   response, MODBUS_MAX_MESSAGE_LENGTH))==0) {
    /* log_printf("Failed to receive response or response is invalid.\n"); */
    /* DumpHex(response, actualResponseSize);  // <--- Dump the received data for debugging */
    laser_displacement_resync(sensor);
    return false;
  }
  
  if (actualResponseSize != expectedResponseSize) {
    /* log_printf("Unexpected data size in response. Expected %d, but got %d.\n", expectedResponseSize, actualResponseSize); */
    /* DumpHex(response, actualResponseSize);  // <--- Dump the received data for debugging */
    laser_displacement_resync(sensor);

    return false;
  }


  uint8_t *src = response;
  uint8_t *dst = (uint8_t *)outValue;

  for (uint16_t i = 0; i < expectedResponseSize; i ++) {
    dst[expectedResponseSize - i - 1] = src[i]; 
  }
  /* // Copy data to output buffer */
  /* memcpy(outValue, response, expectedResponseSize); */
  /* log_printf("response:"); DumpHex(response, expectedResponseSize); */
  return true;
}


int32_t LaserDisplacement_GetDistance(LaserDisplacement *sensor, bool recv_only) {
  uint32_t DistanceValue;
  if (LaserDisplacement_ReadRegister(sensor, 0x0000, 2, &DistanceValue,
                                     sizeof(DistanceValue), recv_only)) {
    /* log_printf("dis: %ld\n", DistanceValue); */
    return DistanceValue >= 2147483647 ? -1 : (int32_t)DistanceValue;
  }
  return -1; // Return -1 or another specific error value if reading fails
}

/** 获取传感器的工作模式 */
bool LaserDisplacement_GetWorkMode(LaserDisplacement *sensor,
                                   LaserWorkMode *mode) {
  uint16_t registerValue;

  if (LaserDisplacement_ReadRegister(sensor, 0x0001, 1, &registerValue,
                                     sizeof(registerValue), false)) {
    *mode = (LaserWorkMode)registerValue;
    return true;
  }
  return false;
}

/** 获取传感器的输出模式 */
bool LaserDisplacement_GetOutputMode(LaserDisplacement *sensor,
                                     LaserOutputMode *mode) {
  uint16_t registerValue;

  if (LaserDisplacement_ReadRegister(sensor, 0x0003, 1, &registerValue,
                                     sizeof(registerValue), false)) {
    *mode = (LaserOutputMode)registerValue;
    return true;
  }
  return false;
}

/** 获取传感器的外部输入模式 */
bool LaserDisplacement_GetExternalInput(LaserDisplacement *sensor,
                                        LaserExternalInput *input) {
  uint16_t registerValue;

  if (LaserDisplacement_ReadRegister(sensor, 0x0005, 1, &registerValue,
                                     sizeof(registerValue), false)) {
    *input = (LaserExternalInput)registerValue;
    return true;
  }
  return false;
}

/** 获取传感器的输出状态 */
bool LaserDisplacement_GetOutputState(LaserDisplacement *sensor,
                                      LaserOutputState *state) {
  uint16_t registerValue;

  if (LaserDisplacement_ReadRegister(sensor, 0x0002, 1, &registerValue,
                                     sizeof(registerValue), false)) {
    *state = (LaserOutputState)registerValue;
    return true;
  }
  return false;
}

/** 获取传感器的显示模式 */
bool LaserDisplacement_GetDisplayMode(LaserDisplacement *sensor,
                                      LaserDisplayMode *mode) {
  uint16_t registerValue;

  if (LaserDisplacement_ReadRegister(sensor, 0x0008, 1, &registerValue,
                                     sizeof(registerValue), false)) {
    *mode = (LaserDisplayMode)registerValue;
    return true;
  }
  return false;
}

/** 获取传感器的保持模式 */
bool LaserDisplacement_GetHoldMode(LaserDisplacement *sensor,
                                   LaserHoldState *mode) {
  uint16_t registerValue;

  if (LaserDisplacement_ReadRegister(sensor, 0x0009, 1, &registerValue,
                                     sizeof(registerValue), false)) {
    *mode = (LaserHoldState)registerValue;
    return true;
  }
  return false;
}

/** 获取传感器的屏幕状态 */
bool LaserDisplacement_GetScreenState(LaserDisplacement *sensor,
                                      LaserScreenState *state) {
  uint16_t registerValue;

  if (LaserDisplacement_ReadRegister(sensor, 0x000A, 1, &registerValue,
                                     sizeof(registerValue), false)) {
    *state = (LaserScreenState)registerValue;
    return true;
  }
  return false;
}

/** 获取传感器的输出定时模式 */
bool LaserDisplacement_GetOutputTimingMode(LaserDisplacement *sensor,
                                           LaserOutputTiming *mode) {
  uint16_t registerValue;

  if (LaserDisplacement_ReadRegister(sensor, 0x0006, 1, &registerValue,
                                     sizeof(registerValue), false)) {
    *mode = (LaserOutputTiming)registerValue;
    return true;
  }
  return false;
}

/** 获取传感器的波特率 */
bool LaserDisplacement_GetBaudRate(LaserDisplacement *sensor,
                                   LaserBaudRate *rate) {
  uint16_t registerValue;

  if (LaserDisplacement_ReadRegister(sensor, 0x000E, 2, &registerValue,
                                     sizeof(registerValue), false)) {
    *rate = (LaserBaudRate)registerValue;
    return true;
  }
  return false;
}

/** 获取传感器的定时时间 */
bool LaserDisplacement_GetTimingTime(LaserDisplacement *sensor,
                                     uint16_t *timingTime) {
  return LaserDisplacement_ReadRegister(sensor, 0x0007, 1, timingTime,
                                        sizeof(uint16_t), false);
}

/** 获取传感器的配置，如调零值、阈值等 */
bool LaserDisplacement_GetConfigurations(LaserDisplacement *sensor,
                                         LaserConfigurations *config) {
  uint16_t buffer[4]; // 4 registers to read

  if (LaserDisplacement_ReadRegister(sensor, 0x000B, 4, buffer,
                                     sizeof(buffer), false)) {
    config->zeroValue = buffer[0];
    config->threshold1 = buffer[1];
    config->threshold2 = buffer[2];
    config->timingTime = buffer[3];
    return true;
  }
  return false;
}

/** 辅助函数：写入多个寄存器 */
static bool LaserDisplacement_WriteMultipleRegisters(LaserDisplacement *sensor,
                                                     uint16_t startAddress,
                                                     uint16_t registerCount,
                                                     const uint16_t *values) {
    // Prepare the data buffer for ModbusRTU_SendRequest
    uint8_t data[2 * registerCount + 5];  // 5 = byte count + 2 bytes for each address and value

    data[0] = (registerCount * 2) & 0xFF;  // number of bytes

    for (int i = 0; i < registerCount; i++) {
        data[1 + i * 2] = (values[i] >> 8) & 0xFF;  // high byte
        data[1 + i * 2 + 1] = values[i] & 0xFF;     // low byte
    }

    // Send the prepared data using ModbusRTU_SendRequest
    if (!ModbusRTU_SendRequest(&sensor->port, MODBUS_WRITE_MULTIPLE_REGISTERS,
                               startAddress, registerCount, data, 1 + 2 * registerCount)) {
        log_printf("ModbusRTU_SendRequest failed for register writing.\n");
        return false;
    }

    uint8_t response[MODBUS_MAX_MESSAGE_LENGTH];
    if (!ModbusRTU_ReadResponse(&sensor->port, response, MODBUS_MAX_MESSAGE_LENGTH)) {

      log_printf("Failed to read response after register writing.\n");
      
      return false;
    }

    // The response is expected to be an echo of the request, so it can be validated if needed.
    return true;
}



bool LaserDisplacement_SetWorkMode(LaserDisplacement *sensor, LaserWorkMode mode) {
    uint16_t modeValue = (uint16_t) mode;
    return LaserDisplacement_WriteMultipleRegisters(sensor, 0x0001, 1, &modeValue);
}

bool LaserDisplacement_SetOutputMode(LaserDisplacement *sensor, LaserOutputMode mode) {
    uint16_t modeValue = (uint16_t) mode;
    return LaserDisplacement_WriteMultipleRegisters(sensor, 0x0003, 1, &modeValue);
}

bool LaserDisplacement_SetExternalInput(LaserDisplacement *sensor, LaserExternalInput input) {
    uint16_t inputValue = (uint16_t) input;
    return LaserDisplacement_WriteMultipleRegisters(sensor, 0x0005, 1, &inputValue);
}

bool LaserDisplacement_SetContinuousOutput(LaserDisplacement *sensor, bool continuous) {
    uint16_t value = continuous ? 0x0001 : 0x0000;
    return LaserDisplacement_WriteMultipleRegisters(sensor, 0x0000, 1, &value);
}

bool LaserDisplacement_SetOutputState(LaserDisplacement *sensor, LaserOutputState state) {
    uint16_t stateValue = (uint16_t) state;
    return LaserDisplacement_WriteMultipleRegisters(sensor, 0x0002, 1, &stateValue);
}

bool LaserDisplacement_SetDisplayMode(LaserDisplacement *sensor, LaserDisplayMode mode) {
    uint16_t modeValue = (uint16_t) mode;
    return LaserDisplacement_WriteMultipleRegisters(sensor, 0x0008, 1, &modeValue);
}

bool LaserDisplacement_SetHoldMode(LaserDisplacement *sensor, LaserHoldState mode) {
    uint16_t modeValue = (uint16_t) mode;
    return LaserDisplacement_WriteMultipleRegisters(sensor, 0x0009, 1, &modeValue);
}

bool LaserDisplacement_SetScreenState(LaserDisplacement *sensor, LaserScreenState state) {
    uint16_t stateValue = (uint16_t) state;
    return LaserDisplacement_WriteMultipleRegisters(sensor, 0x000A, 1, &stateValue);
}

bool LaserDisplacement_SetZeroingCurrentValue(LaserDisplacement *sensor) {
    uint16_t value = 0x0001;
    return LaserDisplacement_WriteMultipleRegisters(sensor, 0x000B, 1, &value);
}

bool LaserDisplacement_SetThresholdValue(LaserDisplacement *sensor, uint16_t threshold1, uint16_t threshold2) {
    uint16_t values[2] = {threshold1, threshold2};
    return LaserDisplacement_WriteMultipleRegisters(sensor, 0x000C, 2, values);
}

bool LaserDisplacement_SetBaudRate(LaserDisplacement *sensor, LaserBaudRate rate) {
    uint16_t rateValue = (uint16_t) rate;
    return LaserDisplacement_WriteMultipleRegisters(sensor, 0x000E, 1, &rateValue);
}

bool LaserDisplacement_Reset(LaserDisplacement *sensor) {
    uint16_t value = 0x0000;
    return LaserDisplacement_WriteMultipleRegisters(sensor, 0x000F, 1, &value);
}

bool LaserDisplacement_SetDifferenceValue(LaserDisplacement *sensor, uint16_t value1, uint16_t value2) {
    uint16_t values[2] = {value1, value2};
    return LaserDisplacement_WriteMultipleRegisters(sensor, 0x0004, 2, values);
}

bool LaserDisplacement_SetOutputTimingMode(LaserDisplacement *sensor, LaserOutputTiming mode) {
    uint16_t modeValue = (uint16_t) mode;
    return LaserDisplacement_WriteMultipleRegisters(sensor, 0x0006, 1, &modeValue);
}

bool LaserDisplacement_SetOutputTimingTime(LaserDisplacement *sensor, uint16_t timingTime) {
    return LaserDisplacement_WriteMultipleRegisters(sensor, 0x0007, 1, &timingTime);
}

// --- LaserDisplacement.c ---
void init_laser_displacement(LaserDisplacement *sensor) {
  ASSERT(sensor);
  ModbusRTU_Init(
      &sensor->port); // Assuming this is correct for your ModbusRTU port setup
  sensor->cur_pos = 0;
  sensor->zero_pos = 0;
}

void laser_displacement_set_baudrate(LaserDisplacement *sensor,
                                     LaserBaudRate rate) {
  ASSERT(sensor);
  LaserDisplacement_SetBaudRate(sensor, rate);
}

void laser_displacement_measure_continously(LaserDisplacement *sensor) {
  ASSERT(sensor);
  uint16_t value = 1; // Value for continuous output
  LaserDisplacement_WriteMultipleRegisters(sensor, 0x0000, 1, &value);
}

bool laser_displacement_get_distance(LaserDisplacement *sensor, bool raw, bool recv_only, int32_t * outValue) {
  ASSERT(sensor);
  ASSERT(outValue);
  
  int32_t distance;
  distance = LaserDisplacement_GetDistance(sensor, recv_only);
  if (distance <0)   /* ASSERT( distance >= 0); */
    return false; 
  distance = distance / 10;	/* 返回值以0.001mm为单位，为了规避浮点数，所有距离都以0.01mm为单位计算 */
  if (raw) {
    *outValue = distance;
    return true;
  } else {
    *outValue = distance - sensor->zero_pos;
    return true;
  }
  return false; // Error or invalid sensor
}

void laser_displacement_set_zero_point(LaserDisplacement *sensor,
                                       uint32_t zero_pos) {
  ASSERT(sensor);
  sensor->zero_pos = zero_pos;
}
