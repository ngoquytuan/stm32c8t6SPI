/**
 * @file	timerHandler.h
 * @brief	Header File for TIMER Handler Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#ifndef __TIMERHANDLER_H
#define __TIMERHANDLER_H
#include "stm32f10x.h"
#include "time.h"
#include "boardutil.h"
#define ONTIME 1
#define STOP   0
//UART1 receiver timeout
extern __IO uint32_t u1out;// 50 is 50ms
extern volatile uint16_t msec_cnt;
extern volatile uint8_t sec_cnt;
extern volatile uint32_t snmp_tick_1ms;
extern struct tm* timeinfo;
void Timer_Configuration(void);

#endif

