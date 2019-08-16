#include "ntpserver.h"
extern volatile uint32_t ms10k;
extern time_t timenow;
//for NTP server
time_t unixTime_last_sync = 1559640302;// lan chuan gio gan nhat 
/* SNTP Packet array */
uint8_t serverPacket[NTP_PACKET_SIZE] = {0};
uint8_t clientPacket[NTP_PACKET_RAWSIZE] = {0};
time_t micros_recv = 0;
time_t micros_offset;
time_t transmitTime;
time_t micros_transmit;
time_t recvTime;


int32_t NTPUDP(uint8_t sn)
{
   int32_t  ret;
   uint16_t size, sentsize=0;
   uint8_t  destip[4];
   uint16_t destport;
	 uint8_t i;
	// Ban tin NTP co size = 56 ( ca header : IP[4],port[2],length[2], tru di header chi con 48
   switch(getSn_SR(sn))
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(sn)) == NTP_PACKET_RAWSIZE)//56 is size raw of NTP message
         {
					 //printf("\r\nsize:%d, ret:%d, NTP: ",size,ret); 
					 ret = recvfrom(sn,clientPacket,size,destip,(uint16_t*)&destport);
					//if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;//56 là max voi ban tin NTP
					 //if(size != NTP_PACKET_RAWSIZE) return 0;// ban tin NTP raw size phai la 56
            
					  
            if(ret <= 0)
            {
               printf("%d: recvfrom error. %d\r\n",sn,ret);
               return ret;
            }
					{
						//in ra ban tin
						/*
						for(i=0;i<48;i++)
						{
						   printf("%x ",*(buf+i));
						}
						*/
						//Tao ban tin NTP
						//ntpserverdefaultconfig();
						/*
						serverPacket[0] = 0x24;   // LI, Version, Mode // Set version number and mode
						serverPacket[1] = 1; // Stratum, or type of clock
						serverPacket[2] = 0;     // Polling Interval
						serverPacket[3] = -12;  // Peer Clock Precision
						serverPacket[12] = 'G';
						serverPacket[13] = 'P';
						serverPacket[14] = 'S';
						//[Reference Timestamp]: unsigned 32-bit seconds value : Lan lay chuan gan nhat la bao gio
						memcpy(&serverPacket[16], &unixTime_last_sync, 4);
						*/
					}
						//Transmit Timestamp, T3 from client, copy and return! 
						memcpy(&serverPacket[24], &clientPacket[40], 4);
						memcpy(&serverPacket[28], &clientPacket[44], 4);
						
						//Thoi gian nhan dc ban tin
						//printf("recvTime: %u, micros_recv:%u\r\n",recvTime,micros_recv);
						recvTime = htonl(recvTime);
						memcpy(&serverPacket[32], &recvTime, 4);
						//phan thap phan
						micros_recv = (micros_recv + 1) * USECSHIFT;
						micros_recv = htonl(micros_recv);
						memcpy(&serverPacket[36], &micros_recv, 4);
						
						transmitTime = (timenow + STARTOFTIME);//gio luc tryen ban tin
						//micros_transmit = 100*ms10k;
					  //micros_transmit = 100*(TIM3->CNT);
						//printf("tranTime: %u, micros_tran:%u\r\n",transmitTime,micros_transmit);
						
						transmitTime = htonl(transmitTime);// gio luc truyen
						memcpy(&serverPacket[40], &transmitTime, 4);
						
						
						//Tinh toan phan thap phan cua thoi diem truyen tin
						//micros_transmit = 100*ms10k;
						micros_transmit = 100*(TIM3->CNT);
						micros_transmit = (micros_transmit + 1) * USECSHIFT;
						micros_transmit = htonl(micros_transmit);//Ko hieu lam gi nhi, nhung dung!
						memcpy(&serverPacket[44], &micros_transmit, 4);
						//Gui tra ban tin NTP
						while(sentsize != NTP_PACKET_SIZE)
            {
               ret = sendto(sn,serverPacket,NTP_PACKET_SIZE,destip,destport);
               if(ret < 0)
               {
                  printf("%d: sendto error. %d\r\n",sn,ret);
                  return ret;
               }
               sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
						wzn_event_handle();
						//printf("\r\nNTPsent \r\n");
         }
         break;
      case SOCK_CLOSED:
				 
         if((ret=socket(sn,Sn_MR_UDP,NTP_PORT,0x00)) != sn)
            return ret;
         printf(" Socket[%d] UDP Socket for NTP Time Server started, port [%d]\r\n", sn, NTP_PORT);
         break;
      default :
         break;
   }
   return 1;
}


/********************************************************************************************************************/
void ntpserverdefaultconfig(void)
{
		
		//Phan co dinh cua ban tin NTP
		serverPacket[0] = 0x24;   // LI, Version, Mode // Set version number and mode
		serverPacket[1] = 1; // Stratum, or type of clock
		serverPacket[2] = 0;     // Polling Interval
		serverPacket[3] = -12;  // Peer Clock Precision
		serverPacket[12] = 'G';
		serverPacket[13] = 'P';
		serverPacket[14] = 'S';
		//[Reference Timestamp]: unsigned 32-bit seconds value : Lan lay chuan gan nhat la bao gio
		memcpy(&serverPacket[16], &unixTime_last_sync, 4);
}
/********************************************************************************************************************/
void wzn_event_handle(void)
{
	uint16_t ir = 0;
	uint8_t sir = 0;
	uint16_t len = 0;
	static const int8_t WZN_ERR = -1;
	
	
	if (ctlwizchip(CW_GET_INTERRUPT, &ir) == WZN_ERR) {
		printf("Cannot get ir...");
	}
	
	if (ir & IK_SOCK_0) {
		sir = getSn_IR(SOCK_UDPS);
		
	
		//printf("IK_SOCK_1");
		
		if ((sir & Sn_IR_SENDOK) > 0) {
			/* Clear Sn_IR_SENDOK flag. */
			setSn_IR(SOCK_UDPS, Sn_IR_SENDOK);
			printf("app_sent();\r\n");
			//app_sent();
		}
		
		if ((sir & Sn_IR_RECV) > 0) {
			//len = getSn_RX_RSR(SOCK_UDPS);
			//printf("UDP packet received!\r\n");
			//printf("recvTime: %u, micros_recv:%u\r\n",recvTime,micros_recv);
			//transmitTime = (timenow + STARTOFTIME);//gio luc tryen ban tin
			//micros_transmit = getns();
			//printf("transmitTime: %u, micros_transmit:%u\r\n",transmitTime,micros_transmit);
			/* Clear Sn_IR_RECV flag. */
			setSn_IR(SOCK_UDPS, Sn_IR_RECV);
			
		}
	}
}