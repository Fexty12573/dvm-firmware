#include "DigitalVoltmeter.h"

#include <pico/stdlib.h>
#include <hardware/uart.h>

#define UART_ID uart0
#define UART_BAUDRATE 115200

void DigitalVoltmeter::communication() {
    auto& dvm = get();
    
    char command_buffer[8] = { 0 };

    while (true) {
        if (dvm.m_uart.has_tx_data()) {
            const UART::RxCommand cmd = dvm.m_uart.read_command();

            switch (cmd) {
                case UART::RxCommand::SetRangeMux:
                    if (dvm.m_uart.read_str_blocking(command_buffer, 1, 100 * 1000)) {
                        dvm.m_range_state.value = command_buffer[0];
                        dvm.apply_range();
                    }
                    break;
                case UART::RxCommand::SetAutozero:
                    if (dvm.m_uart.read_str_blocking(command_buffer, 1, 100 * 1000)) {
                        dvm.set_autozero(command_buffer[0]);
                    }
                    break;
                case UART::RxCommand::SetRange:
                    if (dvm.m_uart.read_str_blocking(command_buffer, 1, 100 * 1000)) {
                        dvm.set_range(static_cast<Range>(command_buffer[0]));
                    }
                    break;
                case UART::RxCommand::SetRefVoltage:
                    if (dvm.m_uart.read_str_blocking(command_buffer, 4, 100 * 1000)) {
                        dvm.m_vref = *reinterpret_cast<float*>(command_buffer);
                    }
                    break;
                case UART::RxCommand::SetGain:
                    if (dvm.m_uart.read_str_blocking(command_buffer, 5, 100 * 1000)) {
                        const auto range = *reinterpret_cast<DigitalVoltmeter::Range*>(command_buffer);
                        const float gain = *reinterpret_cast<float*>(command_buffer + 1);

                        dvm.set_gain(range, gain);
                    }
            }

            // Clear command buffer
            *reinterpret_cast<uint64_t*>(command_buffer) = 0;
        }

        if (!dvm.m_values.empty()) {
            spin_lock_unsafe_blocking(dvm.m_spinlock);

            double value = dvm.m_values.front();
            uint32_t raw_value = dvm.m_raw_values.front();
            dvm.m_values.pop();
            dvm.m_raw_values.pop();
            spin_unlock_unsafe(dvm.m_spinlock);

            dvm.m_uart.write_command(UART::TxCommand::AdcValue);
            dvm.m_uart.write_double(value);
        }
    }
}
