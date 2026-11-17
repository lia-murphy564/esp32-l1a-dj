#pragma once
#include <Arduino.h>
#include "pinout_esp32.h"
#include "MIDIOut.h"
#include <algorithm>
#include <Wire.h>
#include "Display.h"
#include "LMUtil.h"

namespace Peripheral
{
  struct DataState {};

  struct ButtonState : DataState
  {
    uint8_t value = 0;
  };

  struct PotentiometerState : DataState
  {
    uint8_t value = 0; // 0 - 127
  };

using PeripheralID = const char *;
  
class PeripheralBase
{
public:
  virtual ~PeripheralBase() {}

  explicit PeripheralBase(PeripheralID id = nullptr) : _id(id) {}

  virtual void setup() = 0;
  virtual void update() = 0;
  virtual PeripheralID id() const
  {
    return _id;
  }

  virtual void send(MIDIOut &midiOut) {}

private:
  PeripheralID _id;
};

class Button : public PeripheralBase
{
public:
  explicit Button(PeripheralID id, PinAddress pin) : PeripheralBase(id), _pin(pin, PinMode::InputPullup) {}
  explicit Button(PeripheralID id) : PeripheralBase(id) {}

  void setup() override
  {
    _pin.setup();
  }

  void update() override
  {
    // TODO: debounce

    if (!_pin.hasPin()) return;
    updateInternalData(_pin.readDigital());
  }

  bool changed()
  {
    if (_value == _lastSent) return false;
    _lastSent = _value;
    return true;
  }

  void updateInternalData(auto val)
  {
    if (!_debouncer.debounce(val == LOW)) return;

    if (true)
      _value = !_debouncer.stable;
    else if (!_debouncer.stable)
      _value = !_value; 
  }

  int getValue() const
  {
    return _value;
  }

  void send(MIDIOut &midiOut) override
  {
    int midiValue = (_value == LOW) ? 127 : 0;

    if (midiValue == _lastSent) return;
    _lastSent = midiValue;

    midiOut.controlChange(1, 20, midiValue);
  }
  
private:
  PinHandle _pin;
  int _value = LOW;
  int _lastSent = -1;
  LMUtil::Debouncer _debouncer;
};

class Potentiometer : public PeripheralBase
{
public:
  explicit Potentiometer(PeripheralID id, PinAddress pin) : PeripheralBase(id), _pin(pin, PinMode::Input) {}

  void setup() override
  {
    _pin.setup();
  }

  void update() override
  {
    // TODO: smoothing
    _value = _pin.readAnalog();
  }

  bool changed()
  {
    if (_value == _lastSent) return false;
    _lastSent = _value;
    return true;
  }

  int getValue() const
  {
    return _value;
  }

  float getValueSmoothed() const
  {
    return _smoothed;
  }

  void send(MIDIOut &midiOut) override
  {
    int midiValue = std::clamp(_value >> 5, 0, 127);

    if (midiValue == _lastSent) return;
    _lastSent = midiValue;

    // midiOut.controlChange(1, 21, midiValue);
  }

private:
  PinHandle _pin;
  int _value = 0;
  float _smoothed = 0.0f;
  int _lastSent = -1;
};

class RotaryEncoder : public PeripheralBase
{
public:
  explicit RotaryEncoder(PeripheralID id, PinAddress pin_a, PinAddress pin_b, PinAddress pin_sw)
    : PeripheralBase(id),
      pin_a_(pin_a, PinMode::Input),
      pin_b_(pin_b, PinMode::Input),
      _button(id, pin_sw) {}

      void setup() override
      {
        pin_a_.setup();
        pin_b_.setup();
        _button.setup();

        lastStateClock = pin_a_.readDigital();
      }

      void update() override
      {
        // pin_a_.update();
        // pin_b_.update();
        _button.update();

        currentStateClock = pin_a_.readDigital();

        if (currentStateClock != lastStateClock && currentStateClock == LOW)
        {
          if (pin_b_.readDigital() != currentStateClock)
          {
            counter++;
          }
          else
          {
            counter--;
          }
        }

        lastStateClock = currentStateClock;
      }

