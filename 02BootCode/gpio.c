#include "gpio.h"
#include "oled.h"
#include "can.h"
#include "flash.h"

extern uint16_t testkey1;
extern uint16_t testkey2;

extern uint16_t VolShowTest;
extern SysParm_TypeDef gSysParm;
extern uint16_t buffer_write[TEST_BUFEER_SIZE];
extern uint16_t buffer_read[TEST_BUFEER_SIZE];
KeyPressMode keySta;

void GPIO_Init()
{
	gpio_init_type gpio_init_struct;
  /* enable the button clock */
  crm_periph_clock_enable(KEY_CRM_CLK, TRUE);
	
//	crm_periph_clock_enable(CRM_BPR_PERIPH_CLOCK, TRUE);
//	crm_clock_source_enable(CRM_CLOCK_SOURCE_LICK, FALSE);

  /* set default parameter */
  gpio_default_para_init(&gpio_init_struct);

  /* configure OLED_EN pin as output with pull-up/pull-down */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = KEY_01_SELECT_PIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(KEY_PORT, &gpio_init_struct);
	
  /* configure OLED_EN pin as output with pull-up/pull-down */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = KEY_02_ENTER_PIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(KEY_PORT, &gpio_init_struct);	
}

uint8_t Key_Cnt = 0;

void Key_ScanFun()
{	
	uint16_t i,Cnt = 0;
	if(gpio_input_data_bit_read(KEY_PORT,KEY_01_SELECT_PIN) == 0)         //Select Key Press
	{
		for(i = 0;i < 1000;i ++)
		{
			if(gpio_input_data_bit_read(KEY_PORT,KEY_01_SELECT_PIN) == 0) Cnt++;
		}
		if(Cnt > 900)
		{
			testkey2++;//test
			keySta.Dismode ++; //0:normal;1:Select,2:Enter
			if(keySta.Dismode > 2)keySta.Dismode = 0;
			keySta.KeyCnt10s = KEEPSTATUSTIME;
		}
	}
	else if(gpio_input_data_bit_read(KEY_PORT,KEY_02_ENTER_PIN) == 0)     //Enter Key Press
	{
		for(i = 0;i < 1000;i ++)
		{
			if(gpio_input_data_bit_read(KEY_PORT,KEY_02_ENTER_PIN) == 0) Cnt++;
		}
		if(Cnt > 900)
		{
			testkey1++;//test
			keySta.KeyCnt10s = KEEPSTATUSTIME;
			if(keySta.Dismode == 1)      //Adjust CANID
			{
				gSysParm.basePara.CanDataId++;
				if(gSysParm.basePara.CanDataId >=0x7ff) gSysParm.basePara.CanDataId = BOOTLOADER_SLAVEID;//SLAVEBOARD_ID;
				buffer_write[0] = gSysParm.basePara.CanDataId; //write ID to flash
			}
			else if(keySta.Dismode == 2) //Adjust Voltage Output
			{
				VolShowTest++;
				if(VolShowTest > 350) VolShowTest = DEFAULTVOL;
				buffer_write[1] = VolShowTest; //write Voltage to flash
			}
		}		
	}
}
//Type,0:Normal; 1:CANID Set;2VoSet
void DisplayPra(uint8_t Type)
{
	if(Type == 0)
	{
		//Display CANID, 0-row
		OLED_ShowString(0,0,"ID:",16,1);  
		OLED_ShowNum(24,0,gSysParm.basePara.CanDataId,4,16,1);	
		OLED_ShowString(56,0," ",16,1);		
		//Display Voltage, 16-row
		OLED_ShowString(0,16,"Vo:",16,1);
		OLED_ShowNum(24,16,VolShowTest,3,16,1);
		OLED_ShowString(48,16," ",16,1);	
	}
	else if(Type == 1)//select
	{
		OLED_ShowString(0,0,"ID:",16,1);  
		OLED_ShowNum(24,0,gSysParm.basePara.CanDataId,4,16,1);	
		OLED_ShowString(56,0,"_",16,1);
		OLED_ShowString(56,16," ",16,1);
	}
	else if(Type == 2)//Enter
	{
		OLED_ShowString(0,16,"Vo:",16,1);
		OLED_ShowNum(24,16,VolShowTest,3,16,1);
		OLED_ShowString(48,16,"_",16,1);	
		OLED_ShowString(56,0," ",16,1);				
	}
		
	OLED_Refresh();
}


