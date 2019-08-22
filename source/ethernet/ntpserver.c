#include "ntpserver.h"

extern time_t timenow;
//for NTP server
time_t unixTime_last_sync;// = 1564500000;// lan chuan gio gan nhat 1564551507
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
						
						/*for(i=0;i<48;i++)
						{
						   printf("%x ",*(clientPacket+i));
						}*/
						
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

						
					serverPacket[0]=0x24;	// Leap 0x0, Version 0x3, Mode 0x4
				  serverPacket[1]=0x03;	// Stratum 0x1, stratum (GPS)
				  serverPacket[2]=0x06;	// polling minimum (64 seconds - default)
				  //  serverPacket[3] = 0xFA; // precision (reference sketch - ~15 milliseconds)
					//serverPacket[3] = 0xF7; // precision (2^-9 ~2 milliseconds)
					serverPacket[3] = 0xE8; 
					//  serverPacket[3] = 0x09; // precision (2^9 Testing)
					// root delay
					serverPacket[4]=0x0;
					serverPacket[5]=0x0;
					serverPacket[6]=0x2C;
					serverPacket[7]=0x3C;
					
					serverPacket[8]=0x0;
					serverPacket[9]=0x0;
					serverPacket[10]=0x0E;
					serverPacket[11] = 0x7D; // root dispersion
					
					serverPacket[12]=0x47;	// Reference ID, "G"
					serverPacket[13]=0x50;	// Reference ID, "P"
					serverPacket[14]=0x53;	// Reference ID, "S"
					serverPacket[15]=0x00;	// Reference ID, 0x00

						*/
					}
					
					unixTime_last_sync = (timenow + STARTOFTIME);//gio luc tryen ban tin
					unixTime_last_sync = htonl(unixTime_last_sync);// gio luc truyen
					memcpy(&serverPacket[16], &unixTime_last_sync, 4);
					

					
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
	//ex: Reference Timestamp : e1 6 3a 76 77 3a 48 cf
	/*				
	serverPacket[16]=0xE1;
	serverPacket[17]=0x06;
	serverPacket[18]=0x3A;
	serverPacket[19]=0x76;
	//Phan le				
	serverPacket[20]=0;
	serverPacket[21]=0;
	serverPacket[22]=0;
	serverPacket[23]=0;
					*/
					
	unixTime_last_sync = (timenow + STARTOFTIME);//gio luc tryen ban tin
	unixTime_last_sync = htonl(unixTime_last_sync);// gio luc truyen
	memcpy(&serverPacket[16], &unixTime_last_sync, 4);
}
/********************************************************************************************************************/
void wzn_event_handle(void)
{
	uint16_t ir = 0;
	uint8_t sir = 0;
	
	
	if (ctlwizchip(CW_GET_INTERRUPT, &ir) == WZN_ERR) {
		printf("Cannot get ir...");
		return;
	}
	
	if (ir & IK_SOCK_0) 
		{
				sir = getSn_IR(SOCK_UDPS);
				
				if ((sir & Sn_IR_RECV) > 0) {
					/* Clear Sn_IR_RECV flag. */
					setSn_IR(SOCK_UDPS, Sn_IR_RECV);
					return;
				}
				else if ((sir & Sn_IR_SENDOK) > 0) {
					/* Clear Sn_IR_SENDOK flag. */
					setSn_IR(SOCK_UDPS, Sn_IR_SENDOK);
					printf("app_sent();\r\n");
					//app_sent();
				}
		}
}

//Interrupt line 3 PA3 responds to data from W5500 and concatenates a flag.
void EXTI3_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line3) != RESET)
	{
		micros_recv = 100*(TIM3->CNT);
		EXTI_ClearITPendingBit(EXTI_Line3);	//Clear interrupt line
		//micros_recv = 100*ms10k;
		
		recvTime = (timenow + STARTOFTIME);//gio luc nhan dc ban tin
		//Neu goi cai nay thi loi????
		//NTPUDP(SOCK_UDPS);		
		//printf("EXTI3_IRQHandler\r\n");


	}
}