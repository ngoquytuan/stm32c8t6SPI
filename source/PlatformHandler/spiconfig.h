
#ifndef __SPI_H
#define __SPI_H
#include "stm32f10x.h"
#define W5500_SPI                       SPI1
#define DMA_Channel_SPI_WIZCHIP_RX    	DMA1_Channel2
#define DMA_Channel_SPI_WIZCHIP_TX    	DMA1_Channel3
#define DMA_FLAG_SPI_WIZCHIP_TC_RX    	DMA1_FLAG_TC2
#define DMA_FLAG_SPI_WIZCHIP_TC_TX    	DMA1_FLAG_TC3
void SPI1_Init(void);
void stm32_wizchip_dma_transfer(uint8_t receive, const uint8_t *buff, uint16_t btr);
void  wizchip_writeburst(uint8_t* pBuf, uint16_t len);
void wizchip_readburst(uint8_t* pBuf, uint16_t len);
uint8_t wizchip_read();
void  wizchip_write(uint8_t wb);
uint8_t stm32_spi_rw( uint8_t out );
#endif