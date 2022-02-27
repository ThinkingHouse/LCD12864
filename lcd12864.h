#ifndef LCD12864_H
#define LCD12864_H

#include <string> 

class Display
{
    private:
        DigitalOut* m_lcdLed;
        DigitalOut* m_cs;
        SPI* m_spi;

        uint8_t printStr[4][16];

        void instructionRegsiter(uint8_t inst);
        void dataRegister(uint8_t data);

    public:
        Display(PinName t_sck, PinName t_miso, PinName t_mosi, PinName t_cs, PinName t_led);
        void reset();

        void addStr(uint8_t t_x, uint8_t t_y, string t_str);

        void print();

        void clear();

        void setLed(bool t_value);
};



#endif
