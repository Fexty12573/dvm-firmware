#ifndef INCLUDE_DIGITAL_VOLTMETER
#define INCLUDE_DIGITAL_VOLTMETER

#include <cstdint>
#include <queue>

#include <hardware/sync.h>

#include "ShiftRegister.h"
#include "AD7177.h"
#include "UART.h"

class DigitalVoltmeter {
    union RangeMuxes {
        struct {
            uint8_t bit0 : 1;
            uint8_t bit1 : 1;
            uint8_t bit2 : 1;
            uint8_t bit3 : 1;
            uint8_t bit4 : 1;
            uint8_t bit5 : 1;
            uint8_t bit6 : 1;
            uint8_t bit7 : 1;
        };
        uint8_t value;
    };

public:
    enum class Range {
        Measure_200mV,
        Measure_2V,
        Measure_20V
    };

    DigitalVoltmeter();

    static DigitalVoltmeter& get();

    bool initialize();
    void update();

    void set_range(Range range);
    void set_autozero(bool enabled);

private:
    void apply_range() const;
    float convert_to_volts(uint32_t adc_value) const;

    static void communication();

    AD7177 m_adc;

    ShiftRegister m_range_muxes;
    ShiftRegister m_autozero_mux;
    RangeMuxes m_range_state;

    UART m_uart;

    Range m_range;
    bool m_autozero;
    bool m_has_autozero_code;
    uint32_t m_autozero_code;

    int32_t m_spinlock_num;
    spin_lock_t* m_spinlock;

    std::queue<float> m_values;

    float m_vref = 2.5f;
    float m_gains[3] = {
        0.2f,
        2.0f,
        20.0f
    };

    // Pins
    static constexpr ShiftRegister::Pins U23Pins = {
        .strobe = 0,
        .data = 1,
        .clock = 2
    };
    static constexpr ShiftRegister::Pins U24Pins = {
        .strobe = 3,
        .data = 4,
        .clock = 5
    };
    static constexpr AD7177::Pins ADCPins = {
        .cs = 6,
        .din = 7,
        .dout = 8,
        .sclk = 9
    };
    static constexpr UART::Pins UartPins = {
        .rx = 10,
        .tx = 11
    };
    static constexpr uint32_t UartBaudrate = 115200;
};

#endif
