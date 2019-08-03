#include "ff.h"
#include "diskio.h"
#include "stm32f10x.h"
#define SPI_SD                    			SPI1
#define GPIO_CS          								GPIOA                       	/* GPIOA */
#define GPIO_Pin_CS           					GPIO_Pin_4                  	/* PB.11 */
#define SPI_BaudRatePrescaler_SPI_SD  	SPI_BaudRatePrescaler_4			/* 72MHz / 4 */

// SD Card Selection; CS control
#define SD_CS_SELECT()        			GPIO_ResetBits(GPIO_CS, GPIO_Pin_CS)    // MMC CS = L
#define SD_CS_DESELECT()      			GPIO_SetBits(GPIO_CS, GPIO_Pin_CS)      // MMC CS = H
#define GO_IDLE_STATE			0
#define SEND_OP_COND			1
#define SEND_IF_COND			8
#define SET_BLOCKLEN			16
#define READ_BLOCK				17
#define WRITE_BLOCK				24
#define APP_CMD					55
#define READ_OCR				58
#define SD_SEND_OP_COND			(0x40|41)
typedef enum card_type_id {
	NO_CARD,
	CARD_MMC,
	CARD_SD,
	CARD_SD2,
	CARD_SDHC,
	SPI_FLASHM
} card_type_id_t;



uint8_t mmc_mount();
uint8_t flash_mount();
FRESULT getMountedMemorySize(uint8_t mount_ret, uint32_t * totalSize, uint32_t * availableSize);
void Delay_ms(uint32_t ms);
uint8_t mmc_select();
