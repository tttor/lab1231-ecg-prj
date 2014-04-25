#include "ads1298.h"

int delayMultiByteCommand = 1e3;

char ADS1298_Init() {
	return !(	!GPIO_Init_Direction_Value(ADS1298_CS,
	                                       GPIO_OUTPUT, GPIO_LOW) ||
	            !GPIO_Init_Direction_Value(ADS1298_SCLK,
	                                       GPIO_OUTPUT, GPIO_LOW) ||
	            !GPIO_Init_Direction_Value(ADS1298_RESET,
	                                       GPIO_OUTPUT, GPIO_LOW) ||
	            !GPIO_Init_Direction_Value(ADS1298_DIN, GPIO_OUTPUT,
	                                       GPIO_LOW) ||
	            !GPIO_Init_Direction_Value(ADS1298_DOUT, GPIO_INPUT,
	                                       GPIO_LOW) ||
	            !GPIO_Init_Direction_Value(ADS1298_START,
	                                       GPIO_OUTPUT, GPIO_LOW) ||
	            !GPIO_Init_Direction_Value(ADS1298_DRDY, GPIO_INPUT,
	                                       GPIO_LOW));
}

char ADS1298_Read_Register(ADS1298_REGISTER_ENUM reg_addr, char
                           reg_num_read) {
	//int i;

	if(!SPI_Send_Data(ADS1298_SCLK, ADS1298_DIN, 0x20 | (reg_addr &
	                  0x1f), SPI_MSB_FIRST, SPI_LATCH_LOW2HIGH)) {
		//printf("opcode 1 fail\r\n");
		return 0;
	}
	//printf("opcode 1 ok\r\n");
	delay(delayMultiByteCommand);

	if(!SPI_Send_Data(ADS1298_SCLK, ADS1298_DIN, ((reg_num_read-1) &
	                  0x1f), SPI_MSB_FIRST, SPI_LATCH_LOW2HIGH)) {
		//printf("opcode 2 fail\r\n");
		return 0;
	}
	//printf("opcode 2 ok\r\n");
	delay(delayMultiByteCommand);

	//get data
	return SPI_Get_Data(ADS1298_SCLK, ADS1298_DOUT, SPI_MSB_FIRST,
	                    SPI_LATCH_LOW2HIGH);

}

char ADS1298_Write_Register(ADS1298_REGISTER_ENUM reg_addr, char
                            reg_value) {
	//int i;
	int reg_num_write = 1;

	if(!SPI_Send_Data(ADS1298_SCLK, ADS1298_DIN, 0x40 | (reg_addr &
	                  0x1f), SPI_MSB_FIRST, SPI_LATCH_LOW2HIGH)) {
		//printf("opcode 1 fail\r\n");
		return 0;
	}
	//printf("opcode 1 ok\r\n");
	delay(delayMultiByteCommand);

	if(!SPI_Send_Data(ADS1298_SCLK, ADS1298_DIN, ((reg_num_write-1)
	                  & 0x1f), SPI_MSB_FIRST, SPI_LATCH_LOW2HIGH)) {
		//printf("opcode 2 fail\r\n");
		return 0;
	}
	//printf("opcode 2 ok\r\n");
	delay(delayMultiByteCommand);

	//get data
	if(!SPI_Send_Data(ADS1298_SCLK, ADS1298_DIN, reg_value,
	                  SPI_MSB_FIRST, SPI_LATCH_LOW2HIGH)) {
		//printf("opcode 3 fail\r\n");
		return 0;
	}
	//printf("opcode 3 ok\r\n");
	delay(delayMultiByteCommand);

	return 1;
}

//reset ads1298
char ADS1298_Reset() {
	if(!GPIO_Set_Value(ADS1298_RESET, GPIO_HIGH))
		return 0;
	delay(delayMultiByteCommand);
	if(!GPIO_Set_Value(ADS1298_RESET, GPIO_LOW))
		return 0;
	delay(delayMultiByteCommand);
	if(!GPIO_Set_Value(ADS1298_RESET, GPIO_HIGH))
		return 0;
	delay(delayMultiByteCommand);

	return 1;
}

char ADS1298_Send_Command(ADS1298_CMD_ENUM aCommand) {
	if(!SPI_Send_Data(ADS1298_SCLK, ADS1298_DIN, aCommand,
	                  SPI_MSB_FIRST, SPI_LATCH_LOW2HIGH)) {
		return 0;
	}

	return 1;
}

char ADS1298_Start_ADC() {
	return GPIO_Set_Value(ADS1298_START, GPIO_HIGH);
}


char ADS1298_Stop_ADC() {
	return GPIO_Set_Value(ADS1298_START, GPIO_LOW);
}

