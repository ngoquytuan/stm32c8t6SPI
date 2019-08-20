/*
 * sntp.c
 *
 *  Created on: 2014. 12. 15.
 *      Author: Administrator
 */


#include <string.h>
#include <stdio.h>
#include <time.h>
#include "sntp.h"
#include "socket.h"
#include "dns.h"
#include "w5500init.h"

extern time_t timenow;
uint8_t TimeIsSet = 0;
uint16_t RetrySend = 0; //60 giay
uint16_t sycnPeriod = 0;// 1 gio

uint8_t Domain_ntpTimeServer[] = "0.asia.pool.ntp.org";    // for Example domain name
//uint8_t Domain_IP[4]  = {0, };               // Translated IP address by DNS
#define DNS_BUF_SIZE   200
uint8_t ntpTimeServer_buf[DNS_BUF_SIZE];

uint8_t ntpTimeServer_ip[4] ={139, 199, 215, 251};// NTP time server
//uint8_t ntpTimeServer_ip[4] ={192, 168, 1, 6};// NTP time server
//uint8_t ntpTimeServer_buf[56];
uint8_t ntpmessage[48]={0};
//uint8_t ntpServerRespond[56];
// Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
const uint32_t seventyYears = 2208988800;

/*
00)UTC-12:00 Baker Island, Howland Island (both uninhabited)
01) UTC-11:00 American Samoa, Samoa
02) UTC-10:00 (Summer)French Polynesia (most), United States (Aleutian Islands, Hawaii)
03) UTC-09:30 Marquesas Islands
04) UTC-09:00 Gambier Islands;(Summer)United States (most of Alaska)
05) UTC-08:00 (Summer)Canada (most of British Columbia), Mexico (Baja California)
06) UTC-08:00 United States (California, most of Nevada, most of Oregon, Washington (state))
07) UTC-07:00 Mexico (Sonora), United States (Arizona); (Summer)Canada (Alberta)
08) UTC-07:00 Mexico (Chihuahua), United States (Colorado)
09) UTC-06:00 Costa Rica, El Salvador, Ecuador (Galapagos Islands), Guatemala, Honduras
10) UTC-06:00 Mexico (most), Nicaragua;(Summer)Canada (Manitoba, Saskatchewan), United States (Illinois, most of Texas)
11) UTC-05:00 Colombia, Cuba, Ecuador (continental), Haiti, Jamaica, Panama, Peru
12) UTC-05:00 (Summer)Canada (most of Ontario, most of Quebec)
13) UTC-05:00 United States (most of Florida, Georgia, Massachusetts, most of Michigan, New York, North Carolina, Ohio, Washington D.C.)
14) UTC-04:30 Venezuela
15) UTC-04:00 Bolivia, Brazil (Amazonas), Chile (continental), Dominican Republic, Canada (Nova Scotia), Paraguay,
16) UTC-04:00 Puerto Rico, Trinidad and Tobago
17) UTC-03:30 Canada (Newfoundland)
18) UTC-03:00 Argentina; (Summer) Brazil (Brasilia, Rio de Janeiro, Sao Paulo), most of Greenland, Uruguay
19) UTC-02:00 Brazil (Fernando de Noronha), South Georgia and the South Sandwich Islands
20) UTC-01:00 Portugal (Azores), Cape Verde
21) UTC&#177;00:00 Cote d'Ivoire, Faroe Islands, Ghana, Iceland, Senegal; (Summer) Ireland, Portugal (continental and Madeira)
22) UTC&#177;00:00 Spain (Canary Islands), Morocco, United Kingdom
23) UTC+01:00 Angola, Cameroon, Nigeria, Tunisia; (Summer)Albania, Algeria, Austria, Belgium, Bosnia and Herzegovina,
24) UTC+01:00 Spain (continental), Croatia, Czech Republic, Denmark, Germany, Hungary, Italy, Kinshasa, Kosovo,
25) UTC+01:00 Macedonia, France (metropolitan), the Netherlands, Norway, Poland, Serbia, Slovakia, Slovenia, Sweden, Switzerland
26) UTC+02:00 Libya, Egypt, Malawi, Mozambique, South Africa, Zambia, Zimbabwe, (Summer)Bulgaria, Cyprus, Estonia,
27) UTC+02:00 Finland, Greece, Israel, Jordan, Latvia, Lebanon, Lithuania, Moldova, Palestine, Romania, Syria, Turkey, Ukraine
28) UTC+03:00 Belarus, Djibouti, Eritrea, Ethiopia, Iraq, Kenya, Madagascar, Russia (Kaliningrad Oblast), Saudi Arabia,
29) UTC+03:00 South Sudan, Sudan, Somalia, South Sudan, Tanzania, Uganda, Yemen
30) UTC+03:30 (Summer)Iran
31) UTC+04:00 Armenia, Azerbaijan, Georgia, Mauritius, Oman, Russia (European), Seychelles, United Arab Emirates
32) UTC+04:30 Afghanistan
33) UTC+05:00 Kazakhstan (West), Maldives, Pakistan, Uzbekistan
34) UTC+05:30 India, Sri Lanka
35) UTC+05:45 Nepal
36) UTC+06:00 Kazakhstan (most), Bangladesh, Russia (Ural: Sverdlovsk Oblast, Chelyabinsk Oblast)
37) UTC+06:30 Cocos Islands, Myanmar
38) UTC+07:00 Jakarta, Russia (Novosibirsk Oblast), Thailand, Vietnam
39) UTC+08:00 China, Hong Kong, Russia (Krasnoyarsk Krai), Malaysia, Philippines, Singapore, Taiwan, most of Mongolia, Western Australia
40) UTC+09:00 Korea, East Timor, Russia (Irkutsk Oblast), Japan
41) UTC+09:30 Australia (Northern Territory);(Summer)Australia (South Australia))
42) UTC+10:00 Russia (Zabaykalsky Krai); (Summer)Australia (New South Wales, Queensland, Tasmania, Victoria)
43) UTC+10:30 Lord Howe Island
44) UTC+11:00 New Caledonia, Russia (Primorsky Krai), Solomon Islands
45) UTC+11:30 Norfolk Island
46) UTC+12:00 Fiji, Russia (Kamchatka Krai);(Summer)New Zealand
47) UTC+12:45 (Summer)New Zealand
48) UTC+13:00 Tonga
49) UTC+14:00 Kiribati (Line Islands)
*/




