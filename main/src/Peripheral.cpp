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
  // _value = _pin.readAnalog();

  uint32_t sum = 0;
  for (int i = 0; i < kOversample; i++) sum += _pin.readAnalog();
  _value = static_cast<int>(sum / kOversample);

  if (!_primed) { _smoothed = _value; _primed = true; }
  else          { _smoothed += kAlpha * (_value - _smoothed); }

  float pos = (_smoothed - kRawMin) * 127.0f / (kRawMax - kRawMin);
  pos = std::clamp(pos, 0.0f, 127.0f);

  if (_lastSent < 0 || std::fabs(pos - _midiValue) > kHysteresis) {
    _midiValue = static_cast<int>(pos + 0.5f);
  }
}

bool Potentiometer::changed()
{
  return _midiValue != _lastSent;
}

void Potentiometer::send(MIDIOut &midiOut)
{
  int midiValue = std::clamp(_value >> 5, 0, 127);

  if (midiValue == _lastSent) return;
  _lastSent = midiValue;

  // midiOut.controlChange(1, 21, midiValue);
}

RotaryEncoder::RotaryEncoder(PeripheralID id, PinAddress enc_a, PinAddress enc_b, PinAddress enc_sw)
  : PeripheralBase(id),
    _enc_a(enc_a, PinMode::InputPullup),
    _enc_b(enc_b, PinMode::InputPullup),
    _enc_sw(id, enc_sw) {}

void RotaryEncoder::setup()
{
  _enc_a.setup();
  _enc_b.setup();
  _enc_sw.setup();

  _oldAB  = (_enc_a.readDigital() << 1) | _enc_b.readDigital();

  _enc_a.attachISR(&RotaryEncoder::onEdge, this);
  _enc_b.attachISR(&RotaryEncoder::onEdge, this);
}

void RotaryEncoder::update()
{
  _enc_sw.update();

  portENTER_CRITICAL(&_lock);
  int32_t d = _pending;
  _pending = 0;
  portEXIT_CRITICAL(&_lock);

  _delta = d;
  _position += d;
}

// For non-interrupt
// void RotaryEncoder::update()
// {
//   // pin_a_.update();
//   // pin_b_.update();
//   _enc_sw.update();

//   currentStateClock = _enc_a.readDigital();

//   if (currentStateClock != lastStateClock && currentStateClock == LOW)
//   {
//     if (_enc_b.readDigital() != currentStateClock)
//     {
//       counter++;
//     }
//     else
//     {
//       counter--;
//     }
//   }

//   lastStateClock = currentStateClock;
// }

void RotaryEncoder::send(MIDIOut &midiOut)
{
   if (_delta != 0)
  {
    // Relative CC: 64 = no change, 65+ = clockwise, 63- = counter-clockwise.
    uint8_t value = constrain(64 + _delta, 0, 127);

    // TODO: send `value` the same way Button::send does
    (void)value;
  }

  _enc_sw.send(midiOut);
}

void RotaryEncoder::onEdge(void *self)
{
  static_cast<RotaryEncoder *>(self)->readEncoder();
}

void IRAM_ATTR RotaryEncoder::readEncoder()
{
  portENTER_CRITICAL_ISR(&_lock);

  _oldAB <<= 2;
  if (_enc_a.readDigitalISR()) _oldAB |= 0x02;
  if (_enc_b.readDigitalISR()) _oldAB |= 0x01;

  _encval += kEncStates[_oldAB & 0x0f];

  if (_encval > 3)
  {
    int64_t now = esp_timer_get_time();
    _pending += (now - _lastIncUs < kPauseLengthUs) ? kFastIncrement : 1;
    _lastIncUs = now;
    _encval = 0;
  }
  else if (_encval < -3)
  {
    int64_t now = esp_timer_get_time();
    _pending -= (now - _lastDecUs < kPauseLengthUs) ? kFastIncrement : 1;
    _lastDecUs = now;
    _encval = 0;
  }

  portEXIT_CRITICAL_ISR(&_lock);
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
