#include "gpio_mmap.h"

//global var
volatile unsigned long  *gpio;
int gpio_fd;

//init and set pin
char GPIO_Init_Direction_Value(int GPIO_Num, int GPIO_Mode, int GPIO_Value)
{
	int fd = open("/dev/mem", O_RDWR | O_SYNC); //O_SYNC makes the memory uncacheable
	if (fd < 0) {
		return 0;
	}

	// Pad configuration
	volatile unsigned long *pinconf;
	pinconf = (unsigned long*) mmap(NULL, MEMORY_MAP_SIZE, PROT_READ | PROT_WRITE,MAP_SHARED, fd, ADDR_MEM_REG_PAD_CONFIG);
	if (pinconf == MAP_FAILED) {
		close(fd);
		return 0;
	}

	// set pin ads1298
	pinconf[OFFSET_ADDR_GPIO_N(GPIO_Num)] = gpio_pinconf_value(pinconf[OFFSET_ADDR_GPIO_N(GPIO_Num)], GPIO_Num, GPIO_Mode, PULL_TYPE_UP, PULL_ON);	
	//printf("%.8X = %.8lX\r\n", CALC_BASE_OFFSET_ADDR_GPIO_N(GPIO_Num), pinconf[OFFSET_ADDR_GPIO_N(GPIO_Num)]);

	//close fd
	close(fd);

	//konfig pin bank
	gpio_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (gpio_fd < 0) {
		return 0;
	}

	// GPIO configuration
	//volatile ulong *gpio;
	gpio = (unsigned long*) mmap(NULL, MEMORY_MAP_SIZE, PROT_READ | PROT_WRITE,MAP_SHARED, gpio_fd, ADDR_MEM_REG_GPIO_CONFIG);
	if (gpio == MAP_FAILED) {
		close(gpio_fd);
		return 0;
	}
	
	// Configure GPIO pins on bank 5 as output.
	// GPIO 5 is at physical address 0x49056000 = ADDR_MEM_REG_GPIO_CONFIG+0x6000
	unsigned long reg_val = (1<<(GPIO_Num-BASE_GPIO_BANK5));
	if(GPIO_Mode==GPIO_OUTPUT)
		gpio[OFFSET(OFFSET_ADDR_REG_GPIO_OE)] &= ~reg_val;     // set low for output
	else
		gpio[OFFSET(OFFSET_ADDR_REG_GPIO_OE)] |= reg_val;     // set high for input
	// Also disable the wakeupenable and irqenable intertupts
	// GPIO clear_Wakeupenable is offset by 0x80 for each bank
	gpio[OFFSET(OFFSET_ADDR_REG_GPIO_WAKEUP_EN)]  |=  reg_val;     // 0x1: Clear the corresponding bit in the GPIO_WAKEUPENABLE register
	// GPIO clear_irqenable1 is offset by 0x60 for each bank
	gpio[OFFSET(OFFSET_ADDR_REG_GPIO_CLR_IRQ1)]  |=  reg_val;     // 0x1: Clear the corresponding bit in the GPIO_IRQENABLE1 register
	// GPIO clear_irqenable2 is offset by 0x70 for each bank
	gpio[OFFSET(OFFSET_ADDR_REG_GPIO_CLR_IRQ2)]  |=  reg_val;     // 0x1: Clear the corresponding bit in the GPIO_IRQENABLE2 register

	//printf("OFFSET_ADDR_REG_GPIO_OE = %.8X\r\nOFFSET_ADDR_REG_GPIO_CLR_IRQ1 = %.8X\r\nOFFSET_ADDR_REG_GPIO_CLR_IRQ2 = %.8X\r\nOFFSET_ADDR_REG_GPIO_WAKEUP_EN = %.8X\r\n",gpio[OFFSET_ADDR_REG_GPIO_OE/4],gpio[OFFSET_ADDR_REG_GPIO_CLR_IRQ1/4],gpio[OFFSET_ADDR_REG_GPIO_CLR_IRQ2/4],gpio[OFFSET_ADDR_REG_GPIO_WAKEUP_EN/4]);
	//printf("reg val = %.8lX\r\n", reg_val);
	
	//set the def state
	if(GPIO_Mode==GPIO_OUTPUT)
	{
		GPIO_Set_Value(GPIO_Num, GPIO_Value);
	}

	return 1;
}

unsigned long GPIO_Read_Port()
{
	return gpio[OFFSET(OFFSET_ADDR_REG_GPIO_DATAIN)];
}

char GPIO_Get_Value(int GPIO_Num) 
{
	unsigned long pin_val = gpio[OFFSET(OFFSET_ADDR_REG_GPIO_DATAIN)], chk = 1<<(GPIO_Num-BASE_GPIO_BANK5);
	//printf("pin = %.8lX\t%.8X\r\n", pin_val, chk);
	return (pin_val & chk) >> (GPIO_Num-BASE_GPIO_BANK5);
}

char GPIO_Set_Value(int GPIO_Num, char GPIO_Value)
{
	gpio[(GPIO_Value==GPIO_HIGH) ? OFFSET(OFFSET_ADDR_REG_GPIO_SET_DATA):OFFSET(OFFSET_ADDR_REG_GPIO_CLR_DATA)] = (1<<(GPIO_Num-BASE_GPIO_BANK5));	
	return 1;
}

char GPIO_Release()
{
	close(gpio_fd);
	return 1;
}

unsigned long gpio_pinconf_value(unsigned long PINCONF_VAL, char GPIO_Num, char GPIO_Mode, char PIN_PULL_T, char PULL_EN)
{
	unsigned long maskx = MASK_REG_PIN(GPIO_Num), reg_val = GPIO_REG_CONFIG_VALUE(GPIO_Num, GPIO_Mode, PIN_PULL_T, PULL_EN);
	//printf("prev = %.8lX\r\n", PINCONF_VAL);
	return (PINCONF_VAL & maskx) | reg_val;
}
