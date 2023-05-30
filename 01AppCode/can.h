#ifndef __CAN_H
#define __CAN_H

#include "at32f403a_407.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define BAUD_1M
#define BAUD_500K

#define DEFAULTVOL 120

#define SLAVEBOARD_ID    0x001
#define CONFIGURATION_ID 0x312
#define BOOTLOADER_ID    0x700

typedef struct 
{        
	uint8_t     RWCmd;
	uint16_t    CanDataId;
}BpsPara_TypeDef;

typedef struct CanFrame
{
	uint8_t canTxBuf[8];
	uint8_t canRxBuf[8];
	uint8_t canBaudrate;
	uint8_t canTxCmd;
	uint8_t canRxCmd;
	BpsPara_TypeDef basePara;
}SysParm_TypeDef;

enum{
    CAN_TXD_NULL,
    CAN_TXD_DATA,          
};

enum{
    CAN_RXD_NULL,
    CAN_RXD_DATA,    
		CAN_RXD_BOOT,
};

enum{
    BASE_READ = 0,               
    BASE_WRITE                 
};


typedef struct{
    uint16_t    SysEpromCANID;             //CAN ID
    uint16_t    SysEpromVoltage;             //Volage
}SysEprom_TypeDef;

void CAN_DataInit(void);
void can_gpio_config(void);
void can_configuration(void);
void can_transmit_data(void);
uint8_t can_read_para(void);
void can_transmit_cycledata(uint8_t cyc);
void can_transmit_precompile(void);

#endif
