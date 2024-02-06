/* -----------------------------------------------------------------------------
 * Copyright (c) 2013-2017 Arm Limited (or its affiliates). All 
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
 * $Date:        19. September 2017
 * $Revision:    V2.3
 *
 * Project:      OTG Full/Low-Speed Common Driver for ST STM32F1xx
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 2.3
 *    Removed unnecessary include of cmsis_os.h
 *  Version 2.2
 *    Corrected over-current pin configuration
 *  Version 2.1
 *    Added configuration for USB Device VBUS pin so connection/disconnection of 
 *    USB cable can be detected when device is self-powered
 *  Version 2.0
 *    Based on API V2.01
 *  Version 1.03
 *    Based on API V1.10 (namespace prefix ARM_ added)
 *  Version 1.00
 *    Initial release
 */

#include <stdint.h>
#include <string.h>

#include "stm32f10x.h"

#include "GPIO_STM32F10x.h"
#include "OTG_STM32F10x_cl.h"

#include "Driver_USBH.h"
#include "Driver_USBD.h"


extern void USBH_FS_IRQ (uint32_t gintsts);
extern void USBD_FS_IRQ (uint32_t gintsts);

uint8_t otg_fs_role = ARM_USB_ROLE_NONE;




// Common IRQ Routine **********************************************************

/**
  \fn          void OTG_FS_IRQHandler (void)
  \brief       USB Interrupt Routine (IRQ).
*/
void OTG_FS_IRQHandler (void) {
  uint32_t gintsts;

  gintsts = OTG_FS->GINTSTS & OTG_FS->GINTMSK;

#if (defined(MX_USB_OTG_FS_HOST) && defined(MX_USB_OTG_FS_DEVICE))
  switch (otg_fs_role) {
#ifdef MX_USB_OTG_FS_HOST
    case ARM_USB_ROLE_HOST:
      USBH_FS_IRQ (gintsts);
      break;
#endif
#ifdef MX_USB_OTG_FS_DEVICE
    case ARM_USB_ROLE_DEVICE:
      USBD_FS_IRQ (gintsts);
      break;
#endif
    default:
      break;
  }
#else
#ifdef MX_USB_OTG_FS_HOST
  USBH_FS_IRQ (gintsts);
#else
  USBD_FS_IRQ (gintsts);
#endif
#endif
}


// Public Functions ************************************************************

/**
  \fn          void OTG_FS_PinsConfigure (uint8_t pins_mask)
  \brief       Configure single or multiple USB Pin(s).
  \param[in]   Mask of pins to be configured (possible masking values:
               USB_PIN_DP, USB_PIN_DM, USB_PIN_VBUS, USB_PIN_OC, USB_PIN_ID)
*/
void OTG_FS_PinsConfigure (uint8_t pins_mask) {

#ifdef MX_USB_OTG_FS_VBUS_Power_Pin     // Host VBUS power driving pin (output)
  if ((pins_mask & ARM_USB_PIN_VBUS) != 0U) {
    if (otg_fs_role == ARM_USB_ROLE_DEVICE) {
      GPIO_PortClock   (GPIOA, true);
      GPIO_PinConfigure(GPIOA, 9, GPIO_IN_FLOATING, GPIO_MODE_INPUT);
    }
    if (otg_fs_role == ARM_USB_ROLE_HOST) {
      GPIO_PortClock   (MX_USB_OTG_FS_VBUS_Power_GPIOx, true);
#if (USB_OTG_FS_VBUS_Power_Pin_Active == 0)
      GPIO_PinWrite    (MX_USB_OTG_FS_VBUS_Power_GPIOx, MX_USB_OTG_FS_VBUS_Power_GPIO_Pin, 1U);
#else
      GPIO_PinWrite    (MX_USB_OTG_FS_VBUS_Power_GPIOx, MX_USB_OTG_FS_VBUS_Power_GPIO_Pin, 0U);
#endif
      GPIO_PinConfigure(MX_USB_OTG_FS_VBUS_Power_GPIOx, MX_USB_OTG_FS_VBUS_Power_GPIO_Pin, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT10MHZ);
    }
  }
#endif
#ifdef MX_USB_OTG_FS_Overcurrent_Pin    // Host overcurrent sensing pin (input)
  if ((pins_mask & ARM_USB_PIN_OC) != 0U) {
    if (otg_fs_role == ARM_USB_ROLE_HOST) {
      GPIO_PortClock   (MX_USB_OTG_FS_Overcurrent_GPIOx, true);
      GPIO_PinConfigure(MX_USB_OTG_FS_Overcurrent_GPIOx, MX_USB_OTG_FS_Overcurrent_GPIO_Pin, GPIO_IN_FLOATING, GPIO_MODE_INPUT);
    }
  }
#endif
}

