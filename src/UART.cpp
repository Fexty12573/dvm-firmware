#include "UART.h"

#include <pico/stdlib.h>
#include <hardware/uart.h>

#include <cmath>

UART::UART(Pins pins) : m_pins(pins), m_inst(uart0) {
}

void UART::initialize(uint32_t baudrate) {
    uart_init(m_inst, baudrate);

    gpio_set_function(m_pins.rx, GPIO_FUNC_UART);
    gpio_set_function(m_pins.tx, GPIO_FUNC_UART);
}

bool UART::has_tx_data() const {
    return uart_is_readable(m_inst);
}

char UART::read_char() const {
    return has_tx_data() ? uart_getc(m_inst) : 0;
}

UART::RxCommand UART::read_command() const {
    return static_cast<RxCommand>(read_char());
}

size_t UART::read_str(char* buffer, size_t size) const {
    size_t count = 0;
    while (has_tx_data()) {
        buffer[count] = uart_getc(m_inst);
        count++;
    }
    
    return count;
}

float UART::read_float() const {
    float val = 0;
    const auto n = read_str(reinterpret_cast<char*>(&val), sizeof(val));

    return n == sizeof(val) ? val : NAN;
}

void UART::write_command(TxCommand cmd) const {
    if (uart_is_writable(m_inst)) {
        uart_putc_raw(m_inst, static_cast<char>(cmd));
    }
}

void UART::write_str(char* buffer, size_t size) const {
    if (uart_is_writable(m_inst)) {
        for (auto i = 0u; i < size; i++) {
            uart_putc_raw(m_inst, buffer[i]);
        }
    }
}

void UART::write_float(float val) const {
    write_str(reinterpret_cast<char*>(&val), sizeof(val));
}

