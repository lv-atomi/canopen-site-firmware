/*******************************************************************************
    CANopen Object Dictionary definition for CANopenNode V4

    This file was automatically generated by CANopenEditor v4.1-2-gff637a7

    https://github.com/CANopenNode/CANopenNode
    https://github.com/CANopenNode/CANopenEditor

    DON'T EDIT THIS FILE MANUALLY !!!!
********************************************************************************

    File info:
        File Names:   OD.h; OD.c
        Project File: DS301_profile2.eds
        File Version: 1

        Created:      2020/11/28 13:37:00
        Created By:   Janez Paternoster
        Modified:     2021/6/28 12:31:00
        Modified By:  Janez Paternoster

    Device Info:
        Vendor Name:  lv
        Vendor ID:    1234
        Product Name: Demo device
        Product ID:   

        Description:  Demo CANopen device with example usage.
*******************************************************************************/

#ifndef OD_H
#define OD_H
/*******************************************************************************
    Counters of OD objects
*******************************************************************************/


/*******************************************************************************
    Sizes of OD arrays
*******************************************************************************/
#define OD_CNT_ARR_1003 16
#define OD_CNT_ARR_1010 6
#define OD_CNT_ARR_1011 6
#define OD_CNT_ARR_1016 8
#define OD_CNT_ARR_2110 16
#define OD_CNT_ARR_2111 16
#define OD_CNT_ARR_2112 16
#define OD_CNT_ARR_6000 8
#define OD_CNT_ARR_6200 8
#define OD_CNT_ARR_6401 16
#define OD_CNT_ARR_6411 8


