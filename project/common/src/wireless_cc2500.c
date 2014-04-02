#include "wireless_cc2500.h"

int CC2500_SPI_Write(uint8_t* buffer, uint8_t address, int numBytes);
int CC2500_SPI_Read(uint8_t* buffer, uint8_t address, int numBytes);
void CC2500_SPI_Send_Byte(char byte);
uint8_t CC2500_SPI_Send_Receive_Byte(char byte);
void CC2500_LowLevelInit(void);
void CC2500_LowLevelWireless_Init(void);
int CC2500_Check_Status(char status);
void delay(void);

void CC2500_Init(WirelessInitStruct* wirelessStruct)
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

int CC2500_SPI_Write(uint8_t* buffer, uint8_t address, int numBytes) {
	// By default, no burst or strobe
  int burst = 0;
  int strobe = 0;
  int fifo = 0;
  
  // Normal register read/write with more than one byte: burst
  if ((address >= 0x00 && address <= 0x2E) && (numBytes > 1)) {
      burst = 1;
  }
  else if (address >= 0x30 && address <= 0x3D) {
    // Strobe = no burst
    strobe = 1;
  }
  else if (address == 0x3F) {
      fifo = 1;
      if (numBytes > 1) {
        burst = 1;
      }
  }
  
  // Add R/W bit to address (write setting)
  address |= 0x80;
  
  CC2500_NSS_LOW();
  
  if (fifo) {
    // Burst mode FIFO
    if (burst) {
        address |= 0x40;
        uint8_t status = CC2500_SPI_Send_Receive_Byte(address);
        if (!CC2500_Check_Status(status)) {
          return 0;
        }
        while (numBytes >= 1) {
          status = CC2500_SPI_Send_Receive_Byte(*buffer);
          buffer++;
          numBytes--;
          if (CC2500_Check_Status(status) == 2) {
            delay();
          }
        }
      }
      // Normal FIFO : write one byte
      else {
        uint8_t status = CC2500_SPI_Send_Receive_Byte(address);
        if (!CC2500_Check_Status(status)) {
          return 0;
        }
        
        status = CC2500_SPI_Send_Receive_Byte(*buffer);
        if (!CC2500_Check_Status(status)) {
          return 0;
        }
      }  
  }
  // Not a buffer write; check if strobe
  else if (strobe) {
    uint8_t status = CC2500_SPI_Send_Receive_Byte(address);
    if (!CC2500_Check_Status(status)) {       
      return 0;
    }
  }
  
  // Normal Read/Write to registers
  else {
    if (burst) {
      
    }
    else {
      
    }
  }
  
}

int CC2500_SPI_Read(uint8_t* buffer, uint8_t address, int numBytes) {

}

uint8_t CC2500_SPI_Send_Receive_Byte(char byte) {
	// Wait until the time is right
	while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_TXE) == RESET) ;
	
	// Send data
	SPI_I2S_SendData(CC2500_SPI, byte);
	
	// Wait to receive a response (status)
	while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	
	return (uint8_t)SPI_I2S_ReceiveData(CC2500_SPI);
}

void CC2500_SPI_Send_Byte(char byte) {
	// Wait until the time is right
	while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_TXE) == RESET) ;
	
	// Send data
	SPI_I2S_SendData(CC2500_SPI, byte);
}

// Returns 1 if good, 0 if error, 2 if buffer overflow
int CC2500_Check_Status(char status) {
  
}

//TODO: Delay to wait for buffer to empty
void delay() {
  
}