#include "wireless_cc2500.h"
#include <stdio.h>

int CC2500_Read(uint8_t* buffer, uint8_t address, int numBytes);
uint8_t CC2500_SendByte(uint8_t byte);
int CC2500_Write(uint8_t* buffer, uint8_t address, int numBytes);
int CC2500_Read_Reg(uint8_t* buffer, uint8_t address, int numBytes);
void CC2500_LowLevelInit(void);
void CC2500_LowLevelWireless_Init(void);
int CC2500_Check_Status(char status);
void CC2500_Delay(void);

int CC2500_CmdStrobe(uint8_t command) {
	CC2500_NSS_LOW();
	while(GPIO_ReadInputDataBit(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_PIN) != 0) {};

	uint8_t status = CC2500_SendByte(command);
	printf("Status: %d\n", CC2500_Status(status));
	return SUCCESS;
}

void CC2500_TXMode()
{
	CC2500_CmdStrobe(STX);
}

void CC2500_RXMode()
{
	CC2500_CmdStrobe(SRX);
}

void CC2500_Idle()
{
	CC2500_CmdStrobe(SIDLE);
}

int CC2500_TransmitMessage(void* buffer)
{
	return SUCCESS;
}

int CC2500_ReceiveMessage(void* buffer)
{
	return SUCCESS;
}

int CC2500_WriteFIFO(uint8_t* buffer, uint8_t address, int numBytes)
{
	// Set chip select to low and wait for MISO to be low
	CC2500_NSS_LOW();
	while(GPIO_ReadInputDataBit(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_PIN) != 0) {};
		
	// Send the address of the register
	uint8_t status = CC2500_SendByte(address);
	printf("Status: %d\n", CC2500_Status(status));
		
	// Send data
	while(numBytes > 0)
	{
		status = CC2500_SendByte(*buffer);
		numBytes--;
		buffer++;
	}
	
	// Set chip select to high
	CC2500_NSS_HIGH();
	return SUCCESS;
}

int CC2500_ReadFIFO(uint8_t* buffer, uint8_t address, int numBytes)
{
	// Set chip select to low and wait for MISO to be low
	CC2500_NSS_LOW();
	while(GPIO_ReadInputDataBit(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_PIN) != 0) {};
	
	// Send the address of the register
	uint8_t status = CC2500_SendByte(address);
	
	// Read data
	while(numBytes > 0)
	{
		*buffer = CC2500_SendByte(DUMMY_BYTE);
		numBytes--;
		buffer++;
	}
	
	// Set chip select to high
	CC2500_NSS_HIGH();
	return SUCCESS;
}

int CC2500_Write_Reg(uint8_t* buffer, uint8_t address, int numBytes) {
	// Set chip select to low and wait for MISO to be low
	CC2500_NSS_LOW();
	while(GPIO_ReadInputDataBit(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_PIN) != 0) {};
	
	// Send the address of the register
	uint8_t status = CC2500_SendByte(address);
	
	// Send data
	while(numBytes > 0)
	{
		status = CC2500_SendByte(*buffer);
		//printf("Sending: %x\n", *buffer);
		numBytes--;
		buffer++;
	}
	
	// Set chip select to high
	CC2500_NSS_HIGH();
	return SUCCESS;
}

