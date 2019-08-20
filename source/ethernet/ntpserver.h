#include <stdio.h>
#include <time.h>
#include <string.h>
#include "GPIO_STM32F10x.h"             // Keil::Device:GPIO
#include "w5500.h"
#include "socket.h"
#include "socketdefines.h"


/* Shifts usecs in unixToNtpTime */
//??? ko hieu nhung dung! 
#ifndef USECSHIFT
#define USECSHIFT (1LL << 32) * 1.0e-6
#endif
//Number of seconds from 1st January 1900 to start of Unix epoch
//According to the Time protocol in RFC 868 it is 2208988800L.
#define STARTOFTIME 2208988800UL

#ifndef UTIL_H
#define UTIL_H

#define htons(x) ( ((x)<< 8 & 0xFF00) | \
                   ((x)>> 8 & 0x00FF) )
#define ntohs(x) htons(x)

#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
                   ((x)<< 8 & 0x00FF0000UL) | \
                   ((x)>> 8 & 0x0000FF00UL) | \
                   ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)
#endif

extern time_t unixTime_last_sync;// lan chuan gio gan nhat 
/* SNTP Packet array */
extern uint8_t serverPacket[NTP_PACKET_SIZE] ;
extern uint8_t clientPacket[NTP_PACKET_RAWSIZE] ;
extern time_t micros_recv ;
extern time_t micros_offset;
extern time_t transmitTime;
extern time_t micros_transmit;
extern time_t recvTime;

int32_t NTPUDP(uint8_t sn);
void wzn_event_handle(void);
void ntpserverdefaultconfig(void);
int32_t ntpserverprocess(void);