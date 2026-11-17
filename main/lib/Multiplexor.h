#pragma once

#include "pinout_esp32.h"
#include "Peripheral.h"

namespace Peripheral
{
    // using Multiplexer = Multiplexor;

    class Multiplexor : public PeripheralBase
    {
    public:
        Multiplexor(PeripheralID id, PinAddress s0, PinAddress s1, PinAddress s2, PinAddress s3, PinAddress sig);

        void setup();
        void update() override;

        void attach(Button &button, uint8_t ch);

        uint8_t readChannel(uint8_t ch);
        uint16_t readAll();

    private:
        void setChannel(uint8_t ch);

        PinHandle _s0;
        PinHandle _s1;
        PinHandle _s2;
        PinHandle _s3;
        PinHandle _sig;
        uint8_t _currentCh = 0xFF; // invalid byte

        uint16_t _state = 0;

        Button* _buttons[16] = {};
    };
}
