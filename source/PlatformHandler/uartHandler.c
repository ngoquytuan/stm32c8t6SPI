#include <stdio.h>
#include "stm32f10x.h"                  // Device header
#include "GPIO_STM32F10x.h"             // Keil::Device:GPIO
#include "uartHandler.h"   
//Kiem tra dinh nghia fputc
//Redefining low-level library functions to enable direct use of high-level library functions in the C library
//http://www.keil.com/support/man/docs/armlib/armlib_chr1358938931411.htm
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET)
		; 
    USART_SendData(USART1,(char)ch);   
	return ch;
}
//#include <stdarg.h>


/**
  * @brief  Configures the USART1
  * @param  None
  * @return None
  */
void USART1_Init(void)
{
	USART_InitTypeDef USARTx_Init;
	
	USARTx_Init.USART_WordLength = USART_WordLength_8b;
	USARTx_Init.USART_StopBits = USART_StopBits_1;
	USARTx_Init.USART_Parity = USART_Parity_No;
	USARTx_Init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USARTx_Init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USARTx_Init.USART_BaudRate = 115200;
	
	/* Enable UART clock, if using USART2 or USART3 ... we must use RCC_APB1PeriphClockCmd(RCC_APB1Periph_USARTx, ENABLE) */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);
	USART_DeInit(USART1);
	GPIO_PortClock   (GPIOA, true);
	GPIO_PinWrite    (GPIOA, 9, 0);
	GPIO_PinConfigure(GPIOA, 9,
										GPIO_AF_PUSHPULL, 
										GPIO_MODE_OUT50MHZ);
	
	/* Configure PA9 for USART Tx as alternate function push-pull */
	GPIO_PinConfigure(GPIOA, 10,
										GPIO_IN_FLOATING,
										GPIO_MODE_INPUT);
	/* Configure PA10 for USART Rx as input floating */	
	
		/* USART configuration */
	USART_Init(USART1, &USARTx_Init);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	//USART_ITConfig(USART1,USART_IT_TXE,ENABLE);
	/* Enable USART */
	USART_Cmd(USART1, ENABLE);
	//Enable USART_IT_RXNE
	USER_UART_NVIC();
}

void USER_UART_NVIC(void)
{
  NVIC_InitTypeDef  NVIC_InitStructure;
  
	/* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
}

void printmcuclk(void)
{
	// Check mcu clock
	RCC_ClocksTypeDef mcu_clk;
	RCC_GetClocksFreq(&mcu_clk);
	printf(">Thach anh: \r\nADCCLK:%d\r\nHCLK:%d\r\nPCLK1:%d\r\nPCLK2:%d\r\nSYSCLK:%d",
																mcu_clk.ADCCLK_Frequency,mcu_clk.HCLK_Frequency,
																mcu_clk.PCLK1_Frequency,mcu_clk.PCLK2_Frequency,mcu_clk.SYSCLK_Frequency);
}


