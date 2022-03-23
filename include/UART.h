#ifndef INCLUDE_UART_H
#define INCLUDE_UART_H

#include <cstdint>
#include <cstddef>

#include <hardware/uart.h>

class UART {
public:
    struct Pins {
        uint8_t rx;
        uint8_t tx;
    };

    enum class TxCommand : uint8_t {
        AdcValue = 0x00
    };

    enum class RxCommand : uint8_t {
        SetRangeMux = 0x00,
        SetAutozero = 0x01,
        SetRange = 0x02,
        SetRefVoltage = 0x03
    };

    UART(Pins pins);

    void initialize(uint32_t baudrate);

    bool has_tx_data() const;

    // Read Functions
    char read_char() const;
    RxCommand read_command() const;
    size_t read_str(char* buffer, size_t size) const;
    bool read_str_blocking(char* buffer, size_t count, uint64_t timout_us = -1) const;
    float read_float() const;

    // Write Functions
    void write_command(TxCommand cmd) const;
    void write_str(const char* buffer, size_t size) const;
    void write_float(float val) const;

private:
    uart_inst_t* m_inst;
    Pins m_pins;
};

#endif // INCLUDE_UART_H
