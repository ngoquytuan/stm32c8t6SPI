/*
- 29/July/2019 : Make new project on STM32F103C8T6 China kit
UART1 : debug
ADCCLK:36000000
HCLK:72000000
PCLK1:36000000
PCLK2:72000000
SYSCLK:72000000
STM32 Independent WatchDog : 13s
1 LED on PA0
TIM2 : Tao 1 s
SysTick tao 0.1ms
SDcard : SPI1, CS = PA4 disk "\0" 
SPI Flash W25Q16DV: SPI1, CS = PC13
NTP Time Server
SNMPv1 Agent, Trap
webpages
**/

#include "main.h"

 
//////////////////////////////////////////////////////////////////////
// Shared Buffer Definition WEB server////////////////////////////////
//////////////////////////////////////////////////////////////////////
uint8_t RX_BUF[DATA_BUF_SIZEHTTP];
uint8_t TX_BUF[DATA_BUF_SIZEHTTP];
uint8_t socknumlist[] = {5, 6, 7};

volatile uint32_t ms10k;
time_t timenow = 1564551507;


/**************************************************************************/
/* Fatfs structure */
FATFS FS;
/* Size structure for FATFS */
TM_FATFS_Size_t CardSize;


/**
  * @brief  sw_eeprom_stm32
  * @param  make some flash blocks come eeprom for store data
  * @retval  just call this fuction
  */	
void sw_eeprom_stm32(void)
{
		/* Unlock the Flash Program Erase controller */
  FLASH_Unlock();
  /* EEPROM Init */
  if(EE_Init() == FLASH_COMPLETE) printf("Emu EEPROM STM32 ready !\r\n");
}

/**
  * @brief  test_eeprom
  * @param  Call this fuction for test store data to eeprom
  * @retval  
  */	
void test_eeprom(void)
{
	uint16_t a,b,c,d,e,f; 
	uint16_t confirm[3];
	
	//Kiem tra confirm xem dung ko, neu sai thi du lieu bi sai => reset factory setting
	// Neu dung thi load config
	
	EE_ReadVariable(0,&confirm[0]);
	EE_ReadVariable(1,&confirm[1]);
	EE_ReadVariable(2,&confirm[2]);	
	
	if( (confirm[0] == 123) && (confirm[1] == 456) && (confirm[2] == 789))
	{
		printf("Right eeprom data, load configs now\r\n");
		//Load IP
		EE_ReadVariable(4,&a);
		EE_ReadVariable(5,&b);
		EE_ReadVariable(6,&c);
		EE_ReadVariable(7,&d);
		gWIZNETINFO.ip[0] = a;
		gWIZNETINFO.ip[1] = b;
		gWIZNETINFO.ip[2] = c;
		gWIZNETINFO.ip[3] = d;
		//printf("Load ip: %d.%d.%d.%d",gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
		//Load GW
		EE_ReadVariable(8,&a);
		EE_ReadVariable(9,&b);
		EE_ReadVariable(10,&c);
		EE_ReadVariable(11,&d);
		gWIZNETINFO.gw[0] = a;
		gWIZNETINFO.gw[1] = b;
		gWIZNETINFO.gw[2] = c;
		gWIZNETINFO.gw[3] = d;
		//Load SN
		EE_ReadVariable(12,&a);
		EE_ReadVariable(13,&b);
		EE_ReadVariable(14,&c);
		EE_ReadVariable(15,&d);
		gWIZNETINFO.sn[0] = a;
		gWIZNETINFO.sn[1] = b;
		gWIZNETINFO.sn[2] = c;
		gWIZNETINFO.sn[3] = d;
	}
	else
	{
		printf("Wrong eeprom data\r\n");
		EE_WriteVariable(0,123);
		EE_WriteVariable(1,456);
		EE_WriteVariable(2,789);
		//IP 192.168.1.246
		EE_WriteVariable(4,192);
		EE_WriteVariable(5,168);
		EE_WriteVariable(6,1);
		EE_WriteVariable(7,246);
		//GW: 192.168.1.1
		EE_WriteVariable(8,192);
		EE_WriteVariable(9,168);
		EE_WriteVariable(10,1);
		EE_WriteVariable(11,1);
		//SN 255.255.255.0
		EE_WriteVariable(12,255);
		EE_WriteVariable(13,255);
		EE_WriteVariable(14,255);
		EE_WriteVariable(15,0);
	}
		

}