void ADS1298_Get_Data(int *StatusADC,int *arrDataADC) {
	int *ptrData = arrDataADC;

	while(GPIO_Get_Value(ADS1298_DRDY));	//tggu sampe low
	//delay(delayMultiByteCommand);

	//read data
	int i;
	char tmpd;

	//init result
	*StatusADC = 0;
	for(i=0; i<8; i++) {
		*arrDataADC = 0;
		arrDataADC++;
	}
	arrDataADC = ptrData; //reset position back

	GPIO_Set_Value(ADS1298_DIN, GPIO_LOW);
	for(i=0; i<27; i++) {	//27 byte data clock
		tmpd = SPI_Get_Data(ADS1298_SCLK, ADS1298_DOUT,
		                    SPI_MSB_FIRST, SPI_LATCH_LOW2HIGH);

		if(i<3) {	//status reading
			*StatusADC <<= 8;
			*StatusADC |= tmpd;
		} else {	//kanal data adc
			arrDataADC[(i/3)-1] <<= 8;
			arrDataADC[(i/3)-1] |= tmpd;
		}
	}
	arrDataADC = ptrData; //reset position back

	//filter data to 2nd complement
	for(i=0; i<8; i++) {
		//printf("%.8lX --> ", arrDataADC[i]);
		if(arrDataADC[i]>0x7fffff)	//data negatif
			arrDataADC[i] -= 0x800000;
		else //data positif
			arrDataADC[i] += 0x800000;
		//printf("%.8lX\r\n", arrDataADC[i]);
	}
}

char ADS1298_Set_Channel(ADS1298_CHNL_ENUM aChannel,
                         ADS1298_INPUT_POWER_MODE_ENUM isPoweredDown, ADS1298_PGA_ENUM Gain,
                         ADS1298_INPUT_MODE_ENUM InputMode) {
	char tmpd = (isPoweredDown<<7) | (Gain<<4) | (InputMode);
	return ADS1298_Write_Register(REG_CH1SET+aChannel, tmpd);
}

/*
setup ads1298 utk ecg data acquire, 10 lead
*/
char ADS1298_Setup_ECG(char work_mode, char sample_rate)
{
	int i = 0;
	char isOK = 1;
	
	//init the pin
	if(!ADS1298_Init())
		return 1;
	
	//powerup sequence
	if(!GPIO_Set_Value(ADS1298_RESET, GPIO_HIGH))
		return 2;
	usleep(1e6);
	if(!ADS1298_Reset())
		return 3;
	usleep(1e5);
	
	//enable adc
	if(!GPIO_Set_Value(ADS1298_CS, GPIO_LOW))
		return 4;
		
	//stop continuos data convertion
	if(!ADS1298_Send_Command(ADS1298_CMD_SDATAC))
		return 5;
		
	//hi res mod, 500sps
	//mode sesuai sample rate	
	if(!ADS1298_Write_Register(REG_CFG1, sample_rate))
		return 6;
		
	//input shorted
	if(!ADS1298_Write_Register(REG_CFG2, (work_mode==1) ?  0x14:0x00))
		return 7;
		
	if(!ADS1298_Write_Register(REG_CFG3, 0xdc))
		return 8;
		
	if(!ADS1298_Write_Register(REG_WCT1, 0x0a))
		return 9;
		
	if(!ADS1298_Write_Register(REG_WCT2, 0xe3))
		return 10;

	//all chnl config
	for(i=0; i<8; i++) {
		//if(!ADS1298_Write_Register(REG_CH1SET+i, (work_mode==1) ? 0x05:(work_mode==0) ? 0x01:0x00))
		if(!ADS1298_Set_Channel(i,INPUT_ON,PGA_6x,(work_mode==1) ? IN_TEST_SIGNAL:(work_mode==0) ? IN_SHORTED:IN_NORMAL))
		{
			isOK = 0;
			break;
		}
	}
	if(!isOK)
		return 11;
		
	//all step pass ok
	return 0;
}

/*
ads1298 start measurement
*/
char ADS1298_Start_Measurement()
{
	if(!ADS1298_Start_ADC())
		return 1;
		
	if(!ADS1298_Send_Command(ADS1298_CMD_RDATAC))
		return 2;
		
	//all command pass
	return 0;
}

/*
read all ads1298 register
*/
void ADS1298_Read_All_Register()
{
	int i;
	printf("==========================================\r\n");
	for(i=0; i<=0x19; i++) {
		printf("reg 0x%.2X = 0x%.2X\r\n", i, ADS1298_Read_Register(i, 1));
	}
	printf("==========================================\r\n");
}

/*
get sample from ADS1298
*/
void ADS1298_Get_Samples(int max_num_samples, int *adc_sts, int *adc_data)
{
	int i=0, j=0;
	int adc_stsx, adcx[8]={};
	
	while(j<max_num_samples) {
		ADS1298_Get_Data(&adc_stsx, adcx);
		*adc_sts++ = adc_stsx;
		for(i=0; i<8; i++)
			*adc_data++ = adcx[i];
		j++;
	}
}

/*
 * offset data adc
 * */
 void ADS1298_Include_Offset(int max_num_samples, int *adc_data)
 {
	 int i=0, j=0;
	//unsigned int adc_stsx, adcx[8]={};
	
	while(j<max_num_samples) {
		for(i=0; i<8; i++)
			*adc_data++ -= 0x800000; // kenapa offsetnya segini ????
		j++;
	}
 }
 
 /*
 * calc mv of data
 * volt_mode :
 * 	0 = uV
 * 	1 = mV
 * 	2 = V
 * */
void ADS1298_Convert_To_Voltage(int max_num_samples, int *adc_data, double *volt_data, char volt_mode) {
	int i;
	double scale[3] = {1e6,1e3,1};

	for(i=0; i<max_num_samples; i++) {
		*volt_data++ = *adc_data++ * 2.4 * scale[(int)volt_mode] / (0x800000-1);
	}
}
