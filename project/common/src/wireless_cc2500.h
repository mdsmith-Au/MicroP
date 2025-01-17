/*!
 @file wireless_cc2500.h
 @author Nicholas Destounis
 @author Nikolaos Bukas
 @author Michael Smith
 @author Kevin Dam
 @brief Header for TI CC2500 configuration
 */
 
/*! @addtogroup Microp Project Group 1
 *  @{
 */
 
#ifndef _WIRELESS_CC2500_H
#define _WIRELESS_CC2500_H

#include "stdint.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_spi.h"

// Function prototypes

/*!
 Send command strobe
 @param[in] command The command to send
 */
int CC2500_CmdStrobe(int8_t command);

/*!
	Puts wireless into transmit mode
 */
void CC2500_TXMode(void);

/*!
	Puts wireless into receive mode
 */
void CC2500_RXMode(void);

/*!
 Puts wireless into idle mode
 */
void CC2500_Idle(void);

/*!
 Initialize the wireless
 */
void CC2500_Init(void);

/*!
 Transmit wireless message
 @param[in] buffer The message to send
 */
int CC2500_TransmitMessage(void* buffer);

/*!
 Receive wireless message
 @param[in,out] buffer The buffer to store the message in
 */
int CC2500_ReceiveMessage(void* buffer);

/*!
 Read register
 @param[in,out] buffer The destination buffer
 @param[in] header The address
 @param[in] numBytes Number of bytes to read
 */
int CC2500_Read_Reg(int8_t* buffer, int8_t header, int numBytes);

/*!
 Write
 @param[in] buffer The buffer containing data
 @param[in] header The address
 @param[in] numBytes Number of bytes to write
 */
int CC2500_Write_Reg(int8_t* buffer, int8_t header, int numBytes);

/*!
 Write to FIFO buffer
 @param[in] buffer The buffer containing data
 @param[in] header The address
 @param[in] numBytes Number of bytes to write
 */
int CC2500_WriteFIFO(int8_t* buffer, int8_t header, int numBytes);

/*!
 Read FIFO buffer
 @param[in,out] buffer The destination buffer
 @param[in] header The address
 @param[in] numBytes Number of bytes to read
 */
int CC2500_ReadFIFO(int8_t* buffer, int8_t header, int numBytes);

/*!
 Parse wireless status
 @param[in] status The status to check
 */
int CC2500_Status(char status);

// Wireless RF configuration - from TA
#ifndef SMARTRF_CC2500_H
#define SMARTRF_CC2500_H
#define SMARTRF_RADIO_CC2500
#define SMARTRF_SETTING_FSCTRL1 0x0C//0x12 //Frequency offset = 457kHz
#define SMARTRF_SETTING_FSCTRL0 0x00
#define SMARTRF_SETTING_FREQ2 0x5D // Carrier Frequency is 2.433GHz + 8Khz
#define SMARTRF_SETTING_FREQ1 0x93
#define SMARTRF_SETTING_FREQ0 0xC5
// Page 65 of datasheet - FREQ 0,1,2 registers
// We set f_carrier, f_XOSC = 26Mhz
#define SMARTRF_SETTING_MDMCFG4 0x0E //0x2D // BW of channel = 541.666kHz
#define SMARTRF_SETTING_MDMCFG3 0x3B // Baud Rate = 125kb
#define SMARTRF_SETTING_MDMCFG2 0x73 //before demodulator, MSK modulation, 16/16 sync word bits detected
#define SMARTRF_SETTING_MDMCFG1 0x42 //
#define SMARTRF_SETTING_MDMCFG0 0xF8 // Default Channel Spacing of 200kHz
#define SMARTRF_SETTING_CHANNR 0x00 // Channel 0
#define SMARTRF_SETTING_DEVIATN 0x00 //0x01 // 1785kHz
#define SMARTRF_SETTING_FREND1 0xB6
#define SMARTRF_SETTING_FREND0 0x10
#define SMARTRF_SETTING_MCSM0 0x18 // Automatically calibrate When going from IDLE to RX or TX (or FSTXON) check CC2500 datasheet
#define SMARTRF_SETTING_FOCCFG 0x1D // check datasheet
#define SMARTRF_SETTING_BSCFG 0x1C
#define SMARTRF_SETTING_AGCCTRL2 0xC7
#define SMARTRF_SETTING_AGCCTRL1 0x40 //0x00
#define SMARTRF_SETTING_AGCCTRL0 0xB0
#define SMARTRF_SETTING_FSCAL3 0xEA
#define SMARTRF_SETTING_FSCAL2 0x0A
#define SMARTRF_SETTING_FSCAL1 0x00
#define SMARTRF_SETTING_FSCAL0 0x19 //0x11
#define SMARTRF_SETTING_FSTEST 0x59
#define SMARTRF_SETTING_TEST2 0x88
#define SMARTRF_SETTING_TEST1 0x31
#define SMARTRF_SETTING_TEST0 0x0B
#define SMARTRF_SETTING_FIFOTHR 0x07
#define SMARTRF_SETTING_IOCFG2 0x29
#define SMARTRF_SETTING_IOCFG0D 0x06
#define SMARTRF_SETTING_PKTCTRL1 0x04
#define SMARTRF_SETTING_PKTCTRL0 0x05 //0x05 // Fixed Packet Length (0x05)
#define SMARTRF_SETTING_ADDR 0x00 // Global Broadcast Address
#define SMARTRF_SETTING_PKTLEN 0x0A // Packet Length of 10bytes (0xFF)

