#pragma once
#include <Arduino.h>
#include "pinout_esp32.h"

enum class PinMode : uint8_t
{
  Input         = INPUT,
  Output        = OUTPUT,
  InputPullup   = INPUT_PULLUP,
  InputPulldown = INPUT_PULLDOWN,
  Default       = 0xFF, // PWM output
};

class PinHandle
{
  public:
    PinHandle() = default;
    explicit PinHandle(PinAddress pin);
    PinHandle(PinAddress pin, PinMode mode);

    void setPin(PinAddress pin);

    void    setMode(PinMode mode) { _mode = mode; }
    PinMode getMode() const       { return _mode; }
    bool    hasPin() const        { return _hasPin; }

    void setup();

    int  readDigital() const;
    int  readAnalog() const;
    void writeDigital(bool value);
    void writePWM(uint8_t value);

  private:
    uint8_t rawPin() const { return static_cast<uint8_t>(_pin); }

    PinAddress _pin{};
    PinMode    _mode   = PinMode::Default;
    bool       _hasPin = false;
};
