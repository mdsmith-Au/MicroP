#include "wireless_cc2500.h"
#include <stdio.h>

uint8_t CC2500_SendByte(uint8_t byte);
int CC2500_Write(uint8_t* buffer, uint8_t address, int numBytes);
int CC2500_Read(uint8_t* buffer, uint8_t address, int numBytes);
int CC2500_Read_Reg(uint8_t* buffer, uint8_t header, int numBytes);
void CC2500_LowLevelInit(void);
void CC2500_LowLevelWireless_Init(void);
int CC2500_Check_Status(char status);
void CC2500_Delay(void);

int CC2500_SPI_Cmd_Strobe(uint8_t command) {
	CC2500_NSS_LOW();
	while(GPIO_ReadInputDataBit(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_PIN) != 0) {};

	uint8_t status = CC2500_SendByte(command);
	return SUCCESS;
}

int CC2500_Write_Reg(uint8_t* buffer, uint8_t header, int numBytes) {
	// Set chip select to low and wait for MISO to be low
	CC2500_NSS_LOW();
	while(GPIO_ReadInputDataBit(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_PIN) != 0) {};
	
	// Send the address of the register
	uint8_t status = CC2500_SendByte(header);
	
	// Send data
	while(numBytes > 0)
	{
		status = CC2500_SendByte(*buffer);
		printf("Sending: %x\n", *buffer);
		numBytes--;
		buffer++;
	}
	
	// Set chip select to high
	CC2500_NSS_HIGH();
	return SUCCESS;
}

int CC2500_Read_Reg(uint8_t* buffer, uint8_t header, int numBytes) {
	// Set chip select to low and wait for MISO to be low
	CC2500_NSS_LOW();
	while(GPIO_ReadInputDataBit(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_PIN) != 0) {};
	
	// Send the address of the register
	uint8_t status = CC2500_SendByte(header);
	
	// Read data
	while(numBytes > 0)
	{
		*buffer = CC2500_SendByte(DUMMY_BYTE);
		printf("Receiving: %x\n", *buffer);
		numBytes--;
		buffer++;
	}
	
	// Set chip select to high
	CC2500_NSS_HIGH();
	return SUCCESS;
}

uint8_t CC2500_SendByte(uint8_t byte)
{
	// Loop while DR register is not empty
	while(SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_TXE) == RESET) ;
	
	// Send a byte through the SPI peripheral
	SPI_I2S_SendData(CC2500_SPI, byte);
	printf("SendByte: %x\n", byte);
	
	// Wait to receive a byte
	while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	
	// Return the byte read from the SPI bus
	return (uint8_t)SPI_I2S_ReceiveData(CC2500_SPI);
}

// Returns 1 if good, 0 if error, 2 if buffer overflow
int CC2500_Check_Status(char status)
{
		
		return SUCCESS;
}

//TODO: Delay to wait for buffer to empty
void CC2500_Delay()
{
  
}

void CC2500_Init()
{
	// Perform the low-level initialization of the SPI, GPIO, etc...
	CC2500_LowLevelInit();
	CC2500_LowLevelWireless_Init();
}

void CC2500_LowLevelWireless_Init()
{
	
}

void CC2500_LowLevelInit()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	// Enable the peripheral clocks
	RCC_APB1PeriphClockCmd(CC2500_SPI_CLK, ENABLE);							// CLK
	RCC_AHB1PeriphClockCmd(CC2500_SPI_MISO_GPIO_CLK, ENABLE);		// MISO
	RCC_AHB1PeriphClockCmd(CC2500_SPI_MOSI_GPIO_CLK, ENABLE);		// MOSI
	RCC_AHB1PeriphClockCmd(CC2500_SPI_NSS_GPIO_CLK, ENABLE);		// NSS
	RCC_AHB1PeriphClockCmd(CC2500_SPI_INT_GPIO_CLK, ENABLE);		// INT
	
	// Set Alternate Functions of pins
	GPIO_PinAFConfig(CC2500_SPI_SCK_GPIO_PORT, CC2500_SPI_SCK_SOURCE, CC2500_SPI_SCK_AF);
  GPIO_PinAFConfig(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_SOURCE, CC2500_SPI_MISO_AF);
  GPIO_PinAFConfig(CC2500_SPI_MOSI_GPIO_PORT, CC2500_SPI_MOSI_SOURCE, CC2500_SPI_MOSI_AF);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_SCK_PIN;
  GPIO_Init(CC2500_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /* SPI MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  CC2500_SPI_MOSI_PIN;
  GPIO_Init(CC2500_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /* SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_MISO_PIN;
  GPIO_Init(CC2500_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
	
	// TODO: Make sure settings are correct for wireless
	/* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(CC2500_SPI);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  // TODO: Check NSS - software or hardware?
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  // TODO: Prescaler based on clock...which bloody one???? Need to maintain
  // appropriate speed
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  // TODO: We don't use SPI. Does this have an effect?
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(CC2500_SPI, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(CC2500_SPI, ENABLE);

  /* Configure GPIO PIN for Lis Chip select */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_NSS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CC2500_SPI_NSS_GPIO_PORT, &GPIO_InitStructure);

  /* Deselect : Chip Select high */
  GPIO_SetBits(CC2500_SPI_NSS_GPIO_PORT, CC2500_SPI_NSS_PIN);
  
  /* Configure GPIO PINs to detect Interrupts */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_INT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(CC2500_SPI_INT_GPIO_PORT, &GPIO_InitStructure);
}
