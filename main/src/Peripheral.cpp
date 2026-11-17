#include "Peripheral.h"
#include <algorithm>

namespace Peripheral
{

PeripheralBase::PeripheralBase(PeripheralID id)
  : _id(id) {}

Button::Button(PeripheralID id, PinAddress pin)
  : PeripheralBase(id), _pin(pin, PinMode::InputPullup) {}

Button::Button(PeripheralID id)
  : PeripheralBase(id) {}

void Button::setup()
{
  _pin.setup();
}

void Button::update()
{
  // TODO: debounce

  if (!_pin.hasPin()) return;
  updateInternalData(_pin.readDigital());
}

bool Button::changed()
{
  if (_value == _lastSent) return false;
  _lastSent = _value;
  return true;
}

void Button::send(MIDIOut &midiOut)
{
  int midiValue = (_value == LOW) ? 127 : 0;

  if (midiValue == _lastSent) return;
  _lastSent = midiValue;

  midiOut.controlChange(1, 20, midiValue);
}

Potentiometer::Potentiometer(PeripheralID id, PinAddress pin)
  : PeripheralBase(id), _pin(pin, PinMode::Input) {}

void Potentiometer::setup()
{
  _pin.setup();
}

void Potentiometer::update()
{
  // TODO: smoothing
  _value = _pin.readAnalog();
}

bool Potentiometer::changed()
{
  if (_value == _lastSent) return false;
  _lastSent = _value;
  return true;
}

void Potentiometer::send(MIDIOut &midiOut)
{
  int midiValue = std::clamp(_value >> 5, 0, 127);

  if (midiValue == _lastSent) return;
  _lastSent = midiValue;

  // midiOut.controlChange(1, 21, midiValue);
}

RotaryEncoder::RotaryEncoder(PeripheralID id, PinAddress pin_a, PinAddress pin_b, PinAddress pin_sw)
  : PeripheralBase(id),
    pin_a_(pin_a, PinMode::Input),
    pin_b_(pin_b, PinMode::Input),
    _button(id, pin_sw) {}

void RotaryEncoder::setup()
{
  pin_a_.setup();
  pin_b_.setup();
  _button.setup();

  lastStateClock = pin_a_.readDigital();
}

void RotaryEncoder::update()
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

void RotaryEncoder::send(MIDIOut &midiOut)
{
  return;
}

Oled_SSD1306::Oled_SSD1306(PeripheralID id, TwoWire &bus, uint8_t address)
  : PeripheralBase(id),
    _bus(bus),
    _address(address),
    _display(_width, _height, &_bus, _resetPin) {}

void Oled_SSD1306::setup()
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

void Oled_SSD1306::update()
{
  if (!_stale)
    return;

  updateDisplay();
  _stale = false;
}

void Oled_SSD1306::setDisplay(DisplayData &data)
{
  _data = &data;
  _stale = true;

  // if (val.get() == _data.get())
  //   return;
  // _data = val;
  // _stale = true;
}

void Oled_SSD1306::updateDisplay()
{
  _display.clearDisplay();

  if (_data)
    _data->draw(_display);

  _display.display();
}

LED::LED(PeripheralID id, PinAddress pin)
  : PeripheralBase(id),
    _pin(pin, PinMode::Output)
{
  _isPWM = false;
}

LED::LED(PeripheralID id, PinAddress pin, bool usePWM)
  : PeripheralBase(id),
    _pin(pin, usePWM ? PinMode::Default : PinMode::Output)
{
  if (_pin.getMode() == PinMode::Default)
    _isPWM = true;
}

void LED::setup()
{
  _pin.setup();
}

void LED::update()
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

void LED::set(uint8_t state)
{
  _state = state;
  update();
}

void LED::toggle()
{
  _state = !_state;
}

void LED::setBlink(uint16_t period)
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

void LED::pwm(uint8_t value)
{
  _pwmValue = value;
  update();
}

}
