#pragma once

#include "lib/CSDDevice.h"

namespace CSD
{
    class CSDHub : public CSDDevice
    {
    public:
        CSDHub();
        ~CSDHub();
        void setup();
        void update();
    private:
        ComLink_ESPNow comLinkHub{ComRole::Hub};

        MIDIOut midiOut;

        Peripheral::LED myledblue{"ledblue", PinAddress::GPIO_25};
        Peripheral::LED myledwhite{"ledwhite", PinAddress::GPIO_26};
        Peripheral::LED myledgreen{"ledgreen", PinAddress::GPIO_27, true};

        Peripheral::Oled_SSD1306 myDisplay{"oled"};
        DisplayDebug *displayDebug = new DisplayDebug;

        enum ControlID : uint8_t { CTL_BTN1 = 0, CTL_BTN2 = 1, CTL_POT = 2 };
        static constexpr uint8_t MSG_CONTROL = 1;   // the "type" field

    };
}