/*******************************************************************************
    OD data declaration of all groups
*******************************************************************************/
typedef struct {
    uint32_t x1000_deviceType;
    uint32_t x1005_COB_ID_SYNCMessage;
    uint32_t x1006_communicationCyclePeriod;
    uint32_t x1007_synchronousWindowLength;
    uint32_t x1012_COB_IDTimeStampObject;
    uint32_t x1014_COB_ID_EMCY;
    uint16_t x1015_inhibitTimeEMCY;
    uint8_t x1016_consumerHeartbeatTime_sub0;
    uint32_t x1016_consumerHeartbeatTime[OD_CNT_ARR_1016];
    uint16_t x1017_producerHeartbeatTime;
    struct {
        uint8_t highestSub_indexSupported;
        uint32_t vendor_ID;
        uint32_t productCode;
        uint32_t revisionNumber;
        uint32_t serialNumber;
    } x1018_identity;
    uint8_t x1019_synchronousCounterOverflowValue;
    struct {
        uint8_t highestSub_indexSupported;
        uint32_t COB_IDClientToServerTx;
        uint32_t COB_IDServerToClientRx;
        uint8_t node_IDOfTheSDOServer;
    } x1280_SDOClientParameter;
    struct {
        uint8_t highestSub_indexSupported;
        uint32_t COB_IDUsedByRPDO;
        uint8_t transmissionType;
        uint16_t eventTimer;
    } x1400_RPDOCommunicationParameter;
    struct {
        uint8_t highestSub_indexSupported;
        uint32_t COB_IDUsedByRPDO;
        uint8_t transmissionType;
        uint16_t eventTimer;
    } x1401_RPDOCommunicationParameter;
    struct {
        uint8_t highestSub_indexSupported;
        uint32_t COB_IDUsedByRPDO;
        uint8_t transmissionType;
        uint16_t eventTimer;
    } x1402_RPDOCommunicationParameter;
    struct {
        uint8_t highestSub_indexSupported;
        uint32_t COB_IDUsedByRPDO;
        uint8_t transmissionType;
        uint16_t eventTimer;
    } x1403_RPDOCommunicationParameter;
    struct {
        uint8_t numberOfMappedApplicationObjectsInPDO;
        uint32_t applicationObject1;
        uint32_t applicationObject2;
        uint32_t applicationObject3;
        uint32_t applicationObject4;
        uint32_t applicationObject5;
        uint32_t applicationObject6;
        uint32_t applicationObject7;
        uint32_t applicationObject8;
    } x1600_RPDOMappingParameter;
    struct {
        uint8_t numberOfMappedApplicationObjectsInPDO;
        uint32_t applicationObject1;
        uint32_t applicationObject2;
        uint32_t applicationObject3;
        uint32_t applicationObject4;
        uint32_t applicationObject5;
        uint32_t applicationObject6;
        uint32_t applicationObject7;
        uint32_t applicationObject8;
    } x1601_RPDOMappingParameter;
    struct {
        uint8_t numberOfMappedApplicationObjectsInPDO;
        uint32_t applicationObject1;
        uint32_t applicationObject2;
        uint32_t applicationObject3;
        uint32_t applicationObject4;
        uint32_t applicationObject5;
        uint32_t applicationObject6;
        uint32_t applicationObject7;
        uint32_t applicationObject8;
    } x1602_RPDOMappingParameter;
    struct {
        uint8_t numberOfMappedApplicationObjectsInPDO;
        uint32_t applicationObject1;
        uint32_t applicationObject2;
        uint32_t applicationObject3;
        uint32_t applicationObject4;
        uint32_t applicationObject5;
        uint32_t applicationObject6;
        uint32_t applicationObject7;
        uint32_t applicationObject8;
    } x1603_RPDOMappingParameter;
    struct {
        uint8_t highestSub_indexSupported;
        uint32_t COB_IDUsedByTPDO;
        uint8_t transmissionType;
        uint16_t inhibitTime;
        uint16_t eventTimer;
        uint8_t SYNCStartValue;
    } x1800_TPDOCommunicationParameter;
    struct {
        uint8_t highestSub_indexSupported;
        uint32_t COB_IDUsedByTPDO;
        uint8_t transmissionType;
        uint16_t inhibitTime;
        uint16_t eventTimer;
        uint8_t SYNCStartValue;
    } x1801_TPDOCommunicationParameter;
    struct {
        uint8_t highestSub_indexSupported;
        uint32_t COB_IDUsedByTPDO;
        uint8_t transmissionType;
        uint16_t inhibitTime;
        uint16_t eventTimer;
        uint8_t SYNCStartValue;
    } x1802_TPDOCommunicationParameter;
    struct {
        uint8_t highestSub_indexSupported;
        uint32_t COB_IDUsedByTPDO;
        uint8_t transmissionType;
        uint16_t inhibitTime;
        uint16_t eventTimer;
        uint8_t SYNCStartValue;
    } x1803_TPDOCommunicationParameter;
    struct {
        uint8_t numberOfMappedApplicationObjectsInPDO;
        uint32_t applicationObject1;
        uint32_t applicationObject2;
        uint32_t applicationObject3;
        uint32_t applicationObject4;
        uint32_t applicationObject5;
        uint32_t applicationObject6;
        uint32_t applicationObject7;
        uint32_t applicationObject8;
    } x1A00_TPDOMappingParameter;
    struct {
        uint8_t numberOfMappedApplicationObjectsInPDO;
        uint32_t applicationObject1;
        uint32_t applicationObject2;
        uint32_t applicationObject3;
        uint32_t applicationObject4;
        uint32_t applicationObject5;
        uint32_t applicationObject6;
        uint32_t applicationObject7;
        uint32_t applicationObject8;
    } x1A01_TPDOMappingParameter;
    struct {
        uint8_t numberOfMappedApplicationObjectsInPDO;
        uint32_t applicationObject1;
        uint32_t applicationObject2;
        uint32_t applicationObject3;
        uint32_t applicationObject4;
        uint32_t applicationObject5;
        uint32_t applicationObject6;
        uint32_t applicationObject7;
        uint32_t applicationObject8;
    } x1A02_TPDOMappingParameter;
    struct {
        uint8_t numberOfMappedApplicationObjectsInPDO;
        uint32_t applicationObject1;
        uint32_t applicationObject2;
        uint32_t applicationObject3;
        uint32_t applicationObject4;
        uint32_t applicationObject5;
        uint32_t applicationObject6;
        uint32_t applicationObject7;
        uint32_t applicationObject8;
    } x1A03_TPDOMappingParameter;
} OD_PERSIST_COMM_t;

