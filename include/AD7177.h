#ifndef INCLUDE_AD7177
#define INCLUDE_AD7177

#include "ShiftRegister.h"

#include <cstdint>

class AD7177 {
public:
    struct Pins {
        uint8_t cs;
        uint8_t din;
        uint8_t dout;
        uint8_t sclk;
    };

    AD7177(Pins pins);

    void initialize();
    void reset();

    bool data_ready() const;
    uint32_t read_data();

private:
    uint8_t write_byte(uint8_t data, ShiftOrder order = ShiftOrder::MSBFirst);
    uint8_t read_byte();

    uint16_t read_register(uint8_t addr);
    void write_register(uint8_t addr, uint16_t data);

private:
    Pins m_pins;
};

#endif
