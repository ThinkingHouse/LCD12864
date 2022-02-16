#ifndef LCD12864_H
#define LCD12864_H

#include <string> 

class Display
{
    private:
        DigitalOut* m_lcdLed;
        DigitalOut* m_cs;
        SPI* m_spi;

        void instructionRegsiter(uint8_t inst);
        void dataRegister(uint8_t data);

    public:
        Display(PinName t_sck, PinName t_miso, PinName t_mosi, PinName t_cs, PinName t_led);
        void reset();

        void print(string str);

        void clear();
};



#endif
