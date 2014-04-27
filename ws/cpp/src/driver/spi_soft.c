/*
 * spi software
 * */

//lib used
#include "spi_soft.h"

//global var
int SPI_SOFT_DELAY = 1;	//delay clock spi dlm us

/**
 * @brief init spi software and init default state
 * @param fio_SCLK
 * @param SPI_SCLK
 * @param SPI_SCLK_DEFAULT_STATE
 * @param fio_SLAVE_DI
 * @param SPI_SLAVE_DI
 * @param SPI_SLAVE_DI_DEFAULT_STATE
 * @param fio_SLAVE_DO
 * @param SPI_SLAVE_DO
 * @param fio_CS
 * @param SPI_CS
 * @param SPI_CS_DEFAULT_STATE
 * @return 	0 = fail; 1 = success
 */

char SPI_Soft_Init(int SPI_SCLK, char SPI_SCLK_DEFAULT_STATE,int
                   SPI_SLAVE_DI,char SPI_SLAVE_DI_DEFAULT_STATE,int SPI_SLAVE_DO,int
                   SPI_CS, char SPI_CS_DEFAULT_STATE) {
	/*
	 * configure pin
	 * SPI_SCLK	= out
	 * SPI_SLAVE_DI	= out
	 * SPI_SLAVE_DO	= in
	 * SPI_CS	= out
	 * */

	//init sclk
	if (!GPIO_Init_Direction_Value(SPI_SCLK, GPIO_OUTPUT, SPI_SCLK_DEFAULT_STATE))
		return 0;

	//init di
	if (!GPIO_Init_Direction_Value(SPI_SLAVE_DI, GPIO_OUTPUT, SPI_SLAVE_DI_DEFAULT_STATE))
		return 0;

	//init do
	if (!GPIO_Init_Direction_Value(SPI_SLAVE_DO, GPIO_INPUT, GPIO_LOW))
		return 0;

	//init cs
	if (!GPIO_Init_Direction_Value(SPI_CS, GPIO_OUTPUT, SPI_CS_DEFAULT_STATE))
		return 0;

	//final return ok ok
	return 1;
}

/**
 * @brief
 * @param SPI_SCLK
 * @param SPI_LATCH_EDGE
 */

void SPI_Send_Clk(int SPI_SCLK, char SPI_LATCH_EDGE) {
	//latch on low2high or high2low
	GPIO_Set_Value(SPI_SCLK, (SPI_LATCH_EDGE == SPI_LATCH_LOW2HIGH)
	               ? GPIO_HIGH:GPIO_LOW);
	NOP10;
	NOP10;
	NOP10;
	NOP10;
	NOP10;
	GPIO_Set_Value(SPI_SCLK, (SPI_LATCH_EDGE == SPI_LATCH_LOW2HIGH)
	               ? GPIO_LOW:GPIO_HIGH);
	NOP10;
	NOP10;
	NOP10;
	NOP10;
	NOP10;
}

/**
 * @brief 	send data via spi
 * @param 	fio_SCLK
 * @param 	fio_SLAVE_DI
 * @param 	SPI_DATA
 * @param 	SPI_DATA_ORDER
 * @param 	SPI_SHIFT_NUM
 * @param 	SPI_LATCH_EDGE
 * @return	0 = fail; 1 = ok
 */

char SPI_Send_Data(int SPI_SCLK, int SPI_SLAVE_DI, char SPI_DATA, char
                   SPI_DATA_ORDER, char SPI_LATCH_EDGE ) {
	char i, dtx, res = 1;

	for(i = 0; i<8; i++) {
		//msb / lsb first
		dtx = (( ((SPI_DATA_ORDER == SPI_MSB_FIRST) ?
		          (0x80>>i):(0x01<<i)) & SPI_DATA) != 0);
		if(!GPIO_Set_Value(SPI_SLAVE_DI, dtx)) {
			res = 0;
			break;
		}

		//give pulse clock
		SPI_Send_Clk(SPI_SCLK, SPI_LATCH_EDGE);

		//delay clock
		if(SPI_SOFT_DELAY>0)
			usleep(SPI_SOFT_DELAY);
	}

	//make sure right last state pin
	//latch on low2high or high2low
	GPIO_Set_Value(SPI_SCLK, (SPI_LATCH_EDGE == SPI_LATCH_LOW2HIGH)
	               ? GPIO_LOW:GPIO_HIGH);
	//slave di = 0
	GPIO_Set_Value(SPI_SLAVE_DI, GPIO_LOW);

	return res;
}

char SPI_Get_Data(int SPI_SCLK, int SPI_SLAVE_DO, char SPI_DATA_ORDER,
                  char SPI_LATCH_EDGE ) {
	char tmpd = 0;
	int i;

	for(i = 0; i<8; i++) {
		//spare 1bit at LSB for MSB first
		if(SPI_DATA_ORDER == SPI_MSB_FIRST)
			tmpd<<=1;
		else
			tmpd>>=1;

		//pulse clk
		SPI_Send_Clk(SPI_SCLK, SPI_LATCH_EDGE);

		//get the data
		if(GPIO_Get_Value(SPI_SLAVE_DO))
			tmpd |= (SPI_DATA_ORDER == SPI_MSB_FIRST) ?
			        1:0x80;
	}

	return tmpd;
}
