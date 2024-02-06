/* -----------------------------------------------------------------------------
 * Copyright (c) 2013-2015 Arm Limited (or its affiliates). All 
 * rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * $Date:        22. September 2015
 * $Revision:    V2.0
 *
 * Project:      MCI Driver Definitions for STMicroelectronics STM32F10x
 * -------------------------------------------------------------------------- */

#ifndef __MCI_STM32F10X_H
#define __MCI_STM32F10X_H

#include "stm32f10x.h"
#include "GPIO_STM32F10x.h"
#include "DMA_STM32F10x.h"

#include "Driver_MCI.h"

#include "RTE_Components.h"
#include "RTE_Device.h"

#include <string.h>


#if (defined(RTE_Drivers_MCI0) && (RTE_SDIO == 0))
  #error "SDIO not configured in RTE_Device.h!"
#endif

/* SDIO DMA */
#define SDIO_DMA_Instance               DMAx_CHANNELy(RTE_SDIO_DMA_NUMBER, RTE_SDIO_DMA_CHANNEL)
#define SDIO_DMA_Number                 RTE_SDIO_DMA_NUMBER
#define SDIO_DMA_Channel                RTE_SDIO_DMA_CHANNEL
#define SDIO_DMA_Priority               RTE_SDIO_DMA_PRIORITY
#define SDIO_DMA_Handler                DMAx_CHANNELy_EVENT(RTE_SDIO_DMA_NUMBER, RTE_SDIO_DMA_CHANNEL)

#if (RTE_SDIO_BUS_WIDTH_4)
  #define SDIO_D0_Pin                   1
  #define SDIO_D1_Pin                   1
  #define SDIO_D2_Pin                   1
  #define SDIO_D3_Pin                   1
#endif
#if (RTE_SDIO_BUS_WIDTH_8)
  #define SDIO_D4_Pin                   1
  #define SDIO_D5_Pin                   1
  #define SDIO_D6_Pin                   1
  #define SDIO_D7_Pin                   1
#endif

#if (RTE_SDIO_CD_EN)
  #define MemoryCard_CD_Pin             1
  #define MemoryCard_CD_GPIOx           RTE_SDIO_CD_PORT
  #define MemoryCard_CD_GPIO_Pin        RTE_SDIO_CD_PIN
  #define MemoryCard_CD_GPIO_PuPd     ((RTE_SDIO_CD_ACTIVE == 0) ? GPIO_IN_PULL_UP : GPIO_IN_PULL_DOWN)
  #define MemoryCard_CD_Pin_Active    ((RTE_SDIO_CD_ACTIVE == 0) ? 0  : 1)
#endif

#if (RTE_SDIO_WP_EN)
  #define MemoryCard_WP_Pin             1
  #define MemoryCard_WP_GPIOx           RTE_SDIO_WP_PORT
  #define MemoryCard_WP_GPIO_Pin        RTE_SDIO_WP_PIN
  #define MemoryCard_WP_GPIO_PuPd     ((RTE_SDIO_WP_ACTIVE == 0) ? GPIO_IN_PULL_UP : GPIO_IN_PULL_DOWN)
  #define MemoryCard_WP_Pin_Active    ((RTE_SDIO_WP_ACTIVE == 0) ? 0  : 1)
#endif


/* Define 4-bit data bus width */
#if defined(SDIO_D0_Pin) && defined(SDIO_D1_Pin) && defined(SDIO_D2_Pin) && defined(SDIO_D3_Pin)
  #define MCI_BUS_WIDTH_4   1U
#else
  #define MCI_BUS_WIDTH_4   0U
#endif

/* Define 8-bit data bus width */
#if defined(SDIO_D0_Pin) && defined(SDIO_D1_Pin) && defined(SDIO_D2_Pin) && defined(SDIO_D3_Pin) && \
    defined(SDIO_D4_Pin) && defined(SDIO_D5_Pin) && defined(SDIO_D6_Pin) && defined(SDIO_D7_Pin)
  #define MCI_BUS_WIDTH_8   1U
