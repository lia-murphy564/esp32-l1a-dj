#pragma once
#include <Arduino.h>
#include "pinout_esp32.h"

enum class PinMode : uint8_t
{
  Input         = INPUT,
  Output        = OUTPUT,
  InputPullup   = INPUT_PULLUP,
  InputPulldown = INPUT_PULLDOWN,
  Default       = 0xFF, // Custom mode for PWM output
};

class PinHandle
{
  public:
    PinHandle() {}
    explicit PinHandle(PinAddress pin) : _pin(pin), _hasPin(true) {}
    explicit PinHandle(PinAddress pin, PinMode mode) : _pin(pin), _mode(mode), _hasPin(true) {}

    void setPin(PinAddress pin)
    {
      _pin = pin;
      _hasPin = true;
    }

    void setMode(PinMode mode)
    {
      _mode = mode;
    }

    PinMode getMode()
    {
      return _mode;
    }

    void setup()
    {
      if (_hasPin)
      pinMode(static_cast<uint8_t>(_pin), static_cast<uint8_t>(_mode));
    }

    int readDigital()
    {
      if (_hasPin == false)
        return -1;
      return digitalRead(static_cast<uint8_t>(_pin));

    }

    int readAnalog()
    {
      if (_hasPin == false)
        return -1;
      return analogRead(static_cast<uint8_t>(_pin));
    }

    void writeDigital(bool value)
    {
      if (_hasPin == false)
        return;
      digitalWrite(static_cast<uint8_t>(_pin), value ? HIGH : LOW);
    }

    void writePWM(uint8_t value)
    {
      if (_hasPin == false)
        return;
      analogWrite(static_cast<uint8_t>(_pin), value);
    }

    bool hasPin() const
    {
      return _hasPin;
    }

  private:
    PinAddress _pin;
    PinMode _mode = PinMode::Default;
    bool _hasPin = false;
};

