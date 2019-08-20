
/********************************************************************************************
 * SNMP : User Customization Part
 *  - OID Registration
 *  - User defined functions for OID related
 *  	+ Integer value, String
 *  	+ I/O control / Chip registers
 *  	+ Network Informations
 *  	+ Etc.
 *
 *********************************************************************************************/
#include "stm32f10x.h"                  // Device header
#include "snmp_custom.h"


#define LED_R 0
#define LED_G 1
#define LED_B 2
void get_LEDStatus_all(void *ptr, uint8_t *len);

void setMyValue(int value)	//snmpset -v 1 -c public 192.168.1.246 .1.3.6.1.4.1.6.1.2 i 123456
{
	printf("Got my value :%d\r\n",value);
}
/*
void getMyValue()						//snmpget -v 1 -c public 192.168.1.246 .1.3.6.1.4.1.6.1.2
{
	printf("getMyValue\r\n");
	//return 301188;
}*/
	
dataEntryType snmpData[] =
{
    // System MIB
	// SysDescr Entry
	{8, {0x2b, 6, 1, 2, 1, 1, 1, 0},
	SNMPDTYPE_OCTET_STRING, 30, {"GPS clock time server"},
	NULL, NULL},

	// SysObjectID Entry
	{8, {0x2b, 6, 1, 2, 1, 1, 2, 0},
	SNMPDTYPE_OBJ_ID, 8, {"\x2b\x06\x01\x02\x01\x01\x02\x00"},
	NULL, NULL},

	// SysUptime Entry
	{8, {0x2b, 6, 1, 2, 1, 1, 3, 0},
	SNMPDTYPE_TIME_TICKS, 0, {""},
	currentUptime, NULL},

	// sysContact Entry
	{8, {0x2b, 6, 1, 2, 1, 1, 4, 0},
	SNMPDTYPE_OCTET_STRING, 30, {"https://attech.com.vn/lien-he-voi-chung-toi/"},
	NULL, NULL},

	// sysName Entry
	{8, {0x2b, 6, 1, 2, 1, 1, 5, 0},
	SNMPDTYPE_OCTET_STRING, 30, {"https://attech.com.vn/"},
	NULL, NULL},

	// Location Entry
	{8, {0x2b, 6, 1, 2, 1, 1, 6, 0},
	SNMPDTYPE_OCTET_STRING, 30, {"ATTECH"},
	NULL, NULL},

	// SysServices
	{8, {0x2b, 6, 1, 2, 1, 1, 7, 0},
	SNMPDTYPE_INTEGER, 4, {""},
	NULL, NULL},
  // Get the WIZnet W5500-EVB LED Status
	{8, {0x2b, 6, 1, 4, 1, 6, 1, 0},
	SNMPDTYPE_OCTET_STRING, 40, {""},
	get_LEDStatus_all, NULL},
	// Get the my Status
	//{0x0a, {0x2b, 0x06, 0x01, 0x04, 0x01, 0x85, 0xb6, 0x38, 0x01, 0x00},
	//SNMPDTYPE_OCTET_STRING, 40, {""},
	//get_LEDStatus_all, NULL},
	// Set the LED_R (RGB LED)
	{8, {0x2b, 6, 1, 4, 1, 6, 1, 1},
	SNMPDTYPE_INTEGER, 4, {""},
	NULL, set_LEDStatus_R},
	// Set the my value 
	{8, {0x2b, 6, 1, 4, 1, 6, 1, 2},
	SNMPDTYPE_INTEGER, 4, {""},
	NULL, setMyValue},
#ifdef _USE_WIZNET_W5500_EVB_
	// Get the WIZnet W5500-EVB LED Status
	{8, {0x2b, 6, 1, 4, 1, 6, 1, 0},
	SNMPDTYPE_OCTET_STRING, 40, {""},
	get_LEDStatus_all, NULL},

	// Set the LED_R (RGB LED)
	{8, {0x2b, 6, 1, 4, 1, 6, 1, 1},
	SNMPDTYPE_INTEGER, 4, {""},
	NULL, set_LEDStatus_R},

	// Set the LED_G (RGB LED)
	{8, {0x2b, 6, 1, 4, 1, 6, 1, 2},
	SNMPDTYPE_INTEGER, 4, {""},
	NULL, set_LEDStatus_G},

	// Set the LED_B (RGB LED)
	{8, {0x2b, 6, 1, 4, 1, 6, 1, 3},
	SNMPDTYPE_INTEGER, 4, {""},
	NULL, set_LEDStatus_B},
#endif

	// OID Test #1 (long-length OID example, 19865)
	{0x0a, {0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0x9b, 0x19, 0x01, 0x00},
	SNMPDTYPE_OCTET_STRING, 30, {"long-length OID Test #1 tuannq"},
	NULL, NULL},
	
	// OID Test #1 (long-length OID example, 19865)
	{0x0a, {0x2b, 0x06, 0x01, 0x04, 0x01, 0x85, 0xb6, 0x38, 0x01, 0x00},
	SNMPDTYPE_OCTET_STRING, 30, {"long-length OID Test #11 tuannq"},
	NULL, NULL},

	// OID Test #2 (long-length OID example, 22210)
	{0x0a, {0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0xad, 0x42, 0x01, 0x00},
	SNMPDTYPE_OCTET_STRING, 35, {"long-length OID Test #2"},
	NULL, NULL},

	// OID Test #2: SysObjectID Entry
	{0x0a, {0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0xad, 0x42, 0x02, 0x00},
	SNMPDTYPE_OBJ_ID, 0x0a, {"\x2b\x06\x01\x04\x01\x81\xad\x42\x02\x00"},
	NULL, NULL},
	
};

