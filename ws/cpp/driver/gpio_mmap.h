/*
 * GPIO Mapping
 * */

#ifndef _gpio_mmap
#define _gpio_mmap

#ifdef __cplusplus
extern "C" {
#endif

//lib
//#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

//gpio state enum
typedef enum {
	GPIO_OUTPUT = 0,
	GPIO_LOW = 0,
	PULL_OFF = 0,
	PULL_TYPE_DOWN = 0,
	GPIO_INPUT = 1,
	GPIO_HIGH = 1,
	PULL_ON = 1,
	PULL_TYPE_UP = 1
} ENUM_GPIO;

#define NOP1	__asm__("nop")
#define	NOP10	__asm__("nop;nop;nop;nop;nop;nop;nop;nop;nop;nop")

#define MEMORY_MAP_SIZE				0x10000
#define	BASE_GPIO_BANK5				128
#define BASE_OFFSET_ADDR_GPIO130	0x2158
#define CALC_BASE_OFFSET_ADDR_GPIO_N(GPIO_NUMBER) (((char)((GPIO_NUMBER-130)/2) << 2)+BASE_OFFSET_ADDR_GPIO130)
#define OFFSET_ADDR_GPIO_N(GPIO_NUMBER) OFFSET(CALC_BASE_OFFSET_ADDR_GPIO_N(GPIO_NUMBER))
#define	OFFSET(ADDR)	ADDR/4
#define MASK_REG_PIN(GPIO_NUMBER) (unsigned long)((GPIO_NUMBER-130)%2==0) ? 0xffff0000:0x0000ffff
#define GPIO_REG_CONFIG_VALUE(GPIO_NUMBER, GPIO_IN_OUT, PULL_UP_DOWN, PULL_ON_OFF) (((PULL_ON_OFF<<3) | (PULL_UP_DOWN<<4) | (GPIO_IN_OUT<<8) | 0x04) << (((GPIO_NUMBER-130)%2==0) ? 0:16))
//#define MASK_VALUE(AVALUE, AMASK) (AVALUE & AMASK)

#define OFFSET_ADDR_REG_GPIO_OE				0x6034
#define OFFSET_ADDR_REG_GPIO_DATAIN			0x6038
#define OFFSET_ADDR_REG_GPIO_CLR_IRQ1		0x6060
#define OFFSET_ADDR_REG_GPIO_CLR_IRQ2		0x6070
#define OFFSET_ADDR_REG_GPIO_WAKEUP_EN		0x6080
#define OFFSET_ADDR_REG_GPIO_CLR_DATA		0x6090
#define OFFSET_ADDR_REG_GPIO_SET_DATA		0x6094

#define ADDR_MEM_REG_PAD_CONFIG 	0x48000000
#define ADDR_MEM_REG_GPIO_CONFIG	0x49050000

//proto
char GPIO_Init_Direction_Value(int GPIO_Num, int GPIO_Mode, int GPIO_Value);
char GPIO_Release();
char GPIO_Get_Value(int GPIO_Num);
char GPIO_Set_Value(int GPIO_Num, char GPIO_Value);
unsigned long GPIO_Read_Port();
unsigned long gpio_pinconf_value(unsigned long PINCONF_VAL, char PIN_NUMBER, char PIN_MODE, char PIN_PULL_T, char PULL_EN);


#ifdef __cplusplus
}
#endif
#endif
