
#include <pico/stdlib.h>

#include "DigitalVoltmeter.h"

int main() {
    auto& dvm = DigitalVoltmeter::get();

    dvm.initialize();
    
    while (true) {
        dvm.update();
    }
}