const int32_t maxData = (sizeof(snmpData) / sizeof(dataEntryType));

void initTable()
{
	// Example integer value for [OID 1.3.6.1.2.1.1.7.0]
	snmpData[6].u.intval = -15;

}

void get_LEDStatus_all(void *ptr, uint8_t *len)
{
	uint8_t led_status[3] = {0, };

	led_status[LED_R] = 1;
	led_status[LED_G] = 0;
	led_status[LED_B] = 1;

	*len = sprintf((char *)ptr, "GPS 1 [%s] / 2 [%s] ", led_status[LED_R]?"On":"Off", led_status[LED_G]?"On":"Off");
}

void Board_LED_Set(int led, int i)
{
	printf("Board_LED_Set\r\n");
}
void set_LEDStatus_R(int32_t val)
{
	printf("set_LEDStatus_R :%d\r\n",val);
	if(val == 0) 	Board_LED_Set(LED_R, 0);
	else 			Board_LED_Set(LED_R, 1);
}

// W5500-EVB: LED Control ///////////////////////////////////////////////////////////////////////////
#ifdef _USE_WIZNET_W5500_EVB_
void get_LEDStatus_all(void *ptr, uint8_t *len)
{
	uint8_t led_status[3] = {0, };

	//led_status[LED_R] = (uint8_t)Board_LED_Test(LED_R);
	//led_status[LED_G] = (uint8_t)Board_LED_Test(LED_G);
	//led_status[LED_B] = (uint8_t)Board_LED_Test(LED_B);
	led_status[LED_R] = 1;
	led_status[LED_G] = 0;
	led_status[LED_B] = 1;

	*len = sprintf((char *)ptr, "LED R [%s] / G [%s] / B [%s]", led_status[LED_R]?"On":"Off", led_status[LED_G]?"On":"Off", led_status[LED_B]?"On":"Off");
}

void set_LEDStatus_R(int32_t val)
{
	printf("set_LEDStatus_R :%d\r\n",val);
	if(val == 0) 	Board_LED_Set(LED_R, 0);
	else 			Board_LED_Set(LED_R, 1);
}

void set_LEDStatus_G(int32_t val)
{
	printf("set_LEDStatus_G\r\n");
	if(val == 0) 	Board_LED_Set(LED_G, 0);
	else 			Board_LED_Set(LED_G, 1);
}

void set_LEDStatus_B(int32_t val)
{
	printf("set_LEDStatus_B\r\n");
	if(val == 0) 	Board_LED_Set(LED_B, 0);
	else 			Board_LED_Set(LED_B, 1);
}
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////

void initial_Trap(uint8_t * managerIP, uint8_t * agentIP)
{
	// SNMP Trap: WarmStart(1) Trap
	{
		dataEntryType enterprise_oid = {0x0a, {0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0x9b, 0x19, 0x01, 0x00},
												SNMPDTYPE_OBJ_ID, 0x0a, {"\x2b\x06\x01\x04\x01\x81\x9b\x19\x10\x00"},	NULL, NULL};
		// Generic Trap: warmStart COMMUNITY
		snmp_sendTrap(managerIP, agentIP, (void *)COMMUNITY, enterprise_oid, SNMPTRAP_WARMSTART, 0, 0);
	}

}
