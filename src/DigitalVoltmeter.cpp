#include "DigitalVoltmeter.h"

#include <numeric>

#include <pico/stdlib.h>
#include <pico/multicore.h>

DigitalVoltmeter::DigitalVoltmeter()
    : m_range_muxes(U23Pins)
    , m_autozero_mux(U24Pins)
    , m_autozero(false)
    , m_range(Range::Measure_200mV)
    , m_adc(ADCPins)
    , m_autozero_code(0)
    , m_has_autozero_code(false)
    , m_uart(UartPins) {

    m_spinlock_num = spin_lock_claim_unused(true);
    m_spinlock = spin_lock_init(m_spinlock_num);
}

DigitalVoltmeter& DigitalVoltmeter::get() {
    static DigitalVoltmeter dvm;
    return dvm;
}

bool DigitalVoltmeter::initialize() {
    this->set_range(Range::Measure_2V);
    m_adc.initialize();
    m_uart.initialize(UartBaudrate);

    multicore_launch_core1(DigitalVoltmeter::communication);
}


void DigitalVoltmeter::update() {
    // Read ADC and transmit data
    // Adjust range based on voltage read

    if (m_adc.data_ready()) {
        // Read value from ADC
        const auto val = m_adc.read_data();
        
        // Convert to Volts
        float voltage = convert_to_volts(val);

        if (m_autozero) {
            m_autozero_code = val;
            m_autozero = false;
        }

        spin_lock_unsafe_blocking(m_spinlock);
        m_values.push(voltage);
        spin_unlock_unsafe(m_spinlock);
    }
}

void DigitalVoltmeter::set_range(Range range) {
    if (range != m_range) {
        m_range = range;

        switch (m_range) {
        case Range::Measure_200mV:
            m_range_state.bit0 = 0;
            m_range_state.bit1 = 1;
            m_range_state.bit2 = 0;
            m_range_state.bit3 = 1;
            m_range_state.bit4 = 0;
            m_range_state.bit5 = 1;
            m_range_state.bit6 = 0;
            m_range_state.bit7 = 1;
            break;
        case Range::Measure_2V:
            m_range_state.bit0 = 1;
            m_range_state.bit1 = 1;
            m_range_state.bit2 = 0;
            m_range_state.bit3 = 0;
            m_range_state.bit4 = 1;
            m_range_state.bit5 = 1;
            m_range_state.bit6 = 0;
            m_range_state.bit7 = 0;
            break;
        case Range::Measure_20V:
            m_range_state.bit0 = 1;
            m_range_state.bit1 = 0;
            m_range_state.bit2 = 1;
            m_range_state.bit3 = 0;
            m_range_state.bit4 = 1;
            m_range_state.bit5 = 0;
            m_range_state.bit6 = 1;
            m_range_state.bit7 = 0;
            break;
        }
    }

    this->apply_range();
}

void DigitalVoltmeter::set_autozero(bool enabled) {
    if (m_autozero != enabled) {
        m_autozero = enabled;

        uint8_t value = m_autozero ? 0b0110 : 0b1001;

        m_autozero_mux.shift_out(0, ShiftOrder::LSBFirst);
        m_autozero_mux.set_data();

        sleep_ms(10); // Artificial delay to allow muxes to switch
        m_autozero_mux.shift_out(value, ShiftOrder::LSBFirst);
        m_autozero_mux.set_data();
    }
}

void DigitalVoltmeter::apply_range() const {
    if (!m_autozero) {
        m_range_muxes.shift_out(m_range_state.value, ShiftOrder::MSBFirst);
        m_range_muxes.set_data();
    }
}

float DigitalVoltmeter::convert_to_volts(uint32_t adc_value) const {
    // Code = 2^(N - 1) * ((AIN / VREF) + 1)
    // AIN = VREF((Code / 2^31) - 1)
    // AIN = VREF((Code - 2^31) / 2^31)

    bool negative = !(adc_value & 0x80000000);
    constexpr uint32_t two_pow_31 = 0x80000000; // 2^(32 - 1)

    if (m_has_autozero_code) {
        adc_value = adc_value - m_autozero_code + two_pow_31 - 1;
    }
    
    auto voltage = ((static_cast<float>(adc_value) - two_pow_31) / two_pow_31) * m_vref;
    voltage *= m_gains[static_cast<int>(m_range)];

    return negative ? -voltage : voltage;
}
