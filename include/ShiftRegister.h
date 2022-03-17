
#ifndef INCLUDE_SHIFT_REGISTER
#define INCLUDE_SHIFT_REGISTER

#include <cstdint>

enum class ShiftOrder {
    MSBFirst,
    LSBFirst
};

class ShiftRegister {
public:
    struct Pins {
        uint8_t strobe;
        uint8_t data;
        uint8_t clock;
    };

    ShiftRegister(Pins pins);

    void shift_out(uint8_t data, ShiftOrder order) const;
    void set_data() const;

private:
    Pins m_pins;
};

#endif // INCLUDE_SHIFT_REGISTER