#endif

#define DUMMY_BYTE 												0x00
#define FIFO_SIZE 64

// Command Strobes
#define SRES															0x30				// Reset chip
#define SFSTXON														0x31
#define SXOFF															0x32
#define SCAL															0x33				// Calibrate freq. synthesizer
#define SRX																0x34				// Enable RX
#define STX																0x35				// Enable TX
#define SIDLE															0x36				// SIDLE
#define SWOR															0x38
#define SPWD															0x39
#define SFRX															0x3A
#define SFTX															0x3B
#define SWORRST														0x3C
#define SNOP															0x3D				// No operation
	
// Registers	
	
#define SINGLE_WRITE_OFFSET								0x00
#define SINGLE_READ_OFFSET								0x80
#define BURST_WRITE_OFFSET								0x40
#define BURST_READ_OFFSET									0xC0

#define PARTNUM														0x30 + BURST_READ_OFFSET
#define VERSION														0x31 + BURST_READ_OFFSET
#define FREQEST														0x32 + BURST_READ_OFFSET
#define LQI																0x33 + BURST_READ_OFFSET
#define RSSI															0x34 + BURST_READ_OFFSET
#define MARCSTATE													0x35 + BURST_READ_OFFSET
#define WORTIME1													0x36 + BURST_READ_OFFSET
#define WORTIME0													0x37 + BURST_READ_OFFSET
#define PKTSTATUS													0x38 + BURST_READ_OFFSET
#define VCO_VC_DAC												0x39 + BURST_READ_OFFSET
#define TXBYTES														0x3A + BURST_READ_OFFSET
#define RXBYTES														0x3B + BURST_READ_OFFSET
#define RCCTRL1_STATUS										0x3C + BURST_READ_OFFSET
#define RCCTRL0_STATUS										0x3D + BURST_READ_OFFSET
	
