#ifndef GPIOLINK_CRC8_H
#define GPIOLINK_CRC8_H

#include "one_wire.h"
#include <stdint.h>

// 定义状态为enum类型
typedef enum {
  GL_STATE_IDLE,      // 待发送
  GL_STATE_SEND_DATA, // 发送数据
  GL_STATE_SEND_CRC,  // 发送CRC
  GL_STATE_DONE       // 发送完毕
} SendState;

/* // 定义绝色 */
/* typedef enum { */
/*   GL_SENDER,   // 发送方 */
/*   GL_RECEIVER, // 接收方 */
/* } GL_ROLE; */

typedef struct{
  OneWire port;
  uint8_t byte_to_send;
  uint8_t crc_to_send;
} gpiolink;

void init_gpiolink(gpiolink *, bool_t enable_pullup);
void gpiolink_master_send_byte(gpiolink *, uint8_t data);
bool_t gpiolink_slave_recv_byte(gpiolink *, uint8_t *data);

#endif
