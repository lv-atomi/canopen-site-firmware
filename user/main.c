#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "can.h"
#include "timer1.h"
#include "gpio.h"
#include "oled.h"
#include "bmp.h"
#include "flash.h"

extern SysParm_TypeDef gSysParm;
extern KeyPressMode keySta;

error_status err_status;
uint16_t buffer_write[TEST_BUFEER_SIZE];//[0]:ID; [1]:Voltage
uint16_t buffer_read[TEST_BUFEER_SIZE]; //[0]:ID; [1]:Voltage

uint16_t VolShowTest = DEFAULTVOL;
int main(void)
{	

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
		if(TimerCount_1ms == 1)          // Every 100ms send can frame
		{
			TimerCount_1ms = 0;
			can_transmit_data();           // Transmit can date
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
		
		if (gSysParm.canTxCmd == CAN_TXD_DATA)  //send can parameter 
		{   
			gSysParm.canTxCmd = CAN_TXD_NULL;   
		}
  }
}