#define IOCFG2_WRITE_SINGLE								0x00 + SINGLE_WRITE_OFFSET
#define IOCFG1_WRITE_SINGLE								0x01 + SINGLE_WRITE_OFFSET
#define IOCFG0_WRITE_SINGLE								0x02 + SINGLE_WRITE_OFFSET
#define FIFOTHR_WRITE_SINGLE							0x03 + SINGLE_WRITE_OFFSET
#define SYNC1_WRITE_SINGLE								0x04 + SINGLE_WRITE_OFFSET
#define SYNC0_WRITE_SINGLE								0x05 + SINGLE_WRITE_OFFSET
#define PKTLEN_WRITE_SINGLE								0x06 + SINGLE_WRITE_OFFSET
#define PKTCTRL1_WRITE_SINGLE							0x07 + SINGLE_WRITE_OFFSET
#define PKTCTRL0_WRITE_SINGLE							0x08 + SINGLE_WRITE_OFFSET
#define ADDR_WRITE_SINGLE									0x09 + SINGLE_WRITE_OFFSET
#define CHANNR_WRITE_SINGLE						  	0x0A + SINGLE_WRITE_OFFSET
#define FSCTRL1_WRITE_SINGLE							0x0B + SINGLE_WRITE_OFFSET
#define FSCTRL0_WRITE_SINGLE							0x0C + SINGLE_WRITE_OFFSET
#define FREQ2_WRITE_SINGLE						  	0x0D + SINGLE_WRITE_OFFSET
#define FREQ1_WRITE_SINGLE						  	0x0E + SINGLE_WRITE_OFFSET
#define FREQ0_WRITE_SINGLE						  	0x0F + SINGLE_WRITE_OFFSET
#define MDMCFG4_WRITE_SINGLE							0x10 + SINGLE_WRITE_OFFSET
#define MDMCFG3_WRITE_SINGLE            	0x11 + SINGLE_WRITE_OFFSET
#define MDMCFG2_WRITE_SINGLE            	0x12 + SINGLE_WRITE_OFFSET
#define MDMCFG1_WRITE_SINGLE            	0x13 + SINGLE_WRITE_OFFSET
#define MDMCFG0_WRITE_SINGLE            	0x14 + SINGLE_WRITE_OFFSET
#define DEVIATN_WRITE_SINGLE            	0x15 + SINGLE_WRITE_OFFSET
#define MCSM2_WRITE_SINGLE              	0x16 + SINGLE_WRITE_OFFSET
#define MCSM1_WRITE_SINGLE              	0x17 + SINGLE_WRITE_OFFSET
#define MCSM0_WRITE_SINGLE              	0x18 + SINGLE_WRITE_OFFSET
#define FOCCFG_WRITE_SINGLE             	0x19 + SINGLE_WRITE_OFFSET
#define BSCFG_WRITE_SINGLE              	0x1A + SINGLE_WRITE_OFFSET
#define AGCCTRL2_WRITE_SINGLE           	0x1B + SINGLE_WRITE_OFFSET
#define AGCCTRL1_WRITE_SINGLE           	0x1C + SINGLE_WRITE_OFFSET
#define AGCCTRL0_WRITE_SINGLE           	0x1D + SINGLE_WRITE_OFFSET
#define WOREVT1_WRITE_SINGLE            	0x1E + SINGLE_WRITE_OFFSET
#define WOREVT0_WRITE_SINGLE            	0x1F + SINGLE_WRITE_OFFSET
#define WORCTRL_WRITE_SINGLE							0x20 + SINGLE_WRITE_OFFSET
#define FREND1_WRITE_SINGLE             	0x21 + SINGLE_WRITE_OFFSET
#define FREND0_WRITE_SINGLE             	0x22 + SINGLE_WRITE_OFFSET
#define FSCAL3_WRITE_SINGLE             	0x23 + SINGLE_WRITE_OFFSET
#define FSCAL2_WRITE_SINGLE             	0x24 + SINGLE_WRITE_OFFSET
#define FSCAL1_WRITE_SINGLE             	0x25 + SINGLE_WRITE_OFFSET
#define FSCAL0_WRITE_SINGLE             	0x26 + SINGLE_WRITE_OFFSET
#define RCCTRL1_WRITE_SINGLE            	0x27 + SINGLE_WRITE_OFFSET
#define RCCTRL0_WRITE_SINGLE            	0x28 + SINGLE_WRITE_OFFSET
#define FSTEST_WRITE_SINGLE             	0x29 + SINGLE_WRITE_OFFSET
#define PTEST_WRITE_SINGLE              	0x2A + SINGLE_WRITE_OFFSET
#define AGCTEST_WRITE_SINGLE            	0x2B + SINGLE_WRITE_OFFSET
#define TEST2_WRITE_SINGLE              	0x2C + SINGLE_WRITE_OFFSET
#define TEST1_WRITE_SINGLE              	0x2D + SINGLE_WRITE_OFFSET
#define TEST0_WRITE_SINGLE              	0x2E + SINGLE_WRITE_OFFSET
	
