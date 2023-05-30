#include "boot.h"

#define BOOTLOADER_ADDRESS  0x08000000//0x8019000//0x08000000
#define BPR_JUMP_FLAG       0x5AA5
#define SET_MSP(dwValue) __ASM volatile ("msr msp, "#dwValue)
void (*pfTarget)(void); 

/**
  * @brief clear all system status
        1. close all peripherals clock
        2. close pll
        3. disable all interrupt
        4. clear all pending interrupt flag      
  * @param  none
  * @retval none
  */
void app_clear_sys_status()
{
	/*close peripherals clock*/
	CRM->apb2rst = 0xFFFF;
	CRM->apb2rst = 0;
	CRM->apb1rst = 0xFFFF;
	CRM->apb1rst = 0;
	CRM->apb1en = 0;
	CRM->apb2en = 0;
	/*close pll*/
	/* reset sclksel, ahbdiv, apb1div, apb2div, adcdiv and clkout_sel bits */
	CRM->cfg_bit.sclksel = 0;
	CRM->cfg_bit.ahbdiv = 0;
	CRM->cfg_bit.apb1div = 0;
	CRM->cfg_bit.apb2div = 0;
	CRM->cfg_bit.adcdiv_l = 0;
	CRM->cfg_bit.adcdiv_h = 0;
	CRM->cfg_bit.clkout_sel = 0;
	CRM->ctrl_bit.hexten = 0;
	CRM->ctrl_bit.cfden = 0;
	CRM->ctrl_bit.pllen = 0;
	CRM->cfg_bit.pllrcs = 0;
	CRM->cfg_bit.pllhextdiv = 0;
	CRM->cfg_bit.pllmult_l = 0;
	CRM->cfg_bit.pllmult_h = 0;
	CRM->cfg_bit.usbdiv_l = 0;
	CRM->cfg_bit.usbdiv_h = 0;
	CRM->cfg_bit.pllrange = 0;
	/* disable all interrupts and clear pending bits  */
	CRM->clkint_bit.lickstblfc = 0;
	CRM->clkint_bit.lextstblfc = 0;
	CRM->clkint_bit.hickstblfc = 0;
	CRM->clkint_bit.hextstblfc = 0;
	CRM->clkint_bit.pllstblfc = 0;
	CRM->clkint_bit.cfdfc = 0;
	/*colse systick*/
	SysTick->CTRL = 0;
	
	/*disable all interrupt && pending interrupt flag*/
	/*
	user add code...
	*/
}

void app_jump_address_set(void)
{
	uint32_t dwStkPtr, dwJumpAddr;
	dwStkPtr = *(uint32_t *)BOOTLOADER_ADDRESS;
	dwJumpAddr = *(uint32_t *)(BOOTLOADER_ADDRESS + sizeof(uint32_t));

	/*It is necessary to ensure that the clocks of all peripherals
	are turned off, clear all interrupts and clear all interrupt
	pending flags before jump*/
	SET_MSP(dwStkPtr);
	pfTarget = (void (*)(void))dwJumpAddr;
	pfTarget();
}

void app_jump_to_bootloader()
{
	app_clear_sys_status();
	app_jump_address_set();
}
