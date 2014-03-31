#include "wireless_cc2500.h"

void CC2500_ReadByte(char byte);
void CC2500_WriteByte(char byte);
void CC2500_LowLevelInit(void);

void CC2500_LowLevelInit()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	// Enable the SPI peripheral
	RCC_AP2B2PeriphClockCmd(CC2500_SPI_CLK, ENABLE);
}
