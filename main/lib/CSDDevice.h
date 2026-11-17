//
//  CICADA SOUND DEVICES - Device
//

#pragma once

#include "pinout_esp32.h"
#include "pin_handle.h"
#include "Peripheral.h"
#include "Bluetooth.h"
#include "MIDIControl.h"
#include "Communication.h"
#include "Multiplexor.h"
#include "bitmap.h"
#include "LMUtil.h"
#include "LMThread.h"
// #include "_CSDHub.h"
// #include "_CSDModule.h"

class CSDDevice
{
public:
    virtual ~CSDDevice() = default;
    virtual void setup() = 0;
    virtual void update() = 0;

protected:
    CSDDevice() = default;
};

