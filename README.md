# lv-default

Hardware Schematic Detail:
1.CAN_TX --- PB9
  CAN_RX --- PB8
  
2.Key Input
  Key select --- PC15
  Key enter  --- PC14

Version V01:
1. CAN Bus information: 500kbps, standard frame;
2. CAN Bus is worked, CAN Receive is realized by Interrupt, and CAN Transmit is realized by loop, transmit cycle is 100ms;
3. Press key to set CANID is worked
	--Press select key, "_" is visuable follow CANID, press key again "_" moves to Vo, press enter can adjust CANID and Vo;
	--if key is on aciton more than 5 seconds, adjust mode will be closed automatic;
	
Path 
1.Hardware schematic path:SCH_BaseBoardTest_2023-04-27.pdf

2. user software .h and .c--user

This device is divided into 4 sectors, the address please the bellowing information:
1. Flash as EEPROM address: 0X804B000  //0X80FF000
2.  boot rom start address: 0x08000000 - 0x08018FFF,size is 100k
3.  app  rom start address: 0x08019000 - 0x08031FFF,size is 100k
4.  back rom start address: 0x08032000 - 0x0803AFFF,size is 100k

