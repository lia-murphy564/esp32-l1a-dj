#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart.h"
#include <Arduino.h>
#include "lib/CSDDevice.h"
#include "lib/_CSDHub.h"
#include "lib/_CSDModule.h"

#define TARGET_TEST         1
#define TARGET_HUB          2
#define TARGET_MODULE       3

#define BUILD_TARGET        2

#define BAUD_RATE           115200

// #if TARGET == TARGET_HUB
// #endif
// #elif TARGET == TARGET_MODULE
// #endif
// #elif TARGET == TARGET_TEST
// #endif
// else
//    #error "bad target"
// #endif

// ----------------------------------------------------------------------------------- //
//                                  SECTION : DEFINES                                  //
// ----------------------------------------------------------------------------------- //

//  --- COMMON DEFINES HERE --- // 
enum ControlID : uint8_t { CTL_POT = 0, CTL_ENC_SW = 1, CTL_ENC_V = 2 };
constexpr uint8_t MSG_CONTROL = 1;


#if BUILD_TARGET == TARGET_HUB
//  ---- HUB DEFINES HERE ----- // 
Peripheral::Oled_SSD1306 myDisplay("oled");
DisplayDebug *displayDebug = new DisplayDebug;
MIDIOut midiOut;
MIDIController controller;
Peripheral::LED myledblue("ledblue", PinAddress::GPIO_25);
Peripheral::LED myledwhite("ledwhite", PinAddress::GPIO_26);
Peripheral::LED myledgreen("ledgreen", PinAddress::GPIO_27, true);
ComLink_ESPNow comLink(ComRole::Hub);


#elif BUILD_TARGET == TARGET_MODULE
//  ---- MODULE DEFINES HERE --- // 

Peripheral::Multiplexor mux(
    "mux",
    PinAddress::GPIO_5,    // S0
    PinAddress::GPIO_18,   // S1
    PinAddress::GPIO_19,   // S2
    PinAddress::GPIO_23,   // S3
    PinAddress::GPIO_32    // SEL
); 

Peripheral::RotaryEncoder myEncoder{"encoder", PinAddress::GPIO_25, PinAddress::GPIO_26, PinAddress::GPIO_27};
Peripheral::Potentiometer myPot("pot", PinAddress::GPIO_34);

Peripheral::Oled_SSD1306 myDisplay("oled");
DisplayDebug *displayDebug = new DisplayDebug;

MIDIOut midiOut;
MIDIController controller;

ComLink_ESPNow comLink(ComRole::Module);

#elif BUILD_TARGET == TARGET_TEST
//  ---- TEST DEFINES HERE ---- // 
Peripheral::RotaryEncoder myEncoder{"encoder", PinAddress::GPIO_25, PinAddress::GPIO_26, PinAddress::GPIO_27};
MIDIOut midiOut;
MIDIController controller;

#else
    #error "bad target"
#endif