/**
  \fn          void OTG_FS_PinsUnconfigure (uint8_t pins_mask)
  \brief       Unconfigure to reset settings single or multiple USB Pin(s).
  \param[in]   Mask of pins to be unconfigured (possible masking values:
               USB_PIN_DP, USB_PIN_DM, USB_PIN_VBUS, USB_PIN_OC, USB_PIN_ID)
*/
void OTG_FS_PinsUnconfigure (uint8_t pins_mask) {

#ifdef MX_USB_OTG_FS_VBUS_Power_Pin
  if ((pins_mask & ARM_USB_PIN_VBUS) != 0U) {
    if (otg_fs_role == ARM_USB_ROLE_HOST) {
      GPIO_PortClock   (MX_USB_OTG_FS_VBUS_Power_GPIOx, true);
      GPIO_PinConfigure(MX_USB_OTG_FS_VBUS_Power_GPIOx, MX_USB_OTG_FS_VBUS_Power_GPIO_Pin, GPIO_IN_FLOATING, GPIO_MODE_INPUT);
    }
  }
#endif
#ifdef MX_USB_OTG_FS_Overcurrent_Pin
  if ((pins_mask & ARM_USB_PIN_OC) != 0U) {
    if (otg_fs_role == ARM_USB_ROLE_HOST) {
      GPIO_PortClock   (MX_USB_OTG_FS_Overcurrent_GPIOx, true);
      GPIO_PinConfigure(MX_USB_OTG_FS_Overcurrent_GPIOx, MX_USB_OTG_FS_Overcurrent_GPIO_Pin, GPIO_IN_FLOATING, GPIO_MODE_INPUT);
    }
  }
#endif
}

/**
  \fn          void OTG_FS_PinVbusOnOff (bool state)
  \brief       Drive VBUS Pin On/Off.
  \param[in]   state    State On/Off (true = On, false = Off)
*/

void OTG_FS_PinVbusOnOff (bool state) {

#ifdef MX_USB_OTG_FS_VBUS_Power_Pin
  if (otg_fs_role == ARM_USB_ROLE_HOST) {
#if (USB_OTG_FS_VBUS_Power_Pin_Active == 0)
    GPIO_PinWrite (MX_USB_OTG_FS_VBUS_Power_GPIOx, MX_USB_OTG_FS_VBUS_Power_GPIO_Pin,((state == true) ? 0U : 1U));
#else
    GPIO_PinWrite (MX_USB_OTG_FS_VBUS_Power_GPIOx, MX_USB_OTG_FS_VBUS_Power_GPIO_Pin,((state == true) ? 1U : 0U));
#endif
  }
#endif
}

/**
  \fn          bool OTG_FS_PinGetOC (void)
  \brief       Get state of Overcurrent Pin.
  \return      overcurrent state (true = Overcurrent active, false = No overcurrent)
*/
bool OTG_FS_PinGetOC (void) {

#ifdef MX_USB_OTG_FS_Overcurrent_Pin
  if (otg_fs_role == ARM_USB_ROLE_HOST) {
#if (USB_OTG_FS_Overcurrent_Pin_Active == 0)
    return ((GPIO_PinRead (MX_USB_OTG_FS_Overcurrent_GPIOx, MX_USB_OTG_FS_Overcurrent_GPIO_Pin) == 0U) ? true : false);
#else
    return ((GPIO_PinRead (MX_USB_OTG_FS_Overcurrent_GPIOx, MX_USB_OTG_FS_Overcurrent_GPIO_Pin) == 1U) ? true : false);
#endif
  }
  return false;
#else
  return false;
#endif
}
