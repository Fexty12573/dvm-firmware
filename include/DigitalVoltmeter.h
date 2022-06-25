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
        uint8_t value; // Zugriff auf alle Bits
    };

public:
    enum class Range : uint8_t {
        Measure_200mV,
        Measure_2V,
        Measure_20V
    };
    struct IsolatorPins {
        uint8_t control;
        uint8_t disable;
    };

    DigitalVoltmeter();

    static DigitalVoltmeter& get();

    bool initialize();
    void update();

    void set_range(Range range);
    void set_autozero(bool enabled);
    void set_gain(Range range, float gain);

private:
    void apply_range() const;
    double convert_to_volts(uint32_t adc_value) const;

    static void communication();

    AD7177 m_adc;

    ShiftRegister<uint16_t> m_muxes;
    RangeMuxes m_range_state;

    UART m_uart;

    Range m_range;
    bool m_autozero;
    bool m_has_autozero_code;
    uint32_t m_autozero_code;

    int32_t m_spinlock_num;
    spin_lock_t* m_spinlock;

    std::queue<double> m_values;
    std::queue<uint32_t> m_raw_values;

    float m_vref = 2.5f;
    float m_gains[3] = {
        0.1f,
        1.0f,
        10.0f
    };

    // Pins
    static constexpr ShiftRegister<uint16_t>::Pins MuxPins = {
        .strobe = 9,
        .data = 11,
        .clock = 13
    };
    static constexpr AD7177::Pins ADCPins = {
        .cs = 8,
        .din = 12,
        .dout = 7,
        .sclk = 10
    }; // .error = ...
    static constexpr UART::Pins UartPins = {
        .rx = 1,
        .tx = 0
    };
    static constexpr IsolatorPins IsolatorPins = {
        .control = 5,
        .disable = 6
    };
    static constexpr uint32_t UartBaudrate = 115200;
};

#endif
