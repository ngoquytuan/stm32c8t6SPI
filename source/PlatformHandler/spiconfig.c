/**
  * @brief  Initializes the peripherals used by the W5500 driver.
  * @param  None
  * @retval None
  */
#include "spiconfig.h"

void  wizchip_write(uint8_t wb)
{
	while (  SPI_I2S_GetFlagStatus(SPI1 , SPI_I2S_FLAG_TXE) == RESET );  // sending data Wait
	SPI_I2S_SendData(SPI1 , wb);
	while (  SPI_I2S_GetFlagStatus(SPI1 , SPI_I2S_FLAG_RXNE) == RESET ); // Wait for data
	wb = SPI_I2S_ReceiveData(SPI1);        // Dummy read to generate clock
}

uint8_t wizchip_read()
{
	while (  SPI_I2S_GetFlagStatus(SPI1 , SPI_I2S_FLAG_TXE) == RESET );
	SPI_I2S_SendData(SPI1 , 0xff);     // Dummy write to generate clock
	while (  SPI_I2S_GetFlagStatus(SPI1 , SPI_I2S_FLAG_RXNE) == RESET );
	return (unsigned char)SPI_I2S_ReceiveData(SPI1);
}

/*-----------------------------------------------------------------------*/
/* Transmit/Receive a byte to MMC via SPI  (Platform dependent)          */
/*-----------------------------------------------------------------------*/
uint8_t stm32_spi_rw( uint8_t out )
{
	/* Loop while DR register in not empty */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) { ; }

	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI1, out);

	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) { ; }

	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1);
}

void wizchip_readburst(uint8_t* pBuf, uint16_t len)
{
	stm32_wizchip_dma_transfer(1, pBuf, len);  //FALSE(0) for buff->SPI, TRUE(1) for SPI->buff
}

void  wizchip_writeburst(uint8_t* pBuf, uint16_t len)
{
	stm32_wizchip_dma_transfer(0, pBuf, len);  //FALSE(0) for buff->SPI, TRUE(1) for SPI->buff
}

void SPI1_Init(void)
{
 	  GPIO_InitTypeDef GPIO_InitStructure;
  	SPI_InitTypeDef  SPI_InitStructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1 , ENABLE);
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);  

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //Set SPI one-way or two-way data mode: SPI is set to two-wire bidirectional full duplex
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;   //
//	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		    //Clock floating low
//	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	    //Data capture on the first clock edge
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//The idle state of the serial synchronous clock is high
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	    //The second transition edge (rising or falling) of the serial synchronous clock is sampled
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		    //NSS signal is managed by hardware (NSS pin) or software (using SSI bit): internal NSS signal has SSI bit control
	//APB2 Clock Frequency is 72Mhz, baud rate is more than 18 mbit => SPI_BaudRatePrescaler max is 4
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//Configure SPI1 speed to be the highest
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//Specify whether the data transfer starts from the MSB bit or the LSB bit: data transfer starts from the MSB bit
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC Polynomial of value calculation
	SPI_Init(SPI1, &SPI_InitStructure); 
	
	SPI_Cmd(SPI1, ENABLE); //Enable SPI peripherals
	
	/* enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  		 
 

}


/*-----------------------------------------------------------------------*/
/* Transmit/Receive Block using DMA (Platform dependent. STM32 here)     */
/*-----------------------------------------------------------------------*/
void stm32_wizchip_dma_transfer(
	uint8_t receive,			/* FALSE(0) for buff->SPI, TRUE(1) for SPI->buff      */
	const uint8_t *buff,		/* receive TRUE  : 512 byte data block to be transmitted
						   		receive FALSE : Data buffer to store received data    */
	uint16_t btr 				/* Byte count */
)
{
	DMA_InitTypeDef DMA_InitStructure;
	uint16_t rw_workbyte[] = { 0xffff };

	/* shared DMA configuration values */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(W5500_SPI->DR));
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_BufferSize = btr;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_DeInit(DMA_Channel_SPI_WIZCHIP_RX);
	DMA_DeInit(DMA_Channel_SPI_WIZCHIP_TX);

	if ( receive ) {

		/* DMA1 channel2 configuration SPI1 RX ---------------------------------------------*/
		/* DMA1 channel4 configuration SPI2 RX ---------------------------------------------*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buff;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_Init(DMA_Channel_SPI_WIZCHIP_RX, &DMA_InitStructure);

		/* DMA1 channel3 configuration SPI1 TX ---------------------------------------------*/
		/* DMA1 channel5 configuration SPI2 TX ---------------------------------------------*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rw_workbyte;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
		DMA_Init(DMA_Channel_SPI_WIZCHIP_TX, &DMA_InitStructure);

	} else {

		/* DMA1 channel2 configuration SPI1 RX ---------------------------------------------*/
		/* DMA1 channel4 configuration SPI2 RX ---------------------------------------------*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rw_workbyte;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
		DMA_Init(DMA_Channel_SPI_WIZCHIP_RX, &DMA_InitStructure);

		/* DMA1 channel3 configuration SPI1 TX ---------------------------------------------*/
		/* DMA1 channel5 configuration SPI2 TX ---------------------------------------------*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buff;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_Init(DMA_Channel_SPI_WIZCHIP_TX, &DMA_InitStructure);
	}

	/* Enable DMA RX Channel */
	DMA_Cmd(DMA_Channel_SPI_WIZCHIP_RX, ENABLE);
	/* Enable DMA TX Channel */
	DMA_Cmd(DMA_Channel_SPI_WIZCHIP_TX, ENABLE);

	/* Enable SPI TX/RX request */
	SPI_I2S_DMACmd(W5500_SPI, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);

	/* Wait until DMA1_Channel 3 Transfer Complete */
	/// not needed: while (DMA_GetFlagStatus(DMA_FLAG_SPI_WIZCHIP_TC_TX) == RESET) { ; }
	/* Wait until DMA1_Channel 2 Receive Complete */
	while (DMA_GetFlagStatus(DMA_FLAG_SPI_WIZCHIP_TC_RX) == RESET) { ; }
	// same w/o function-call:
	// while ( ( ( DMA1->ISR ) & DMA_FLAG_SPI_WIZCHIP_TC_RX ) == RESET ) { ; }

	/* Disable DMA RX Channel */
	DMA_Cmd(DMA_Channel_SPI_WIZCHIP_RX, DISABLE);
	/* Disable DMA TX Channel */
	DMA_Cmd(DMA_Channel_SPI_WIZCHIP_TX, DISABLE);

	/* Disable SPI RX/TX request */
	SPI_I2S_DMACmd(W5500_SPI, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);
}