/**************************************************************************/
int main(void)
{	
	int32_t ret = 0;
	// MCU Initialization
	//RCC_Configuration();
	SystemInit();
	SystemCoreClockUpdate();
	/* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClock / 10000))
  { 
    /* Capture error */ 
    while (1);
  }
	//Thu vien tang thap
	GPIO_config();
	SPI1_Init();
	USART1_Init();
	//printmcuclk();
	Timer_Configuration();
	//using stm32's flash to store data
	//EEPROM STM32 init
	sw_eeprom_stm32();
	test_eeprom();
	delay_ms(1);
	//Thu vien tang cao
	w5500_lib_init();
	if(!disk_initialize(0)) printf("SDcard initialize OK\n");
	
	printf("Run, now is %s\r\n",ctime(&timenow));

	/* Try to mount card */
	if (f_mount(&FS, "\0", 1) == FR_OK) {//f_mount will call disk_initialize
		TM_FATFS_GetDriveSize("\0", &CardSize);
		/* Unmount SDCARD */
		//f_mount(NULL, "\0", 1);
	}
	//Get time from ntp time server
	SNTP_init();
	//SNTP_init(SOCK_SNTP,sntp_ip,11,sntp_buf);	
	
	ntpserverdefaultconfig();
	/* SNMP(Simple Network Management Protocol) Agent Initialize */
	// NMS (SNMP manager) IP address
	snmpd_init(managerIP,agentIP,SOCK_agent,SOCK_trap);	
	loadwebpages();
	// IWDG Initialization: STM32 Independent WatchDog
	IWDG_Config();
	//timeinfo = localtime( &timenow );
	//printf("Current local time and date: %s\r\n", asctime(timeinfo));
	while(1)
	{
		IWDG_ReloadCounter(); // Feed IWDG
		networkSevices();
		/**********************************************************************/

		/**********************************************************************/
		
		if(sec_cnt > 1)
		{
			sec_cnt = 0;
			//timeinfo = localtime( &timenow );
			//printf("sec_cnt :%d, ms10k : %d, timenow :%d\r\n",sec_cnt,ms10k,timenow);
		/*if( (ret = SNTP_run(&sntp)) == 0) {
			printf("SNTP ERR : %d\r\n", ret);
		}*/
		SNTP_run();
		
		}
		
		

	}

		
}//end of main

void networkSevices()
{
	int32_t ret = 0;	
	checklink();//Kiem tra day mang con cam ko
	// NTP UDP server chay dau tien cho nhanh
	if( (ret = NTPUDP(SOCK_UDPS)) < 0) {
			printf("SOCKET ERROR : %d\r\n", ret);
	}
	{	//SNMPv1 run
			//Run SNMP Agent Fucntion
			/* SNMP Agent Handler */
			//SMI Network Management Private Enterprise Codes: : moi cong ty phai dang ky 1 so rieng, 
			//tham khao : https://www.iana.org/assignments/enterprise-numbers/enterprise-numbers
			// Vi du Arduino : 36582
    	// SNMP Agent daemon process : User can add the OID and OID mapped functions to snmpData[] array in snmprun.c/.h
			// [net-snmp version 5.7 package for windows] is used for this demo.
			// * Command example
    	// [Command] Get:			  snmpget -v 1 -c public 192.168.1.246 .1.3.6.1.2.1.1.1.0 			// (sysDescr)
    	// [Command] Get: 			snmpget -v 1 -c public 192.168.1.246 .1.3.6.1.4.1.6.1.0 			// (Custom, get LED status)
    	// [Command] Get-Next: 	snmpwalk -v 1 -c public 192.168.1.246 .1.3.6.1
			// [Command] Set: 			snmpset -v 1 -c public 192.168.1.246 .1.3.6.1.4.1.6.2.0 i 1			// (Custom, LED 'On')
    	// [Command] Set: 			snmpset -v 1 -c public 192.168.1.246 .1.3.6.1.4.1.6.2.0 i 0			// (Custom, LED 'Off')
			snmpd_run();	
	}
	{	// web server 	
			httpServer_run(0);
			httpServer_run(1);
			httpServer_run(2);
		}
	//SNTP_sevice();
}

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


