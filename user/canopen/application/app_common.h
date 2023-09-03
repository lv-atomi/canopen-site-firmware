#ifndef APP_COMMON_H
#define APP_COMMON_H

#include "log.h"
#include "CANopen.h"
#include "storage/CO_storage.h"
#include "OD.h"


/** Get int32_t value from memory buffer, see @ref CO_getUint8 */
static inline int8_t CO_getInt8(const void *buf) {
  int8_t value; memmove(&value, buf, sizeof(value)); return value;
}
/** Get int16_t value from memory buffer, see @ref CO_getUint8 */
static inline int16_t CO_getInt16(const void *buf) {
  int16_t value; memmove(&value, buf, sizeof(value)); return value;
}
/** Get int32_t value from memory buffer, see @ref CO_getUint8 */
static inline int32_t CO_getInt32(const void *buf) {
  int32_t value; memmove(&value, buf, sizeof(value)); return value;
}

/** Write int8_t value into memory buffer, see @ref CO_setUint8 */
static inline uint8_t CO_setInt8(void *buf, int8_t value) {
    memmove(buf, &value, sizeof(value)); return sizeof(value);
}

/** Write int16_t value into memory buffer, see @ref CO_setUint8 */
static inline uint8_t CO_setInt16(void *buf, int16_t value) {
    memmove(buf, &value, sizeof(value)); return sizeof(value);
}

/** Write int32_t value into memory buffer, see @ref CO_setUint8 */
static inline uint8_t CO_setInt32(void *buf, int32_t value) {
    memmove(buf, &value, sizeof(value)); return sizeof(value);
}

#endif