      int getValueButton() const
      {
        return _button.getValue();
      }

      int getValuePosition() const
      {
        return counter;
      }

      void send(MIDIOut &midiOut) override
      {
        return;
      }

private:
  PinHandle pin_a_;
  PinHandle pin_b_;
  Button _button;

  int counter;
  int currentStateClock;
  int lastStateClock;
};

//Todo: U8g2 font
class Oled_SSD1306 : public PeripheralBase
{
public:
  explicit Oled_SSD1306(PeripheralID id, TwoWire &bus = Wire, uint8_t address = 0x3C)
    : PeripheralBase(id),
      _bus(bus),
      _address(address),
      _display(_width, _height, &_bus, _resetPin) {}

  void setup() override
  {
    _display.begin(SSD1306_SWITCHCAPVCC, _address);
    _display.clearDisplay();
    _display.setTextColor(SSD1306_WHITE);

    _display.setTextSize(1);
    _display.setCursor(0, 0);
    _display.println("L1A - DEBUG");

    _display.setTextSize(2);
    _display.setCursor(0, 12);
    _display.println("CC NaN");

    _display.display();
  }

  void update() override
  {
    if (!_stale)
      return;

    updateDisplay();
    _stale = false;
  }

  void setDisplay(DisplayData &data)
  {
    _data = &data;
    _stale = true;
    
    // if (val.get() == _data.get())
    //   return;
    // _data = val;
    // _stale = true;
  }

private:
  void updateDisplay()
  {
    _display.clearDisplay();

    if (_data)
      _data->draw(_display);

    _display.display();

  }

  TwoWire &_bus;
  uint8_t _address;
  Adafruit_SSD1306 _display;

  static constexpr uint8_t _width = 128;
  static constexpr uint8_t _height = 32;
  static constexpr int8_t _resetPin = -1;

  DisplayData *_data = nullptr;
  bool _stale = false;
};

class LED : public PeripheralBase
{
public:
  explicit LED(PeripheralID id, PinAddress pin)
    : PeripheralBase(id),
      _pin(pin, PinMode::Output) {
        _isPWM = false;
      }

  explicit LED(PeripheralID id, PinAddress pin, bool usePWM)
    : PeripheralBase(id),
      _pin(pin, usePWM ? PinMode::Default : PinMode::Output) {
        if (_pin.getMode() == PinMode::Default)
          _isPWM = true;
      }

    void setup() override
    {
      _pin.setup();
    }

    void update() override
    {
      if (isBlinking)
      {
        if (millis() - _lastBlinkTime >= _blinkPeriod)
        {
          if (_isPWM)
          {
            _pwmValue += _fadeDirection;
            if (_pwmValue == 0 || _pwmValue >= 255)
              _fadeDirection = -_fadeDirection;
          }
          else
          {
            _state = !_state;
            if (_state > 1)
              _state = 1;
          }

          _lastBlinkTime = millis();
        }
      }

      if (_isPWM)
        _pin.writePWM(_state ? LMUtil::gamma8(_pwmValue) : 0);
      else
        _pin.writeDigital(_state);
    }

    void set(uint8_t state)
    {
      _state = state;
      update();
    }

    void toggle()
    {
      _state = !_state;
    }

    void setBlink(uint16_t period)
    {
      _blinkPeriod = period;

      if (_blinkPeriod == 0)
      {
        isBlinking = false;
        return;
      }

      isBlinking = true;
      _lastBlinkTime = millis();
    }

    void pwm(uint8_t value)
    {
      _pwmValue = value;
      update();
    }

private: 
  PinHandle _pin;

  bool isBlinking = false;

  // Digital
  uint8_t _state = 0;
  uint16_t _blinkPeriod = 0;
  uint32_t _lastBlinkTime = 0;

  // PWM
  bool _isPWM = false;
  uint8_t _pwmValue = 0; // 0 - 255
  int8_t _fadeDirection = 1; // 1 = up, -1 = down
};

} // namespace Peripheral