typedef struct {
    uint8_t x1001_errorRegister;
    uint8_t x1010_storeParameters_sub0;
    uint32_t x1010_storeParameters[OD_CNT_ARR_1010];
    uint8_t x1011_restoreDefaultParameters_sub0;
    uint32_t x1011_restoreDefaultParameters[OD_CNT_ARR_1011];
    struct {
        uint8_t highestSub_indexSupported;
        uint32_t COB_IDClientToServerRx;
        uint32_t COB_IDServerToClientTx;
    } x1200_SDOServerParameter;
    uint8_t x2100_errorStatusBits[1];
    uint8_t x2110_variableInt32_sub0;
    int32_t x2110_variableInt32[OD_CNT_ARR_2110];
    uint8_t x6000_readDigitalInput8_bit_sub0;
    uint8_t x6000_readDigitalInput8_bit[OD_CNT_ARR_6000];
    uint8_t x6200_writeDigitalOutput8_bit_sub0;
    uint8_t x6200_writeDigitalOutput8_bit[OD_CNT_ARR_6200];
    uint8_t x6401_readAnalogInput16_bit_sub0;
    int16_t x6401_readAnalogInput16_bit[OD_CNT_ARR_6401];
    uint8_t x6411_writeAnalogOutput16_bit_sub0;
    int16_t x6411_writeAnalogOutput16_bit[OD_CNT_ARR_6411];
} OD_RAM_t;

typedef struct {
    uint32_t x2106_power_onCounter;
    uint8_t x2112_variableNV_Int32AutoSave_sub0;
    int32_t x2112_variableNV_Int32AutoSave[OD_CNT_ARR_2112];
} OD_PERSIST_APP_AUTO_t;

typedef struct {
    uint8_t x2111_variableInt32Save_sub0;
    int32_t x2111_variableInt32Save[OD_CNT_ARR_2111];
    struct {
        uint8_t highestSub_indexSupported;
        int64_t I64;
        uint64_t U64;
        float32_t R32;
        float64_t R64;
        uint16_t parameterWithDefaultValue;
    } x2120_demoRecord;
    struct {
        uint8_t highestSub_indexSupported;
        char stringShort[4];
        char stringLong[111];
        uint8_t octetString[3];
    } x2121_demoStrings;
} OD_PERSIST_APP_t;

typedef struct {
    uint32_t x6412_lvtest;
} OD_test_t;

#ifndef OD_ATTR_PERSIST_COMM
#define OD_ATTR_PERSIST_COMM
#endif
extern OD_ATTR_PERSIST_COMM OD_PERSIST_COMM_t OD_PERSIST_COMM;

#ifndef OD_ATTR_RAM
#define OD_ATTR_RAM
#endif
extern OD_ATTR_RAM OD_RAM_t OD_RAM;

#ifndef OD_ATTR_PERSIST_APP_AUTO
#define OD_ATTR_PERSIST_APP_AUTO
#endif
extern OD_ATTR_PERSIST_APP_AUTO OD_PERSIST_APP_AUTO_t OD_PERSIST_APP_AUTO;

#ifndef OD_ATTR_PERSIST_APP
#define OD_ATTR_PERSIST_APP
#endif
extern OD_ATTR_PERSIST_APP OD_PERSIST_APP_t OD_PERSIST_APP;

#ifndef OD_ATTR_test
#define OD_ATTR_test
#endif
extern OD_ATTR_test OD_test_t OD_test;

#ifndef OD_ATTR_OD
#define OD_ATTR_OD
#endif
extern OD_ATTR_OD OD_t *OD;


