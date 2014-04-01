#include "LCD_driver.h"

// Private method definitions
void LCD_GPIO_setup(void);
void printToAddress(char* string, int length, uint8_t address);
void send(char data, int type);
void delay(void);

// Mutex to prevent conflicting singals
// being sent to the LCD (i.e. start sending data, send command halfway through)
osMutexDef (MutexLCD);
osMutexId Mutex_LCD_id;

// Perform initial LCD configuration, such as GPIO commands
void LCD_configure(void) {
    LCD_GPIO_setup();
		
		// Create Mutex
    Mutex_LCD_id = osMutexCreate(osMutex(MutexLCD));
	
    // Send initial setup commands to the display, such as enabling the second row,
    // ensuring the display is on, clearing it and resetting the cursor.
    send(functionSet, CMD);
    send(displayOn, CMD);
		send(displayCursorHome, CMD);
    clearLCD(); 
    
}

// Print a string to the LCD, starting at the beginning of a specified row
// Rows start from 1; only 2 rows
// Length limited to 24 characters
// NOTE: time to execute is approx. 1msec/character in string + 1 msec
void printLCDString(char* string, int row) {
    printLCDToPos(string, row, 1);
}

// Print a string to the LCD starting at a given row/column
// Rows and columns start from 1; 2 rows, 24 columns
// Length limited to 24 characters
// NOTE: time to execute is approx. 1msec/character in string + 1 msec
void printLCDToPos(char* string, int row, int col) {
	int length = strlen(string);
    
    // Prevent users from printing past visible area (i.e. into second row)
    if (length > 24) {
        length = 24;
    }

    // Row limited to only 1 or 2
    if ((row > 2) || (row < 1)) {
        return;
    }

    // Columns limited from 1 to 24
    if ((col < 1) || (col > 24)) {
        return;
    }
    // Exceptional situation: a high column number with a moderate length string
    // could ALSO go into the second row -> limit length to max visible area
    if ((col > 16) && (length > 8)) {
        length = 8;
    }

    // Use printToAddress, with calculations:
    // Addresses start from 0, columns from 1
    // Address starts from 0: row 1, col 1.  Columns in increasing order, max visible is col 24
    // Addresses are in decimal, although when accessing row 2, the addresses start at BCD 40 = 0x40
    printToAddress(string, length, (col-1) + (row-1)*0x40);

}

// Clears the LCD screen
// NOTE: Time to execute is approx. 1 msec
void clearLCD() {
    osMutexWait(Mutex_LCD_id, osWaitForever);
    send(clearDisplay, CMD);
		// Must wait 1.5 ms...approx to 2
		osDelay(2);
    osMutexRelease(Mutex_LCD_id);
}

// Private method: print a string to a position using the LCD's address format spec
void printToAddress(char* string, int length, uint8_t address) {
    osMutexWait(Mutex_LCD_id, osWaitForever);
    // Address commands have a 1 in the 8th bit (MSB), followed by the address -> OR with 0x80
    // This sets the cursor to that position
    send(0x80 | address, CMD);
    
    // Send string one char at a time
    for (int i = 0; i < length; i++) {
        send(string[i], ASCII);
    }
    
    osMutexRelease(Mutex_LCD_id);
}

// Private method: send a command (with bits specified by the char)
// or an ASCII character, as specified in the argument type
// Signals that identify the bits as a command or dataare handled automatically, as 
// are proper delays
// See Generic Keypad and LCD Tutorial, p.2
// For chracters, each new character is written in sequence after the old one, unless commands
// are sent to explicitely override said functionality
void send(char data, int type) {
    // Place command/data on data lines, one bit at a time
    GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_0, (BitAction)(data & BIT1));
    GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_1, (BitAction)(data & BIT2));
    GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_2, (BitAction)(data & BIT3));
    GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_3, (BitAction)(data & BIT4));
    GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_4, (BitAction)(data & BIT5));
    GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_5, (BitAction)(data & BIT6));
    GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_6, (BitAction)(data & BIT7));
    GPIO_WriteBit(LCD_GPIO_BANK, LCD_DATA_7, (BitAction)(data & BIT8));
  
    // Command
    if (type == CMD) {
        // Command mode has REG_SEL = 0, Read/Write = 0, Enable 0
      GPIO_ResetBits(LCD_GPIO_BANK, LCD_REG_SEL | LCD_RW | LCD_EN);
    }
    // Data
    else {
      // ASCII write has REG_SEL = 1, Read/Write = 0, Enable 0
      GPIO_ResetBits(LCD_GPIO_BANK, LCD_RW | LCD_EN);
      GPIO_SetBits(LCD_GPIO_BANK, LCD_REG_SEL);
    }
    
    // Bring enable up/down -> trig on falling edge
    GPIO_SetBits(LCD_GPIO_BANK, LCD_EN);
    // Give LCD time to process and reset
    delay();
    GPIO_ResetBits(LCD_GPIO_BANK, LCD_EN);
}


void LCD_GPIO_setup() {
    GPIO_InitTypeDef LCD_GPIO_InitStruct;

    // Enable requisite GPIO bank
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    // Set all pins for the LCD to output.  We do not read any data
    LCD_GPIO_InitStruct.GPIO_Pin   = LCD_DATA_0 | LCD_DATA_1 | LCD_DATA_2 | LCD_DATA_3 | LCD_DATA_4 | LCD_DATA_5 | LCD_DATA_6 | LCD_DATA_7 | LCD_EN | LCD_RW | LCD_REG_SEL;
    LCD_GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT; 
    LCD_GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    LCD_GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz; 
    LCD_GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(LCD_GPIO_BANK, &LCD_GPIO_InitStruct);
}

void delay() {
	int i = 0;
	while (i < DELAY) {
		i++;
	}
}

