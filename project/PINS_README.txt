/* -------------------------------------------------------------
GPIO PINS:

C8  - Roll motor PWM
A2 - Pitch motor PWM

/* -------------------------------------------------------------
TIMERS (BASE BOARD):

TIM2 - Interrupt timer for motor thread
TIM9 - Pitch motor PWM (channel 1)
TIM3 - Roll motor PWM (channel 3)

/* -------------------------------------------------------------

Wireless SPI Pins (Remote)
-- Board -- Wireless (eZ430)
PB12: SPI2_NSS -> P17
PB13: SPI2_SCK -> P16
PB14: SPI2_MISO -> P15
PB15: SPI2_MOSI -> P18
Data from "eZ430-RF2500 Development Tool", p. 8 + p.18 (schematic for CC2500)
Discovery Board manual, p24/25 (DocID 022256 Rev 2)

