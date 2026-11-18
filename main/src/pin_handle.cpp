#include "pin_handle.h"

PinHandle::PinHandle(PinAddress pin)
  : _pin(pin), _hasPin(true) {}

PinHandle::PinHandle(PinAddress pin, PinMode mode)
  : _pin(pin), _mode(mode), _hasPin(true) {}

void PinHandle::setPin(PinAddress pin)
{
  _pin = pin;
  _hasPin = true;
}

void PinHandle::setup()
{
  if (!(_hasPin || _mode == PinMode::Default))
    return;
  pinMode(rawPin(), static_cast<uint8_t>(_mode));
}

int PinHandle::readDigital() const
{
  if (!_hasPin)
    return -1;
  return digitalRead(rawPin());
}

int PinHandle::readAnalog() const
{
  if (!_hasPin)
    return -1;
  return analogRead(rawPin());
}

void PinHandle::writeDigital(bool value)
{
  if (!_hasPin)
    return;
  digitalWrite(rawPin(), value ? HIGH : LOW);
}

void PinHandle::writePWM(uint8_t value)
{
  if (!_hasPin)
    return;
  analogWrite(rawPin(), value);
}

void PinHandle::attachISR(isrfn fn, void *arg, int mode)
{
  attachInterruptArg(digitalPinToInterrupt(rawPin()), fn, arg, mode);
}

int IRAM_ATTR PinHandle::readDigitalISR() const
{
  return gpio_get_level(static_cast<gpio_num_t>(rawPin()));
}
