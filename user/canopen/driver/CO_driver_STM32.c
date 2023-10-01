/*
 * CAN module object for STM32(and xxx32 clones) (FD)CAN peripheral IP.
 *
 * This file is a template for other microcontrollers.
 *
 * @file        CO_driver.c
 * @ingroup     CO_driver
 * @author	Lv, Qi			2023
 * 		Hamed Jafarzadeh 	2022
 * 		Tilen Marjerle		2021
 * 		Janez Paternoster	2020
 * @copyright   2004 - 2020 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <https://github.com/CANopenNode/CANopenNode>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Implementation Author:               Tilen Majerle <tilen@majerle.eu>
 */

/*
Organization

6000 PSU current read
6001 PSU voltage read
6002 PSU current set
6003 PSU voltage set
6004 PSU module temperature

6100 GPIO input x8
6101 HBridge Motor

6200 Camera Module0
6201 Camera Module1

6300 Fan0
6301 Fan1
6302 Fan2
6303 Thermal DS18B20 x4
6304 Aircondition load
6305 Aircondition status

6400 Trigger Input x2
6401 Trigger Output x2
6402 Capacitor displacement
6403 Thermocouple/PT100 x2
6404 Motor(HBridge / brushless)

6500 Stackable Module0
6501 Stackable Module1
6502 Stackable Module2
6503 Stackable Module3
6504 Stackable Module4
6505 Stackable Module5
6506 Stackable Module6
6507 Stackable Module7

6600 Weight sensor x4

6700 Line Current
6701 Line voltage
6702 IGBT Temp
6703 Plate Temp
6704 System Load
6705 Fan0

 */


#include "301/CO_driver.h"
#include "CO_app_STM32.h"
#include "can.h"
#include "log.h"

extern char debug[];

/* Local CAN module object */
static CO_CANmodule_t* CANModule_local = NULL; /* Local instance of global CAN module */

/* CAN masks for identifiers */
#define CANID_MASK 0x07FF /*!< CAN standard ID mask */
#define FLAG_RTR   0x8000 /*!< RTR flag, part of identifier */

/******************************************************************************/
void CO_CANsetConfigurationMode(void *CANptr) {
  /* Put CAN module in configuration mode */
  if (CANptr != NULL) {
    can_reset(((CANopenNodeSTM32 *)CANptr)->CANHandle);
  }
}