// ----------------------------------------------------------------------------------- //
//                                  SECTION : SETUP                                    //
// ----------------------------------------------------------------------------------- //
extern "C" void app_main(void)
{
    initArduino();
    Serial.begin(BAUD_RATE);
    esp_log_level_set("*", ESP_LOG_NONE);

#if BUILD_TARGET == TARGET_HUB
//  ---- HUB SETUP HERE ------- // 
    Wire.begin(21, 22); // SDA, SCL 

    myledblue.setup();
    myledwhite.setup();
    myledgreen.setup();

    myDisplay.setDisplay(*displayDebug);
    myDisplay.setup();

    controller.setOutput(midiOut);
    controller.setup();

    comLink.begin();


#elif BUILD_TARGET == TARGET_MODULE
//  ---- MODULE SETUP HERE ---- // 
    Wire.begin(21, 22); // SDA, SCL 

    myDisplay.setDisplay(*displayDebug);
    myDisplay.setup();

    myPot.setup();
    myEncoder.setup();

    comLink.begin();

#elif BUILD_TARGET == TARGET_TEST
//  ---- TEST SETUP HERE ------ // 
    // myEncoder.setup();
    controller.addControl(myEncoder);
    controller.setOutput(midiOut);
    controller.setup();

#else
    #error "bad target"
#endif

// ----------------------------------------------------------------------------------- //
//                                  SECTION : UPDATE                                   //
// ----------------------------------------------------------------------------------- //
    while (true)
    {
#if BUILD_TARGET == TARGET_HUB
//  ---- HUB UPDATE HERE ------ // 
        comLink.update();

        // static uint32_t lastBeat = 0;
        // if (millis() - lastBeat > 1000) {
        //     lastBeat = millis();
        //     Serial.println("hub alive");
        // }

        myledblue.update();
        myledwhite.update();
        myledgreen.update();

        static int potv = 0, encswval = 0, encv = 0;
        Event e;
        
        while (comLink.receive(e))
        {
            // Serial.printf("rx ctl:%d val:%d\n", e.msg.control, e.msg.value);

            switch (e.msg.control)
            {
                case CTL_POT: 
                    potv = e.msg.value;
                    midiOut.controlChange(1, 20, std::clamp(potv, 0, 127));
                    // myledblue.toggle();
                    break;
                case CTL_ENC_SW: 
                    encswval = e.msg.value;
                    midiOut.controlChange(1, 21, encswval == LOW ? 127 : 0);
                    // myledwhite.toggle();
                    break;
                case CTL_ENC_V:
                    encv = e.msg.value;  
                    midiOut.controlChange(1, 22, std::clamp(encv, 0, 127));
                    // myledgreen.toggle();
                    break;
            }
        }

        controller.update();

        displayDebug->setPot(potv);
        displayDebug->setButtons(encswval, encv);
        myDisplay.setDisplay(*displayDebug);
        
        myDisplay.update();

#elif BUILD_TARGET == TARGET_MODULE
//  ---- MODULE UPDATE HERE --- // 

        mux.update();
        myPot.update();
        myEncoder.update();

        displayDebug->setPot(myPot.getMidiValue());
        displayDebug->setButtons(myEncoder.getValueButton(), myEncoder.getValuePosition());

        myDisplay.setDisplay(*displayDebug);
        myDisplay.update();

        comLink.update();

        if (myPot.changed()) 
        {
            Serial.printf("tx pot %d\n", myPot.getMidiValue());
            comLink.send(MSG_CONTROL, CTL_POT, myPot.getMidiValue());
        }
        // else // encoder
        // {
        //     comLink.send(MSG_CONTROL, CTL_ENC_SW, myEncoder.getValueButton());
        //     comLink.send(MSG_CONTROL, CTL_ENC_V, myEncoder.getValuePosition());
        // }


#elif BUILD_TARGET == TARGET_TEST
//  ---- TEST UPDATE HERE ----- // 
    // myEncoder.update();

    // Encoder state
    static int lastBtn = -1;
    int btn = myEncoder.getValueButton();

    // if (myEncoder.changed() || btn != lastBtn)
    // {
    //     int d = myEncoder.getDelta();
    //     Serial.printf("pos:%d delta:%+d dir:%s SW:%d\n",
    //                   myEncoder.getValuePosition(), d,
    //                   d > 0 ? "CW" : d < 0 ? "CCW" : "--",
    //                   btn);
    //     lastBtn = btn;
    // }

    controller.update();         // calls myEncoder.update()
    controller.sendMidi();       // calls myEncoder.send(midiOut)

#else
    #error "bad target"
#endif
    vTaskDelay(1);
    } // while(true)
} // app_main(void)












// // MIDIController controller;
// // MIDIOut midiOut;

// // bool bTEST = true;
// // bool bHub = false;

// // CSD::CSDHub csdHub;
// // CSD::CSDModule csdModule;

// // GLOBAL DEFINES
// // Peripheral::Oled_SSD1306 myDisplay("oled");
// // DisplayDebug *displayDebug = new DisplayDebug;

// // enum ControlID : uint8_t { CTL_BTN1 = 0, CTL_BTN2 = 1, CTL_POT = 2 };
// // constexpr uint8_t MSG_CONTROL = 1;   // the "type" field

// // HUB DEFINES
// // ComLink_ESPNow comLinkHub(ComRole::Hub);

// // MIDIOut midiOut;

// // Peripheral::LED myledblue("ledblue", PinAddress::GPIO_25);
// // Peripheral::LED myledwhite("ledwhite", PinAddress::GPIO_26);
// // Peripheral::LED myledgreen("ledgreen", PinAddress::GPIO_27, true);

// // MODULE DEFINES
// // ComLink_ESPNow comLinkModule(ComRole::Module);

// // Peripheral::Multiplexor mux(
// //     "mux",
// //     PinAddress::GPIO_5,    // S0
// //     PinAddress::GPIO_18,   // S1
// //     PinAddress::GPIO_19,   // S2
// //     PinAddress::GPIO_23,   // S3
// //     PinAddress::GPIO_32    // SEL
// // ); 

// // Peripheral::Button buttons[16] = {
// //     Peripheral::Button("btn0"),  Peripheral::Button("btn1"),  Peripheral::Button("btn2"),  Peripheral::Button("btn3"),
// //     Peripheral::Button("btn4"),  Peripheral::Button("btn5"),  Peripheral::Button("btn6"),  Peripheral::Button("btn7"),
// //     Peripheral::Button("btn8"),  Peripheral::Button("btn9"),  Peripheral::Button("btn10"), Peripheral::Button("btn11"),
// //     Peripheral::Button("btn12"), Peripheral::Button("btn13"), Peripheral::Button("btn14"), Peripheral::Button("btn15"),
// // };

// // Peripheral::Potentiometer myPot("pot", PinAddress::GPIO_34);
// // Peripheral::RotaryEncoder myEnc("enc", PinAddress::GPIO_18, PinAddress::GPIO_19, PinAddress::GPIO_21);
// // Peripheral::Button myButton("button", PinAddress::GPIO_13);

// // Peripheral::LED ledYellow("ledyellow", PinAddress::GPIO_27);