#define IOCFG2_WRITE_BURST								0x00 + BURST_WRITE_OFFSET
#define IOCFG1_WRITE_BURST              	0x01 + BURST_WRITE_OFFSET
#define IOCFG0_WRITE_BURST              	0x02 + BURST_WRITE_OFFSET
#define FIFOTHR_WRITE_BURST             	0x03 + BURST_WRITE_OFFSET
#define SYNC1_WRITE_BURST               	0x04 + BURST_WRITE_OFFSET
#define SYNC0_WRITE_BURST               	0x05 + BURST_WRITE_OFFSET
#define PKTLEN_WRITE_BURST              	0x06 + BURST_WRITE_OFFSET
#define PKTCTRL1_WRITE_BURST            	0x07 + BURST_WRITE_OFFSET
#define PKTCTRL0_WRITE_BURST            	0x08 + BURST_WRITE_OFFSET
#define ADDR_WRITE_BURST                	0x09 + BURST_WRITE_OFFSET
#define CHANNR_WRITE_BURST              	0x0A + BURST_WRITE_OFFSET
#define FSCTRL1_WRITE_BURST             	0x0B + BURST_WRITE_OFFSET
#define FSCTRL0_WRITE_BURST             	0x0C + BURST_WRITE_OFFSET
#define FREQ2_WRITE_BURST               	0x0D + BURST_WRITE_OFFSET
#define FREQ1_WRITE_BURST               	0x0E + BURST_WRITE_OFFSET
#define FREQ0_WRITE_BURST               	0x0F + BURST_WRITE_OFFSET
#define MDMCFG4_WRITE_BURST             	0x10 + BURST_WRITE_OFFSET
#define MDMCFG3_WRITE_BURST             	0x11 + BURST_WRITE_OFFSET
#define MDMCFG2_WRITE_BURST             	0x12 + BURST_WRITE_OFFSET
#define MDMCFG1_WRITE_BURST             	0x13 + BURST_WRITE_OFFSET
#define MDMCFG0_WRITE_BURST             	0x14 + BURST_WRITE_OFFSET
#define DEVIATN_WRITE_BURST             	0x15 + BURST_WRITE_OFFSET
#define MCSM2_WRITE_BURST               	0x16 + BURST_WRITE_OFFSET
#define MCSM1_WRITE_BURST               	0x17 + BURST_WRITE_OFFSET
#define MCSM0_WRITE_BURST               	0x18 + BURST_WRITE_OFFSET
#define FOCCFG_WRITE_BURST              	0x19 + BURST_WRITE_OFFSET
#define BSCFG_WRITE_BURST               	0x1A + BURST_WRITE_OFFSET
#define AGCCTRL2_WRITE_BURST            	0x1B + BURST_WRITE_OFFSET
#define AGCCTRL1_WRITE_BURST            	0x1C + BURST_WRITE_OFFSET
#define AGCCTRL0_WRITE_BURST            	0x1D + BURST_WRITE_OFFSET
#define WOREVT1_WRITE_BURST             	0x1E + BURST_WRITE_OFFSET
#define WOREVT0_WRITE_BURST             	0x1F + BURST_WRITE_OFFSET
#define WORCTRL_WRITE_BURST             	0x20 + BURST_WRITE_OFFSET
#define FREND1_WRITE_BURST              	0x21 + BURST_WRITE_OFFSET
#define FREND0_WRITE_BURST              	0x22 + BURST_WRITE_OFFSET
#define FSCAL3_WRITE_BURST              	0x23 + BURST_WRITE_OFFSET
#define FSCAL2_WRITE_BURST              	0x24 + BURST_WRITE_OFFSET
#define FSCAL1_WRITE_BURST              	0x25 + BURST_WRITE_OFFSET
#define FSCAL0_WRITE_BURST              	0x26 + BURST_WRITE_OFFSET
#define RCCTRL1_WRITE_BURST             	0x27 + BURST_WRITE_OFFSET
#define RCCTRL0_WRITE_BURST             	0x28 + BURST_WRITE_OFFSET
#define FSTEST_WRITE_BURST              	0x29 + BURST_WRITE_OFFSET
#define PTEST_WRITE_BURST               	0x2A + BURST_WRITE_OFFSET
#define AGCTEST_WRITE_BURST             	0x2B + BURST_WRITE_OFFSET
#define TEST2_WRITE_BURST               	0x2C + BURST_WRITE_OFFSET
#define TEST1_WRITE_BURST               	0x2D + BURST_WRITE_OFFSET
#define TEST0_WRITE_BURST               	0x2E + BURST_WRITE_OFFSET
	
