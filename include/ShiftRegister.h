
#ifndef INCLUDE_SHIFT_REGISTER
#define INCLUDE_SHIFT_REGISTER

#include <cstdint>

#include "ShiftRegister.h"
#include "pico/stdlib.h"

enum class ShiftOrder {
    MSBFirst,
    LSBFirst
};

template<class T>
class ShiftRegister {
public:
    struct Pins {
        uint8_t strobe;
        uint8_t data;
        uint8_t clock;
    };

    ShiftRegister(Pins pins);

    void shift_out(T data, ShiftOrder order) const;
    void set_data() const;

private:
    Pins m_pins;
};

template<class T>
ShiftRegister<T>::ShiftRegister(Pins pins)
    : m_pins(pins) {
    
    gpio_init(m_pins.strobe);
    gpio_set_dir(m_pins.strobe, GPIO_OUT);
    
    gpio_init(m_pins.data);
    gpio_set_dir(m_pins.data, GPIO_OUT);

    gpio_init(m_pins.clock);
    gpio_set_dir(m_pins.clock, GPIO_OUT);
}

template<class T>
void ShiftRegister<T>::shift_out(T data, ShiftOrder order) const {
    auto clk_pin = m_pins.clock;
    auto data_pin = m_pins.data;

    const uint8_t bit_count = sizeof(T) * 8;

    if (order == ShiftOrder::MSBFirst) {
        for (auto i = 0; i < bit_count; i++, data >>= 1) {
            gpio_put(data_pin, data & 1);

            gpio_put(clk_pin, true);
            gpio_put(clk_pin, false);
        }
    } else if (order == ShiftOrder::LSBFirst) {
        for (auto i = 0; i < bit_count; i++, data <<= 1) {
            gpio_put(data_pin, data & 0x80);

            gpio_put(clk_pin, true);
            gpio_put(clk_pin, false);
        }
    }
}

template<class T>
void ShiftRegister<T>::set_data() const {
    gpio_put(m_pins.strobe, true);
    gpio_put(m_pins.strobe, false);
}

#endif // INCLUDE_SHIFT_REGISTER
