#pragma once

#include <cstdint>

// #include <Arduino.h>
// #include "pinout_esp32.h"
// #include "pin_handle.h"
// #include "Peripheral.h"
// #include "Bluetooth.h"
// #include "MIDIControl.h"
// #include "Communication.h"
// #include "Multiplexor.h"
// #include "bitmap.h"
// #include "LMUtil.h"
// #include "LMThread.h"

enum class StateType : uint8_t
{
    Digital,
    Analog,
    Encoder
};

class StateBase
{
public:
    explicit StateBase(uint8_t id);
    virtual ~StateBase() = default;

    StateBase(const StateBase&) = delete;
    StateBase& operator=(const StateBase&) = delete;

    virtual StateType type() const = 0;

    uint8_t id() const { return _id; }
    bool changed() const { return _changed; }
    void clearChanged() { _changed = false; }

protected:
    void markChanged() { _changed = true; }

private:
    uint8_t _id;
    bool _changed = false;
};

class ButtonState : public StateBase
{
public:
    StateType type() const override { return StateType::Digital; }

    void set(bool pressed);

private:
    bool _pressed = false;
};
