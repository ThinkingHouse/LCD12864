#include "mbed.h"
#include "display.h"

DigitalOut lcdLed(PB_10);

/*
  ST7920 12864ZB LCD module via the clock synchronized serial interface.
 
  Data sheet for 12864ZB lcd display module:
    http://www.digole.com/images/file/Digole_12864_LCD.pdf
  
  LCD Pin Configurations:
    |-----|------|--------|-------|
    | LCD | DESCRIPTIONS  | KL25Z |
    | PIN |SERIAL|PARALLEL|  PIN  |
    | ----|------|--------|-------|
    | VSS | GND  |  GND   | GND   |
    | VDD | 5V   |  5V    | 5V    |
    | RS  | CS   |  RS    | PTC17 |
    | R/W | SID  |  RW    | PTD2  |
    | E   | SCLK |  E     | PTD1  |
    | BLA | 5V   |  5V    | 5V    |
    | BLK | GND  |  GND   | GND   |
    |-----|------|--------|-------|
 
 */
 
SPI spi(PA_7, PA_6, PA_5); // The default format is set to 8-bits, mode 0, and a clock frequency of 1MHz
 
DigitalOut PinRS(PC_4);
 
/*
  Timing Diagram of Serial Mode Data Transfer on p.20
 
 |-------------------|------------------------|------------------------|
 |  Synchronizing    | 1-st Byte Higher data  |  2-nd Byte Lower data  |
 |-------------------|------------------------|------------------------|
 | 1 1 1 1 1 RW RS 0 | D7 D6 D5 D4  0  0  0  0| D3 D2 D1 D0  0  0  0  0|
 |-------------------|------------------------|------------------------|
*/
 
void InstructionRegsiter(uint8_t inst)
{
    spi.write(0xf8);        // Sync.    - 1  1  1  1  1  0  0  0
    spi.write(inst & 0xf0); // HighData - D7 D6 D5 D4 0  0  0  0
    spi.write(inst << 4);   // LowData  - D3 D2 D1 D0 0  0  0  0
    // According to table "Instruction Set 1: (RE=0: Basic Instruction)" on p.10
    if (inst == 0x01) 
    {
        wait_ms(1.6);
    } 
    else 
    {
        wait_us(72);
    }
}
 
void DataRegister(uint8_t data)
{
    spi.write(0xfa);        // Sync.    - 1  1  1  1  1  0  1  0
    spi.write(data & 0xf0); // HighData - D7 D6 D5 D4 0  0  0  0
    spi.write(data << 4);   // LowData  - D3 D2 D1 D0 0  0  0  0
    wait_us(72);
}
 
// Manual p.28
void ResetLCD()
{
    PinRS = 1;
    //Function set [DL=1 8-bit interface; DL=0 4-bit interface;
    //              RE=1: extended instruction; RE=0: basic instruction]
    // RS RW DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
    //  0  0   0   0   1  DL   X  RE   0   0
    InstructionRegsiter(0x30);
    wait_us(100);
    //Function set [DL=1 8-bit interface; DL=0 4-bit interface;
    //              RE=1: extended instruction; RE=0: basic instruction]
    // RS RW DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
    //  0  0   0   0   1  DL   X  RE   0   0
    InstructionRegsiter(0x30);
    wait_us(47);
    // DisplayControl [D=1: Display ON; C=1: Cursor ON; B=1: Character Blink ON]
    // RS RW DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
    //  0  0   0   0   0   0   1   D   C   B
    InstructionRegsiter(0x0c);
    wait_us(100);
    // Display Clear
    InstructionRegsiter(0x01);
    wait_ms(10);
    //Set cursor position and display shift when doing write or read operation
    // RS RW DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
    //  0  0   0   0   0   0   0   1 I/D   S
    InstructionRegsiter(0x06);

    lcdLed = 1;
}

void printLCD(string str)
{
    for (size_t i = 0; i < str.size(); i++) 
    {
        DataRegister(str.c_str()[i]);
    }
}

void clearDisplay()
{
    InstructionRegsiter(0x01); // Display Clear, AC = 0      
    InstructionRegsiter(0x80); 
}