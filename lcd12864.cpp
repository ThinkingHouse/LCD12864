#include "mbed.h"
#include "lcd12864.h"
#include <string.h>
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

Display::Display(PinName t_sck, PinName t_miso, PinName t_mosi, PinName t_cs, PinName t_led)
{
    m_spi = new SPI(t_mosi, t_miso, t_sck);
    m_cs = new DigitalOut(t_cs);
    m_lcdLed = new DigitalOut(t_led);
    reset();
}

/*
  Timing Diagram of Serial Mode Data Transfer on p.20
 
 |-------------------|------------------------|------------------------|
 |  Synchronizing    | 1-st Byte Higher data  |  2-nd Byte Lower data  |
 |-------------------|------------------------|------------------------|
 | 1 1 1 1 1 RW RS 0 | D7 D6 D5 D4  0  0  0  0| D3 D2 D1 D0  0  0  0  0|
 |-------------------|------------------------|------------------------|
*/
 
void Display::instructionRegsiter(uint8_t inst)
{
    m_spi->write(0xf8);        // Sync.    - 1  1  1  1  1  0  0  0
    m_spi->write(inst & 0xf0); // HighData - D7 D6 D5 D4 0  0  0  0
    m_spi->write(inst << 4);   // LowData  - D3 D2 D1 D0 0  0  0  0
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
 
void Display::dataRegister(uint8_t data)
{
    m_spi->write(0xfa);        // Sync.    - 1  1  1  1  1  0  1  0
    m_spi->write(data & 0xf0); // HighData - D7 D6 D5 D4 0  0  0  0
    m_spi->write(data << 4);   // LowData  - D3 D2 D1 D0 0  0  0  0
    wait_us(72);
}
 
// Manual p.28
void Display::reset()
{
    *m_cs = 1;
    //Function set [DL=1 8-bit interface; DL=0 4-bit interface;
    //              RE=1: extended instruction; RE=0: basic instruction]
    // RS RW DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
    //  0  0   0   0   1  DL   X  RE   0   0
    instructionRegsiter(0x30);
    wait_us(100);
    //Function set [DL=1 8-bit interface; DL=0 4-bit interface;
    //              RE=1: extended instruction; RE=0: basic instruction]
    // RS RW DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
    //  0  0   0   0   1  DL   X  RE   0   0
    instructionRegsiter(0x30);
    wait_us(47);
    // DisplayControl [D=1: Display ON; C=1: Cursor ON; B=1: Character Blink ON]
    // RS RW DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
    //  0  0   0   0   0   0   1   D   C   B
    instructionRegsiter(0x0c);
    wait_us(100);
    // Display Clear
    instructionRegsiter(0x01);
    wait_ms(10);
    //Set cursor position and display shift when doing write or read operation
    // RS RW DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
    //  0  0   0   0   0   0   0   1 I/D   S
    instructionRegsiter(0x06);
}

void Display::addStr(uint8_t t_x, uint8_t t_y, char* t_str)
{
    if (t_x > 3 || t_y > 16)
    {
        return;
    }

    for(int i = 0; i < 16; i++)
    {
        printStr[t_x][i] = t_str[i];
    }
}

void Display::print()
{
    for (size_t i = 0; i < 16; i++) 
    {
        dataRegister(printStr[0][i]);
    }

    for (size_t i = 0; i < 16; i++) 
    {
        dataRegister(printStr[2][i]);
    }

    for (size_t i = 0; i < 16; i++) 
    {
        dataRegister(printStr[1][i]);
    }

    for (size_t i = 0; i < 16; i++) 
    {
        dataRegister(printStr[3][i]);
    }
}

void Display::clear()
{
    instructionRegsiter(0x01); // Display Clear, AC = 0      
    instructionRegsiter(0x80); 
}

void Display::setLed(bool t_value)
{
    *m_lcdLed = t_value;
}