void SNTP_init()
{
	uint8_t i;
	//ntpformat NTPformat;
	int32_t ret = 0;
	/* DNS client initialization */
   DNS_init(SOCK_SNTP, ntpTimeServer_buf);
	/* DNS procssing */
   if ((ret = DNS_run(gWIZNETINFO.dns, Domain_ntpTimeServer, ntpTimeServer_ip)) > 0) // try to 1st DNS
   {
      printf("> 1st DNS Reponsed\r\n");

   }
	    if(ret > 0)
   {
      printf("> Translated %s to %d.%d.%d.%d\r\n",Domain_ntpTimeServer,ntpTimeServer_ip[0],ntpTimeServer_ip[1],ntpTimeServer_ip[2],ntpTimeServer_ip[3]);
   }//Tim cach luu cai nay lai nhi?
//	NTPformat.dstaddr[0] = ntpTimeServer_ip[0];
//	NTPformat.dstaddr[1] = ntpTimeServer_ip[1];
//	NTPformat.dstaddr[2] = ntpTimeServer_ip[2];
//	NTPformat.dstaddr[3] = ntpTimeServer_ip[3];


//	
//	NTPformat.leap = 0;           /* leap indicator */
//	NTPformat.version = 4;        /* version number */
//	NTPformat.mode = 3;           /* mode */
//	NTPformat.stratum = 0;        /* stratum */
//	NTPformat.poll = 0;           /* poll interval */
//	NTPformat.precision = 0;      /* precision */
//	NTPformat.rootdelay = 0;      /* root delay */
//	NTPformat.rootdisp = 0;       /* root dispersion */
//	NTPformat.refid = 0;          /* reference ID */
//	NTPformat.reftime = 0;        /* reference time */
//	NTPformat.org = 0;            /* origin timestamp */
//	NTPformat.rec = 0;            /* receive timestamp */
//	NTPformat.xmt = 1;            /* transmit timestamp */

//	Flag = (NTPformat.leap<<6)+(NTPformat.version<<3)+NTPformat.mode; //one byte Flag
//	memcpy(ntpmessage,(void const*)(&Flag),1);
	/*
	printf("NTP server: %d:%d:%d:%d my message :",NTPformat.dstaddr[0],NTPformat.dstaddr[1],NTPformat.dstaddr[2],NTPformat.dstaddr[3]);
	for(i=0;i<48;i++)
	{
		printf("%d ",*(ntpmessage+i));
	
	}*/
	// Initialize values needed to form NTP request
	  // (see URL above for details on the packets)
	  ntpmessage[0] = 0b11100011;   // LI, Version, Mode
	  ntpmessage[1] = 0;     // Stratum, or type of clock
	  ntpmessage[2] = 6;     // Polling Interval
	  ntpmessage[3] = 0xEC;  // Peer Clock Precision
	  // 8 bytes of zero for Root Delay & Root Dispersion
	  ntpmessage[12]  = 49;
	  ntpmessage[13]  = 0x4E;
	  ntpmessage[14]  = 49;
	  ntpmessage[15]  = 52;
}


