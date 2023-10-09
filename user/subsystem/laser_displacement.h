#ifndef LASER_DISPLACEMENT_H
#define LASER_DISPLACEMENT_H

#include "modbus_rtu.h"
#include "rs485.h"
#include <stdint.h>
#include <stdbool.h>
    
typedef struct {
  ModbusRTUPort port;
  uint32_t cur_pos;
  uint32_t zero_pos;
} LaserDisplacement;

typedef enum {
    WORK_MODE_HIGH_ACCURACY,
    WORK_MODE_STANDARD,
    WORK_MODE_HIGH_SPEED,
    WORK_MODE_UNKNOWN
} LaserWorkMode;

typedef enum {
    OUTPUT_MODE_NORMAL,
    OUTPUT_MODE_ONE_POINT_TEACH,
    OUTPUT_MODE_TWO_POINT_TEACH,
    OUTPUT_MODE_THREE_POINT_TEACH,
    OUTPUT_MODE_UNKNOWN
} LaserOutputMode;

typedef enum {
    EXTERNAL_INPUT_ZEROING,
    EXTERNAL_INPUT_TEACH,
    EXTERNAL_INPUT_STOP_LASER,
    EXTERNAL_INPUT_UNKNOWN
} LaserExternalInput;

typedef enum {
    OUTPUT_STATE_ALWAYS_OPEN,
    OUTPUT_STATE_ALWAYS_CLOSE,
    OUTPUT_STATE_UNKNOWN
} LaserOutputState;

typedef enum {
    DISPLAY_MODE_REGULAR,
    DISPLAY_MODE_REVERSE,
    DISPLAY_MODE_OFFSET,
    DISPLAY_MODE_UNKNOWN
} LaserDisplayMode;

typedef enum {
    HOLD_STATE_ON,
    HOLD_STATE_OFF,
    HOLD_STATE_UNKNOWN
} LaserHoldState;

typedef enum {
    SCREEN_STATE_TIMED_OFF,
    SCREEN_STATE_ALWAYS_ON,
    SCREEN_STATE_UNKNOWN
} LaserScreenState;

typedef enum {
    OUTPUT_TIMING_NONE,
    OUTPUT_TIMING_EXTENDED,
    OUTPUT_TIMING_DELAYED,
    OUTPUT_TIMING_SINGLE_SHOT,
    OUTPUT_TIMING_UNKNOWN
} LaserOutputTiming;

typedef enum {
  BAUD_RATE_4800 = 0x000012C0,
  BAUD_RATE_9600 = 0x00002580,
  BAUD_RATE_38400 = 0x00009600,
  BAUD_RATE_115200 = 0x0001C200,
  BAUD_RATE_256000 = 0x0003E800,
  BAUD_RATE_UNKNOWN
} LaserBaudRate;

typedef struct {
    uint16_t zeroValue;
    uint16_t threshold1;
    uint16_t threshold2;
    uint16_t timingTime;   // 定时时间
} LaserConfigurations;



// Function API Declarations

/** 获取传感器的工作模式 */
bool LaserDisplacement_GetWorkMode(LaserDisplacement *sensor, LaserWorkMode *mode);

/** 获取传感器的输出模式 */
bool LaserDisplacement_GetOutputMode(LaserDisplacement *sensor, LaserOutputMode *mode);

/** 获取传感器的外部输入模式 */
bool LaserDisplacement_GetExternalInput(LaserDisplacement *sensor, LaserExternalInput *input);

/** 获取传感器的输出状态 */
bool LaserDisplacement_GetOutputState(LaserDisplacement *sensor, LaserOutputState *state);

/** 获取传感器的显示模式 */
bool LaserDisplacement_GetDisplayMode(LaserDisplacement *sensor, LaserDisplayMode *mode);

/** 获取传感器的保持模式 */
bool LaserDisplacement_GetHoldMode(LaserDisplacement *sensor, LaserHoldState *mode);

/** 获取传感器的屏幕状态 */
bool LaserDisplacement_GetScreenState(LaserDisplacement *sensor, LaserScreenState *state);

/** 获取传感器的输出定时模式 */
bool LaserDisplacement_GetOutputTimingMode(LaserDisplacement *sensor, LaserOutputTiming *mode);

/** 获取传感器的波特率 */
bool LaserDisplacement_GetBaudRate(LaserDisplacement *sensor, LaserBaudRate *rate);

/** 获取传感器的定时时间 */
bool LaserDisplacement_GetTimingTime(LaserDisplacement *sensor, uint16_t *timingTime);

/** 获取传感器的配置，如调零值、阈值等 */
bool LaserDisplacement_GetConfigurations(LaserDisplacement *sensor, LaserConfigurations *config);


/**
 * Initializes the LaserDisplacement sensor with default parameters.
 * @param sensor Pointer to the LaserDisplacement structure.
 * @param modbusAddr Modbus address of the sensor.
 * @return true if initialization was successful, false otherwise.
 */
bool LaserDisplacement_Init(LaserDisplacement *sensor, uint8_t modbusAddr);

void init_laser_displacement(LaserDisplacement *);
void laser_displacement_set_baudrate(LaserDisplacement *, LaserBaudRate);
void laser_displacement_measure_continously(LaserDisplacement *);
bool laser_displacement_get_distance(LaserDisplacement *, bool raw, bool recv_only, int32_t* outValue);
void laser_displacement_set_zero_point(LaserDisplacement *, uint32_t zero_pos);

bool LaserDisplacement_SetWorkMode(LaserDisplacement *sensor, LaserWorkMode mode);
bool LaserDisplacement_SetOutputMode(LaserDisplacement *sensor, LaserOutputMode mode);
bool LaserDisplacement_SetExternalInput(LaserDisplacement *sensor, LaserExternalInput input);
bool LaserDisplacement_SetContinuousOutput(LaserDisplacement *sensor, bool continuous);
bool LaserDisplacement_SetOutputState(LaserDisplacement *sensor, LaserOutputState state);
bool LaserDisplacement_SetDisplayMode(LaserDisplacement *sensor, LaserDisplayMode mode);
bool LaserDisplacement_SetHoldMode(LaserDisplacement *sensor, LaserHoldState mode);
bool LaserDisplacement_SetScreenState(LaserDisplacement *sensor, LaserScreenState state);
bool LaserDisplacement_SetZeroingCurrentValue(LaserDisplacement *sensor);
bool LaserDisplacement_SetThresholdValue(LaserDisplacement *sensor, uint16_t threshold1, uint16_t threshold2);
bool LaserDisplacement_SetBaudRate(LaserDisplacement *sensor, LaserBaudRate rate);
bool LaserDisplacement_Reset(LaserDisplacement *sensor);
bool LaserDisplacement_SetDifferenceValue(LaserDisplacement *sensor, uint16_t value1, uint16_t value2);
bool LaserDisplacement_SetOutputTimingMode(LaserDisplacement *sensor, LaserOutputTiming mode);
bool LaserDisplacement_SetOutputTimingTime(LaserDisplacement *sensor, uint16_t timingTime);

#endif
