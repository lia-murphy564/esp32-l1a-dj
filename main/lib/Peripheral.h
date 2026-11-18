#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "pinout_esp32.h"
#include "pin_handle.h"
#include "MIDIOut.h"
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

  explicit PeripheralBase(PeripheralID id = nullptr);

  virtual void setup() = 0;
  virtual void update() = 0;
  virtual PeripheralID id() const { return _id; }

  virtual void send(MIDIOut &midiOut) {}

private:
  PeripheralID _id;
};

class Button : public PeripheralBase
{
public:
  explicit Button(PeripheralID id, PinAddress pin);
  explicit Button(PeripheralID id);

  void setup() override;
  void update() override;

  bool changed();

  void updateInternalData(auto val)
  {
    if (!_debouncer.debounce(val == LOW)) return;

    if (true)
      _value = !_debouncer.stable;
    else if (!_debouncer.stable)
      _value = !_value;
  }

  int getValue() const { return _value; }

  void send(MIDIOut &midiOut) override;

private:
  PinHandle _pin;
  int _value = LOW;
  int _lastSent = -1;
  LMUtil::Debouncer _debouncer;
};

class Potentiometer : public PeripheralBase
{
public:
  explicit Potentiometer(PeripheralID id, PinAddress pin);

  void setup() override;
  void update() override;

  bool changed();

  int getValue() const { return _value; }
  float getValueSmoothed() const { return _smoothed; }
  int getMidiValue() const { return _midiValue; }

  void send(MIDIOut &midiOut) override;

private:
  PinHandle _pin;

  int _value = 0;
  float _smoothed = 0.0f;
  int _lastSent = -1;
  int _midiValue = 0; // 0 - 127
  bool  _primed    = false;

  static constexpr int kOversample = 16;
  static constexpr float kAlpha = 0.20f;
  static constexpr float kHysteresis = 0.8f;
  static constexpr int kRawMin = 140;
  static constexpr int kRawMax = 4070;
};

class RotaryEncoder : public PeripheralBase
{
public:
  explicit RotaryEncoder(PeripheralID id, PinAddress pin_a, PinAddress pin_b, PinAddress pin_sw);

  void setup() override;
  void update() override;

  int getValueButton() const { return _button.getValue(); }
  int getValuePosition() const { return counter; }

  void send(MIDIOut &midiOut) override;

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
  explicit Oled_SSD1306(PeripheralID id, TwoWire &bus = Wire, uint8_t address = 0x3C);

  void setup() override;
  void update() override;

  void setDisplay(DisplayData &data);

private:
  void updateDisplay();

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
  explicit LED(PeripheralID id, PinAddress pin);
  explicit LED(PeripheralID id, PinAddress pin, bool usePWM);

  void setup() override;
  void update() override;

  void set(uint8_t state);
  void toggle();
  void setBlink(uint16_t period);
  void pwm(uint8_t value);

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
