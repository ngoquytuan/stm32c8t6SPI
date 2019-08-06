/**
  ******************************************************************************
  * @file    RTC/Calendar/stm32f10x_it.h 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_IT_H
#define __STM32F10x_IT_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f10x.h"
//extern __IO uint32_t ms10k;
#ifdef USE_INTERNAL_RTC
extern __IO uint32_t TimeDisplay;
#endif
extern __IO uint32_t TimingDelay;
extern __IO uint32_t task1s;
extern __IO uint32_t task100ms; 
extern __IO uint32_t u1out ;
extern __IO uint32_t u2out ;
extern uint16_t phystatus_check_cnt;
extern __IO uint16_t count1ms;
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void RTC_IRQHandler(void);

extern const uint8_t RX_BUFFER_SIZE0;
extern uint8_t USART1_index,USART1_rx_data_buff[];
void USART1_IRQHandler(void);

//extern const uint8_t RX2_BUFFER_SIZE;
//extern uint8_t USART2_index,rx2_data_buff[];
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);

#endif /* __STM32F10x_IT_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
