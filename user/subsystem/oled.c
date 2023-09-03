#include "oled.h"
#include "gpio.h"
#include "log.h"
#include "i2c_soft.h"
#include "oledfont.h"
#include <string.h>

#define OLED_CMD    0
#define OLED_DATA   1

void oled_write_register_byte(OLEDPort * devport, uint8_t dat, uint8_t mode) {
  static uint8_t buf[2];

  ASSERT(devport);
  buf[0] = mode ? 0x40 : 0x00;	/* OLED_CMD-> 0x00; OLED_DATA -> 0x40 */
  buf[1] = dat;
  i2c_master_transmit_soft(&devport->i2c,
			   buf, 2,
			   0);  
}

void init_oled(OLEDPort *devport) {
  /* printf("6.1.1:%p %p %p %p\n", OD, &OD->list[9], OD->list[9].extension, OD->list[9].extension->write); */
  /* printf("6.1.2:%p %p %p %p\n", OD, &OD->list[9], OD->list[9].extension, OD->list[9].extension->write); */
  /* printf("6.1.3:%p %p %p %p\n", OD, &OD->list[9], OD->list[9].extension, OD->list[9].extension->write); */
  ASSERT(devport);

  init_gpio_output(&devport->en,
		   GPIO_OUTPUT_PUSH_PULL,
		   GPIO_DRIVE_STRENGTH_STRONGER);
  /* printf("6.2.init 1010h extension write:%p\n", OD->list[9].extension->write); */
  gpio_set(&devport->en, 0);	/* OLED EN pin ON */
  /* printf("6.3.init 1010h extension write:%p\n", OD->list[9].extension->write); */
  init_i2c_soft(&devport->i2c);
  /* printf("6.4.init 1010h extension write:%p\n", OD->list[9].extension->write); */
  
  oled_write_register_byte(devport, 0xAE, OLED_CMD); /*display off*/
  oled_write_register_byte(devport, 0xD5, OLED_CMD); /*set osc division*/
  oled_write_register_byte(devport, 0xF0, OLED_CMD);
  oled_write_register_byte(devport, 0xA8, OLED_CMD); /*multiplex ratio*/
  oled_write_register_byte(devport, 0x27, OLED_CMD); /*duty = 1/40*/
  oled_write_register_byte(devport, 0xD3, OLED_CMD); /*set display offset*/
  oled_write_register_byte(devport, 0x00, OLED_CMD);
  oled_write_register_byte(devport, 0x40, OLED_CMD); /*Set Display Start Line */
  oled_write_register_byte(devport, 0x8d, OLED_CMD); /*set charge pump enable*/
  oled_write_register_byte(devport, 0x14, OLED_CMD);
  oled_write_register_byte(devport, 0x20, OLED_CMD); /*Set page address mode*/
  oled_write_register_byte(devport, 0x02, OLED_CMD);
  oled_write_register_byte(devport, 0xA1, OLED_CMD); /*set segment remap*/
  oled_write_register_byte(devport, 0xC8, OLED_CMD); /*Com scan direction*/
  oled_write_register_byte(devport, 0xDA, OLED_CMD); /*set COM pins*/
  oled_write_register_byte(devport, 0x12, OLED_CMD);
  oled_write_register_byte(devport, 0xAD, OLED_CMD); /*Internal IREF Setting*/
  oled_write_register_byte(devport, 0x30, OLED_CMD);
  oled_write_register_byte(devport, 0x81, OLED_CMD); /*contract control*/
  oled_write_register_byte(devport, 0xfF, OLED_CMD); /*128*/
  oled_write_register_byte(devport, 0xD9, OLED_CMD); /*set pre-charge period*/
  oled_write_register_byte(devport, 0x22, OLED_CMD);
  oled_write_register_byte(devport, 0xdb, OLED_CMD); /*set vcomh*/
  oled_write_register_byte(devport, 0x20, OLED_CMD);
  oled_write_register_byte(devport, 0xA4, OLED_CMD); /*Set Entire Display On/Off*/
  oled_write_register_byte(devport, 0xA6, OLED_CMD); /*normal / reverse*/
  oled_write_register_byte(devport, 0x0C, OLED_CMD); /*set lower column address*/
  oled_write_register_byte(devport, 0x11, OLED_CMD); /*set higher column address*/
  /* printf("6.5.init 1010h extension write:%p\n", OD->list[9].extension->write); */

  oled_write_register_byte(devport, 0xAF, OLED_CMD); /*display ON*/
  /* printf("6.6.init 1010h extension write:%p\n", OD->list[9].extension->write); */

  oled_clear(devport);
  /* printf("6.7.init 1010h extension write:%p\n", OD->list[9].extension->write); */

}

