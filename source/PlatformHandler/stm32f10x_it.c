/**
  ******************************************************************************
  * @file    RTC/Calendar/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "GPIO_STM32F10x.h"             // Keil::Device:GPIO
//#include "snmp.h"
#define ONTIME 1
#define STOP   0
/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup RTC_Calendar
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#ifdef USE_INTERNAL_RTC
__IO uint32_t TimeDisplay = 0;
#endif
//__IO uint32_t ms10k = 0;
__IO uint32_t TimingDelay = 0;
__IO uint32_t task1s = 1000;
__IO uint32_t task100ms = 100;
//UART1 receiver timeout
__IO uint32_t u1out = 50;// 50 is 50ms
__IO uint32_t u2out = 50;// 50 is 50ms

__IO uint16_t count1ms = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}



// USART Receiver buffer
const uint8_t RX_BUFFER_SIZE0 = 25;
uint8_t USART1_index=0,USART1_rx_data_buff[RX_BUFFER_SIZE0];

void USART1_IRQHandler(void)
{
	char USART1_data;
	/* RXNE handler */
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		//USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		USART1_data=(USART1->DR & (uint16_t)0x01FF);
		u1out = 50;// 50ms
		USART1_rx_data_buff[USART1_index++]=USART1_data;

		if(USART1_index==RX_BUFFER_SIZE0) USART1_index=0;
		//printf("Kiem tra uart 1\r\n");	
	}
	
//	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
//  {   
//    /* Write one byte to the transmit data register */
//    printf("Kiem tra uart 1\r\n");
//		USART_ClearITPendingBit(USART1, USART_IT_TXE);
//  }

}

// USART2 Receiver buffer
//const uint8_t RX2_BUFFER_SIZE = 50;
//uint8_t USART2_index=0,rx2_data_buff[RX2_BUFFER_SIZE];

void USART2_IRQHandler(void)
{
	char RX2_data;
	/* RXNE handler */
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		//printf("USART_IT_RXNE\r\n");
		//USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		RX2_data=(USART2->DR & (uint16_t)0x01FF);
		u2out=50;// 50ms
		//rx2_data_buff[USART2_index++]=RX2_data;
		//if(USART2_index == RX2_BUFFER_SIZE) USART2_index=0;	
	}
	
	  if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		/* Clear IDLE flag by reading status register first */
    /* And follow by reading data register */
    volatile uint32_t tmp;                  /* Must be volatile to prevent optimizations */
    tmp = USART2->SR;                       /* Read status register */
    tmp = USART2->DR;                       /* Read data register */
    (void)tmp;                              /* Prevent compiler warnings */
		
		//USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		//RX2_data=(USART2->DR & (uint16_t)0x01FF);
		u2out=50;// 50ms
		//rx2_data_buff[USART2_index++]=RX2_data;
		//if(USART2_index == RX2_BUFFER_SIZE) USART2_index=0;	
	}
	
}
/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles RTC global interrupt request.
  * @param  None
  * @retval None
  */
#ifdef USE_INTERNAL_RTC
uint8_t ledbit = 1;
void RTC_IRQHandler(void)
{
  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
  {
    /* Clear the RTC Second interrupt */
    RTC_ClearITPendingBit(RTC_IT_SEC);

    /* Toggle LED1 */
    ledbit = !ledbit;
    GPIO_PinWrite(GPIOB, 8, ledbit);
    /* Enable time update */
    TimeDisplay = 1;

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    
  }
}
#endif

void SysTick_Handler(void)
{
  //ms10k++;
	//if(ms10k > 9999) ms10k = 0; 
}
/**
  * @brief  This function handles TIM global interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
	#ifdef TEST_TIM
		tim_irq_fuc();
	#endif
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
