#ifndef _ads1298
#define _ads1298

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "spi_soft.h"

//pin ads1298
#define ADS1298_CS		146
#define	ADS1298_SCLK	134
#define ADS1298_RESET	135
#define ADS1298_DIN		133
#define ADS1298_DOUT	132
#define ADS1298_START	131
#define ADS1298_DRDY	130

//register list
typedef enum {
	REG_ID,
	REG_CFG1, REG_CFG2, REG_CFG3,
	REG_LOFF, REG_CH1SET, REG_CH2SET, REG_CH3SET, REG_CH4SET,
	REG_CH5SET, REG_CH6SET, REG_CH7SET, REG_CH8SET, REG_RLD_SENSP,
	REG_RLD_SENSN, REG_LOFF_SENSP, REG_LOFF_SENSN, REG_LOFF_FLIP,
	REG_LOFF_STATP, REG_LOFF_STATN, REG_GPIO, REG_PACE, REG_RESP, REG_CFG4,
	REG_WCT1, REG_WCT2
}
ADS1298_REGISTER_ENUM;

typedef enum {
    INPUT_ON, INPUT_OFF
} ADS1298_INPUT_POWER_MODE_ENUM;

typedef enum {
    PGA_6x, PGA_1x, PGA_2x, PGA_3x, PGA_4x, PGA_8x, PGA_12x
} ADS1298_PGA_ENUM;

typedef enum {
    IN_NORMAL, IN_SHORTED, IN_RLD_MEASURE, IN_MVDD, IN_TEMP_MEASURE,
    IN_TEST_SIGNAL, IN_RLD_DRP, IN_RLD_DRN
} ADS1298_INPUT_MODE_ENUM;

typedef enum {
    CH1, CH2, CH3, CH4, CH5, CH6, CH7, CH8
} ADS1298_CHNL_ENUM;

typedef enum {
	ADC_MODE_NOISE,
	ADC_MODE_SIGNAL_TEST,
	ADC_MODE_RUN
} ADS1298_WORK_MODE_ENUM;

//commnd list
typedef enum {
    ADS1298_CMD_WAKEUP 	= 	0x02,
    ADS1298_CMD_STANDBY	=	0x04,
    ADS1298_CMD_RESET	=	0x06,
    ADS1298_CMD_START	=	0x08,
    ADS1298_CMD_STOP	=	0x0a,
    ADS1298_CMD_RDATAC	=	0x10,
    ADS1298_CMD_SDATAC	=	0x11,
    ADS1298_CMD_RDATA	=	0x12
} ADS1298_CMD_ENUM;

//voltage mode
typedef enum {
	ADS1298_uV,
	ADS1298_mV,
	ADS1298_V
} ADS1298_VOLTAGE_UNIT;

//sampe rate config
typedef enum {
	ADS1298_250SPS 	= 0x06,
	ADS1298_500SPS 	= 0x86,
	ADS1298_1000SPS	= 0x85
} ADS1298_SAMPLE_RATE;

extern int delayMultiByteCommand;

char ADS1298_Init();
char ADS1298_Read_Register(ADS1298_REGISTER_ENUM reg_addr, char
                           reg_num_read);
char ADS1298_Write_Register(ADS1298_REGISTER_ENUM reg_addr, char
                            reg_value);
char ADS1298_Reset();
char ADS1298_Send_Command(ADS1298_CMD_ENUM aCommand);
char ADS1298_Start_ADC();
char ADS1298_Stop_ADC();
void ADS1298_Get_Data(int *StatusADC,int *arrDataADC);
char ADS1298_Set_Channel(ADS1298_CHNL_ENUM aChannel,
                         ADS1298_INPUT_POWER_MODE_ENUM isPoweredDown, ADS1298_PGA_ENUM Gain,
                         ADS1298_INPUT_MODE_ENUM InputMode);
char ADS1298_Setup_ECG(char work_mode, char sample_rate);
char ADS1298_Start_Measurement();
void ADS1298_Read_All_Register();
void ADS1298_Get_Samples(int max_num_samples, int *adc_sts, int *adc_data);
void ADS1298_Include_Offset(int max_num_samples, int *adc_data);
void ADS1298_Convert_To_Voltage(int max_num_samples, int *adc_data, double *volt_data, char volt_mode);

#ifdef __cplusplus
}
#endif
#endif
