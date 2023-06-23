#include "slavestation_14.h"

/*
 * Slaveboard model 1, can station 14:
 * mainly used for air condition control
 * -485 interface
 *     + connect to inverter with modbus
 *     + control inverter output frequency
 *     + recv working status & warnings
 *     + TBD
 * -FAN x3
 *     + Fan speed sense
 *     + Fan speed control
 * -Temperature sensor x3
 *     + DS18B20
 *
 */
