#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "can.h"
#include "timer1.h"
#include "gpio.h"
#include "oled.h"
#include "bmp.h"
#include "flash.h"
#include "boot.h"

extern SysParm_TypeDef gSysParm;
extern KeyPressMode keySta;

enum{
  Mode_INIT,  
	Mode_WAIT,
	Mode_BOOT,
	Mode_JUMP,
};
uint8_t workmode = Mode_INIT;
uint32_t gBootDelayCnt = 0;  

error_status err_status;
uint8_t  CycleSendFlg = 0;//0: send, 1: stop send
uint16_t buffer_write[TEST_BUFEER_SIZE];//[0]:ID; [1]:Voltage
uint16_t buffer_read[TEST_BUFEER_SIZE]; //[0]:ID; [1]:Voltage

uint16_t VolShowTest = DEFAULTVOL;
int main(void)
{	

	system_clock_config(); //8M HSE
  at32_board_init();
	
//	Timer1_Init();
//	OLED_Init();

//	GPIO_Init();
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  can_gpio_config();
  can_configuration();
  CAN_DataInit();  //Data Init

	
	/* read data from flash */
//  flash_read(TEST_FLASH_ADDRESS_START, buffer_read, TEST_BUFEER_SIZE);
//	gSysParm.basePara.CanDataId = buffer_read[0];
//	if((gSysParm.basePara.CanDataId >= 0x7ff)||(gSysParm.basePara.CanDataId == 0)) gSysParm.basePara.CanDataId = SLAVEBOARD_ID;
//	VolShowTest = buffer_read[1];
//	if((VolShowTest > 350)||(VolShowTest == 0)) VolShowTest = DEFAULTVOL;
	
  while(1)
  {		
		switch(workmode)
		{
			case Mode_INIT:
				Init_Ram();
				workmode = Mode_WAIT;
				break;
			
			case Mode_WAIT:
				gBootDelayCnt++;
				if(gBootDelayCnt > BOOTWAITTIME)
				{
					gBootDelayCnt = BOOTWAITTIME+1;
					workmode = Mode_JUMP;
				}
				if(gSysParm.canRxCmd == CAN_RXD_BOOT)
				{
					gSysParm.canRxCmd = CAN_RXD_NULL;
					if(gSysParm.canRxBuf[0] == 0x52 && gSysParm.canRxBuf[1] == 0x45 && gSysParm.canRxBuf[2] == 0x51) //REQ
					{
						can_transmit_req(gSysParm.canRxBuf[3],gSysParm.canRxBuf[4]);
						workmode = Mode_BOOT;
					}
				}
				
			case Mode_BOOT:
				if(gSysParm.canRxCmd == CAN_RXD_BOOT)
				{
					gSysParm.canRxCmd = CAN_RXD_NULL;
					if(gSysParm.canRxBuf[0] == 0x44 && gSysParm.canRxBuf[1] == 0x4f && gSysParm.canRxBuf[2] == 0x45) //DOE
					{
						Block_done();
					}
					else if(gSysParm.canRxBuf[0] == 0x43 && gSysParm.canRxBuf[1] == 0x4f && gSysParm.canRxBuf[2] == 0x4D \
						&& gSysParm.canRxBuf[3] == 0x50 && gSysParm.canRxBuf[4] == 0x4C && gSysParm.canRxBuf[5] == 0x45 \
					  && gSysParm.canRxBuf[6] == 0x54 && gSysParm.canRxBuf[7] == 0x4C)// Complete
					{
						Block_copy();
					}
					else if(gSysParm.canRxBuf[0] == 0x47 && gSysParm.canRxBuf[1] == 0x4f && gSysParm.canRxBuf[2] == 0x54 \
						&& gSysParm.canRxBuf[3] == 0x4F && gSysParm.canRxBuf[4] == 0x41 && gSysParm.canRxBuf[5] == 0x50 \
					  && gSysParm.canRxBuf[6] == 0x50)// go to app
					{
						
						Relay_Reset();  //before jump, replay can message
						workmode = Mode_JUMP;
					}
					else
					{						
						main_data_transmit();
					}
				}
			
				break;
			
			case Mode_JUMP:
				boot_jump_to_app();
				break;
		}
  }
}