int CC2500_Read_Reg(uint8_t* buffer, uint8_t address, int numBytes) {
	// Set chip select to low and wait for MISO to be low
	CC2500_NSS_LOW();
	while(GPIO_ReadInputDataBit(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_PIN) != 0) {};
	
	// Send the address of the register
	uint8_t status = CC2500_SendByte(address);
	
	// Read data
	while(numBytes > 0)
	{
		*buffer = CC2500_SendByte(DUMMY_BYTE);
		//printf("Receiving: %x\n", *buffer);
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
	//printf("SendByte: %x\n", byte);
	
	// Wait to receive a byte
	while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	
	// Return the byte read from the SPI bus
	return (uint8_t)SPI_I2S_ReceiveData(CC2500_SPI);
}

// Returns 1 if good, 0 if error, 2 if buffer overflow
int CC2500_Status(char status)
{
	int chip_rdy = status & CHIP_RDY_MASK;
	chip_rdy = chip_rdy >> 3;
	
	if(chip_rdy == CHIP_RDY_ERROR)
		return ERROR;
		
	int state = status & STATE_MASK;
	state = state >> 4;
	
	switch(state)
	{
		case IDLE_STATE: return IDLE_STATE;
		case RX_STATE: return RX_STATE;
		case TX_STATE: return TX_STATE;
		case FSTXON_STATE: return FSTXON_STATE;
		case CALIBRATE_STATE: return CALIBRATE_STATE;
		case SETTLING_STATE: return SETTLING_STATE;
		case RXFIFO_OVERFLOW_STATE: return RXFIFO_OVERFLOW_STATE;
		case TXFIFO_UNDERFLOW_STATE: return TXFIFO_UNDERFLOW_STATE;
		default: return ERROR;
	}
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
	CC2500_CmdStrobe(SRES);
	CC2500_CmdStrobe(SIDLE);
	while(GPIO_ReadInputDataBit(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_PIN) != 0) {};
	
	// Init the wireless settings using burst mode
	// Note that we have to send registers in a certain order (i.e. by their addresses)
	uint8_t registerData[16];
	registerData[0] = SMARTRF_SETTING_IOCFG2;
	CC2500_Write_Reg(registerData, IOCFG2_WRITE_SINGLE, 1);

	registerData[0] = SMARTRF_SETTING_IOCFG0D;
	registerData[1] = SMARTRF_SETTING_FIFOTHR;
	CC2500_Write_Reg(registerData, IOCFG0_WRITE_BURST, 2);
	
	registerData[0] = SMARTRF_SETTING_PKTLEN;
	registerData[1] = SMARTRF_SETTING_PKTCTRL1;
	registerData[2] = SMARTRF_SETTING_PKTCTRL0;
	registerData[3] = SMARTRF_SETTING_ADDR;
	registerData[4] = SMARTRF_SETTING_CHANNR;
	registerData[5] = SMARTRF_SETTING_FSCTRL1;
	registerData[6] = SMARTRF_SETTING_FSCTRL0;
	registerData[7] = SMARTRF_SETTING_FREQ2;
	registerData[8] = SMARTRF_SETTING_FREQ1;
	registerData[9] = SMARTRF_SETTING_FREQ0;
	registerData[10] = SMARTRF_SETTING_MDMCFG4;
	registerData[11] = SMARTRF_SETTING_MDMCFG3;
	registerData[12] = SMARTRF_SETTING_MDMCFG2;
	registerData[13] = SMARTRF_SETTING_MDMCFG1;
	registerData[14] = SMARTRF_SETTING_MDMCFG0;
	registerData[15] = SMARTRF_SETTING_DEVIATN;
	CC2500_Write_Reg(registerData, PKTLEN_WRITE_BURST, 16);
	
	registerData[0] = SMARTRF_SETTING_MCSM0;
	registerData[1] = SMARTRF_SETTING_FOCCFG;
	registerData[2] = SMARTRF_SETTING_BSCFG;
	registerData[3] = SMARTRF_SETTING_AGCCTRL2;
	registerData[4] = SMARTRF_SETTING_AGCCTRL1;
	registerData[5] = SMARTRF_SETTING_AGCCTRL0;
	CC2500_Write_Reg(registerData, MCSM0_WRITE_BURST, 6);
	
	registerData[0] = SMARTRF_SETTING_FREND1;
	registerData[1] = SMARTRF_SETTING_FREND0;
	registerData[2] = SMARTRF_SETTING_FSCAL3;
	registerData[3] = SMARTRF_SETTING_FSCAL2;
	registerData[4] = SMARTRF_SETTING_FSCAL1;
	registerData[5] = SMARTRF_SETTING_FSCAL0;
	CC2500_Write_Reg(registerData, FREND1_WRITE_BURST, 6);
	
	registerData[0] = SMARTRF_SETTING_FSTEST;
	CC2500_Write_Reg(registerData, FSTEST_WRITE_SINGLE, 1);
	
	registerData[0] = SMARTRF_SETTING_TEST2;
	registerData[1] = SMARTRF_SETTING_TEST1;
	registerData[2] = SMARTRF_SETTING_TEST0;
	CC2500_Write_Reg(registerData, TEST2_WRITE_BURST, 3);
	
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
	RCC_AHB1PeriphClockCmd(CC2500_SPI_INT0_GPIO_CLK, ENABLE);		// INT
	RCC_AHB1PeriphClockCmd(CC2500_SPI_INT2_GPIO_CLK, ENABLE);		// INT
	
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
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_INT0_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(CC2500_SPI_INT0_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CC2500_SPI_INT2_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(CC2500_SPI_INT2_GPIO_PORT, &GPIO_InitStructure);
}

void CC2500_TXGDIOInterrupts_Config()
{
	uint8_t buffer;
	
	// Configure GDIO2 to interrupt when TX FIFO is full
	buffer = 0x03;
	CC2500_Write_Reg(&buffer, IOCFG2_WRITE_SINGLE, 1);
	
	// Configure GDIO0 to interrupt when sync word has been received
	buffer = 0x06;
	CC2500_Write_Reg(&buffer, IOCFG0_WRITE_SINGLE, 1);
}

void CC2500_RXGDIOInterrupts_Config()
{
	uint8_t buffer;
	EXTI_InitTypeDef extiInit;
	NVIC_InitTypeDef nvicInit;
	
	// Configure GDIO2 to interrupt when RX FIFO is full
	buffer = 0x01;
	CC2500_Write_Reg(&buffer, IOCFG2_WRITE_SINGLE, 1);
	
	SYSCFG_EXTILineConfig(CC2500_SPI_INT2_EXTI_PORT_SOURCE, CC2500_SPI_INT2_EXTI_PIN_SOURCE);
	
	extiInit.EXTI_Line = CC2500_SPI_INT2_EXTI_LINE;						
	extiInit.EXTI_Mode = EXTI_Mode_Interrupt;		
	extiInit.EXTI_Trigger = EXTI_Trigger_Rising;
	extiInit.EXTI_LineCmd = ENABLE;
	EXTI_Init(&extiInit);
	
	
	nvicInit.NVIC_IRQChannel = CC2500_SPI_INT2_EXTI_IRQn;					
	nvicInit.NVIC_IRQChannelCmd = ENABLE;						
	nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
	nvicInit.NVIC_IRQChannelSubPriority = 1;				
	NVIC_Init(&nvicInit);
	
	// Configure GDIO0 to interrupt when sync word has been received
	buffer = 0x06;
	CC2500_Write_Reg(&buffer, IOCFG0_WRITE_SINGLE, 1);
	
	SYSCFG_EXTILineConfig(CC2500_SPI_INT0_EXTI_PORT_SOURCE, CC2500_SPI_INT0_EXTI_PIN_SOURCE);
	
	extiInit.EXTI_Line = CC2500_SPI_INT0_EXTI_LINE;						
	extiInit.EXTI_Mode = EXTI_Mode_Interrupt;		
	extiInit.EXTI_Trigger = EXTI_Trigger_Rising;
	extiInit.EXTI_LineCmd = ENABLE;
	EXTI_Init(&extiInit);
	
	nvicInit.NVIC_IRQChannel = CC2500_SPI_INT0_EXTI_IRQn;					
	nvicInit.NVIC_IRQChannelCmd = ENABLE;						
	nvicInit.NVIC_IRQChannelPreemptionPriority = 1;
	nvicInit.NVIC_IRQChannelSubPriority = 1;				
	NVIC_Init(&nvicInit);
}
