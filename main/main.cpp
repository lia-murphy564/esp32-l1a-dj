#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart.h"
#include <Arduino.h>
#include "lib/CSDDevice.h"
#include "lib/_CSDHub.h"
#include "lib/_CSDModule.h"

#define TARGET_HUB 0

// MIDIController controller;
// MIDIOut midiOut;

bool bHub = false;

CSD::CSDHub csdHub;
CSD::CSDModule csdModule;

// GLOBAL DEFINES
Peripheral::Oled_SSD1306 myDisplay("oled");
DisplayDebug *displayDebug = new DisplayDebug;

enum ControlID : uint8_t { CTL_BTN1 = 0, CTL_BTN2 = 1, CTL_POT = 2 };
constexpr uint8_t MSG_CONTROL = 1;   // the "type" field

// HUB DEFINES
// ComLink_ESPNow comLinkHub(ComRole::Hub);

// MIDIOut midiOut;

// Peripheral::LED myledblue("ledblue", PinAddress::GPIO_25);
// Peripheral::LED myledwhite("ledwhite", PinAddress::GPIO_26);
// Peripheral::LED myledgreen("ledgreen", PinAddress::GPIO_27, true);

// MODULE DEFINES
ComLink_ESPNow comLinkModule(ComRole::Module);

Peripheral::Multiplexor mux(
    "mux",
    PinAddress::GPIO_5,    // S0
    PinAddress::GPIO_18,   // S1
    PinAddress::GPIO_19,   // S2
    PinAddress::GPIO_23,   // S3
    PinAddress::GPIO_32    // SEL
); 

Peripheral::Button buttons[16] = {
    Peripheral::Button("btn0"),  Peripheral::Button("btn1"),  Peripheral::Button("btn2"),  Peripheral::Button("btn3"),
    Peripheral::Button("btn4"),  Peripheral::Button("btn5"),  Peripheral::Button("btn6"),  Peripheral::Button("btn7"),
    Peripheral::Button("btn8"),  Peripheral::Button("btn9"),  Peripheral::Button("btn10"), Peripheral::Button("btn11"),
    Peripheral::Button("btn12"), Peripheral::Button("btn13"), Peripheral::Button("btn14"), Peripheral::Button("btn15"),
};

Peripheral::Potentiometer myPot("pot", PinAddress::GPIO_34);
// Peripheral::RotaryEncoder myEnc("enc", PinAddress::GPIO_18, PinAddress::GPIO_19, PinAddress::GPIO_21);
Peripheral::Button myButton("button", PinAddress::GPIO_13);

Peripheral::LED ledYellow("ledyellow", PinAddress::GPIO_27);


extern "C" void app_main(void)
{
    initArduino();

    Serial.begin(115200);
    Wire.begin(21, 22); // SDA, SCL

    // uart_set_baudrate(UART_NUM_0, 115200);

    // Testing
    if (TARGET_HUB) // HUB
    {
        csdHub.setup();



        // Serial.begin(115200);
        // Wire.begin(21, 22); // SDA, SCL

        // myDisplay.setDisplay(*displayDebug);
        // myDisplay.setup();

        // comLinkHub.begin();
    }
    else // MODULE
    {
        csdModule.setup();


        Serial.begin(115200);
        Wire.begin(21, 22); // SDA, SCL

        myDisplay.setDisplay(*displayDebug);
        myDisplay.setup();

        for (uint8_t i = 0; i < 16; i++)
        {
        mux.attach(buttons[i], i);
        // controller.addControl(buttons[i]);
        }

        myButton.setup();
        myPot.setup();

        ledYellow.setup();

        mux.setup();
        comLinkModule.begin();
    }

    // controller.setOutput(midiOut);

    // controller.addControl(myButton);
    // controller.addControl(myPot);
    // controller.addControl(myEnc);
    // controller.addControl(myEnc, 20); // ControlHandle, cc number

    // controller.setup();

    int count = 0;
    while (1) {
        // Testing
        if (bHub) // HUB
        {
            csdHub.update();


            // static int hubBtn1 = 0, hubBtn2 = 0;

            // Event e;
            
            // while (comLinkHub.receive(e))
            // {
            // switch (e.msg.control)
            // {
            //     case CTL_BTN1: 
            //         hubBtn1 = e.msg.value;
            //         midiOut.controlChange(1, 20, hubBtn1 == LOW ? 127 : 0);
            //         break;
            //     case CTL_BTN2: 
            //         hubBtn2 = e.msg.value;
            //         midiOut.controlChange(1, 21, hubBtn2 == LOW ? 127 : 0);
            //         break;
            //     case CTL_POT:  
            //         displayDebug->setPot(e.msg.value);
            //         midiOut.controlChange(1, 22, std::clamp(e.msg.value >> 5, 0, 127));
            //         break;
            // }

            // displayDebug->setButtons(hubBtn1, hubBtn2);
            // myDisplay.setDisplay(*displayDebug);
            // }

            // comLinkHub.update();
            
            // myDisplay.update();

            // myledblue.update();
            // myledwhite.update();
            // myledgreen.update();
        }
        else // MODULE
        {
            csdModule.update();




            mux.update();

            myButton.update();
            myPot.update();

            ledYellow.update();

            int buttonState = myButton.getValue();
            int potValue = myPot.getValue();
            // int encButtonValue = myEnc.getValueButton();
            // int encPosition = myEnc.getValuePosition();

            if (myButton.changed()) 
            {
            comLinkModule.send(MSG_CONTROL, CTL_BTN1, buttonState);
            ledYellow.set(buttonState);
            }

            if (myPot.changed())
            comLinkModule.send(MSG_CONTROL, CTL_POT, potValue);

            displayDebug->setButtons(buttonState, 1010);
            displayDebug->setPot(potValue);


            // for (uint8_t i = 0; i < 16; i++)
            // {
            //     int v = buttons[i].getValue();
            //     if (v == LOW && lastValue[i] != LOW)
            //         displayDebug->setButtons
            //     lastValue[i] = v;
            // }

            myDisplay.setDisplay(*displayDebug);
            myDisplay.update();

            comLinkModule.update();
        }
    }
}