void SysTick_Handler(void)
{
  ms10k++;
	if(ms10k > 9999) ms10k = 0; 
}



//Interrupt line 3 PA3 responds to data from W5500 and concatenates a flag.
void EXTI3_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line3) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line3);	//Clear interrupt line
		micros_recv = 100*ms10k;
		recvTime = (timenow + STARTOFTIME);//gio luc nhan dc ban tin	
		//W5500RecInt=1;
		//printf("EXTI3_IRQHandler\r\n");
		//ntpserverprocess();
	}
}

void loadwebpages()
{
		//Lien quan den webserver
		//reg_httpServer_cbfunc(NVIC_SystemReset, NULL); 
		reg_httpServer_cbfunc(NVIC_SystemReset, IWDG_ReloadCounter); // Callback: STM32 MCU Reset / WDT Reset (IWDG)
		/* HTTP Server Initialization  */
		httpServer_init(TX_BUF, RX_BUF, MAX_HTTPSOCK, socknumlist);
		reg_httpServer_webContent((uint8_t *)"index.html", (uint8_t *)index_page);				// index.html 		: Main page example
		reg_httpServer_webContent((uint8_t *)"netinfo.html", (uint8_t *)netinfo_page);			// netinfo.html 	: Network information example page
		reg_httpServer_webContent((uint8_t *)"netinfo.js", (uint8_t *)wiz550web_netinfo_js);	// netinfo.js 		: JavaScript for Read Network configuration 	(+ ajax.js)
		reg_httpServer_webContent((uint8_t *)"img.html", (uint8_t *)img_page);					// img.html 		: Base64 Image data example page

		// Example #1
		reg_httpServer_webContent((uint8_t *)"dio.html", (uint8_t *)dio_page);					// dio.html 		: Digital I/O control example page
		reg_httpServer_webContent((uint8_t *)"dio.js", (uint8_t *)wiz550web_dio_js);			// dio.js 			: JavaScript for digital I/O control 	(+ ajax.js)

		// Example #2
		reg_httpServer_webContent((uint8_t *)"ain.html", (uint8_t *)ain_page);					// ain.html 		: Analog input monitor example page
		reg_httpServer_webContent((uint8_t *)"ain.js", (uint8_t *)wiz550web_ain_js);			// ain.js 			: JavaScript for Analog input monitor	(+ ajax.js)

		// Example #3
		reg_httpServer_webContent((uint8_t *)"ain_gauge.html", (uint8_t *)ain_gauge_page);		// ain_gauge.html 	: Analog input monitor example page; using Google Gauge chart
		reg_httpServer_webContent((uint8_t *)"ain_gauge.js", (uint8_t *)ain_gauge_js);			// ain_gauge.js 	: JavaScript for Google Gauge chart		(+ ajax.js)

		// AJAX JavaScript functions
		reg_httpServer_webContent((uint8_t *)"ajax.js", (uint8_t *)wiz550web_ajax_js);			// ajax.js			: JavaScript for AJAX request transfer
		
		//favicon.ico
		reg_httpServer_webContent((uint8_t *)"favicon.ico", (uint8_t *)pageico);			// favicon.ico
		//config page
		reg_httpServer_webContent((uint8_t *)"config.html", (uint8_t *)configpage);			// config.html
		display_reg_webContent_list();
}