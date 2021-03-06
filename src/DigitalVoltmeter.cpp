#include "DigitalVoltmeter.h"

#include <numeric>

#include <pico/stdlib.h>
#include <pico/multicore.h>

DigitalVoltmeter::DigitalVoltmeter()
    : m_muxes(MuxPins)
    , m_autozero(false)
    , m_range(Range::Measure_200mV)
    , m_adc(ADCPins)
    , m_autozero_code(0)
    , m_has_autozero_code(false)
    , m_uart(UartPins) {
}

DigitalVoltmeter& DigitalVoltmeter::get() {
    static DigitalVoltmeter dvm;
    return dvm;
}

bool DigitalVoltmeter::initialize() {
    this->set_range(Range::Measure_2V);
    m_adc.initialize();
    m_uart.initialize(UartBaudrate);

    gpio_init(IsolatorPins.control);
    gpio_init(IsolatorPins.disable);
    gpio_set_dir(IsolatorPins.control, true);
    gpio_set_dir(IsolatorPins.disable, true);
    gpio_put(IsolatorPins.control, true);
    gpio_put(IsolatorPins.disable, false);

    m_spinlock_num = spin_lock_claim_unused(true);
    m_spinlock = spin_lock_init(m_spinlock_num);

    multicore_launch_core1(DigitalVoltmeter::communication);

    return true;
}


void DigitalVoltmeter::update() {
    // Read ADC and transmit data
    // Adjust range based on voltage read
    static const char bruh[] = "balls\n";
    // m_uart.write_str(bruh, sizeof(bruh));
    if (m_adc.data_ready()) {
        // m_uart.write_command(UART::TxCommand::Message);
        // m_uart.write_int(sizeof(bruh));
        // m_uart.write_str("adc wert!!!!! :O pog\n", 21);

        const auto val = m_adc.read_data();
        
        float voltage = convert_to_volts(val);

        if (m_autozero) {
            m_autozero_code = val;
            m_autozero = false;
        }

        spin_lock_unsafe_blocking(m_spinlock);
        m_values.push(voltage);
        m_raw_values.push(val);
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

        uint16_t autozero_value = m_autozero ? 0b01100000 : 0b10010000;
        uint16_t value = (autozero_value << 8) | static_cast<uint16_t>(m_range_state.value);

        m_muxes.shift_out(static_cast<uint16_t>(m_range_state.value), ShiftOrder::MSBFirst);
        m_muxes.set_data();

        sleep_ms(1); // Artificial delay to allow muxes to switch
        m_muxes.shift_out(value, ShiftOrder::MSBFirst);
        m_muxes.set_data();
    }
}

void DigitalVoltmeter::set_gain(Range range, float gain) {
    const auto index = static_cast<uint8_t>(range);
    if (index < 3) {
        m_gains[index] = gain;
    }
}

void DigitalVoltmeter::apply_range() const {
    if (!m_autozero) {
        uint16_t autozero_value = m_autozero ? 0b01100000 : 0b10010000;
        uint16_t value = (autozero_value << 8) | static_cast<uint16_t>(m_range_state.value);
        
        m_muxes.shift_out(0U, ShiftOrder::MSBFirst);
        m_muxes.set_data();

        sleep_ms(1); // Artificial delay to allow muxes to switch
        m_muxes.shift_out(value, ShiftOrder::MSBFirst);
        m_muxes.set_data();
    }
}

double DigitalVoltmeter::convert_to_volts(uint32_t adc_value) const {
    // Code = 2^(N - 1) * ((AIN / VREF) + 1)
    // AIN = VREF((Code / 2^31) - 1)
    // AIN = VREF((Code - 2^31) / 2^31)

    constexpr uint32_t two_pow_31 = 0x80000000; // 2^(32 - 1)
    if (m_has_autozero_code) {
        adc_value = adc_value - m_autozero_code + two_pow_31 - 1;
    }
    
    double voltage = ((static_cast<double>(adc_value) - two_pow_31) / two_pow_31) * m_vref;
    voltage *= m_gains[static_cast<int>(m_range)];

    return voltage;
}