//画点
// x:0~127
// y:0~63
// t:1 填充 0,清空
void oled_drawpoint(OLEDPort * devport, uint8_t x, uint8_t y, uint8_t t) {
  uint8_t i, m, n;
  i = (y / 8) % OLED_GRAM_MAX_Y;
  x = x % OLED_GRAM_MAX_X;
  m = y % 8;
  n = 1 << m;
  if (t) {
    devport->OLED_GRAM[i][x] |= n;
  } else {
    devport->OLED_GRAM[i][x] = ~devport->OLED_GRAM[i][x];
    devport->OLED_GRAM[i][x] |= n;
    devport->OLED_GRAM[i][x] = ~devport->OLED_GRAM[i][x];
  }
}


void oled_refresh(OLEDPort * devport) {
  uint8_t i, n, old;
  uint8_t * gram = NULL;
  for (i = 0; i < 5; i++) {
    gram = (uint8_t *)(&devport->OLED_GRAM[i][0]);
    oled_write_register_byte(devport, 0xb0 + i, OLED_CMD); //设置行起始地址
    oled_write_register_byte(devport, 0x0c, OLED_CMD);     //设置低列起始地址
    oled_write_register_byte(devport, 0x11, OLED_CMD);     //设置高列起始地址

    old = gram[-1];
    gram[-1] = 0x40;		/* WARNING: access out of oled_gram, that's why we must declare padding byte in OLEDPort structure */
    i2c_master_transmit_soft(&devport->i2c,
			     gram-1,
			     72+1,
			     0);
    gram[-1] = old;
  }
}

void oled_display_off(OLEDPort * devport) {
  oled_write_register_byte(devport, 0x8D, OLED_CMD); //电荷泵使能
  oled_write_register_byte(devport, 0x10, OLED_CMD); //关闭电荷泵
  oled_write_register_byte(devport, 0xAE, OLED_CMD); //关闭屏幕
}

void oled_display_on(OLEDPort * devport) {
  oled_write_register_byte(devport, 0x8D, OLED_CMD); //电荷泵使能
  oled_write_register_byte(devport, 0x14, OLED_CMD); //开启电荷泵
  oled_write_register_byte(devport, 0xAF, OLED_CMD); //点亮屏幕
}

void oled_showchar(OLEDPort * devport,
		   uint8_t x, uint8_t y, char chr, uint8_t size1,
                   uint8_t mode) {
  uint8_t i, m, temp, size2, chr1;
  uint8_t x0 = x, y0 = y;
  if (size1 == 8)
    size2 = 6;
  else
    size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) *
            (size1 / 2); //得到字体一个字符对应点阵集所占的字节数
  chr1 = chr - ' ';      //计算偏移后的值
  for (i = 0; i < size2; i++) {
    if (size1 == 8) {
      temp = asc2_0806[chr1][i];
    } //调用0806字体
    else if (size1 == 12) {
      temp = asc2_1206[chr1][i];
    } //调用1206字体
    else if (size1 == 16) {
      temp = asc2_1608[chr1][i];
    } //调用1608字体
    else if (size1 == 24) {
      temp = asc2_2412[chr1][i];
    } //调用2412字体
    else
      return;
 
    for (m = 0; m < 8; m++) {
      if (temp & 0x01)
        oled_drawpoint(devport, x, y, mode);
      else
        oled_drawpoint(devport, x, y, !mode);
      temp >>= 1;
      y++;
    }
    x++;
    if ((size1 != 8) && ((x - x0) == size1 / 2)) {
      x = x0;
      y0 = y0 + 8;
    }
    y = y0;
  }
}

void oled_showstring(OLEDPort *devport,
		     uint8_t x, uint8_t y, char *str,
		     uint16_t len, /* 0-> auto */
		     uint8_t font_size, uint8_t mode) {
  char * chr = str;
  if (len == 0) len = strlen(str);
  while ((*chr >= ' ') && (*chr <= '~') && ((chr - str) < len)) //判断是不是非法字符!
  {
    oled_showchar(devport, x, y, *chr, font_size, mode);
    if (font_size == 8)
      x += 6;
    else
      x += font_size / 2;
    chr++;
  }
  
}

void oled_clear(OLEDPort * devport) {
  //uint8_t i, n;
  
  memset(&devport->OLED_GRAM[0][0], 0, sizeof(devport->OLED_GRAM));
  oled_refresh(devport); //更新显示
}
