#include "Multiplexor.h"

namespace Peripheral
{

Multiplexor::Multiplexor(PeripheralID id, PinAddress s0, PinAddress s1, PinAddress s2, PinAddress s3, PinAddress sig)
  : PeripheralBase(id),
    _s0(s0, PinMode::Output),
    _s1(s1, PinMode::Output),
    _s2(s2, PinMode::Output),
    _s3(s3, PinMode::Output),
    _sig(sig, PinMode::InputPullup)
{}

void Multiplexor::setup()
{
  _s0.setup();
  _s1.setup();
  _s2.setup();
  _s3.setup();
  _sig.setup();
}

void Multiplexor::update()
{
  for (uint8_t ch = 0; ch < 16; ch++)
  {
    if (_buttons[ch] == nullptr) continue;
    _buttons[ch]->updateInternalData(readChannel(ch));
  }
  // _state = readAll();
}

void Multiplexor::attach(Button &button, uint8_t ch)
{
  if (ch > 0x0F) return;
  _buttons[ch] = &button;
}

uint8_t Multiplexor::readChannel(uint8_t ch)
{
  setChannel(ch);
  delayMicroseconds(5); // sig delay
  return _sig.readDigital();
}

uint16_t Multiplexor::readAll()
{
  uint16_t state = 0;
  for (uint8_t ch = 0; ch < 16; ch++)
  {
    if (readChannel(ch))
      state |= (1 << ch);
  }
  return state;
}

void Multiplexor::setChannel(uint8_t ch)
{
  ch &= 0x0F;
  if (ch == _currentCh) return;

  _s0.writeDigital(ch & 0x01);
  _s1.writeDigital((ch >> 1) & 0x01);
  _s2.writeDigital((ch >> 2) & 0x01);
  _s3.writeDigital((ch >> 3) & 0x01);

  _currentCh = ch;
}

}
