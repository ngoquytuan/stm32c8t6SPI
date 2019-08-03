/**
 * @file	rccHandler.c
 * @brief	RCC Handler Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */
#include "GPIO_STM32F10x.h"             // Keil::Device:GPIO
#include "stm32f10x_gpio.h"
#include "rccHandler.h"
#include "stm32f10x.h"
//#include "boardutil.h"

/**
  * @brief  Sets System clock frequency to 72MHz and configure HCLK, PCLK2
  *         and PCLK1 prescalers.
  * @param  None
  * @return None
  */
void RCC_Configuration(void)
{
ErrorStatus HSEStartUpStatus;

 /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK */
    //RCC_PCLK2Config(RCC_HCLK_Div1);
    RCC_PCLK2Config(RCC_HCLK_Div2); // for Baudrate 600 support

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);
//    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }



  /* TIM2 clock enable */
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3|RCC_APB1Periph_USART2, ENABLE);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 | RCC_APB1Periph_USART2, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD
  											|RCC_APB2Periph_AFIO  | RCC_APB2Periph_USART1, ENABLE);

  }
}


// IWDG: STM32 Independent Watchdog Initialization
void IWDG_Config(void)
{
	
	/* Check if the system has resumed from IWDG reset */
  if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
  { 
    /* IWDGRST flag set */
    /* Turn on LED1 */
    GPIO_PinWrite(GPIOA, 1, 0);
    /* Clear reset flags */
    RCC_ClearFlag();
  }
  else
  {
    /* WWDGRST flag is not set */
    /* Turn off LED1 */
    GPIO_PinWrite(GPIOA, 1, 1);
  }
	
	//RCC_LSICmd(ENABLE); //open LSI
	//while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		RCC_ClearFlag();
	}

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_128); // 40Khz / 128 = 0.31KHz; 1 / 0.31KHz = 3.22ms
	//IWDG_SetReload(1250); // 1s, max 0xfff
	IWDG_SetReload(0xfff); // 4095 * 3.22ms = 13185.9ms = 13 seconds; it means if IWDG was not reloaded, MCU will reset!

	//IWDG_ReloadCounter();
	IWDG_Enable();
}

/**
* @brief  GPIO_config cho kit C8T6 china : LED PA1
  * @param  GPIO_PinWrite chu y  GPIO_Pin_0 # 0???
  * @retval  
  */
void GPIO_config(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;			//Interrupt parameter interrupt priority

	//LED PA1 : cho kit C8T6 china : LED PA1
	GPIO_PortClock   (GPIOA, true);
	GPIO_PinConfigure(GPIOA, 1, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT10MHZ);
	GPIO_PinWrite(GPIOA, 1, 0);
	
	// PA4 : cho kit C8T6 china : CS SD
	//GPIO_PortClock   (GPIOA, true);
	//GPIO_PinConfigure(GPIOA, GPIO_Pin_4, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ);
	//GPIO_PinWrite(GPIOA, GPIO_Pin_4, 1);
	// PC13 : cho kit C8T6 china : CS FLASH
	//GPIO_PortClock   (GPIOC, true);
	//GPIO_PinConfigure(GPIOC, GPIO_Pin_13, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ);
	//GPIO_PinWrite(GPIOC, GPIO_Pin_13, 1);
	
	// W5500_RST Pin initialization configuration (PA2) 
	GPIO_PinConfigure(GPIOA, 2, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT10MHZ);
	GPIO_PinWrite(GPIOA, 2, 1);
	
	// Initialize the network module W5500-CS pin (PA0)
	GPIO_PinConfigure(GPIOA, 0, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ);
	GPIO_PinWrite(GPIOA, 0, 1);
	// Initialize the network module SDCARD-CS pin (PA4)
	GPIO_PinConfigure(GPIOA, 4, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ);
	GPIO_PinWrite(GPIOA, 4, 1);
	
	
	// W5500_INT Pin initialization configuration(PA3)
	GPIO_PinConfigure(GPIOA, 3, GPIO_IN_PULL_UP, GPIO_MODE_INPUT);
	

	// Enable the EXTI3 Interrupt 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;				//W5500_INT External interrupt channel
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//Preemption priority 2 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;			//Subpriority 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//Enable external interrupt channel
	NVIC_Init(&NVIC_InitStructure);


  	// Connect EXTI Line3 to PA3 
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource3);


	// PA3 as W5500 interrupt input 
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}