#define IOCFG2_READ_SINGLE								0x00 + SINGLE_READ_OFFSET
#define IOCFG1_READ_SINGLE              	0x01 + SINGLE_READ_OFFSET
#define IOCFG0_READ_SINGLE              	0x02 + SINGLE_READ_OFFSET
#define FIFOTHR_READ_SINGLE             	0x03 + SINGLE_READ_OFFSET
#define SYNC1_READ_SINGLE               	0x04 + SINGLE_READ_OFFSET
#define SYNC0_READ_SINGLE               	0x05 + SINGLE_READ_OFFSET
#define PKTLEN_READ_SINGLE              	0x06 + SINGLE_READ_OFFSET
#define PKTCTRL1_READ_SINGLE            	0x07 + SINGLE_READ_OFFSET
#define PKTCTRL0_READ_SINGLE            	0x08 + SINGLE_READ_OFFSET
#define ADDR_READ_SINGLE                	0x09 + SINGLE_READ_OFFSET
#define CHANNR_READ_SINGLE              	0x0A + SINGLE_READ_OFFSET
#define FSCTRL1_READ_SINGLE             	0x0B + SINGLE_READ_OFFSET
#define FSCTRL0_READ_SINGLE             	0x0C + SINGLE_READ_OFFSET
#define FREQ2_READ_SINGLE               	0x0D + SINGLE_READ_OFFSET
#define FREQ1_READ_SINGLE               	0x0E + SINGLE_READ_OFFSET
#define FREQ0_READ_SINGLE               	0x0F + SINGLE_READ_OFFSET
#define MDMCFG4_READ_SINGLE             	0x10 + SINGLE_READ_OFFSET
#define MDMCFG3_READ_SINGLE             	0x11 + SINGLE_READ_OFFSET
#define MDMCFG2_READ_SINGLE             	0x12 + SINGLE_READ_OFFSET
#define MDMCFG1_READ_SINGLE             	0x13 + SINGLE_READ_OFFSET
#define MDMCFG0_READ_SINGLE             	0x14 + SINGLE_READ_OFFSET
#define DEVIATN_READ_SINGLE             	0x15 + SINGLE_READ_OFFSET
#define MCSM2_READ_SINGLE               	0x16 + SINGLE_READ_OFFSET
#define MCSM1_READ_SINGLE               	0x17 + SINGLE_READ_OFFSET
#define MCSM0_READ_SINGLE               	0x18 + SINGLE_READ_OFFSET
#define FOCCFG_READ_SINGLE              	0x19 + SINGLE_READ_OFFSET
#define BSCFG_READ_SINGLE               	0x1A + SINGLE_READ_OFFSET
#define AGCCTRL2_READ_SINGLE            	0x1B + SINGLE_READ_OFFSET
#define AGCCTRL1_READ_SINGLE            	0x1C + SINGLE_READ_OFFSET
#define AGCCTRL0_READ_SINGLE            	0x1D + SINGLE_READ_OFFSET
#define WOREVT1_READ_SINGLE             	0x1E + SINGLE_READ_OFFSET
#define WOREVT0_READ_SINGLE             	0x1F + SINGLE_READ_OFFSET
#define WORCTRL_READ_SINGLE             	0x20 + SINGLE_READ_OFFSET
#define FREND1_READ_SINGLE              	0x21 + SINGLE_READ_OFFSET
#define FREND0_READ_SINGLE              	0x22 + SINGLE_READ_OFFSET
#define FSCAL3_READ_SINGLE              	0x23 + SINGLE_READ_OFFSET
#define FSCAL2_READ_SINGLE              	0x24 + SINGLE_READ_OFFSET
#define FSCAL1_READ_SINGLE              	0x25 + SINGLE_READ_OFFSET
#define FSCAL0_READ_SINGLE              	0x26 + SINGLE_READ_OFFSET
#define RCCTRL1_READ_SINGLE             	0x27 + SINGLE_READ_OFFSET
#define RCCTRL0_READ_SINGLE             	0x28 + SINGLE_READ_OFFSET
#define FSTEST_READ_SINGLE              	0x29 + SINGLE_READ_OFFSET
#define PTEST_READ_SINGLE               	0x2A + SINGLE_READ_OFFSET
#define AGCTEST_READ_SINGLE             	0x2B + SINGLE_READ_OFFSET
#define TEST2_READ_SINGLE               	0x2C + SINGLE_READ_OFFSET
#define TEST1_READ_SINGLE               	0x2D + SINGLE_READ_OFFSET
#define TEST0_READ_SINGLE               	0x2E + SINGLE_READ_OFFSET
	
