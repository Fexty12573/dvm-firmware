#include "AD7177.h"
#include "AD7177Define.h"

#include <pico/stdlib.h>

#define STALL() sleep_us(1)

AD7177::AD7177(Pins pins)
    : m_pins(pins) {
}

void AD7177::initialize() {
    gpio_init(m_pins.cs);
    gpio_init(m_pins.din);
    gpio_init(m_pins.dout);
    gpio_init(m_pins.sclk);

    gpio_set_dir(m_pins.cs, GPIO_OUT);
    gpio_set_dir(m_pins.din, GPIO_OUT);
    gpio_set_dir(m_pins.dout, GPIO_IN);
    gpio_set_dir(m_pins.sclk, GPIO_OUT);

    gpio_put(m_pins.cs, false);

    write_register(
        AD7177_GPIOCON_REG, 
        AD7177_GPIOCON_REG_DATA0 | AD7177_GPIOCON_REG_OP_EN0 | AD7177_GPIOCON_REG_OP_EN1
    );

    write_register(
        AD7177_ADCMODE_REG,
        AD7177_ADCMODE_REG_CLKSEL(0x2)
    );

    write_register(
        AD7177_CHMAP0_REG,
        AD7177_CHMAP_REG_AINPOS(0x4) | AD7177_CHMAP_REG_AINNEG(0x3) | AD7177_CHMAP_REG_CH_EN
    );

    write_register(
        AD7177_SETUPCON0_REG,
        AD7177_SETUP_CONF_REG_AIN_BUF(0x3) | AD7177_SETUP_CONF_REG_REF_BUF(0x3) | AD7177_SETUP_CONF_REG_BI_UNIPOLAR
    );

    write_register(
        AD7177_FILTCON0_REG,
        AD7177_FILT_CONF_REG_ODR(0x14) | AD7177_FILT_CONF_REG_ORDER(0x3)
    );

    write_register(
        AD7177_IFMODE_REG,
        AD7177_IFMODE_REG_ALT_SYNC | AD7177_IFMODE_REG_CONT_READ | AD7177_IFMODE_REG_DATA_WL32
    );
}

void AD7177::reset() {
    gpio_put(m_pins.sclk, true);
    sleep_ms(10);
    gpio_put(m_pins.cs, false);

    for (auto i = 0; i < 10; i++) {
        write_byte(0xFF, ShiftOrder::LSBFirst);
    }

    gpio_put(m_pins.cs, true);
    sleep_ms(10);
    gpio_put(m_pins.cs, false);
}

bool AD7177::data_ready() const {
    return !gpio_get(m_pins.dout);
}

uint8_t AD7177::write_byte(uint8_t data, ShiftOrder order) {
    if (order == ShiftOrder::MSBFirst) {
        for (auto i = 0; i < 8; i++, data <<= 1) {
            gpio_put(m_pins.din, data & 0x80);
            STALL();
            gpio_put(m_pins.sclk, false);
            STALL();
            gpio_put(m_pins.sclk, true);
        }
    } else if (order == ShiftOrder::LSBFirst) {
        for (auto i = 0; i < 8; i++, data >>= 1) {
            gpio_put(m_pins.din, data & 0x01);
            STALL();
            gpio_put(m_pins.sclk, false);
            STALL();
            gpio_put(m_pins.sclk, true);
        }
    }

    STALL();

    return 0;
}

uint8_t AD7177::read_byte() {
    uint8_t val = 0;

    for (auto i = 0u; i < 8; i++) {
        gpio_put(m_pins.sclk, false);
        STALL();
        val |= (static_cast<uint8_t>(gpio_get(m_pins.dout)) << i);
        gpio_put(m_pins.sclk, true);
        STALL();
    }

    return val;
}

uint32_t AD7177::read_data() {
    uint32_t val = 0;

    for (auto i = 0u; i < 32; i++) {
        gpio_put(m_pins.sclk, false); 
        STALL();
        val |= (static_cast<uint32_t>(gpio_get(m_pins.dout)) << (31 - i));
        gpio_put(m_pins.sclk, true); 
        STALL();
    }

    return val;
}

uint16_t AD7177::read_register(uint8_t addr) {
    uint16_t register_value = 0;

    gpio_put(m_pins.cs, false);
    STALL();

    register_value = static_cast<uint16_t>(read_byte()) << 8;
    register_value |= read_byte();

    gpio_put(m_pins.cs, true);
    STALL();

    return register_value;
}

void AD7177::write_register(uint8_t addr, uint16_t data) {
    write_byte(addr);
    write_byte(data >> 8);
    write_byte(data & 0xFF);
}
