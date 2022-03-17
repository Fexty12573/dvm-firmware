#include "DigitalVoltmeter.h"

#include <pico/stdlib.h>
#include <hardware/uart.h>

#define UART_ID uart0
#define UART_BAUDRATE 115200

void DigitalVoltmeter::communication() {
    auto& dvm = get();

    const auto fill_command = [&dvm](UART::RxCommand cmd, char* buffer, size_t& count) -> bool {
        switch (cmd) {
        case UART::RxCommand::SetRangeMux:
        case UART::RxCommand::SetAutozero:
            if (count == 1) {
                return true;
            } else {
                buffer[count] = dvm.m_uart.read_char();
                count++;
                return false;
            }
        }
    };
    const auto exec_command = [&dvm](UART::RxCommand cmd, char* buffer) {
        switch (cmd) {
        case UART::RxCommand::SetRangeMux:
            dvm.m_range_state.value = buffer[0];
            dvm.apply_range();
        case UART::RxCommand::SetAutozero:
            dvm.set_autozero(buffer[0]);
            break;
        }
    };
    
    bool reading_command = false;
    UART::RxCommand current_cmd = static_cast<UART::RxCommand>(-1);

    char command_buffer[8] = { 0 };
    size_t command_count = 0;

    while (true) {
        if (dvm.m_uart.has_tx_data()) {
            if (reading_command) {
                if (fill_command(current_cmd, command_buffer, command_count)) {
                    reading_command = false;
                    exec_command(current_cmd, command_buffer);
                }
            } else {
                current_cmd = dvm.m_uart.read_command();
                reading_command = true;
                command_count = 0;
                *reinterpret_cast<uint64_t*>(command_buffer) = 0ULL; // Clear Buffer
            }
        }

        if (!dvm.m_values.empty()) {
            spin_lock_unsafe_blocking(dvm.m_spinlock);
            
            float value = dvm.m_values.front();
            dvm.m_values.pop();
            spin_unlock_unsafe(dvm.m_spinlock);

            dvm.m_uart.write_command(UART::TxCommand::AdcValue);
            dvm.m_uart.write_float(value);
        }
    }
}
