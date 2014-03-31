#ifndef _WIRELESS_CC2500_H
#define _WIRELESS_CC2500_H
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

#include "stdint.h"

// Wireless RF configuration - from TA
#ifndef SMARTRF_CC2500_H
#define SMARTRF_CC2500_H
#define SMARTRF_RADIO_CC2500
#define SMARTRF_SETTING_FSCTRL1 0x0C//0x12 //Frequency offset = 457kHz
#define SMARTRF_SETTING_FSCTRL0 0x00
#define SMARTRF_SETTING_FREQ2 0x5D // Carrier Frequency is 2.433GHz
#define SMARTRF_SETTING_FREQ1 0x93
#define SMARTRF_SETTING_FREQ0 0xB1
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
//! @} 

typedef struct {
	int test;
}WirelessInitStruct;

void WirelessInit(WirelessInitStruct* initStruct);

// Wireless data send/receive
void SendData(void* data, uint8_t address, int size);
void ReceiveData(void* data, uint8_t address, int size);

// SPI Interface Pins

#define CC2500_SPI                       SPI2
#define CC2500_SPI_CLK                   RCC_APB2Periph_SPI2

#define CC2500_SPI_SCK_PIN               GPIO_Pin_13                  /* PA.05 */
#define CC2500_SPI_SCK_GPIO_PORT         GPIOB                       /* GPIOA */
#define CC2500_SPI_SCK_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define CC2500_SPI_SCK_SOURCE            GPIO_PinSource13
#define CC2500_SPI_SCK_AF                GPIO_AF_SPI2

#define CC2500_SPI_MISO_PIN              GPIO_Pin_14                  /* PA.6 */
#define CC2500_SPI_MISO_GPIO_PORT        GPIOB                       /* GPIOA */
#define CC2500_SPI_MISO_GPIO_CLK         RCC_AHB1Periph_GPIOB
#define CC2500_SPI_MISO_SOURCE           GPIO_PinSource14
#define CC2500_SPI_MISO_AF               GPIO_AF_SPI2

#define CC2500_SPI_MOSI_PIN              GPIO_Pin_15                  /* PA.7 */
#define CC2500_SPI_MOSI_GPIO_PORT        GPIOB                       /* GPIOA */
#define CC2500_SPI_MOSI_GPIO_CLK         RCC_AHB1Periph_GPIOB
#define CC2500_SPI_MOSI_SOURCE           GPIO_PinSource15
#define CC2500_SPI_MOSI_AF               GPIO_AF_SPI2

#define CC2500_SPI_NSS_PIN							 GPIO_Pin_12
#define CC2500_SPI_NSS_GPIO_PORT				 GPIOB
#define CC2500_SPI_NSS_GPIO_CLK					 RCC_AHB1Periph_GPIOE
#define CC2500_SPI_NSS_SOURCE            GPIO_PinSource12
#define CC2500_SPI_NSS_AF                GPIO_AF_SPI2

#define CC2500_SPI_INT1_PIN              GPIO_Pin_0                  /* PE.00 */
#define CC2500_SPI_INT1_GPIO_PORT        GPIOE                       /* GPIOE */
#define CC2500_SPI_INT1_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define CC2500_SPI_INT1_EXTI_LINE        EXTI_Line0
#define CC2500_SPI_INT1_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define CC2500_SPI_INT1_EXTI_PIN_SOURCE  EXTI_PinSource0
#define CC2500_SPI_INT1_EXTI_IRQn        EXTI0_IRQn 

#define CC2500_SPI_INT2_PIN              GPIO_Pin_1                  /* PE.01 */
#define CC2500_SPI_INT2_GPIO_PORT        GPIOE                       /* GPIOE */
#define CC2500_SPI_INT2_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define CC2500_SPI_INT2_EXTI_LINE        EXTI_Line1
#define CC2500_SPI_INT2_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define CC2500_SPI_INT2_EXTI_PIN_SOURCE  EXTI_PinSource1
#define CC2500_SPI_INT2_EXTI_IRQn        EXTI1_IRQn 

#endif
