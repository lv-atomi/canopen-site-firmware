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
#define APP_ADDRESS         0x08019000

enum{
  Mode_INIT,  
	Mode_CAN_CYCLE,
	Mode_CAN_BOOT,
};
uint8_t workmode = Mode_INIT;

error_status err_status;
uint8_t  CycleSendFlg = 0;//0: send, 1: stop send
uint16_t buffer_write[TEST_BUFEER_SIZE];//[0]:ID; [1]:Voltage
uint16_t buffer_read[TEST_BUFEER_SIZE]; //[0]:ID; [1]:Voltage

uint16_t VolShowTest = DEFAULTVOL;
int main(void)
{	
	SCB->VTOR = APP_ADDRESS;  
	system_clock_config(); //8M HSE
  at32_board_init();
	
	Timer1_Init();
	OLED_Init();

	GPIO_Init();
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  can_gpio_config();
  can_configuration();
  CAN_DataInit();  //Data Init
	
	/* read data from flash */
  flash_read(TEST_FLASH_ADDRESS_START, buffer_read, TEST_BUFEER_SIZE);
	gSysParm.basePara.CanDataId = buffer_read[0];
	if((gSysParm.basePara.CanDataId >= 0x7ff)||(gSysParm.basePara.CanDataId == 0)) gSysParm.basePara.CanDataId = SLAVEBOARD_ID;
	VolShowTest = buffer_read[1];
	if((VolShowTest > 350)||(VolShowTest == 0)) VolShowTest = DEFAULTVOL;
	
  while(1)
  {		
		
		DisplayPra(keySta.Dismode);      // OLED Display
		switch(workmode)
		{
			case Mode_INIT:
				TimerCount_1ms = 0;
				CycleSendFlg = 0;
				workmode = Mode_CAN_CYCLE;
			break;
			
			case Mode_CAN_CYCLE:
				
			if((TimerCount_1ms == 1)&&(CycleSendFlg == 0))          // Every 100ms send can frame
			{
				TimerCount_1ms = 0;
				can_transmit_data();           // Transmit can date
			}							
			break;

		}

		
		if (gSysParm.canRxCmd == CAN_RXD_DATA)  //if Received Read command
		{
			gSysParm.canRxCmd = CAN_RXD_NULL;
			if (can_read_para() == 0)             //check sum is correct
			{
				if (gSysParm.basePara.RWCmd == BASE_WRITE) 
				{                           
          
				}
				gSysParm.canTxCmd = CAN_TXD_DATA;   //Send data flag
			 }
		}
		else if(gSysParm.canRxCmd == CAN_RXD_BOOT)//Bootloader command
		{
			gSysParm.canRxCmd = CAN_RXD_NULL;
			if(gSysParm.canRxBuf[0] == 0x42 && gSysParm.canRxBuf[1] == 0x44 && gSysParm.canRxBuf[2] == 0x43)//01 stop cycle send
			{
				if(gSysParm.canRxBuf[3] == 0x01)//stop send
				{				
					can_transmit_cycledata(0x01); //stop, successful				
					delay_ms(5);
					CycleSendFlg = 1;  //send response frame failed, need to debug;
				}
				else if(gSysParm.canRxBuf[3] == 0x00)//cycle send
				{					
					can_transmit_cycledata(0x01); //cycle, successful
					delay_ms(5);
					CycleSendFlg = 0;
				}
			}
			else if(gSysParm.canRxBuf[0] == 0x53 && gSysParm.canRxBuf[1] == 0x4C && gSysParm.canRxBuf[2] == 0x41 && gSysParm.canRxBuf[3] == 0x48)//Pre-compile
			{

				if((gSysParm.canRxBuf[4] == gSysParm.basePara.CanDataId>>8)&&((gSysParm.canRxBuf[5] == gSysParm.basePara.CanDataId>>4)&0x0f)&&((gSysParm.canRxBuf[6] == gSysParm.basePara.CanDataId)&0x0f))
				{
					can_transmit_precompile();
				}
			}
			else if(gSysParm.canRxBuf[0] == 0x52 && gSysParm.canRxBuf[1] == 0x45 && gSysParm.canRxBuf[2] == 0x53 && gSysParm.canRxBuf[3] == 0x45 && gSysParm.canRxBuf[4] == 0x54)//Reset
			{
				delay_ms(5);
//				NVIC_SystemReset();
				app_jump_to_bootloader();
			}
		}
		
		if (gSysParm.canTxCmd == CAN_TXD_DATA)  //send can parameter 
		{   
			gSysParm.canTxCmd = CAN_TXD_NULL;   
		}
  }
}