#define IOCFG2_READ_BURST									0x00 + BURST_READ_OFFSET
#define IOCFG1_READ_BURST               	0x01 + BURST_READ_OFFSET
#define IOCFG0_READ_BURST               	0x02 + BURST_READ_OFFSET
#define FIFOTHR_READ_BURST              	0x03 + BURST_READ_OFFSET
#define SYNC1_READ_BURST                	0x04 + BURST_READ_OFFSET
#define SYNC0_READ_BURST                	0x05 + BURST_READ_OFFSET
#define PKTLEN_READ_BURST               	0x06 + BURST_READ_OFFSET
#define PKTCTRL1_READ_BURST             	0x07 + BURST_READ_OFFSET
#define PKTCTRL0_READ_BURST             	0x08 + BURST_READ_OFFSET
#define ADDR_READ_BURST                 	0x09 + BURST_READ_OFFSET
#define CHANNR_READ_BURST               	0x0A + BURST_READ_OFFSET
#define FSCTRL1_READ_BURST              	0x0B + BURST_READ_OFFSET
#define FSCTRL0_READ_BURST              	0x0C + BURST_READ_OFFSET
#define FREQ2_READ_BURST                	0x0D + BURST_READ_OFFSET
#define FREQ1_READ_BURST                	0x0E + BURST_READ_OFFSET
#define FREQ0_READ_BURST                	0x0F + BURST_READ_OFFSET
#define MDMCFG4_READ_BURST              	0x10 + BURST_READ_OFFSET
#define MDMCFG3_READ_BURST              	0x11 + BURST_READ_OFFSET
#define MDMCFG2_READ_BURST              	0x12 + BURST_READ_OFFSET
#define MDMCFG1_READ_BURST              	0x13 + BURST_READ_OFFSET
#define MDMCFG0_READ_BURST              	0x14 + BURST_READ_OFFSET
#define DEVIATN_READ_BURST              	0x15 + BURST_READ_OFFSET
#define MCSM2_READ_BURST                	0x16 + BURST_READ_OFFSET
#define MCSM1_READ_BURST                	0x17 + BURST_READ_OFFSET
#define MCSM0_READ_BURST                	0x18 + BURST_READ_OFFSET
#define FOCCFG_READ_BURST               	0x19 + BURST_READ_OFFSET
#define BSCFG_READ_BURST                	0x1A + BURST_READ_OFFSET
#define AGCCTRL2_READ_BURST             	0x1B + BURST_READ_OFFSET
#define AGCCTRL1_READ_BURST             	0x1C + BURST_READ_OFFSET
#define AGCCTRL0_READ_BURST             	0x1D + BURST_READ_OFFSET
#define WOREVT1_READ_BURST              	0x1E + BURST_READ_OFFSET
#define WOREVT0_READ_BURST              	0x1F + BURST_READ_OFFSET
#define WORCTRL_READ_BURST              	0x20 + BURST_READ_OFFSET
#define FREND1_READ_BURST               	0x21 + BURST_READ_OFFSET
#define FREND0_READ_BURST               	0x22 + BURST_READ_OFFSET
#define FSCAL3_READ_BURST               	0x23 + BURST_READ_OFFSET
#define FSCAL2_READ_BURST               	0x24 + BURST_READ_OFFSET
#define FSCAL1_READ_BURST               	0x25 + BURST_READ_OFFSET
#define FSCAL0_READ_BURST               	0x26 + BURST_READ_OFFSET
#define RCCTRL1_READ_BURST              	0x27 + BURST_READ_OFFSET
#define RCCTRL0_READ_BURST              	0x28 + BURST_READ_OFFSET
#define FSTEST_READ_BURST               	0x29 + BURST_READ_OFFSET
#define PTEST_READ_BURST                	0x2A + BURST_READ_OFFSET
#define AGCTEST_READ_BURST              	0x2B + BURST_READ_OFFSET
#define TEST2_READ_BURST                	0x2C + BURST_READ_OFFSET
#define TEST1_READ_BURST                	0x2D + BURST_READ_OFFSET
#define TEST0_READ_BURST                	0x2E + BURST_READ_OFFSET
	
#define CC2500_CONFG_REGS_ADDR_MIN				0x00
#define CC2500_CONFG_REGS_ADDR_MAX				0x2E
#define CC2500_STATUS_REGS_ADDR_MIN				0x30
#define CC2500_STATUS_REGS_ADDR_MAX				0x3F
	
#define FIFO_WRITE_ADDRESS					 			0x3F
#define FIFO_READ_ADDRESS									0xBF
#define FIFO_WRITE_BURST_ADDRESS					0x7F
#define FIFO_READ_BURST_ADDRESS						0xFF
	
	
// Errors and states
#define ERROR															0x09
#define SUCCESS														0x08
	