#else
  #define MCI_BUS_WIDTH_8   0U
#endif

/* Define Card Detect pin existence */
#if defined(MemoryCard_CD_Pin)
  #define MCI_CD_PIN        1U
#else
  #define MCI_CD_PIN        0U
#endif

/* Define Write Protect pin existence */
#if defined(MemoryCard_WP_Pin)
  #define MCI_WP_PIN        1U
#else
  #define MCI_WP_PIN        0U
#endif

/* SDIO Adapter Clock definition */
#define SDIOCLK       (uint32_t)RTE_HCLK    /* SDIO adapter clock */

#ifndef SDIO_MASK_STBITERRIE
#define SDIO_MASK_STBITERRIE    0U
#endif
#ifndef SDIO_STA_STBITERR
#define SDIO_STA_STBITERR       0U
#endif
#ifndef SDIO_ICR_STBITERRC
#define SDIO_ICR_STBITERRC      0U
#endif

/* Interrupt clear Mask */
#define SDIO_ICR_BIT_Msk       (SDIO_ICR_CCRCFAILC | \
                                SDIO_ICR_DCRCFAILC | \
                                SDIO_ICR_CTIMEOUTC | \
                                SDIO_ICR_DTIMEOUTC | \
                                SDIO_ICR_TXUNDERRC | \
                                SDIO_ICR_RXOVERRC  | \
                                SDIO_ICR_CMDRENDC  | \
                                SDIO_ICR_CMDSENTC  | \
                                SDIO_ICR_DATAENDC  | \
                                SDIO_ICR_STBITERRC | \
                                SDIO_ICR_DBCKENDC  | \
                                SDIO_ICR_SDIOITC)

/* Error interrupt mask */
#define SDIO_STA_ERR_BIT_Msk   (SDIO_STA_CCRCFAIL | \
                                SDIO_STA_DCRCFAIL | \
                                SDIO_STA_CTIMEOUT | \
                                SDIO_STA_DTIMEOUT | \
                                SDIO_STA_STBITERR)

/* Driver flag definitions */
#define MCI_INIT      ((uint8_t)0x01)   /* MCI initialized           */
#define MCI_POWER     ((uint8_t)0x02)   /* MCI powered on            */
#define MCI_SETUP     ((uint8_t)0x04)   /* MCI configured            */
#define MCI_RESP_LONG ((uint8_t)0x08)   /* Long response expected    */
#define MCI_RESP_CRC  ((uint8_t)0x10)   /* Check response CRC error  */
#define MCI_DATA_XFER ((uint8_t)0x20)   /* Transfer data             */
#define MCI_DATA_READ ((uint8_t)0x40)   /* Read transfer             */
#define MCI_READ_WAIT ((uint8_t)0x80)   /* Read wait operation start */

#define MCI_RESPONSE_EXPECTED_Msk (ARM_MCI_RESPONSE_SHORT      | \
                                   ARM_MCI_RESPONSE_SHORT_BUSY | \
                                   ARM_MCI_RESPONSE_LONG)

/* MCI Transfer Information Definition */
typedef struct _MCI_XFER {
  uint8_t *buf;                         /* Data buffer                        */
  uint32_t cnt;                         /* Data bytes to transfer             */
} MCI_XFER;

/* MCI Driver State Definition */
typedef struct _MCI_INFO {
  ARM_MCI_SignalEvent_t cb_event;       /* Driver event callback function     */
  ARM_MCI_STATUS        status;         /* Driver status                      */
  uint32_t             *response;       /* Pointer to response buffer         */
  MCI_XFER              xfer;           /* Data transfer description          */
  uint8_t volatile      flags;          /* Driver state flags                 */
  uint32_t              dctrl;          /* Data control register value        */
  uint32_t              dlen;           /* Data length register value         */
} MCI_INFO;

#endif /* __MCI_STM32F10X_H */