// // Peripheral::RotaryEncoder myEncoder{"encoder", PinAddress::GPIO_25, PinAddress::GPIO_26, PinAddress::GPIO_27};


// // ----- SECTION DEFINES -----
// #if TARGET == TARGET_HUB
// #endif
// #elif TARGET == TARGET_MODULE
// #endif
// #elif TARGET == TARGET_TEST
// #endif
// else
//     // bad target
// #endif



// extern "C" void app_main(void)
// {

//     initArduino();

//     Serial.begin(115200);
//     Wire.begin(21, 22); // SDA, SCL
//     // uart_set_baudrate(UART_NUM_0, 115200);

// // ----- SECTION SETUP -----

// #if TARGET == TARGET_HUB
//     csdHub.setup();

//     // Serial.begin(115200);
//     // Wire.begin(21, 22); // SDA, SCL

//     // myDisplay.setDisplay(*displayDebug);
//     // myDisplay.setup();

//     // comLinkHub.begin();
// #endif
// #elif TARGET == TARGET_MODULE
//     csdModule.setup();

//     myDisplay.setDisplay(*displayDebug);
//     myDisplay.setup();

//     // for (uint8_t i = 0; i < 16; i++)
//     // {
//     // mux.attach(buttons[i], i);
//     // // controller.addControl(buttons[i]);
//     // }

//     // myButton.setup();
//     myPot.setup();

//     // ledYellow.setup();

//     // controller.setOutput(midiOut);

//     // controller.addControl(myButton);
//     // controller.addControl(myPot);
//     // controller.addControl(myEnc);
//     // controller.addControl(myEnc, 20); // ControlHandle, cc number

//     // controller.setup();

//     // mux.setup();
//     comLinkModule.begin();
// #endif
// #elif TARGET == TARGET_TEST
// #endif
// else
//     // bad target
// #endif



//     while (1) {
// // ----- SECTION UPDATE -----
// #if TARGET == TARGET_HUB
// #endif
// #elif TARGET == TARGET_MODULE
// #endif
// #elif TARGET == TARGET_TEST
// #endif
// else
//     // bad target
// #endif

//     }
// }
//         // // Testing
//         // if (bHub) // HUB
//         // {
//         //     csdHub.update();


//         //     // static int hubBtn1 = 0, hubBtn2 = 0;

//         //     // Event e;
            
//         //     // while (comLinkHub.receive(e))
//         //     // {
//         //     // switch (e.msg.control)
//         //     // {
//         //     //     case CTL_BTN1: 
//         //     //         hubBtn1 = e.msg.value;
//         //     //         midiOut.controlChange(1, 20, hubBtn1 == LOW ? 127 : 0);
//         //     //         break;
//         //     //     case CTL_BTN2: 
//         //     //         hubBtn2 = e.msg.value;
//         //     //         midiOut.controlChange(1, 21, hubBtn2 == LOW ? 127 : 0);
//         //     //         break;
//         //     //     case CTL_POT:  
//         //     //         displayDebug->setPot(e.msg.value);
//         //     //         midiOut.controlChange(1, 22, std::clamp(e.msg.value >> 5, 0, 127));
//         //     //         break;
//         //     // }

//         //     // displayDebug->setButtons(hubBtn1, hubBtn2);
//         //     // myDisplay.setDisplay(*displayDebug);
//         //     // }

//         //     // comLinkHub.update();
            
//         //     // myDisplay.update();

//         //     // myledblue.update();
//         //     // myledwhite.update();
//         //     // myledgreen.update();
//         // }
//         // else // MODULE
//         // {
//         //     csdModule.update();




//         //     // mux.update();

//         //     // myButton.update();
//         //     myPot.update();

//         //     // ledYellow.update();

//         //     // int buttonState = myButton.getValue();
//         //     int potValue = myPot.getValue();
//         //     // int encButtonValue = myEnc.getValueButton();
//         //     // int encPosition = myEnc.getValuePosition();

//         //     // if (myButton.changed()) 
//         //     // {
//         //     // comLinkModule.send(MSG_CONTROL, CTL_BTN1, buttonState);
//         //     // ledYellow.set(buttonState);
//         //     // }

//         //     // if (myPot.changed()) {
//         //     //     comLinkModule.send(MSG_CONTROL, CTL_POT, potValue);
//         //     //     printf("val: %u, valSmoothed: %f, midiVal: %u\n", myPot.getValue(), myPot.getValueSmoothed(), myPot.getMidiValue());
//         //     // }

//         //     // displayDebug->setButtons(buttonState, 1010);
//         //     displayDebug->setPot(potValue);


//         //     // for (uint8_t i = 0; i < 16; i++)
//         //     // {
//         //     //     int v = buttons[i].getValue();
//         //     //     if (v == LOW && lastValue[i] != LOW)
//         //     //         displayDebug->setButtons
//         //     //     lastValue[i] = v;
//         //     // }

//         //     myDisplay.setDisplay(*displayDebug);
//         //     myDisplay.update();

//         //     comLinkModule.update();
//         // }
//     }
// }
