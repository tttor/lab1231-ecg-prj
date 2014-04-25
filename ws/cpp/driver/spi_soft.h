#ifndef _spi_soft
#define _spi_soft

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <unistd.h>
#include "gpio_mmap.h"

typedef enum {
	SPI_MSB_FIRST,
	SPI_LSB_FIRST
}
SPI_DATA_ORDER_ENUM;

typedef enum {
    SPI_LATCH_LOW2HIGH,
    SPI_LATCH_HIGH2LOW
} SPI_LATCH_ENUM;

#define delay(delayValue) usleep(delayValue)

//var
extern int SPI_SOFT_DELAY;

char SPI_Soft_Init(int SPI_SCLK, char SPI_SCLK_DEFAULT_STATE,int 
SPI_SLAVE_DI,char SPI_SLAVE_DI_DEFAULT_STATE,int SPI_SLAVE_DO,int 
SPI_CS, char SPI_CS_DEFAULT_STATE);
char SPI_Send_Data(int SPI_SCLK, int SPI_SLAVE_DI, char SPI_DATA, 
char SPI_DATA_ORDER, char SPI_LATCH_EDGE);
void SPI_Send_Clk(int SPI_SCLK, char SPI_LATCH_EDGE);
char SPI_Get_Data(int SPI_SCLK, int SPI_SLAVE_DO, char 
SPI_DATA_ORDER, char SPI_LATCH_EDGE );

#ifdef __cplusplus
}
#endif
#endif