#define CHIP_RDY_MASK											0x80
#define STATE_MASK												0x70
#define FIFO_BYTES_MASK										0xF0
	
#define CHIP_RDY_ERROR										0x01
	
#define IDLE_STATE												0x00
#define RX_STATE													0x01
#define TX_STATE													0x02
#define FSTXON_STATE											0x03
#define CALIBRATE_STATE										0x04
#define SETTLING_STATE										0x05
#define RXFIFO_OVERFLOW_STATE							0x06
#define TXFIFO_UNDERFLOW_STATE						0x07

// SPI Interface Pins

#define CC2500_SPI                      	SPI2
#define CC2500_SPI_CLK                  	RCC_APB1Periph_SPI2
	
#define CC2500_SPI_SCK_PIN              	GPIO_Pin_13                 
#define CC2500_SPI_SCK_GPIO_PORT        	GPIOB                       
#define CC2500_SPI_SCK_GPIO_CLK         	RCC_AHB1Periph_GPIOB
#define CC2500_SPI_SCK_SOURCE           	GPIO_PinSource13
#define CC2500_SPI_SCK_AF               	GPIO_AF_SPI2
	
#define CC2500_SPI_MISO_PIN             	GPIO_Pin_14                 
#define CC2500_SPI_MISO_GPIO_PORT       	GPIOB                       
#define CC2500_SPI_MISO_GPIO_CLK        	RCC_AHB1Periph_GPIOB
#define CC2500_SPI_MISO_SOURCE          	GPIO_PinSource14
#define CC2500_SPI_MISO_AF              	GPIO_AF_SPI2
	
#define CC2500_SPI_MOSI_PIN             	GPIO_Pin_15                 
#define CC2500_SPI_MOSI_GPIO_PORT       	GPIOB                       
#define CC2500_SPI_MOSI_GPIO_CLK        	RCC_AHB1Periph_GPIOB
#define CC2500_SPI_MOSI_SOURCE          	GPIO_PinSource15
#define CC2500_SPI_MOSI_AF              	GPIO_AF_SPI2
	
#define CC2500_SPI_NSS_PIN								GPIO_Pin_12
#define CC2500_SPI_NSS_GPIO_PORT					GPIOB
#define CC2500_SPI_NSS_GPIO_CLK						RCC_AHB1Periph_GPIOE
#define CC2500_SPI_NSS_SOURCE           	GPIO_PinSource12
#define CC2500_SPI_NSS_AF               	GPIO_AF_SPI2

#define CC2500_SPI_INT0_PIN              	GPIO_Pin_11                 
#define CC2500_SPI_INT0_GPIO_PORT        	GPIOB                      
#define CC2500_SPI_INT0_GPIO_CLK         	RCC_AHB1Periph_GPIOB
#define CC2500_SPI_INT0_EXTI_LINE        	EXTI_Line11
#define CC2500_SPI_INT0_EXTI_PORT_SOURCE 	EXTI_PortSourceGPIOB
#define CC2500_SPI_INT0_EXTI_PIN_SOURCE  	EXTI_PinSource11
#define CC2500_SPI_INT0_EXTI_IRQn        	EXTI15_10_IRQn

#define CC2500_SPI_INT2_PIN              	GPIO_Pin_10                  
#define CC2500_SPI_INT2_GPIO_PORT        	GPIOB                       
#define CC2500_SPI_INT2_GPIO_CLK         	RCC_AHB1Periph_GPIOB
#define CC2500_SPI_INT2_EXTI_LINE        	EXTI_Line10
#define CC2500_SPI_INT2_EXTI_PORT_SOURCE 	EXTI_PortSourceGPIOB
#define CC2500_SPI_INT2_EXTI_PIN_SOURCE  	EXTI_PinSource10
#define CC2500_SPI_INT2_EXTI_IRQn        	EXTI15_10_IRQn 

#define CC2500_NSS_LOW()       						GPIO_ResetBits(CC2500_SPI_NSS_GPIO_PORT, CC2500_SPI_NSS_PIN)
#define CC2500_NSS_HIGH()      						GPIO_SetBits(CC2500_SPI_NSS_GPIO_PORT, CC2500_SPI_NSS_PIN)

#endif

//! @} 
