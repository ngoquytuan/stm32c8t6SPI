#ifndef	W5500_INI
#define	W5500_INI
#include <stdio.h>
#include "stm32f10x.h"
#include "GPIO_STM32F10x.h"             // Keil::Device:GPIO
#include "wizchip_conf.h"
#include "delay.h"
#include "spiconfig.h"
#include "socketdefines.h"
static const int8_t WZN_ERR = -1;
/*
#define W5500_CS_GPIO_PORT    GPIOA
#define W5500_CS_PIN					0
#define W5500_RST							2	//W5500 RST
#define W5500_RST_GPIO				GPIOA
*/
void w5500_lib_init(void);
void Net_Conf(wiz_NetInfo netinfo);
void checklink();
extern wiz_NetInfo gWIZNETINFO;
extern uint8_t DNS_2nd[4];
#endif