int8_t SNTP_run()//datetime sntp;
{
	uint32_t ret;
	uint16_t size;
	uint32_t destip = 0;
	uint16_t destport;
	//uint16_t startindex = 40; //last 8-byte of data_buf[size is 48 byte] is xmt, so the startindex should be 40
	int8_t i;
	uint32_t sec;
	if (sycnPeriod >= 160) // dong bo lai thoi gian
	{
		TimeIsSet = 0;
		sycnPeriod = 0;
	}
	if(TimeIsSet == 1) return 1;
	
	switch(getSn_SR(SOCK_SNTP))
	{
	case SOCK_UDP:
		if ((size = getSn_RX_RSR(SOCK_SNTP)) > 0)
		{
			//printf("\r\nsize:%d, ret:%d, NTP: ",size,ret);
			if (size > 56) size = 56;	// if Rx data size is lager than TX_RX_MAX_BUF_SIZE
			recvfrom(SOCK_SNTP, ntpTimeServer_buf, size, (uint8_t *)&destip, &destport);
			//Mot ban tin tu NTP Time Server
			//24 3 6 e8 0 0 2c 3c 0 0 e 7d 8e 93 5c 5 e1 6 3a 76 77 3a 48 cf 0 0 0 0 0 0 0 0 e1 6 3e 9d 25 4f 82 99 e1 6 3e 9d 25 52 19 13
//			for(i=0;i<48;i++)
//						{
//						   printf("%x ",*(ntpTimeServer_buf+i));
//						}
			sec = (ntpTimeServer_buf[40]<<24) + (ntpTimeServer_buf[41]<<16) + (ntpTimeServer_buf[42]<<8) + ntpTimeServer_buf[43] ;
			//printf("Seconds: %u\r\n",sec-seventyYears);
			timenow = sec-seventyYears;
			
			printf("\r\nSeconds: %u\r\n",timenow);
			TimeIsSet = 1;
			close(SOCK_SNTP);

			return 1;
		}
				if(TimeIsSet == 0) //chua chinh gio
			{
				if(RetrySend > 5) //Try Again gui ban tin hoi gio
				{
					RetrySend = 0;
					sendto(SOCK_SNTP,ntpmessage,48,ntpTimeServer_ip,123);
					//sendto(SOCK_SNTP,ntpmessage,48,ntpTimeServer_ip,123);
					/*
					printf("Gui ban tin di :");//35 0 6 236 0 0 0 0 0 0 0 0 49 78 49 52 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
					for(i=0;i<48;i++)
					{
						printf("%d ",*(ntpmessage+i));
					
					}
					printf("\r\n");*/
				}
				return 0;
			}
			
			
		
		break;
	case SOCK_CLOSED:
	    //printf("%d:SNTP client try to start at port %d\r\n",SOCK_SNTP,sntp_port);
		//socket(NTP_SOCKET,Sn_MR_UDP,sntp_port,0);
		if((ret=socket(SOCK_SNTP,Sn_MR_UDP,sntp_port,0x00)) != SOCK_SNTP)
            return ret;
		//printf("%d:Opened, port [%d]\r\n",SOCK_SNTP, sntp_port);
		printf(" Socket[%d] UDP Socket for SNTP client started at port [%d]\r\n", SOCK_SNTP, sntp_port);
		break;
	}
	// Return value
	// 0 - failed / 1 - success
	return 0;
}


