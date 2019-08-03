
#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x_rcc.h"
void delay(__IO uint32_t num);
void delay_ms(__IO uint32_t num);
void delay_us(__IO uint32_t num);

#endif