/******************************************************************************/
void CO_CANsetNormalMode(CO_CANmodule_t *CANmodule) {
  /* Put CAN module in normal mode */
  if (CANmodule->CANptr != NULL) {
    /* can interrupt config */
    if (((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle == CAN1) {
      can_reset(CAN1);
      can_configuration();
    } else if (((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle == CAN2) {
      /* ignore CAN2 */
    }
    
    CANmodule->CANnormal = true;
  }
}


/******************************************************************************/
CO_ReturnError_t CO_CANmodule_init(CO_CANmodule_t *CANmodule, void *CANptr,
                                   CO_CANrx_t rxArray[], uint16_t rxSize,
                                   CO_CANtx_t txArray[], uint16_t txSize,
                                   uint16_t CANbitRate) {

  /* verify arguments */
  if (CANmodule == NULL || rxArray == NULL || txArray == NULL) {
    return CO_ERROR_ILLEGAL_ARGUMENT;
  }

  /* Hold CANModule variable */
  CANmodule->CANptr = CANptr;

  /* Keep a local copy of CANModule */
  CANModule_local = CANmodule;
  /* log_printf("canmod local:%p -> %p, canptr:%p\n", CANModule_local, CANmodule, CANptr); */

  /* Configure object variables */
  CANmodule->rxArray = rxArray;
  CANmodule->rxSize = rxSize;
  CANmodule->txArray = txArray;
  CANmodule->txSize = txSize;
  CANmodule->CANerrorStatus = 0;
  CANmodule->CANnormal = false;
  CANmodule->useCANrxFilters = false; /* Do not use HW filters */
  CANmodule->bufferInhibitFlag = false;
  CANmodule->firstCANtxMessage = true;
  CANmodule->CANtxCount = 0U;
  CANmodule->errOld = 0U;

  /* Reset all variables */
  for (uint16_t i = 0U; i < rxSize; i++) {
    rxArray[i].ident = 0U;
    rxArray[i].mask = 0xFFFFU;
    rxArray[i].object = NULL;
    rxArray[i].CANrx_callback = NULL;
  }
  for (uint16_t i = 0U; i < txSize; i++) {
    txArray[i].bufferFull = false;
  }

  /***************************************/
  /* STM32 related configuration */
  /***************************************/
  ((CANopenNodeSTM32 *)CANptr)->HWInitFunction();

  /*
   * Configure global filter that is used as last check if message did not pass
   * any of other filters:
   *
   * We do not rely on hardware filters in this example
   * and are performing software filters instead
   *
   * Accept non-matching standard ID messages
   * Reject non-matching extended ID messages
   */

  //CAN_FilterTypeDef FilterConfig;
  can_filter_init_type FilterConfig;
#if defined(CAN)
  FilterConfig.FilterBank = 0;
#else
  if (((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle == CAN1) {
    //FilterConfig.FilterBank = 0;
    FilterConfig.filter_number = 0;
  } else {
    //FilterConfig.FilterBank = 14;
    FilterConfig.filter_number = 14;
  }
#endif
  /* STM32 HAL */
  /* FilterConfig.FilterMode = CAN_FILTERMODE_IDMASK; */
  /* FilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; */
  /* FilterConfig.FilterIdHigh = 0x0; */
  /* FilterConfig.FilterIdLow = 0x0; */
  /* FilterConfig.FilterMaskIdHigh = 0x0; */
  /* FilterConfig.FilterMaskIdLow = 0x0; */
  /* FilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0; */

  /* FilterConfig.FilterActivation = ENABLE; */
  /* FilterConfig.SlaveStartFilterBank = 14; */
  
  /* if (HAL_CAN_ConfigFilter(((CANopenNodeSTM32 *)CANptr)->CANHandle, */
  /*                          &FilterConfig) != HAL_OK) { */
  /*   return CO_ERROR_ILLEGAL_ARGUMENT; */
  /* } */

  /* AT32 library */
  FilterConfig.filter_mode = CAN_FILTER_MODE_ID_MASK;
  FilterConfig.filter_bit = CAN_FILTER_32BIT;
  FilterConfig.filter_id_high = 0x0;
  FilterConfig.filter_id_low = 0x0;
  FilterConfig.filter_mask_high = 0x0;
  FilterConfig.filter_mask_low = 0x0;
  //  FilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;     // FIXME:can_rx_fifo_num_type

  FilterConfig.filter_activate_enable = TRUE;
  //FilterConfig.SlaveStartFilterBank = 14;      // FIXME: ignore this?
  FilterConfig.filter_number = 0;
  
  can_filter_init(((CANopenNodeSTM32 *)CANptr)->CANHandle,
		  &FilterConfig);

  /* Enable notifications */
  /* Activate the CAN notification interrupts */
  /* if (HAL_CAN_ActivateNotification(((CANopenNodeSTM32 *)CANptr)->CANHandle, */
  /*                                  CAN_IT_RX_FIFO0_MSG_PENDING | */
  /* 				   CAN_IT_RX_FIFO1_MSG_PENDING | */
  /* 				   CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK) { */
  /*   return CO_ERROR_ILLEGAL_ARGUMENT; */
  /* } */


  return CO_ERROR_NO;
}

/******************************************************************************/
void CO_CANmodule_disable(CO_CANmodule_t *CANmodule) {
  if (CANmodule != NULL && CANmodule->CANptr != NULL) {
    //HAL_CAN_Stop(((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle);
    can_reset(((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle);
  }
}

/******************************************************************************/
CO_ReturnError_t
CO_CANrxBufferInit(CO_CANmodule_t *CANmodule, uint16_t index, uint16_t ident,
                   uint16_t mask, bool_t rtr, void *object,
                   void (*CANrx_callback)(void *object, void *message)) {
  CO_ReturnError_t ret = CO_ERROR_NO;

  if (CANmodule != NULL && object != NULL && CANrx_callback != NULL &&
      index < CANmodule->rxSize) {
    CO_CANrx_t *buffer = &CANmodule->rxArray[index];

    /* Configure object variables */
    buffer->object = object;
    buffer->CANrx_callback = CANrx_callback;

    /*
     * Configure global identifier, including RTR bit
     *
     * This is later used for RX operation match case
     */
    buffer->ident = (ident & CANID_MASK) | (rtr ? FLAG_RTR : 0x00);
    buffer->mask = (mask & CANID_MASK) | FLAG_RTR;

    /* Set CAN hardware module filter and mask. */
    if (CANmodule->useCANrxFilters) {
      __NOP();
    }
  } else {
    ret = CO_ERROR_ILLEGAL_ARGUMENT;
  }

  return ret;
}

/******************************************************************************/
CO_CANtx_t *CO_CANtxBufferInit(CO_CANmodule_t *CANmodule, uint16_t index,
                               uint16_t ident, bool_t rtr, uint8_t noOfBytes,
                               bool_t syncFlag) {
  CO_CANtx_t *buffer = NULL;

  if (CANmodule != NULL && index < CANmodule->txSize) {
    buffer = &CANmodule->txArray[index];

    /* CAN identifier, DLC and rtr, bit aligned with CAN module transmit buffer
     */
    buffer->ident =
        ((uint32_t)ident & CANID_MASK) | ((uint32_t)(rtr ? FLAG_RTR : 0x00));
    buffer->DLC = noOfBytes;
    buffer->bufferFull = false;
    buffer->syncFlag = syncFlag;
  }
  return buffer;
}

/**
 * \brief           Send CAN message to network
 * This function must be called with atomic access.
 *
 * \param[in]       CANmodule: CAN module instance
 * \param[in]       buffer: Pointer to buffer to transmit
 */
static uint8_t prv_send_can_message(CO_CANmodule_t *CANmodule,
                                    CO_CANtx_t *buffer) {

  uint8_t success = 0;

  /* Check if TX FIFO is ready to accept more messages */
  //static CAN_TxHeaderTypeDef tx_hdr;
  static can_tx_message_type tx_hdr;
  /* Check if TX FIFO is ready to accept more messages */
  /* at32 */
  /* log_printf("CAN1 tsts_bit dumps: %ld tm0ef:%d tm1ef:%d tm2ef:%d\n", */
  /* 	     CAN1->tsts, */
  /* 	     CAN1->tsts_bit.tm0ef, */
  /* 	     CAN1->tsts_bit.tm1ef, */
  /* 	     CAN1->tsts_bit.tm2ef */
  /* 	     ); */
  if ((((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle->tsts_bit.tm0ef) ||
      (((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle->tsts_bit.tm1ef) ||
      (((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle->tsts_bit.tm2ef)) {
    /*
     * RTR flag is part of identifier value
     * hence it needs to be properly decoded
     */
    
    /* at32 */
    tx_hdr.standard_id = buffer->ident & CANID_MASK;
    tx_hdr.extended_id = 0u;
    tx_hdr.id_type = CAN_ID_STANDARD;
    tx_hdr.frame_type = (buffer->ident & FLAG_RTR) ? CAN_TFT_REMOTE : CAN_TFT_DATA;
    tx_hdr.dlc = buffer->DLC;
    memcpy(tx_hdr.data, buffer->data, sizeof(tx_hdr.data));

    uint32_t TxMailboxNum; // Transmission MailBox number

    /* Now add message to FIFO. Should not fail */
    TxMailboxNum = can_message_transmit(((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle,
					&tx_hdr);
    success = TxMailboxNum != CAN_TX_STATUS_NO_EMPTY;
    /* log_printf("tx: id:%ld, frame type:%s dlc:%d, mailbox:%ld, success:%d\n", */
    /* 	       tx_hdr.standard_id, */
    /* 	       buffer->ident & FLAG_RTR ? "Remote" : "Data", */
    /* 	       tx_hdr.dlc, */
    /* 	       TxMailboxNum, */
    /* 	       success); */
  }
  return success;
}

/******************************************************************************/
CO_ReturnError_t CO_CANsend(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer) {
  CO_ReturnError_t err = CO_ERROR_NO;

  /* Verify overflow */
  if (buffer->bufferFull) {
    if (!CANmodule->firstCANtxMessage) {
      /* don't set error, if bootup message is still on buffers */
      CANmodule->CANerrorStatus |= CO_CAN_ERRTX_OVERFLOW;
    }
    err = CO_ERROR_TX_OVERFLOW;
    /* log_printf("buffer overflow\n"); */
  }

  /*
   * Send message to CAN network
   *
   * Lock interrupts for atomic operation
   */
  CO_LOCK_CAN_SEND(CANmodule);
  if (prv_send_can_message(CANmodule, buffer)) {
    /* log_printf("send can message...\n"); */
    /* DumpHex(buffer->data, buffer->DLC); */
    CANmodule->bufferInhibitFlag = buffer->syncFlag;
  } else {
    /* log_printf("send can message failed\n"); */
    buffer->bufferFull = true;
    CANmodule->CANtxCount++;
  }
  CO_UNLOCK_CAN_SEND(CANmodule);

  return err;
}

/******************************************************************************/
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule) {
  uint32_t tpdoDeleted = 0U;

  CO_LOCK_CAN_SEND(CANmodule);
  /* Abort message from CAN module, if there is synchronous TPDO.
   * Take special care with this functionality. */
  if (/*messageIsOnCanBuffer && */ CANmodule->bufferInhibitFlag) {
    /* clear TXREQ */
    CANmodule->bufferInhibitFlag = false;
    tpdoDeleted = 1U;
  }
  /* delete also pending synchronous TPDOs in TX buffers */
  if (CANmodule->CANtxCount > 0) {
    for (uint16_t i = CANmodule->txSize; i > 0U; --i) {
      if (CANmodule->txArray[i].bufferFull) {
        if (CANmodule->txArray[i].syncFlag) {
          CANmodule->txArray[i].bufferFull = false;
          CANmodule->CANtxCount--;
          tpdoDeleted = 2U;
        }
      }
    }
  }
  CO_UNLOCK_CAN_SEND(CANmodule);
  if (tpdoDeleted) {
    CANmodule->CANerrorStatus |= CO_CAN_ERRTX_PDO_LATE;
  }
}

/******************************************************************************/
/* Get error counters from the module. If necessary, function may use
 * different way to determine errors. */
//static uint16_t rxErrors = 0, txErrors = 0, overflow = 0;

void CO_CANmodule_process(CO_CANmodule_t *CANmodule) {
  uint32_t err = 0;

  // CANOpen just care about Bus_off, Warning, Passive and Overflow
  // I didn't find overflow error register in STM32, if you find it please let
  // me know

  err = ((((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle->ests_bit.bof) << 1)  |
    ((((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle->ests_bit.epf) << 0) |
    ((((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle->ests_bit.rec) << 10) | /* recv err counter */
    ((((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle->ests_bit.tec) << 18); /* trans err counter */
  /* log_printf("canmod ptr:%p, old err code:%ld, new err code:%ld\n", */
  /* 	     CANmodule, CANmodule->errOld, err); */

  if (CANmodule->errOld != err) {
    uint16_t status = CANmodule->CANerrorStatus;

    CANmodule->errOld = err;
    /* log_printf("error status changed, let's find out what happened..., errOld update:%ld\n", CANmodule->errOld); */

    if (can_flag_get(((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle, CAN_BOF_FLAG)) {
      can_flag_clear(((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle, CAN_BOF_FLAG);
      // In this driver, we assume that auto bus recovery is activated ! so this
      // error will eventually handled automatically.
      /* log_printf("bof cleared, assume auto bus recovery activated\n"); */
    } else {
      /* recalculate CANerrorStatus, first clear some flags */
      status &= 0xFFFF ^ (CO_CAN_ERRTX_BUS_OFF | CO_CAN_ERRRX_WARNING |
                          CO_CAN_ERRRX_PASSIVE | CO_CAN_ERRTX_WARNING |
                          CO_CAN_ERRTX_PASSIVE);

      if (can_receive_error_counter_get(((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle) > 0) { /* FIXME what's the err counter threshold? */
	/* log_printf("rec cleared\n"); */
	((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle->ests_bit.rec = 0; /* clear recv err cnt */
        status |= CO_CAN_ERRRX_WARNING;
      } else if (can_transmit_error_counter_get(((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle) > 0) {
	/* log_printf("tec cleared\n"); */
	((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle->ests_bit.tec = 0; /* clear recv err cnt */
        status |= CO_CAN_ERRTX_WARNING;
      }

      if (can_flag_get(((CANopenNodeSTM32 *)CANmodule->CANptr)->CANHandle, CAN_EPF_FLAG)) {
	/* log_printf("epf triggered, do nothing\n"); */
        status |= CO_CAN_ERRRX_PASSIVE | CO_CAN_ERRTX_PASSIVE;
      }
    }

    CANmodule->CANerrorStatus = status;
  }
}


/**
 * \brief           Read message from RX FIFO
 * \param           hfdcan: pointer to an FDCAN_HandleTypeDef structure that
 * contains the configuration information for the specified FDCAN. \param[in]
 * fifo: Fifo number to use for read \param[in]       fifo_isrs: List of
 * interrupts for respected FIFO
 */
static void prv_read_can_received_msg(can_type *hcan, uint32_t fifo,
                                      uint32_t fifo_isrs)
{

  CO_CANrxMsg_t rcvMsg;
  CO_CANrx_t *buffer =
      NULL;             /* receive message buffer from CO_CANmodule_t object. */
  uint16_t index;       /* index of received message */
  uint32_t rcvMsgIdent; /* identifier of the received message */
  uint8_t messageFound = 0;

  /* at32 */
  static can_rx_message_type rx_hdr;
  /* Read received message from FIFO */
  can_message_receive(hcan, fifo, &rx_hdr);
  /* Setup identifier (with RTR) and length */
  rcvMsg.ident =
      rx_hdr.standard_id | (rx_hdr.frame_type == CAN_TFT_REMOTE ? FLAG_RTR : 0x00);
  rcvMsg.dlc = rx_hdr.dlc;
  rcvMsgIdent = rcvMsg.ident;
  memcpy(rcvMsg.data, rx_hdr.data, min(8, rx_hdr.dlc));
  /* log_printf("rx: id:%ld, len:%d\n", rcvMsg.ident, rcvMsg.dlc); */
  /* DumpHex(rcvMsg.data, rcvMsg.dlc); */

  /*
   * Hardware filters are not used for the moment
   * \todo: Implement hardware filters...
   */
  if (CANModule_local->useCANrxFilters) {
    __BKPT(0);
  } else {
    /*
     * We are not using hardware filters, hence it is necessary
     * to manually match received message ID with all buffers
     */
    
    buffer = CANModule_local->rxArray;
    for (index = CANModule_local->rxSize; index > 0U; --index, ++buffer) {
      /* log_printf("filter: idx:%d, rcvid:0x%lx, ident:0x%x, mask:0x%x, found:%d\n", */
      /* 		 index, rcvMsgIdent, */
      /* 		 buffer->ident, buffer->mask, */
      /* 		 (((rcvMsgIdent ^ buffer->ident) & buffer->mask) == 0U) */
      /* 		 ); */
      if (((rcvMsgIdent ^ buffer->ident) & buffer->mask) == 0U) {
        messageFound = 1;
        break;
      }
    }
    //log_printf("msg found:%d\n", messageFound);
  }

  /* Call specific function, which will process the message */
  if (messageFound && buffer != NULL && buffer->CANrx_callback != NULL) {
    //log_printf("canrx_callback\n");
    buffer->CANrx_callback(buffer->object, (void *)&rcvMsg);
  }
}

/**
 * \brief           Rx FIFO 0 callback.
 * \param[in]       hcan: pointer to an CAN_HandleTypeDef structure that
 * contains the configuration information for the specified CAN.
 */
void HAL_CAN_RxFifo0MsgPendingCallback(can_type *hcan) {
  prv_read_can_received_msg(hcan, CAN_RX_FIFO0, 0);
}

/**
 * \brief           Rx FIFO 1 callback.
 * \param[in]       hcan: pointer to an CAN_HandleTypeDef structure that
 * contains the configuration information for the specified CAN.
 */
void HAL_CAN_RxFifo1MsgPendingCallback(can_type *hcan) {
  prv_read_can_received_msg(hcan, CAN_RX_FIFO1, 0);
}

/**
 * \brief           TX buffer has been well transmitted callback
 * \param[in]       hcan: pointer to an CAN_HandleTypeDef structure that
 * contains the configuration information for the specified CAN. \param[in]
 * MailboxNumber: the mailbox number that has been transmitted
 */
void CO_CANinterrupt_TX(CO_CANmodule_t *CANmodule, uint32_t MailboxNumber) {
  CANmodule->firstCANtxMessage =
      false; /* First CAN message (bootup) was sent successfully */
  CANmodule->bufferInhibitFlag = false; /* Clear flag from previous message */
  if (CANmodule->CANtxCount >
      0U) { /* Are there any new messages waiting to be send */
    CO_CANtx_t *buffer =
        &CANmodule->txArray[0]; /* Start with first buffer handle */
    uint16_t i;

    /*
     * Try to send more buffers, process all empty ones
     *
     * This function is always called from interrupt,
     * however to make sure no preemption can happen, interrupts are anyway
     * locked (unless you can guarantee no higher priority interrupt will try to
     * access to CAN instance and send data, then no need to lock interrupts..)
     */
    CO_LOCK_CAN_SEND(CANmodule);
    for (i = CANmodule->txSize; i > 0U; --i, ++buffer) {
      /* Try to send message */
      if (buffer->bufferFull) {
	/* log_printf("send in interrupt, txsize:%d\n", CANmodule->txSize); */
        if (prv_send_can_message(CANmodule, buffer)) {
          buffer->bufferFull = false;
          CANmodule->CANtxCount--;
          CANmodule->bufferInhibitFlag = buffer->syncFlag;
        }
      }
    }
    /* Clear counter if no more messages */
    if (i == 0U) {
      CANmodule->CANtxCount = 0U;
    }
    CO_UNLOCK_CAN_SEND(CANmodule);
  }
}

void HAL_CAN_TxMailbox0CompleteCallback(can_type *hcan) {
  can_flag_clear(CAN1, CAN_TM0TCF_FLAG);
  CO_CANinterrupt_TX(CANModule_local, CAN_TX_MAILBOX0);
}

void HAL_CAN_TxMailbox1CompleteCallback(can_type *hcan) {
  can_flag_clear(CAN1, CAN_TM1TCF_FLAG);
  CO_CANinterrupt_TX(CANModule_local, CAN_TX_MAILBOX1);
}

void HAL_CAN_TxMailbox2CompleteCallback(can_type *hcan) {
  can_flag_clear(CAN1, CAN_TM2TCF_FLAG);
  CO_CANinterrupt_TX(CANModule_local, CAN_TX_MAILBOX2);
}


void CAN1_SE_IRQHandler(void) {
  __IO uint32_t err_index = 0;
  if (can_flag_get(CAN1, CAN_ETR_FLAG) != RESET) {
    err_index = (CAN1->ests & 0x70) >> 4;
    can_flag_clear(CAN1, CAN_ETR_FLAG);
    /* log_printf("xfguo||clear etr, ERC:%ld, tec:%d, rec:%d\n", */
    /* 	       err_index, CAN1->ests_bit.tec, CAN1->ests_bit.rec); */
    /* error type is stuff error */
    if (err_index == 0x00000010) {
      /* when stuff error occur: in order to ensure communication normally,
      user must restart can or send a frame of highest priority message here */
      //printf("reconfigure can\n");
      can_reset(CAN1);
      can_configuration();
    }
    err_index=1;
  }
  if (can_flag_get(CAN1, CAN_EOIF_FLAG) != RESET) {
    err_index=2;
    //log_printf("can1.eoif set, cleared\n");
    can_flag_clear(CAN1, CAN_EOIF_FLAG);
  }
  if (err_index == 0){
    /* log_printf("unknown err set, can1.msts:%ld can1.ests:%ld eaf:%d, epf:%d, bof:%d, etr:%d, tec:%d, rec:%d\n", */
    /* 	       CAN1->msts, CAN1->ests, */
    /* 	       CAN1->ests_bit.eaf, CAN1->ests_bit.epf, CAN1->ests_bit.bof, */
    /* 	       CAN1->ests_bit.etr, CAN1->ests_bit.tec, CAN1->ests_bit.rec); */

    /* log_printf("call canmodule process to handle the error?\n"); */
    /* CO_CANmodule_process(CANModule_local); */
  }
}

void USBFS_H_CAN1_TX_IRQHandler(void){
  if (can_flag_get(CAN1, CAN_TM0TCF_FLAG) != RESET) {
    can_flag_clear(CAN1, CAN_TM0TCF_FLAG);
    HAL_CAN_TxMailbox0CompleteCallback(CAN1); /* Mailbox number is irrelevant in at32 */
  } else if (can_flag_get(CAN1, CAN_TM1TCF_FLAG) != RESET) {
    can_flag_clear(CAN1, CAN_TM1TCF_FLAG);
    HAL_CAN_TxMailbox1CompleteCallback(CAN1); /* Mailbox number is irrelevant in at32 */
  } else if (can_flag_get(CAN1, CAN_TM2TCF_FLAG) != RESET) {
    can_flag_clear(CAN1, CAN_TM2TCF_FLAG);
    HAL_CAN_TxMailbox2CompleteCallback(CAN1); /* Mailbox number is irrelevant in at32 */
  }
}

void USBFS_L_CAN1_RX0_IRQHandler(void) { // CAN Receive interrupt handle function
  /* log_printf("can msg\n"); */
  if (can_flag_get(CAN1, CAN_RF0MN_FLAG) != RESET) {
    HAL_CAN_RxFifo0MsgPendingCallback(CAN1);
  }
}
