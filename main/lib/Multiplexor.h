#pragma once

#include "pinout_esp32.h"
#include "Peripheral.h"

namespace Peripheral
{
    // using Multiplexer = Multiplexor;

    class Multiplexor : public PeripheralBase
    {
    public:
        Multiplexor(PeripheralID id, PinAddress s0, PinAddress s1, PinAddress s2, PinAddress s3, PinAddress sig)
            : PeripheralBase(id),
              _s0(s0, PinMode::Output),
              _s1(s1, PinMode::Output),
              _s2(s2, PinMode::Output),
              _s3(s3, PinMode::Output),
              _sig(sig, PinMode::InputPullup)
        {}

        void setup()
        {
            _s0.setup();
            _s1.setup();
            _s2.setup();
            _s3.setup();
            _sig.setup();
        }

        void update() override
        {
            for (uint8_t ch = 0; ch < 16; ch++)
            {
                if (_buttons[ch] == nullptr) continue;
                _buttons[ch]->updateInternalData(readChannel(ch));
            }
            // _state = readAll();
        }

        void attach(Button &button, uint8_t ch)
        {
            if (ch > 0x0F) return;
            _buttons[ch] = &button;
        }

        uint8_t readChannel(uint8_t ch)
        {
            setChannel(ch);
            delayMicroseconds(5); // sig delay
            return _sig.readDigital();
        }

        uint16_t readAll()
        {
            uint16_t state = 0;
            for (uint8_t ch = 0; ch < 16; ch++)
            {
                if (readChannel(ch))
                    state |= (1 << ch);
            }
            return state;
        }

    private:
        void setChannel(uint8_t ch)
        {
            ch &= 0x0F;
            if (ch == _currentCh) return;

            _s0.writeDigital(ch & 0x01);
            _s1.writeDigital((ch >> 1) & 0x01);
            _s2.writeDigital((ch >> 2) & 0x01);
            _s3.writeDigital((ch >> 3) & 0x01);

            _currentCh = ch;
        }

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