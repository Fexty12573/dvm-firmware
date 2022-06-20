
#include <pico/stdlib.h>

#include "DigitalVoltmeter.h"

int main() {
    auto& dvm = DigitalVoltmeter::get();

    if (!dvm.initialize()) {}
    
    while (true) {
        dvm.update();
    }
}