/*******************************************************************************
    Object dictionary entries - shortcuts
*******************************************************************************/
#define OD_ENTRY_H1000 &OD->list[0]
#define OD_ENTRY_H1001 &OD->list[1]
#define OD_ENTRY_H1003 &OD->list[2]
#define OD_ENTRY_H1005 &OD->list[3]
#define OD_ENTRY_H1006 &OD->list[4]
#define OD_ENTRY_H1007 &OD->list[5]
#define OD_ENTRY_H1008 &OD->list[6]
#define OD_ENTRY_H1009 &OD->list[7]
#define OD_ENTRY_H100A &OD->list[8]
#define OD_ENTRY_H1010 &OD->list[9]
#define OD_ENTRY_H1011 &OD->list[10]
#define OD_ENTRY_H1012 &OD->list[11]
#define OD_ENTRY_H1014 &OD->list[12]
#define OD_ENTRY_H1015 &OD->list[13]
#define OD_ENTRY_H1016 &OD->list[14]
#define OD_ENTRY_H1017 &OD->list[15]
#define OD_ENTRY_H1018 &OD->list[16]
#define OD_ENTRY_H1019 &OD->list[17]
#define OD_ENTRY_H1200 &OD->list[18]
#define OD_ENTRY_H1280 &OD->list[19]
#define OD_ENTRY_H1400 &OD->list[20]
#define OD_ENTRY_H1401 &OD->list[21]
#define OD_ENTRY_H1402 &OD->list[22]
#define OD_ENTRY_H1403 &OD->list[23]
#define OD_ENTRY_H1600 &OD->list[24]
#define OD_ENTRY_H1601 &OD->list[25]
#define OD_ENTRY_H1602 &OD->list[26]
#define OD_ENTRY_H1603 &OD->list[27]
#define OD_ENTRY_H1800 &OD->list[28]
#define OD_ENTRY_H1801 &OD->list[29]
#define OD_ENTRY_H1802 &OD->list[30]
#define OD_ENTRY_H1803 &OD->list[31]
#define OD_ENTRY_H1A00 &OD->list[32]
#define OD_ENTRY_H1A01 &OD->list[33]
#define OD_ENTRY_H1A02 &OD->list[34]
#define OD_ENTRY_H1A03 &OD->list[35]
#define OD_ENTRY_H2100 &OD->list[36]
#define OD_ENTRY_H2106 &OD->list[37]
#define OD_ENTRY_H2110 &OD->list[38]
#define OD_ENTRY_H2111 &OD->list[39]
#define OD_ENTRY_H2112 &OD->list[40]
#define OD_ENTRY_H2120 &OD->list[41]
#define OD_ENTRY_H2121 &OD->list[42]
#define OD_ENTRY_H2122 &OD->list[43]
#define OD_ENTRY_H6000 &OD->list[44]
#define OD_ENTRY_H6200 &OD->list[45]
#define OD_ENTRY_H6401 &OD->list[46]
#define OD_ENTRY_H6411 &OD->list[47]
#define OD_ENTRY_H6412 &OD->list[48]


/*******************************************************************************
    Object dictionary entries - shortcuts with names
*******************************************************************************/
#define OD_ENTRY_H1000_deviceType &OD->list[0]
#define OD_ENTRY_H1001_errorRegister &OD->list[1]
#define OD_ENTRY_H1003_pre_definedErrorField &OD->list[2]
#define OD_ENTRY_H1005_COB_ID_SYNCMessage &OD->list[3]
#define OD_ENTRY_H1006_communicationCyclePeriod &OD->list[4]
#define OD_ENTRY_H1007_synchronousWindowLength &OD->list[5]
#define OD_ENTRY_H1008_manufacturerDeviceName &OD->list[6]
#define OD_ENTRY_H1009_manufacturerHardwareVersion &OD->list[7]
#define OD_ENTRY_H100A_manufacturerSoftwareVersion &OD->list[8]
#define OD_ENTRY_H1010_storeParameters &OD->list[9]
#define OD_ENTRY_H1011_restoreDefaultParameters &OD->list[10]
#define OD_ENTRY_H1012_COB_IDTimeStampObject &OD->list[11]
#define OD_ENTRY_H1014_COB_ID_EMCY &OD->list[12]
#define OD_ENTRY_H1015_inhibitTimeEMCY &OD->list[13]
#define OD_ENTRY_H1016_consumerHeartbeatTime &OD->list[14]
#define OD_ENTRY_H1017_producerHeartbeatTime &OD->list[15]
#define OD_ENTRY_H1018_identity &OD->list[16]
#define OD_ENTRY_H1019_synchronousCounterOverflowValue &OD->list[17]
#define OD_ENTRY_H1200_SDOServerParameter &OD->list[18]
#define OD_ENTRY_H1280_SDOClientParameter &OD->list[19]
#define OD_ENTRY_H1400_RPDOCommunicationParameter &OD->list[20]
#define OD_ENTRY_H1401_RPDOCommunicationParameter &OD->list[21]
#define OD_ENTRY_H1402_RPDOCommunicationParameter &OD->list[22]
#define OD_ENTRY_H1403_RPDOCommunicationParameter &OD->list[23]
#define OD_ENTRY_H1600_RPDOMappingParameter &OD->list[24]
#define OD_ENTRY_H1601_RPDOMappingParameter &OD->list[25]
#define OD_ENTRY_H1602_RPDOMappingParameter &OD->list[26]
#define OD_ENTRY_H1603_RPDOMappingParameter &OD->list[27]
#define OD_ENTRY_H1800_TPDOCommunicationParameter &OD->list[28]
#define OD_ENTRY_H1801_TPDOCommunicationParameter &OD->list[29]
#define OD_ENTRY_H1802_TPDOCommunicationParameter &OD->list[30]
#define OD_ENTRY_H1803_TPDOCommunicationParameter &OD->list[31]
#define OD_ENTRY_H1A00_TPDOMappingParameter &OD->list[32]
#define OD_ENTRY_H1A01_TPDOMappingParameter &OD->list[33]
#define OD_ENTRY_H1A02_TPDOMappingParameter &OD->list[34]
#define OD_ENTRY_H1A03_TPDOMappingParameter &OD->list[35]
#define OD_ENTRY_H2100_errorStatusBits &OD->list[36]
#define OD_ENTRY_H2106_power_onCounter &OD->list[37]
#define OD_ENTRY_H2110_variableInt32 &OD->list[38]
#define OD_ENTRY_H2111_variableInt32Save &OD->list[39]
#define OD_ENTRY_H2112_variableNV_Int32AutoSave &OD->list[40]
#define OD_ENTRY_H2120_demoRecord &OD->list[41]
#define OD_ENTRY_H2121_demoStrings &OD->list[42]
#define OD_ENTRY_H2122_demoDomain &OD->list[43]
#define OD_ENTRY_H6000_readDigitalInput8_bit &OD->list[44]
#define OD_ENTRY_H6200_writeDigitalOutput8_bit &OD->list[45]
#define OD_ENTRY_H6401_readAnalogInput16_bit &OD->list[46]
#define OD_ENTRY_H6411_writeAnalogOutput16_bit &OD->list[47]
#define OD_ENTRY_H6412_lvtest &OD->list[48]


