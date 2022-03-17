
#include "ShiftRegister.h"
#include "pico/stdlib.h"

ShiftRegister::ShiftRegister(Pins pins)
    : m_pins(pins) {
    
    gpio_init(m_pins.strobe);
    gpio_set_dir(m_pins.strobe, GPIO_OUT);
    
    gpio_init(m_pins.data);
    gpio_set_dir(m_pins.data, GPIO_OUT);

    gpio_init(m_pins.clock);
    gpio_set_dir(m_pins.clock, GPIO_OUT);
}

void ShiftRegister::shift_out(uint8_t data, ShiftOrder order) const {
    auto clk_pin = m_pins.clock;
    auto data_pin = m_pins.data;

    if (order == ShiftOrder::MSBFirst) {
        for (auto i = 0; i < 8; i++, data >>= 1) {
            gpio_put(data_pin, data & 1);

            gpio_put(clk_pin, true);
            gpio_put(clk_pin, false);
        }
    } else if (order == ShiftOrder::LSBFirst) {
        for (auto i = 0; i < 8; i++, data <<= 1) {
            gpio_put(data_pin, data & 0x80);

            gpio_put(clk_pin, true);
            gpio_put(clk_pin, false);
        }
    }
}

void ShiftRegister::set_data() const {
    gpio_put(m_pins.strobe, true);
    gpio_put(m_pins.strobe, false);
}
