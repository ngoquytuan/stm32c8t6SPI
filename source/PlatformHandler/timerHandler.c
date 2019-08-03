/**
 * @file	timerHandler.c
 * @brief	TIMER Handler Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include "timerHandler.h"
#include "ff.h"
#include "diskio.h"
struct tm* timeinfo;
extern time_t timenow;
volatile UINT Timer;
extern void disk_timerproc (void);
extern uint16_t RetrySend ; //60 giay
extern uint16_t sycnPeriod ;// 1 gio
//#include "dhcp.h"
//#include "httpServer.h" // for function httpServer_time_handler()
volatile uint32_t snmp_tick_1ms = 0;
volatile uint16_t msec_cnt = 0;
volatile uint8_t sec_cnt = 0;
volatile uint16_t phystatus_check_cnt = 0;
/*---------------------------------------------*/
/* 1kHz timer process                          */
/*---------------------------------------------*/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET )
	{
		// Also cleared the wrong interrupt flag in the ISR
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // Clear the interrupt flag

		//Todo
		//Timer++;	/* Increment performance counter */
		snmp_tick_1ms++;
		msec_cnt++; // milli second
		phystatus_check_cnt++;
		if(msec_cnt >= 1000 - 1) // second
		{
			sec_cnt++;
			timenow++;
			//httpServer_time_handler();
			//DHCP_time_handler();
			msec_cnt = 0;
			RetrySend++;
			sycnPeriod++;
		}
		disk_timerproc();	/* Disk timer process */
		/*if(sec_cnt >= 60 - 1)
		{
			sec_cnt = 0;
		}*/
	}
}

/**
  * @brief  Configures the Timer
  * @param  None
  * @return None
  */
void Timer_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 1000;// count 10000 tick then interrupt, gia tri max cua bo dem
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM2, 71, TIM_PSCReloadMode_Immediate);

  /* TIM enable counter */
  TIM_Cmd(TIM2, ENABLE);

  /* TIM IT enable */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

#ifdef ENABLE_TIMER_INT
  // Timer for delay

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 1;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM3, 71, TIM_PSCReloadMode_Immediate);

  /* TIM enable counter */
  TIM_Cmd(TIM3, DISABLE);

  /* TIM IT enable */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
#endif

}

/*---------------------------------------------------------*/
/* User provided RTC function for FatFs module             */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called back     */
/* from FatFs module.                                      */

#if !FF_FS_NORTC && !FF_FS_READONLY
DWORD get_fattime (void)
{
	/* Get local time */
	timeinfo = localtime( &timenow );
	/* Pack date and time into a DWORD variable */
	return	  ((DWORD)(timeinfo->tm_year - 80) << 25)//Year origin from the 1980 (0..127, e.g. 37 for 2017)
			| (((DWORD)timeinfo->tm_mon+1) << 21)//Month (1..12)
			| ((DWORD)timeinfo->tm_mday << 16)
			| ((DWORD)timeinfo->tm_hour << 11)
			| ((DWORD)timeinfo->tm_min << 5)
			| ((DWORD)timeinfo->tm_sec >> 1);
}
#endif
