/**
 * @file	httpUtil.c
 * @brief	HTTP Server Utilities	
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */
#include "stm32f10x.h"                  // Device header
#include "wizchip_conf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "httpUtil.h"
#include "ConfigData.h"
#include "eeprom_stm.h"//save IP
/************************************************************************************************/
#define DIOn			16
#define AINn			7 	// AIN for PTM/Temp.Sensor + A0 ~ A5
// Pre-defined Get CGI functions
void make_json_dio(uint8_t * buf, uint16_t * len, uint8_t pin);
void make_json_ain(uint8_t * buf, uint16_t * len, uint8_t pin);
void make_json_netinfo(uint8_t * buf, uint16_t * len);

// Pre-defined Set CGI functions
int8_t set_diodir(uint8_t * uri);
int8_t set_diostate(uint8_t * uri);
int8_t todo(uint8_t * uri);
// Data IO Direction
typedef enum
{
	NotUsed	= 0,
	Input	= 1,
	Output	= 2
} IO_Direction_Type;

// Data IO Status
typedef enum
{
	Off	= 0,
	On 	= 1
} IO_Status_Type;

uint8_t new_device_ip[4];
/************************************************************************************************/
uint8_t predefined_get_cgi_processor(uint8_t * uri_name, uint8_t * buf, uint16_t * len)
{
		uint8_t ret = 1;	// ret = 1 means 'uri_name' matched
	uint8_t cgibuf[14] = {0, };
	int8_t cgi_dio = -1;
	int8_t cgi_ain = -1;

	uint8_t i;

	if(strcmp((const char *)uri_name, "todo.cgi") == 0)
	{
		// to do
		;//make_json_todo(buf, len);
	}
	else if(strcmp((const char *)uri_name, "get_netinfo.cgi") == 0)
	{
		make_json_netinfo(buf, len);
	}
	else
	{
		// get_dio0.cgi ~ get_dio15.cgi
		for(i = 0; i < DIOn; i++)
		{
			memset(cgibuf, 0x00, 14);
			sprintf((char *)cgibuf, "get_dio%d.cgi", i);
			if(strcmp((const char *)uri_name, (const char *)cgibuf) == 0)
			{
				make_json_dio(buf, len, i);
				cgi_dio = i;
				break;
			}
		}

		if(cgi_dio < 0)
		{
			// get_ain0.cgi ~ get_ain5.cgi (A0 - A5), get_ain6.cgi for on-board potentiometer / Temp.Sensor
			for(i = 0; i < AINn; i++)
			{
				memset(cgibuf, 0x00, 14);
				sprintf((char *)cgibuf, "get_ain%d.cgi", i);
				if(strcmp((const char *)uri_name, (const char *)cgibuf) == 0)
				{
					make_json_ain(buf, len, i);
					cgi_ain = i;
					break;
				}
			}
		}

		if((cgi_dio < 0) && (cgi_ain < 0)) ret = 0;
	}

	return ret;
}
/************************************************************************************************/
uint8_t predefined_set_cgi_processor(uint8_t * uri_name, uint8_t * uri, uint8_t * buf, uint16_t * len)
{
	uint8_t ret = 1;	// ret = '1' means 'uri_name' matched
	uint8_t val = 0;

	if(strcmp((const char *)uri_name, "set_diodir.cgi") == 0)
	{
		val = set_diodir(uri);
		*len = sprintf((char *)buf, "%d", val);
	}
	else if(strcmp((const char *)uri_name, "set_diostate.cgi") == 0)
	{
		val = set_diostate(uri);
		*len = sprintf((char *)buf, "%d", val);
	}
	else
	{
		ret = 0;
	}

	return ret;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void make_json_serial_data(uint8_t * buf, uint16_t * len)
{
	uint8_t * buf_ptr;
	//buf_ptr = getUSART1buf();
	*len = sprintf((char *)buf,"getRs232DataCallback({\"data\":\"%s\"});", buf_ptr); // serial buffer

	// Serial data buffer clear
	//USART1_flush();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t http_get_cgi_handler(uint8_t * uri_name, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_OK;
	uint16_t len = 0;

	if(predefined_get_cgi_processor(uri_name, buf, &len))
	{
		;
	}
	else if(strcmp((const char *)uri_name, "example.cgi") == 0)
	{
		// To do
		;
	}
	else if(strcmp((const char *)uri_name, "get_serial_data.cgi") == 0)
	{
		make_json_serial_data(buf, &len);
	}
	else
	{
		// CGI file not found
		ret = HTTP_FAILED;
	}

	if(ret)	*file_len = len;
	return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t * set_basic_config_setting(uint8_t * uri)
{
	uint8_t * param;
	uint8_t str_size, baudrate_idx;
	S2E_Packet *value = get_S2E_Packet_pointer();

	// Redirection IP address init
	uint8_t return_ip[4] = {value->network_info_common.local_ip[0],
							value->network_info_common.local_ip[1],
							value->network_info_common.local_ip[2],
							value->network_info_common.local_ip[3]};

	uint8_t * ip = return_ip;
  
	//boc tach IP
		if((param = get_http_param_value((char *)uri, "ip")))
		{
			inet_addr_((uint8_t*)param, value->network_info_common.local_ip);

			return_ip[0] = value->network_info_common.local_ip[0];
			return_ip[1] = value->network_info_common.local_ip[1];
			return_ip[2] = value->network_info_common.local_ip[2];
			return_ip[3] = value->network_info_common.local_ip[3];
			//printf("IP: %d.%d.%d.%d\r\n",return_ip[0],return_ip[1],return_ip[2],return_ip[3]);
		}
		//boc tach Getway
		if((param = get_http_param_value((char *)uri, "gw")))
		{
			inet_addr_((uint8_t*)param, value->network_info_common.gateway);
			//printf("Getway: %d.%d.%d.%d\r\n",value->network_info_common.gateway[0],value->network_info_common.gateway[1],value->network_info_common.gateway[2],value->network_info_common.gateway[3]);
		}
		//boc tach Subnet
		if((param = get_http_param_value((char *)uri, "sn")))
		{
			inet_addr_((uint8_t*)param, value->network_info_common.subnet);
			//printf("Subnet: %d.%d.%d.%d\r\n",value->network_info_common.subnet[0],value->network_info_common.subnet[1],value->network_info_common.subnet[2],value->network_info_common.subnet[3]);
		}
		

	//save_S2E_Packet_to_storage();

	return ip;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void make_cgi_basic_config_response_page(uint16_t delay, uint8_t * url, uint8_t * cgi_response_buf, uint16_t * len)
{
	//S2E_Packet *value = get_S2E_Packet_pointer();

	//printf("Open IP: %d.%d.%d.%d\r\n",value->network_info_common.local_ip[0],value->network_info_common.local_ip[1],value->network_info_common.local_ip[2],value->network_info_common.local_ip[3]);
	//printf("Open gateway: %d.%d.%d.%d\r\n",value->network_info_common.gateway[0],value->network_info_common.gateway[1],value->network_info_common.gateway[2],value->network_info_common.gateway[3]);
	//printf("Open subnet: %d.%d.%d.%d\r\n",value->network_info_common.subnet[0],value->network_info_common.subnet[1],value->network_info_common.subnet[2],value->network_info_common.subnet[3]);
	
	*len = sprintf((char *)cgi_response_buf,"<html><head><title>WIZ550web - Configuration</title><script language=javascript>j=%d;function func(){document.getElementById('delay').innerText=' '+j+' ';if(j>0)j--;setTimeout('func()',1000);if(j<=0)location.href='http://%d.%d.%d.%d';}</script></head><body onload='func()'>Please wait for a while, the module will boot in<span style='color:red;' id='delay'></span> seconds.</body></html>", delay, url[0], url[1], url[2], url[3]);
	//printf("\r\n%s\r\n",cgi_response_buf);
	return;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void saveip(){
		S2E_Packet *value = get_S2E_Packet_pointer();
		
		printf("Save new eeprom data\r\n");
		EE_WriteVariable(0,123);
		EE_WriteVariable(1,456);
		EE_WriteVariable(2,789);
		//IP 192.168.1.246
		EE_WriteVariable(4,value->network_info_common.local_ip[0]);
		EE_WriteVariable(5,value->network_info_common.local_ip[1]);
		EE_WriteVariable(6,value->network_info_common.local_ip[2]);
		EE_WriteVariable(7,value->network_info_common.local_ip[3]);
		//GW: 192.168.1.1
		EE_WriteVariable(8, value->network_info_common.gateway[0]);
		EE_WriteVariable(9, value->network_info_common.gateway[1]);
		EE_WriteVariable(10,value->network_info_common.gateway[2]);
		EE_WriteVariable(11,value->network_info_common.gateway[3]);
		//SN 255.255.255.0
		EE_WriteVariable(12,value->network_info_common.subnet[0]);
		EE_WriteVariable(13,value->network_info_common.subnet[1]);
		EE_WriteVariable(14,value->network_info_common.subnet[2]);
		EE_WriteVariable(15,value->network_info_common.subnet[3]);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t http_post_cgi_handler(uint8_t * uri_name, st_http_request * p_http_request, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_OK;
	uint16_t len = 0;
	uint8_t val = 0;
	uint8_t* newip;

	if(predefined_set_cgi_processor(uri_name, p_http_request->URI, buf, &len))
	{
		;
	}
	else if(strcmp((const char *)uri_name, "config.cgi") == 0)
	{
		
		newip		= set_basic_config_setting(p_http_request->URI);
		new_device_ip[0]= newip[0];
		new_device_ip[1]= newip[1];
		new_device_ip[2]= newip[2];
		new_device_ip[3]= newip[3];
		saveip();
		//printf("new IP: %d.%d.%d.%d\r\n",new_device_ip[0],new_device_ip[1],new_device_ip[2],new_device_ip[3]);
		make_cgi_basic_config_response_page(5, new_device_ip, buf, &len);
		ret = HTTP_RESET;
	}
	else
	{
		// CGI file not found
		ret = HTTP_FAILED;
	}

	if(ret)	*file_len = len;
	return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pre-defined Get CGI functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void make_json_dio(uint8_t * buf, uint16_t * len, uint8_t pin)
{
	uint8_t pin_state 	= 1;//Chip_GPIO_GetPinState(LPC_GPIO, dio_ports[pin], dio_pins[pin]);
	uint8_t pin_dir 	= 2;//Chip_GPIO_GetPinDIR(LPC_GPIO, dio_ports[pin], dio_pins[pin]);

	*len = sprintf((char *)buf, "DioCallback({\"dio_p\":\"%d\",\
											\"dio_s\":\"%d\",\
											\"dio_d\":\"%d\"\
											});",
											pin,					// Digital io pin number
											pin_state,				// Digital io status
											pin_dir					// Digital io directions
											);
}

void make_json_ain(uint8_t * buf, uint16_t * len, uint8_t pin)
{
	*len = sprintf((char *)buf, "AinCallback({\"ain_p\":\"%d\",\
											\"ain_v\":\"%d\"\
											});",
											pin,					// ADC input pin number
											6		// get_ADC_val(pin), ADC input value
											);
}

void make_json_netinfo(uint8_t * buf, uint16_t * len)
{
	wiz_NetInfo netinfo;
	ctlnetwork(CN_GET_NETINFO, (void*) &netinfo);

	// DHCP: 1 - Static, 2 - DHCP
	*len = sprintf((char *)buf, "NetinfoCallback({\"mac\":\"%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\",\
											\"ip\":\"%d.%d.%d.%d\",\
											\"gw\":\"%d.%d.%d.%d\",\
											\"sn\":\"%d.%d.%d.%d\",\
											\"dns\":\"%d.%d.%d.%d\",\
											\"dhcp\":\"%d\"\
											});",
											netinfo.mac[0], netinfo.mac[1], netinfo.mac[2], netinfo.mac[3], netinfo.mac[4], netinfo.mac[5],
											netinfo.ip[0], netinfo.ip[1], netinfo.ip[2], netinfo.ip[3],
											netinfo.gw[0], netinfo.gw[1], netinfo.gw[2], netinfo.gw[3],
											netinfo.sn[0], netinfo.sn[1], netinfo.sn[2], netinfo.sn[3],
											netinfo.dns[0], netinfo.dns[1], netinfo.dns[2], netinfo.dns[3],
											netinfo.dhcp
											);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pre-defined Set CGI functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int8_t set_diodir(uint8_t * uri)
{
	uint8_t * param;
	uint8_t pin = 0, val = 0;

	//printf("uri :%s\r\n",uri);
	
	if((param = get_http_param_value((char *)uri, "pin"))) // GPIO; D0 ~ D15
	{
		pin = (uint8_t)ATOI(param, 10);
		if(pin > 15) return -1;

		if((param = get_http_param_value((char *)uri, "val")))  // Direction; NotUsed/Input/Output
		{
			val = (uint8_t)ATOI(param, 10);
			if(val > Output) val = Output;
		}
	}

	if(val == Input) 		;//Chip_GPIO_SetPinDIRInput(LPC_GPIO, dio_ports[pin], dio_pins[pin]);	// Input
	else 					;//Chip_GPIO_SetPinDIROutput(LPC_GPIO, dio_ports[pin], dio_pins[pin]); // Output

	return pin;
}



int8_t set_diostate(uint8_t * uri)
{
	uint8_t * param;
	uint8_t pin = 0, val = 0;

	//printf("uri :%s\r\n",uri);
	//Toi day trong uri van chua nguyen ban tin
	if((param = get_http_param_value((char *)uri, "pin"))) // GPIO; D0 ~ D15
	{
		pin = (uint8_t)ATOI(param, 10);
		if(pin > 15) return -1;

		if((param = get_http_param_value((char *)uri, "val")))  // State; high(on)/low(off)
		{
			val = (uint8_t)ATOI(param, 10);
			if(val > On) val = On;
		}

		if(val == On) 		;//Chip_GPIO_SetPinState(LPC_GPIO, dio_ports[pin], dio_pins[pin], true); 	// High
		else				;//Chip_GPIO_SetPinState(LPC_GPIO, dio_ports[pin], dio_pins[pin], false);	// Low
	}

	return pin;
}

