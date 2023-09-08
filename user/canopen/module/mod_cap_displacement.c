//#include "CO_app_STM32.h"
#include "301/CO_ODinterface.h"
#include "301/CO_driver.h"
#include "OD.h"
#include "app_common.h"
#include "mod_cap_displacement.h"
#include <stdint.h>
#include "cap_displacement.h"

/*
 */

OD_extension_t OD_6402_extension;

CapacitorDisplacementMeasurePort displacement = {
    .spi = {.miso = {GPIOB, GPIO_PINS_SOURCE15},
            .clk = {GPIOB, GPIO_PINS_SOURCE13},
            .controller = SPI2,
            .init_type = {
                .transmission_mode = SPI_TRANSMIT_SIMPLEX_RX,
                .master_slave_mode = SPI_MODE_SLAVE,
                .mclk_freq_division = SPI_MCLK_DIV_32,
                .first_bit_transmission = SPI_FIRST_BIT_LSB,
                .frame_bit_num = SPI_FRAME_8BIT,
                .clock_polarity = SPI_CLOCK_POLARITY_HIGH,
                .clock_phase = SPI_CLOCK_PHASE_2EDGE,
                .cs_mode_selection = SPI_CS_SOFTWARE_MODE,
            }}};

/* BaseModule temperature */
// 读取容栅尺当前状态
// 6402.1 位移
// 6402.2 时延
// 6402.3 清零
static ODR_t my_OD_read_6402(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {

  uint32_t delay=0;
  log_printf("read 6402, subidx:%d\n", stream->subIndex);

  switch (stream->subIndex){
  case 0:
    CO_setUint8(buf, 3);
    *countRead = sizeof(uint8_t);
    return ODR_OK;
  case 1:			/* read displacement */
    CO_setInt32(buf, read_displacement(&displacement, &delay, 0));
    break;
  case 2:			/* read delay */
    read_displacement(&displacement, &delay, 0);
    CO_setInt32(buf, (int32_t)delay);
    break;
  case 3:			/* read zero */
    return ODR_WRITEONLY;
  default:
    return ODR_IDX_NOT_EXIST;
  }
  *countRead = 4;
  return ODR_OK;
}

static ODR_t my_OD_write_6402(OD_stream_t *stream, const void *buf,
			     OD_size_t count, OD_size_t *countWritten) {

  log_printf("write 6402, subidx:%d\n", stream->subIndex);

  switch (stream->subIndex){
  case 0:
  case 1:			/* write displacement */
  case 2:			/* write delay */
    return ODR_READONLY;
  case 3:			/* set zero */
    displacement_set_zero(&displacement);
    break;
  default:
    return ODR_IDX_NOT_EXIST;
  }
  *countWritten = 4;
  return ODR_OK;
}

void cap_displacement_peripheral_init() {
  init_capacitor_displacement_measurement(&displacement);
}

CO_ReturnError_t app_cap_displacement_init() {
  // 初始化每个OD扩展，并设置读写回调函数
  OD_entry_t * param_6402 = OD_ENTRY_H6402_capacitorDisplacement;
  OD_6402_extension.object = NULL;
  OD_6402_extension.read = my_OD_read_6402;
  OD_6402_extension.write = my_OD_write_6402;
  OD_extension_init(param_6402, &OD_6402_extension);

  // 初始化外设
  cap_displacement_peripheral_init();
  return CO_ERROR_NO;
}