/*******************************************************************************
    OD config structure
*******************************************************************************/
#ifdef CO_MULTIPLE_OD
#define OD_INIT_CONFIG(config) {\
    (config).CNT_NMT = 0;\
    (config).ENTRY_H1017 = OD_ENTRY_H1017;\
    (config).CNT_HB_CONS = 0;\
    (config).CNT_ARR_1016 = OD_CNT_ARR_1016;\
    (config).ENTRY_H1016 = OD_ENTRY_H1016;\
    (config).CNT_EM = 0;\
    (config).ENTRY_H1001 = OD_ENTRY_H1001;\
    (config).ENTRY_H1014 = OD_ENTRY_H1014;\
    (config).ENTRY_H1015 = OD_ENTRY_H1015;\
    (config).CNT_ARR_1003 = OD_CNT_ARR_1003;\
    (config).ENTRY_H1003 = OD_ENTRY_H1003;\
    (config).CNT_SDO_SRV = 0;\
    (config).ENTRY_H1200 = OD_ENTRY_H1200;\
    (config).CNT_SDO_CLI = 0;\
    (config).ENTRY_H1280 = OD_ENTRY_H1280;\
    (config).CNT_TIME = 0;\
    (config).ENTRY_H1012 = OD_ENTRY_H1012;\
    (config).CNT_SYNC = 0;\
    (config).ENTRY_H1005 = OD_ENTRY_H1005;\
    (config).ENTRY_H1006 = OD_ENTRY_H1006;\
    (config).ENTRY_H1007 = OD_ENTRY_H1007;\
    (config).ENTRY_H1019 = OD_ENTRY_H1019;\
    (config).CNT_RPDO = 0;\
    (config).ENTRY_H1400 = OD_ENTRY_H1400;\
    (config).ENTRY_H1600 = OD_ENTRY_H1600;\
    (config).CNT_TPDO = 0;\
    (config).ENTRY_H1800 = OD_ENTRY_H1800;\
    (config).ENTRY_H1A00 = OD_ENTRY_H1A00;\
    (config).CNT_LEDS = 0;\
    (config).CNT_GFC = 0;\
    (config).ENTRY_H1300 = NULL;\
    (config).CNT_SRDO = 0;\
    (config).ENTRY_H1301 = NULL;\
    (config).ENTRY_H1381 = NULL;\
    (config).ENTRY_H13FE = NULL;\
    (config).ENTRY_H13FF = NULL;\
    (config).CNT_LSS_SLV = 0;\
    (config).CNT_LSS_MST = 0;\
    (config).CNT_GTWA = 0;\
    (config).CNT_TRACE = 0;\
}
#endif

#endif /* OD_H */
